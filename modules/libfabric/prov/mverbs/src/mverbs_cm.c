/*
 * Copyright (c) Intel Corporation, Inc.  All rights reserved.
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

#include "mverbs_ofi.h"

static int mverbs_copy_addr(void *dst_addr, size_t *dst_addrlen, void *src_addr)
{
    size_t src_addrlen = ofi_sizeofaddr(src_addr);

    if (*dst_addrlen == 0) {
        *dst_addrlen = src_addrlen;
        return -FI_ETOOSMALL;
    }

    if (*dst_addrlen < src_addrlen) {
        memcpy(dst_addr, src_addr, *dst_addrlen);
    } else {
        memcpy(dst_addr, src_addr, src_addrlen);
    }
    *dst_addrlen = src_addrlen;
    return 0;
}

static int mverbs_msg_ep_setname(fid_t ep_fid, void *addr, size_t addrlen)
{
    void *save_addr;
    struct rdma_cm_id *id;
    int ret;
    struct mverbs_ep *ep =
        container_of(ep_fid, struct mverbs_ep, util_ep.ep_fid);

    if (addrlen != ep->info_attr.src_addrlen) {
        VRB_INFO(FI_LOG_EP_CTRL, "addrlen expected: %zu, got: %zu.\n",
                 ep->info_attr.src_addrlen, addrlen);
        return -FI_EINVAL;
    }

    save_addr = ep->info_attr.src_addr;

    ep->info_attr.src_addr = malloc(ep->info_attr.src_addrlen);
    if (!ep->info_attr.src_addr) {
        VRB_WARN(FI_LOG_EP_CTRL, "memory allocation failure\n");
        ret = -FI_ENOMEM;
        goto err1;
    }

    memcpy(ep->info_attr.src_addr, addr, ep->info_attr.src_addrlen);

    ret = mverbs_create_ep(ep, RDMA_PS_TCP, &id);
    if (ret)
        goto err2;

    if (ep->id)
        rdma_destroy_ep(ep->id);

    ep->id = id;
    ep->ibv_qp = ep->id->qp;
    free(save_addr);

    return 0;
err2:
    free(ep->info_attr.src_addr);
err1:
    ep->info_attr.src_addr = save_addr;
    return ret;
}

static int mverbs_msg_ep_getname(fid_t ep, void *addr, size_t *addrlen)
{
    struct sockaddr *sa;
    struct mverbs_ep *_ep = container_of(ep, struct mverbs_ep, util_ep.ep_fid);
    sa = rdma_get_local_addr(_ep->id);
    return mverbs_copy_addr(addr, addrlen, sa);
}

static int mverbs_msg_ep_getpeer(struct fid_ep *ep, void *addr, size_t *addrlen)
{
    struct sockaddr *sa;
    struct mverbs_ep *_ep = container_of(ep, struct mverbs_ep, util_ep.ep_fid);
    sa = rdma_get_peer_addr(_ep->id);
    return mverbs_copy_addr(addr, addrlen, sa);
}

static inline void mverbs_msg_ep_prepare_cm_data(
    const void *param, size_t param_size, struct mverbs_cm_data_hdr *cm_hdr)
{
    cm_hdr->size = (uint8_t)param_size;
    memcpy(cm_hdr->data, param, cm_hdr->size);
}

static inline void mverbs_ep_prepare_rdma_cm_param(
    struct rdma_conn_param *conn_param, void *priv_data, size_t priv_data_size)
{
    conn_param->private_data = priv_data;
    conn_param->private_data_len = (uint8_t)priv_data_size;
    conn_param->responder_resources = RDMA_MAX_RESP_RES;
    conn_param->initiator_depth = RDMA_MAX_INIT_DEPTH;
    conn_param->flow_control = 1;
    conn_param->rnr_retry_count = 7;
}

static void mverbs_msg_ep_prepare_rdma_cm_hdr(void *priv_data,
                                              const struct rdma_cm_id *id)
{
    struct mverbs_rdma_cm_hdr *rdma_cm_hdr = priv_data;

    /* ip_version=6 would requires IPoIB to be installed and the IP link
     * to be UP, which we don't want. As a work-around, we set ip_version to 0,
     * which let the CMA kernel code to skip any requirement for IPoIB. */
    rdma_cm_hdr->ip_version = 0;
    rdma_cm_hdr->port = htons(ofi_addr_get_port(&id->route.addr.src_addr));

    /* Record the GIDs */
    memcpy(rdma_cm_hdr->src_addr,
           &((struct ofi_sockaddr_ib *)&id->route.addr.src_addr)->sib_addr, 16);
    memcpy(rdma_cm_hdr->dst_addr,
           &((struct ofi_sockaddr_ib *)&id->route.addr.dst_addr)->sib_addr, 16);
}

static int mverbs_msg_ep_connect(struct fid_ep *ep_fid, const void *addr,
                                 const void *param, size_t paramlen)
{
    struct mverbs_ep *ep =
        container_of(ep_fid, struct mverbs_ep, util_ep.ep_fid);
    size_t priv_data_len;
    struct mverbs_cm_data_hdr *cm_hdr;
    off_t rdma_cm_hdr_len = 0;
    int ret;

    if (OFI_UNLIKELY(paramlen > MVERBS_CM_DATA_SIZE))
        return -FI_EINVAL;

    if (!ep->id->qp) {
        ret = fi_control(&ep_fid->fid, FI_ENABLE, NULL);
        if (ret) {
            VRB_WARN_ERR(FI_LOG_EP_CTRL, "fi_control", ret);
            return ret;
        }
    }

    if (ep->id->route.addr.src_addr.sa_family == AF_IB)
        rdma_cm_hdr_len = sizeof(struct mverbs_rdma_cm_hdr);

    priv_data_len = sizeof(*cm_hdr) + paramlen + rdma_cm_hdr_len;
    ep->cm_priv_data = malloc(priv_data_len);
    if (!ep->cm_priv_data)
        return -FI_ENOMEM;

    if (rdma_cm_hdr_len)
        mverbs_msg_ep_prepare_rdma_cm_hdr(ep->cm_priv_data, ep->id);

    cm_hdr = (void *)((char *)ep->cm_priv_data + rdma_cm_hdr_len);
    mverbs_msg_ep_prepare_cm_data(param, paramlen, cm_hdr);
    mverbs_ep_prepare_rdma_cm_param(&ep->conn_param, ep->cm_priv_data,
                                    priv_data_len);
    ep->conn_param.retry_count = 15;

    if (ep->srx)
        ep->conn_param.srq = 1;

    ofi_genlock_lock(&mverbs_ep2_progress(ep)->ep_lock);
    assert(ep->state == VRB_IDLE);
    ep->state = VRB_RESOLVE_ROUTE;
    ret = rdma_resolve_route(ep->id, MVERBS_RESOLVE_TIMEOUT);
    if (ret) {
        ret = -errno;
        VRB_WARN_ERRNO(FI_LOG_EP_CTRL, "rdma_resolve_route");
        free(ep->cm_priv_data);
        ep->cm_priv_data = NULL;
        ep->state = VRB_IDLE;
    }
    ofi_genlock_unlock(&mverbs_ep2_progress(ep)->ep_lock);
    return ret;
}

static int mverbs_msg_ep_accept(struct fid_ep *ep, const void *param,
                                size_t paramlen)
{
    struct rdma_conn_param conn_param;
    struct mverbs_connreq *connreq;
    int ret;
    struct mverbs_cm_data_hdr *cm_hdr;
    struct mverbs_ep *_ep = container_of(ep, struct mverbs_ep, util_ep.ep_fid);

    if (OFI_UNLIKELY(paramlen > MVERBS_CM_DATA_SIZE))
        return -FI_EINVAL;

    if (!_ep->id->qp) {
        ret = fi_control(&ep->fid, FI_ENABLE, NULL);
        if (ret) {
            VRB_WARN_ERR(FI_LOG_EP_CTRL, "fi_control", ret);
            return ret;
        }
    }

    cm_hdr = alloca(sizeof(*cm_hdr) + paramlen);
    mverbs_msg_ep_prepare_cm_data(param, paramlen, cm_hdr);
    mverbs_ep_prepare_rdma_cm_param(&conn_param, cm_hdr,
                                    sizeof(*cm_hdr) + paramlen);

    if (_ep->srx)
        conn_param.srq = 1;

    ofi_genlock_lock(&mverbs_ep2_progress(_ep)->ep_lock);
    assert(_ep->state == VRB_REQ_RCVD);
    _ep->state = VRB_ACCEPTING;
    ret = rdma_accept(_ep->id, &conn_param);
    if (ret) {
        VRB_WARN_ERRNO(FI_LOG_EP_CTRL, "rdma_accept");
        _ep->state = VRB_DISCONNECTED;
        ret = -errno;
    } else {
        connreq =
            container_of(_ep->info_attr.handle, struct mverbs_connreq, handle);
        free(connreq);
    }
    ofi_genlock_unlock(&mverbs_ep2_progress(_ep)->ep_lock);
    return ret;
}

static int mverbs_msg_alloc_xrc_params(void **adjusted_param, const void *param,
                                       size_t *paramlen)
{
    struct mverbs_xrc_cm_data *cm_data;
    size_t cm_datalen = sizeof(*cm_data) + *paramlen;

    *adjusted_param = NULL;

    if (cm_datalen > VRB_CM_DATA_SIZE) {
        VRB_WARN(FI_LOG_EP_CTRL, "XRC CM data overflow %zu\n", cm_datalen);
        return -FI_EINVAL;
    }

    cm_data = malloc(cm_datalen);
    if (!cm_data) {
        VRB_WARN(FI_LOG_EP_CTRL, "Unable to allocate XRC CM data\n");
        return -FI_ENOMEM;
    }

    if (*paramlen)
        memcpy((cm_data + 1), param, *paramlen);

    *paramlen = cm_datalen;
    *adjusted_param = cm_data;
    return FI_SUCCESS;
}

static int mverbs_msg_xrc_ep_reject(struct mverbs_connreq *connreq,
                                    const void *param, size_t paramlen)
{
    struct mverbs_xrc_cm_data *cm_data;
    int ret;

    ret = mverbs_msg_alloc_xrc_params((void **)&cm_data, param, &paramlen);
    if (ret)
        return ret;

    mverbs_set_xrc_cm_data(cm_data, connreq->xrc.is_reciprocal,
                           connreq->xrc.conn_tag, connreq->xrc.port, 0, 0);
    ret = rdma_reject(connreq->id, cm_data, (uint8_t)paramlen) ? -errno : 0;
    free(cm_data);
    return ret;
}

static int mverbs_msg_ep_reject(struct fid_pep *pep, fid_t handle,
                                const void *param, size_t paramlen)
{
    struct mverbs_connreq *connreq =
        container_of(handle, struct mverbs_connreq, handle);
    struct mverbs_cm_data_hdr *cm_hdr;
    struct mverbs_pep *_pep = container_of(pep, struct mverbs_pep, pep_fid);
    int ret;

    if (OFI_UNLIKELY(paramlen > MVERBS_CM_DATA_SIZE))
        return -FI_EINVAL;

    cm_hdr = alloca(sizeof(*cm_hdr) + paramlen);
    mverbs_msg_ep_prepare_cm_data(param, paramlen, cm_hdr);

    ofi_mutex_lock(&_pep->eq->event_lock);
    if (connreq->is_xrc) {
        ret = mverbs_msg_xrc_ep_reject(connreq, cm_hdr,
                                       (uint8_t)(sizeof(*cm_hdr) + paramlen));
    } else if (connreq->id) {
        ret = rdma_reject(connreq->id, cm_hdr,
                          (uint8_t)(sizeof(*cm_hdr) + paramlen)) ?
                  -errno :
                  0;
    } else {
        ret = -FI_EBUSY;
    }
    ofi_mutex_unlock(&_pep->eq->event_lock);

    if (ret)
        VRB_WARN_ERR(FI_LOG_EP_CTRL, "rdma_reject", ret);

    free(connreq);
    return ret;
}

static int mverbs_msg_ep_shutdown(struct fid_ep *ep, uint64_t flags)
{
    struct mverbs_ep *_ep = container_of(ep, struct mverbs_ep, util_ep.ep_fid);

    if (_ep->id)
        return rdma_disconnect(_ep->id) ? -errno : 0;
    return 0;
}

struct fi_ops_cm mverbs_msg_ep_cm_ops = {
    .size = sizeof(struct fi_ops_cm),
    .setname = mverbs_msg_ep_setname,
    .getname = mverbs_msg_ep_getname,
    .getpeer = mverbs_msg_ep_getpeer,
    .connect = mverbs_msg_ep_connect,
    .listen = fi_no_listen,
    .accept = mverbs_msg_ep_accept,
    .reject = fi_no_reject,
    .shutdown = mverbs_msg_ep_shutdown,
    .join = fi_no_join,
};

static int mverbs_msg_xrc_cm_common_verify(struct mverbs_xrc_ep *ep,
                                           size_t paramlen)
{
    int ret;

    if (!mverbs_is_xrc_ep(&ep->base_ep)) {
        VRB_WARN(FI_LOG_EP_CTRL, "EP is not using XRC\n");
        return -FI_EINVAL;
    }

    if (!ep->srqn) {
        ret = fi_control(&ep->base_ep.util_ep.ep_fid.fid, FI_ENABLE, NULL);
        if (ret)
            return ret;
    }

    if (OFI_UNLIKELY(paramlen >
                     MVERBS_CM_DATA_SIZE - sizeof(struct mverbs_xrc_cm_data)))
        return -FI_EINVAL;

    return FI_SUCCESS;
}

static int mverbs_msg_xrc_ep_connect(struct fid_ep *ep, const void *addr,
                                     const void *param, size_t paramlen)
{
    void *adjusted_param;
    struct mverbs_ep *_ep = container_of(ep, struct mverbs_ep, util_ep.ep_fid);
    struct mverbs_xrc_ep *xrc_ep =
        container_of(_ep, struct mverbs_xrc_ep, base_ep);
    int ret;
    struct mverbs_cm_data_hdr *cm_hdr;

    ret = mverbs_msg_xrc_cm_common_verify(xrc_ep, paramlen);
    if (ret)
        return ret;

    cm_hdr = malloc(sizeof(*cm_hdr) + paramlen);
    if (!cm_hdr)
        return -FI_ENOMEM;

    mverbs_msg_ep_prepare_cm_data(param, paramlen, cm_hdr);
    paramlen += sizeof(*cm_hdr);

    ret = mverbs_msg_alloc_xrc_params(&adjusted_param, cm_hdr, &paramlen);
    if (ret) {
        free(cm_hdr);
        return ret;
    }

    xrc_ep->conn_setup = calloc(1, sizeof(*xrc_ep->conn_setup));
    if (!xrc_ep->conn_setup) {
        VRB_WARN(FI_LOG_EP_CTRL,
                 "Unable to allocate connection setup memory\n");
        free(adjusted_param);
        free(cm_hdr);
        return -FI_ENOMEM;
    }
    xrc_ep->conn_setup->conn_tag = MVERBS_CONN_TAG_INVALID;

    ofi_mutex_lock(&xrc_ep->base_ep.eq->event_lock);
    ret = mverbs_connect_xrc(xrc_ep, NULL, 0, adjusted_param, paramlen);
    ofi_mutex_unlock(&xrc_ep->base_ep.eq->event_lock);

    free(adjusted_param);
    free(cm_hdr);
    return ret;
}

static int mverbs_msg_xrc_ep_accept(struct fid_ep *ep, const void *param,
                                    size_t paramlen)
{
    void *adjusted_param;
    struct mverbs_ep *_ep = container_of(ep, struct mverbs_ep, util_ep.ep_fid);
    struct mverbs_xrc_ep *xrc_ep =
        container_of(_ep, struct mverbs_xrc_ep, base_ep);
    int ret;
    struct mverbs_cm_data_hdr *cm_hdr;

    ret = mverbs_msg_xrc_cm_common_verify(xrc_ep, paramlen);
    if (ret)
        return ret;

    cm_hdr = alloca(sizeof(*cm_hdr) + paramlen);
    mverbs_msg_ep_prepare_cm_data(param, paramlen, cm_hdr);
    paramlen += sizeof(*cm_hdr);

    ret = mverbs_msg_alloc_xrc_params(&adjusted_param, cm_hdr, &paramlen);
    if (ret)
        return ret;

    ofi_mutex_lock(&xrc_ep->base_ep.eq->event_lock);
    ret = mverbs_accept_xrc(xrc_ep, 0, adjusted_param, paramlen);
    ofi_mutex_unlock(&xrc_ep->base_ep.eq->event_lock);

    free(adjusted_param);
    return ret;
}

struct fi_ops_cm mverbs_msg_xrc_ep_cm_ops = {
    .size = sizeof(struct fi_ops_cm),
    .setname = mverbs_msg_ep_setname,
    .getname = mverbs_msg_ep_getname,
    .getpeer = mverbs_msg_ep_getpeer,
    .connect = mverbs_msg_xrc_ep_connect,
    .listen = fi_no_listen,
    .accept = mverbs_msg_xrc_ep_accept,
    .reject = fi_no_reject,
    .shutdown = mverbs_msg_ep_shutdown,
    .join = fi_no_join,
};

static int mverbs_pep_setname(fid_t pep_fid, void *addr, size_t addrlen)
{
    struct mverbs_pep *pep;
    int ret;

    pep = container_of(pep_fid, struct mverbs_pep, pep_fid);

    if (pep->src_addrlen && (addrlen != pep->src_addrlen)) {
        VRB_INFO(FI_LOG_FABRIC, "addrlen expected: %zu, got: %zu.\n",
                 pep->src_addrlen, addrlen);
        return -FI_EINVAL;
    }

    /* Re-create id if already bound */
    if (pep->bound) {
        ret = rdma_destroy_id(pep->id);
        if (ret) {
            VRB_WARN_ERRNO(FI_LOG_FABRIC, "rdma_destroy_id");
            return -errno;
        }
        ret = rdma_create_id(NULL, &pep->id, &pep->pep_fid.fid, RDMA_PS_TCP);
        if (ret) {
            VRB_WARN_ERRNO(FI_LOG_FABRIC, "rdma_cm_id\n");
            return -errno;
        }
    }

    ret = rdma_bind_addr(pep->id, (struct sockaddr *)addr);
    if (ret) {
        VRB_WARN_ERRNO(FI_LOG_FABRIC, "rdma_bind_addr");
        return -errno;
    }

    return 0;
}

static int mverbs_pep_getname(fid_t pep, void *addr, size_t *addrlen)
{
    struct mverbs_pep *_pep;
    struct sockaddr *sa;

    _pep = container_of(pep, struct mverbs_pep, pep_fid);
    sa = rdma_get_local_addr(_pep->id);
    return mverbs_copy_addr(addr, addrlen, sa);
}

static int mverbs_pep_listen(struct fid_pep *pep_fid)
{
    struct mverbs_pep *pep;
    struct sockaddr *addr;
    int ret;

    pep = container_of(pep_fid, struct mverbs_pep, pep_fid);

    addr = rdma_get_local_addr(pep->id);
    ofi_straddr_log(&mverbs_prov, FI_LOG_INFO, FI_LOG_EP_CTRL, "listening on",
                    addr);

    ret = rdma_listen(pep->id, pep->backlog);
    if (ret)
        return -errno;

    if (mverbs_is_xrc_info(pep->info)) {
        ret = rdma_listen(pep->xrc_ps_udp_id, pep->backlog);
        if (ret)
            ret = -errno;
    }
    return ret;
}

static struct fi_ops_cm mverbs_pep_cm_ops = {
    .size = sizeof(struct fi_ops_cm),
    .setname = mverbs_pep_setname,
    .getname = mverbs_pep_getname,
    .getpeer = fi_no_getpeer,
    .connect = fi_no_connect,
    .listen = mverbs_pep_listen,
    .accept = fi_no_accept,
    .reject = mverbs_msg_ep_reject,
    .shutdown = fi_no_shutdown,
    .join = fi_no_join,
};

struct fi_ops_cm *mverbs_pep_ops_cm(struct mverbs_pep *pep)
{
    return &mverbs_pep_cm_ops;
}
