/*
 * Copyright (c) 2013-2015 Intel Corporation, Inc.  All rights reserved.
 * (C) Copyright 2020 Hewlett Packard Enterprise Development LP
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "config.h"

#include "ofi_iov.h"

#include "mverbs_ofi.h"
#include <malloc.h>

static void mverbs_set_credit_handler(
    struct fid_domain *domain_fid,
    ssize_t (*credit_handler)(struct fid_ep *ep, uint64_t credits))
{
    struct mverbs_domain *domain;

    domain = container_of(domain_fid, struct mverbs_domain,
                          util_domain.domain_fid.fid);
    domain->send_credits = credit_handler;
}

static bool mverbs_flow_ctrl_available(struct fid_ep *ep_fid)
{
    struct mverbs_ep *ep =
        container_of(ep_fid, struct mverbs_ep, util_ep.ep_fid);

    // only enable if we are not using SRQ
    return (!ep->srx && ep->ibv_qp && ep->ibv_qp->qp_type == IBV_QPT_RC);
}

static int mverbs_enable_ep_flow_ctrl(struct fid_ep *ep_fid, uint64_t threshold)
{
    struct mverbs_ep *ep;
    uint64_t credits_to_give;

    if (!mverbs_flow_ctrl_available(ep_fid))
        return -FI_ENOSYS;

    ep = container_of(ep_fid, struct mverbs_ep, util_ep.ep_fid);
    ofi_genlock_lock(&mverbs_ep2_progress(ep)->ep_lock);
    ep->threshold = threshold;

    /*
     * Both sides assume 1 credit to start with. Previously received credits
     * from peer should also be added in.
     */
    ep->peer_rq_credits = 1 + ep->saved_peer_rq_credits;
    ep->saved_peer_rq_credits = 0;

    /*
     * Preposted recvs may happen before flow control is enabled.
     * Send credit update if needed.
     */
    if (ep->rq_credits_avail >= ep->threshold) {
        credits_to_give = ep->rq_credits_avail;
        ep->rq_credits_avail = 0;
    } else {
        credits_to_give = 0;
    }

    if (credits_to_give && mverbs_ep2_domain(ep)->send_credits(
                               &ep->util_ep.ep_fid, credits_to_give)) {
        ep->rq_credits_avail += credits_to_give;
    }
    ofi_genlock_unlock(&mverbs_ep2_progress(ep)->ep_lock);

    return FI_SUCCESS;
}

struct ofi_ops_flow_ctrl mverbs_ops_flow_ctrl = {
    .size = sizeof(struct ofi_ops_flow_ctrl),
    .add_credits = mverbs_add_credits,
    .enable = mverbs_enable_ep_flow_ctrl,
    .set_send_handler = mverbs_set_credit_handler,
    .available = mverbs_flow_ctrl_available,
};

static int mverbs_domain_ops_open(struct fid *fid, const char *name,
                                  uint64_t flags, void **ops, void *context)
{
    if (flags)
        return -FI_EBADFLAGS;

    if (!strcasecmp(name, OFI_OPS_FLOW_CTRL)) {
        *ops = &mverbs_ops_flow_ctrl;
        return 0;
    }

    return -FI_ENOSYS;
}

#if MVERBS_HAVE_QUERY_EX
static int mverbs_odp_flag(struct ibv_context *verbs)
{
    struct ibv_query_device_ex_input input = {0};
    struct ibv_device_attr_ex attr;
    int ret;

    if (!mverbs_gl_data.use_odp)
        return 0;

    ret = ibv_query_device_ex(verbs, &input, &attr);
    if (ret)
        return 0;

    return attr.odp_caps.general_caps & IBV_ODP_SUPPORT ? VRB_USE_ODP : 0;
}
#else
static int mverbs_odp_flag(struct ibv_context *verbs) { return 0; }
#endif /* MVERBS_HAVE_QUERY_EX */

static int mverbs_domain_bind(struct fid *fid, struct fid *bfid, uint64_t flags)
{
    struct mverbs_domain *domain;
    struct mverbs_eq *eq;

    domain =
        container_of(fid, struct mverbs_domain, util_domain.domain_fid.fid);

    switch (bfid->fclass) {
        case FI_CLASS_EQ:
            switch (domain->ep_type) {
                case FI_EP_MSG:
                    eq = container_of(bfid, struct mverbs_eq, eq_fid);
                    domain->eq = eq;
                    domain->eq_flags = flags;
                    break;
                case FI_EP_DGRAM:
                    return -FI_EINVAL;
                default:
                    /* Shouldn't go here */
                    assert(0);
                    return -FI_EINVAL;
            }
            break;

        default:
            return -EINVAL;
    }

    return 0;
}

static int mverbs_domain_close(fid_t fid)
{
    int ret;
    struct mverbs_fabric *fab;
    struct mverbs_domain *domain =
        container_of(fid, struct mverbs_domain, util_domain.domain_fid.fid);

    mverbs_close_progress(&domain->progress);

    switch (domain->ep_type) {
        case FI_EP_DGRAM:
            fab =
                container_of(&domain->util_domain.fabric->fabric_fid,
                             struct mverbs_fabric, util_fabric.fabric_fid.fid);
            /* Even if it's invoked not for the first time
             * (e.g. multiple domains per fabric), it's safe
             */
            if (mverbs_gl_data.dgram.use_name_server)
                ofi_ns_stop_server(&fab->name_server);
            break;
        case FI_EP_MSG:
            if (domain->ext_flags & VRB_USE_XRC) {
                ret = mverbs_domain_xrc_cleanup(domain);
                if (ret)
                    return ret;
            }
            break;
        default:
            /* Never should go here */
            assert(0);
            return -FI_EINVAL;
    }

    ofi_mr_cache_cleanup(&domain->cache);

    if (domain->pd) {
        ret = ibv_dealloc_pd(domain->pd);
        if (ret)
            return -ret;
        domain->pd = NULL;
    }

    ret = ofi_domain_close(&domain->util_domain);
    if (ret)
        return ret;

    fi_freeinfo(domain->info);
    free(domain);
    return 0;
}

static int mverbs_open_device_by_name(struct mverbs_domain *domain,
                                      const char *name)
{
    struct ibv_context **dev_list;
    int i, ret = -FI_ENODEV;

    if (!name)
        return -FI_EINVAL;

    dev_list = rdma_get_devices(NULL);
    if (!dev_list)
        return -errno;

    for (i = 0; dev_list[i] && ret; i++) {
        const char *rdma_name = ibv_get_device_name(dev_list[i]->device);
        switch (domain->ep_type) {
            case FI_EP_MSG:
                ret = domain->ext_flags & VRB_USE_XRC ?
                          mverbs_cmp_xrc_domain_name(name, rdma_name) :
                          strcmp(name, rdma_name);
                break;
            case FI_EP_DGRAM:
                ret =
                    strncmp(name, rdma_name,
                            strlen(name) - strlen(mverbs_dgram_domain.suffix));
                break;
            default:
                VRB_WARN(FI_LOG_DOMAIN, "Unsupported EP type - %d\n",
                         domain->ep_type);
                /* Never should go here */
                assert(0);
                ret = -FI_EINVAL;
                break;
        }

        if (!ret)
            domain->verbs = dev_list[i];
    }
    rdma_free_devices(dev_list);
    return ret;
}

static struct fi_ops mverbs_fid_ops = {
    .size = sizeof(struct fi_ops),
    .close = mverbs_domain_close,
    .bind = mverbs_domain_bind,
    .control = fi_no_control,
    .ops_open = mverbs_domain_ops_open,
};

static struct fi_ops_domain mverbs_msg_domain_ops = {
    .size = sizeof(struct fi_ops_domain),
    .av_open = fi_no_av_open,
    .cq_open = mverbs_cq_open,
    .endpoint = mverbs_open_ep,
    .scalable_ep = fi_no_scalable_ep,
    .cntr_open = fi_no_cntr_open,
    .poll_open = fi_no_poll_open,
    .stx_ctx = fi_no_stx_context,
    .srx_ctx = mverbs_srq_context,
    .query_atomic = mverbs_query_atomic,
    .query_collective = fi_no_query_collective,
};

static struct fi_ops_domain mverbs_dgram_domain_ops = {
    .size = sizeof(struct fi_ops_domain),
    .av_open = mverbs_dgram_av_open,
    .cq_open = mverbs_cq_open,
    .endpoint = mverbs_open_ep,
    .scalable_ep = fi_no_scalable_ep,
    .poll_open = fi_no_poll_open,
    .stx_ctx = fi_no_stx_context,
    .srx_ctx = fi_no_srx_context,
    .query_atomic = fi_no_query_atomic,
    .query_collective = fi_no_query_collective,
};

static int mverbs_domain(struct fid_fabric *fabric, struct fi_info *info,
                         struct fid_domain **domain, void *context)
{
    struct ofi_mem_monitor *memory_monitors[OFI_HMEM_MAX] = {
        [FI_HMEM_SYSTEM] = default_monitor,
        [FI_HMEM_CUDA] = default_cuda_monitor,
        [FI_HMEM_ROCR] = default_rocr_monitor,
        [FI_HMEM_ZE] = default_ze_monitor,
    };
    enum fi_hmem_iface iface;
    struct mverbs_domain *_domain;
    int ret;
    struct mverbs_fabric *fab =
        container_of(fabric, struct mverbs_fabric, util_fabric.fabric_fid);
    const struct fi_info *fi =
        mverbs_get_verbs_info(mverbs_util_prov.info, info->domain_attr->name);
    if (!fi)
        return -FI_EINVAL;

    ret = ofi_check_domain_attr(&mverbs_prov, fabric->api_version,
                                fi->domain_attr, info);
    if (ret)
        return ret;

    _domain = calloc(1, sizeof *_domain);
    if (!_domain)
        return -FI_ENOMEM;

    ret = ofi_domain_init(fabric, info, &_domain->util_domain, context,
                          OFI_LOCK_MUTEX);
    if (ret)
        goto err1;

    _domain->info = fi_dupinfo(info);
    if (!_domain->info)
        goto err2;

    _domain->ep_type = VRB_EP_TYPE(info);
    _domain->ext_flags |= mverbs_is_xrc_info(info) ? VRB_USE_XRC : 0;

    ret = mverbs_open_device_by_name(_domain, info->domain_attr->name);
    if (ret)
        goto err3;

    _domain->pd = ibv_alloc_pd(_domain->verbs);
    if (!_domain->pd) {
        ret = -errno;
        goto err3;
    }

    _domain->ext_flags |= mverbs_odp_flag(_domain->verbs);
    _domain->util_domain.domain_fid.fid.fclass = FI_CLASS_DOMAIN;
    _domain->util_domain.domain_fid.fid.context = context;
    _domain->util_domain.domain_fid.fid.ops = &mverbs_fid_ops;
    _domain->util_domain.domain_fid.mr = &mverbs_mr_ops;

    _domain->cache.entry_data_size = sizeof(struct mverbs_mem_desc);
    _domain->cache.add_region = mverbs_mr_cache_add_region;
    _domain->cache.delete_region = mverbs_mr_cache_delete_region;
    ret = ofi_mr_cache_init(&_domain->util_domain, memory_monitors,
                            &_domain->cache);
    if (ret) {
        VRB_INFO(FI_LOG_MR, "MR cache init failed: %s. MR caching disabled.\n",
                 fi_strerror(-ret));
    } else {
        for (iface = 0; iface < OFI_HMEM_MAX; iface++) {
            if (_domain->cache.monitors[iface])
                VRB_INFO(FI_LOG_MR, "MR cache enabled for %s memory\n",
                         fi_tostr(&iface, FI_TYPE_HMEM_IFACE));
        }
    }

    switch (_domain->ep_type) {
        case FI_EP_DGRAM:
            if (mverbs_gl_data.dgram.use_name_server) {
                /* Even if it's invoked not for the first time
                 * (e.g. multiple domains per fabric), it's safe
                 */
                fab->name_server.port = mverbs_gl_data.dgram.name_server_port;
                fab->name_server.name_len = sizeof(struct ofi_ib_ud_ep_name);
                fab->name_server.service_len = sizeof(int);
                fab->name_server.service_cmp = mverbs_dgram_ns_service_cmp;
                fab->name_server.is_service_wildcard =
                    mverbs_dgram_ns_is_service_wildcard;

                ofi_ns_init(&fab->name_server);
                ofi_ns_start_server(&fab->name_server);
            }
            _domain->util_domain.domain_fid.ops = &mverbs_dgram_domain_ops;
            break;
        case FI_EP_MSG:
            if (_domain->ext_flags & VRB_USE_XRC) {
                ret = mverbs_domain_xrc_init(_domain);
                if (ret)
                    goto err4;
            }
            _domain->util_domain.domain_fid.ops = &mverbs_msg_domain_ops;
            break;
        default:
            VRB_INFO(FI_LOG_DOMAIN,
                     "Ivalid EP type is provided, "
                     "EP type :%d\n",
                     _domain->ep_type);
            ret = -FI_EINVAL;
            goto err4;
    }

    ret = mverbs_init_progress(&_domain->progress, _domain->verbs);
    if (ret)
        goto err4;

    *domain = &_domain->util_domain.domain_fid;
    return FI_SUCCESS;
err4:
    ofi_mr_cache_cleanup(&_domain->cache);
    if (ibv_dealloc_pd(_domain->pd))
        VRB_WARN_ERRNO(FI_LOG_DOMAIN, "ibv_dealloc_pd");
err3:
    fi_freeinfo(_domain->info);
err2:
    if (ofi_domain_close(&_domain->util_domain))
        VRB_WARN(FI_LOG_DOMAIN, "ofi_domain_close fails");
err1:
    free(_domain);
    return ret;
}

static int mverbs_trywait(struct fid_fabric *fabric, struct fid **fids,
                          int count)
{
    struct mverbs_cq *cq;
    struct mverbs_eq *eq;
    int ret, i;

    for (i = 0; i < count; i++) {
        switch (fids[i]->fclass) {
            case FI_CLASS_CQ:
                cq =
                    container_of(fids[i], struct mverbs_cq, util_cq.cq_fid.fid);
                ret = mverbs_cq_trywait(cq);
                if (ret)
                    return ret;
                break;
            case FI_CLASS_EQ:
                eq = container_of(fids[i], struct mverbs_eq, eq_fid.fid);
                ret = mverbs_eq_trywait(eq);
                if (ret)
                    return ret;
                break;
            case FI_CLASS_CNTR:
            case FI_CLASS_WAIT:
                return -FI_ENOSYS;
            default:
                return -FI_EINVAL;
        }
    }
    return FI_SUCCESS;
}

static int mverbs_fabric_close(fid_t fid)
{
    struct mverbs_fabric *fab;
    int ret;

    fab = container_of(fid, struct mverbs_fabric, util_fabric.fabric_fid.fid);
    ret = ofi_fabric_close(&fab->util_fabric);
    if (ret)
        return ret;
    free(fab);

    return 0;
}

static struct fi_ops mverbs_fi_ops = {
    .size = sizeof(struct fi_ops),
    .close = mverbs_fabric_close,
    .bind = fi_no_bind,
    .control = fi_no_control,
    .ops_open = fi_no_ops_open,
};

static struct fi_ops_fabric mverbs_ops_fabric = {
    .size = sizeof(struct fi_ops_fabric),
    .domain = mverbs_domain,
    .passive_ep = mverbs_passive_ep,
    .eq_open = mverbs_eq_open,
    .wait_open = fi_no_wait_open,
    .trywait = mverbs_trywait};

int mverbs_fabric(struct fi_fabric_attr *attr, struct fid_fabric **fabric,
                  void *context)
{
    struct mverbs_fabric *fab;
    const struct fi_info *cur, *info = mverbs_util_prov.info;
    int ret = FI_SUCCESS;

    fab = calloc(1, sizeof(*fab));
    if (!fab)
        return -FI_ENOMEM;

    for (cur = info; cur; cur = info->next) {
        ret = ofi_fabric_init(&mverbs_prov, cur->fabric_attr, attr,
                              &fab->util_fabric, context);
        if (ret != -FI_ENODATA)
            break;
    }
    if (ret) {
        free(fab);
        return ret;
    }

    fab->info = cur;

    *fabric = &fab->util_fabric.fabric_fid;
    (*fabric)->fid.fclass = FI_CLASS_FABRIC;
    (*fabric)->fid.ops = &mverbs_fi_ops;
    (*fabric)->ops = &mverbs_ops_fabric;

    return 0;
}
