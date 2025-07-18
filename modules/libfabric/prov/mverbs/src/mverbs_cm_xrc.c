/*
 * Copyright (c) 2018-2019 Cray Inc. All rights reserved.
 * Copyright (c) 2018-2019 System Fabric Works, Inc. All rights reserved.
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

void mverbs_next_xrc_conn_state(struct mverbs_xrc_ep *ep)
{
    switch (ep->conn_state) {
        case VRB_XRC_UNCONNECTED:
            ep->conn_state = VRB_XRC_ORIG_CONNECTING;
            break;
        case VRB_XRC_ORIG_CONNECTING:
            ep->conn_state = VRB_XRC_ORIG_CONNECTED;
            break;
        case VRB_XRC_ORIG_CONNECTED:
            ep->conn_state = VRB_XRC_RECIP_CONNECTING;
            break;
        case VRB_XRC_RECIP_CONNECTING:
            ep->conn_state = VRB_XRC_CONNECTED;
            break;
        case VRB_XRC_CONNECTED:
        case VRB_XRC_ERROR:
            break;
        default:
            assert(0);
            VRB_WARN(FI_LOG_EP_CTRL, "Unkown XRC connection state %d\n",
                     ep->conn_state);
    }
}

void mverbs_prev_xrc_conn_state(struct mverbs_xrc_ep *ep)
{
    switch (ep->conn_state) {
        case VRB_XRC_UNCONNECTED:
            break;
        case VRB_XRC_ORIG_CONNECTING:
            ep->conn_state = VRB_XRC_UNCONNECTED;
            break;
        case VRB_XRC_ORIG_CONNECTED:
            ep->conn_state = VRB_XRC_ORIG_CONNECTING;
            break;
        case VRB_XRC_RECIP_CONNECTING:
            ep->conn_state = VRB_XRC_ORIG_CONNECTED;
            break;
        case VRB_XRC_CONNECTED:
            ep->conn_state = VRB_XRC_RECIP_CONNECTING;
            break;
        case VRB_XRC_ERROR:
            break;
        default:
            assert(0);
            VRB_WARN(FI_LOG_EP_CTRL, "Unkown XRC connection state %d\n",
                     ep->conn_state);
    }
}

void mverbs_save_priv_data(struct mverbs_xrc_ep *ep, const void *data,
                           size_t len)
{
    ep->conn_setup->event_len = MIN(sizeof(ep->conn_setup->event_data), len);
    memcpy(ep->conn_setup->event_data, data, ep->conn_setup->event_len);
}

void mverbs_set_xrc_cm_data(struct mverbs_xrc_cm_data *local, int reciprocal,
                            uint32_t conn_tag, uint16_t port, uint32_t tgt_qpn,
                            uint32_t srqn)
{
    local->version = VRB_XRC_VERSION;
    local->reciprocal = reciprocal ? 1 : 0;
    local->port = htons(port);
    local->conn_tag = htonl(conn_tag);
    local->tgt_qpn = htonl(tgt_qpn);
    local->srqn = htonl(srqn);
}

int mverbs_verify_xrc_cm_data(struct mverbs_xrc_cm_data *remote,
                              int private_data_len)
{
    if (sizeof(*remote) > private_data_len) {
        VRB_WARN(FI_LOG_EP_CTRL, "XRC MSG EP CM data length mismatch\n");
        return -FI_EINVAL;
    }

    if (remote->version != VRB_XRC_VERSION) {
        VRB_WARN(FI_LOG_EP_CTRL,
                 "XRC MSG EP connection protocol mismatch "
                 "(local %" PRIu8 ", remote %" PRIu8 ")\n",
                 VRB_XRC_VERSION, remote->version);
        return -FI_EINVAL;
    }
    return FI_SUCCESS;
}

static void mverbs_log_ep_conn(struct mverbs_xrc_ep *ep, char *desc)
{
    struct sockaddr *addr;
    char buf[OFI_ADDRSTRLEN];
    size_t len;

    if (!fi_log_enabled(&mverbs_prov, FI_LOG_INFO, FI_LOG_EP_CTRL))
        return;

    VRB_INFO(FI_LOG_EP_CTRL, "EP %p, %s\n", (void *)ep, desc);
    VRB_INFO(FI_LOG_EP_CTRL,
             "EP %p, CM ID %p, TGT CM ID %p, SRQN %d Peer SRQN %d\n",
             (void *)ep, (void *)ep->base_ep.id, (void *)ep->tgt_id, ep->srqn,
             ep->peer_srqn);

    if (ep->base_ep.id) {
        addr = rdma_get_local_addr(ep->base_ep.id);
        len = sizeof(buf);
        ofi_straddr(buf, &len, ep->base_ep.info_attr.addr_format, addr);
        VRB_INFO(FI_LOG_EP_CTRL, "EP %p src_addr: %s\n", (void *)ep, buf);

        addr = rdma_get_peer_addr(ep->base_ep.id);
        len = sizeof(buf);
        ofi_straddr(buf, &len, ep->base_ep.info_attr.addr_format, addr);
        VRB_INFO(FI_LOG_EP_CTRL, "EP %p dst_addr: %s\n", (void *)ep, buf);
    }

    if (ep->base_ep.ibv_qp) {
        VRB_INFO(FI_LOG_EP_CTRL, "EP %p, INI QP Num %d\n", (void *)ep,
                 ep->base_ep.ibv_qp->qp_num);
        VRB_INFO(FI_LOG_EP_CTRL, "EP %p, Remote TGT QP Num %d\n", (void *)ep,
                 ep->ini_conn->tgt_qpn);
    }
    if (ep->tgt_ibv_qp)
        VRB_INFO(FI_LOG_EP_CTRL, "EP %p, TGT QP Num %d\n", (void *)ep,
                 ep->tgt_ibv_qp->qp_num);
}

void mverbs_free_xrc_conn_setup(struct mverbs_xrc_ep *ep, int disconnect)
{
    assert(ofi_mutex_held(&ep->base_ep.eq->event_lock));
    assert(ep->conn_setup);

    /* If a disconnect is requested then the XRC bidirectional connection
     * has completed and a disconnect sequence is started (the XRC INI QP
     * side disconnect is initiated when the remote target disconnect is
     * received). */
    if (disconnect) {
        assert(ep->tgt_id);
        assert(!ep->tgt_id->qp);

        if (ep->tgt_id->ps == RDMA_PS_UDP) {
            rdma_destroy_id(ep->tgt_id);
            ep->tgt_id = NULL;
        } else {
            rdma_disconnect(ep->tgt_id);
        }

        if (ep->base_ep.id->ps == RDMA_PS_UDP) {
            rdma_destroy_id(ep->base_ep.id);
            ep->base_ep.id = NULL;
        }
    }

    mverbs_eq_clear_xrc_conn_tag(ep);
    if (!disconnect) {
        free(ep->conn_setup);
        ep->conn_setup = NULL;
        free(ep->base_ep.info_attr.src_addr);
        ep->base_ep.info_attr.src_addr = NULL;
        ep->base_ep.info_attr.src_addrlen = 0;
    }
}

int mverbs_connect_xrc(struct mverbs_xrc_ep *ep, struct sockaddr *addr,
                       int reciprocal, void *param, size_t paramlen)
{
    struct mverbs_domain *domain = mverbs_ep2_domain(&ep->base_ep);
    int ret;

    assert(ofi_mutex_held(&ep->base_ep.eq->event_lock));
    assert(!ep->base_ep.id && !ep->base_ep.ibv_qp && !ep->ini_conn);

    domain->xrc.lock_acquire(&domain->xrc.ini_lock);
    ret = mverbs_get_shared_ini_conn(ep, &ep->ini_conn);
    if (ret) {
        VRB_WARN(FI_LOG_EP_CTRL, "Get of shared XRC INI connection failed %d\n",
                 ret);
        if (!reciprocal) {
            free(ep->conn_setup);
            ep->conn_setup = NULL;
        }
        domain->xrc.lock_release(&domain->xrc.ini_lock);
        return ret;
    }

    mverbs_eq_set_xrc_conn_tag(ep);
    mverbs_add_pending_ini_conn(ep, reciprocal, param, paramlen);
    mverbs_sched_ini_conn(ep->ini_conn);
    domain->xrc.lock_release(&domain->xrc.ini_lock);

    return FI_SUCCESS;
}

void mverbs_ep_ini_conn_done(struct mverbs_xrc_ep *ep, uint32_t tgt_qpn)
{
    struct mverbs_domain *domain = mverbs_ep2_domain(&ep->base_ep);

    assert(ofi_mutex_held(&ep->base_ep.eq->event_lock));
    assert(ep->base_ep.id && ep->ini_conn);

    domain->xrc.lock_acquire(&domain->xrc.ini_lock);
    assert(ep->ini_conn->state == VRB_INI_QP_CONNECTING ||
           ep->ini_conn->state == VRB_INI_QP_CONNECTED);

    /* If this was a physical INI/TGT QP connection, remove the QP
     * from control of the RDMA CM. We don't want the shared INI QP
     * to be destroyed if this endpoint closes. */
    if (ep->base_ep.id == ep->ini_conn->phys_conn_id) {
        ep->ini_conn->phys_conn_id = NULL;
        ep->ini_conn->state = VRB_INI_QP_CONNECTED;
        ep->ini_conn->tgt_qpn = tgt_qpn;
        ep->base_ep.id->qp = NULL;
        VRB_DBG(FI_LOG_EP_CTRL, "Set INI Conn QP %d remote TGT QP %d\n",
                ep->ini_conn->ini_qp->qp_num, ep->ini_conn->tgt_qpn);
    }

    mverbs_log_ep_conn(ep, "INI Connection Done");
    mverbs_sched_ini_conn(ep->ini_conn);
    domain->xrc.lock_release(&domain->xrc.ini_lock);
}

void mverbs_ep_ini_conn_rejected(struct mverbs_xrc_ep *ep)
{
    assert(ofi_mutex_held(&ep->base_ep.eq->event_lock));
    assert(ep->base_ep.id && ep->ini_conn);

    mverbs_log_ep_conn(ep, "INI Connection Rejected");
    mverbs_put_shared_ini_conn(ep);
    ep->conn_state = VRB_XRC_ERROR;
}

void mverbs_ep_tgt_conn_done(struct mverbs_xrc_ep *ep)
{
    mverbs_log_ep_conn(ep, "TGT Connection Done\n");

    if (ep->tgt_id->qp) {
        assert(ep->tgt_ibv_qp == ep->tgt_id->qp);
        ep->tgt_id->qp = NULL;
    }
}

int mverbs_resend_shared_accept_xrc(struct mverbs_xrc_ep *ep,
                                    struct mverbs_connreq *connreq,
                                    struct rdma_cm_id *id)
{
    struct rdma_conn_param conn_param = {0};
    struct mverbs_xrc_cm_data *cm_data = ep->accept_param_data;

    assert(ofi_mutex_held(&ep->base_ep.eq->event_lock));
    assert(cm_data && ep->tgt_ibv_qp);
    assert(ep->tgt_ibv_qp->qp_num == connreq->xrc.tgt_qpn);
    assert(ep->peer_srqn == connreq->xrc.peer_srqn);

    mverbs_set_xrc_cm_data(cm_data, connreq->xrc.is_reciprocal,
                           connreq->xrc.conn_tag, connreq->xrc.port, 0,
                           ep->srqn);
    conn_param.private_data = cm_data;
    conn_param.private_data_len = ep->accept_param_len;

    conn_param.responder_resources = RDMA_MAX_RESP_RES;
    conn_param.initiator_depth = RDMA_MAX_INIT_DEPTH;
    conn_param.flow_control = 1;
    conn_param.rnr_retry_count = 7;
    if (ep->base_ep.srx)
        conn_param.srq = 1;
    conn_param.qp_num = ep->tgt_ibv_qp->qp_num;

    return rdma_accept(id, &conn_param);
}

int mverbs_accept_xrc(struct mverbs_xrc_ep *ep, int reciprocal, void *param,
                      size_t paramlen)
{
    struct sockaddr *addr;
    struct mverbs_connreq *connreq;
    struct rdma_conn_param conn_param = {0};
    struct mverbs_xrc_cm_data *cm_data = param;
    struct mverbs_xrc_cm_data connect_cm_data = {0};
    int ret;

    assert(ofi_mutex_held(&ep->base_ep.eq->event_lock));
    addr = rdma_get_local_addr(ep->tgt_id);
    if (addr)
        ofi_straddr_dbg(&mverbs_prov, FI_LOG_CORE, "src_addr", addr);

    addr = rdma_get_peer_addr(ep->tgt_id);
    if (addr)
        ofi_straddr_dbg(&mverbs_prov, FI_LOG_CORE, "dest_addr", addr);

    connreq = container_of(ep->base_ep.info_attr.handle, struct mverbs_connreq,
                           handle);
    ret = mverbs_ep_create_tgt_qp(ep, connreq->xrc.tgt_qpn);
    if (ret)
        return ret;

    ep->peer_srqn = connreq->xrc.peer_srqn;
    ep->remote_pep_port = connreq->xrc.port;
    ep->recip_accept = connreq->xrc.is_reciprocal;
    mverbs_set_xrc_cm_data(cm_data, connreq->xrc.is_reciprocal,
                           connreq->xrc.conn_tag, connreq->xrc.port, 0,
                           ep->srqn);
    conn_param.private_data = cm_data;
    conn_param.private_data_len = paramlen;
    conn_param.responder_resources = RDMA_MAX_RESP_RES;
    conn_param.initiator_depth = RDMA_MAX_INIT_DEPTH;
    conn_param.flow_control = 1;
    conn_param.rnr_retry_count = 7;
    if (ep->base_ep.srx)
        conn_param.srq = 1;

    if (!ep->tgt_id->qp)
        conn_param.qp_num = ep->tgt_ibv_qp->qp_num;

    ep->conn_setup->remote_conn_tag = connreq->xrc.conn_tag;

    assert(ep->conn_state == VRB_XRC_UNCONNECTED ||
           ep->conn_state == VRB_XRC_ORIG_CONNECTED);
    mverbs_next_xrc_conn_state(ep);

    ret = rdma_accept(ep->tgt_id, &conn_param);
    if (OFI_UNLIKELY(ret)) {
        ret = -errno;
        VRB_WARN(FI_LOG_EP_CTRL, "XRC TGT, rdma_accept error %d\n", ret);
        mverbs_prev_xrc_conn_state(ep);
        return ret;
    }
    free(connreq);

    if (ep->tgt_id->ps == RDMA_PS_UDP &&
        mverbs_eq_add_sidr_conn(ep, cm_data, paramlen))
        VRB_WARN(FI_LOG_EP_CTRL, "SIDR connection accept not added to map\n");

    /* The passive side of the initial shared connection using
     * SIDR is complete, initiate reciprocal connection */
    if (ep->tgt_id->ps == RDMA_PS_UDP && !reciprocal) {
        mverbs_next_xrc_conn_state(ep);
        mverbs_ep_tgt_conn_done(ep);
        ret = mverbs_connect_xrc(ep, NULL, VRB_RECIP_CONN, &connect_cm_data,
                                 sizeof(connect_cm_data));
        if (ret) {
            VRB_WARN(FI_LOG_EP_CTRL, "XRC reciprocal connect error %d\n", ret);
            mverbs_prev_xrc_conn_state(ep);
            ep->tgt_id->qp = NULL;
        }
    }

    return ret;
}

int mverbs_process_xrc_connreq(struct mverbs_ep *ep,
                               struct mverbs_connreq *connreq)
{
    struct mverbs_xrc_ep *xrc_ep =
        container_of(ep, struct mverbs_xrc_ep, base_ep);

    assert(ep->info_attr.src_addr);
    assert(ep->info_attr.dest_addr);

    xrc_ep->conn_setup = calloc(1, sizeof(*xrc_ep->conn_setup));
    if (!xrc_ep->conn_setup) {
        VRB_WARN(FI_LOG_EP_CTRL,
                 "Unable to allocate connection setup memory\n");
        return -FI_ENOMEM;
    }
    xrc_ep->conn_setup->conn_tag = MVERBS_CONN_TAG_INVALID;

    /* This endpoint was created on the passive side of a connection
     * request. The reciprocal connection request will go back to the
     * passive port indicated by the active side */
    ofi_addr_set_port(ep->info_attr.src_addr, 0);
    ofi_addr_set_port(ep->info_attr.dest_addr, connreq->xrc.port);
    xrc_ep->tgt_id = connreq->id;
    xrc_ep->tgt_id->context = &ep->util_ep.ep_fid.fid;

    return FI_SUCCESS;
}
