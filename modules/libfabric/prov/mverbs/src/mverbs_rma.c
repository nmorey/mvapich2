/*
 * Copyright (c) 2013-2018 Intel Corporation, Inc.  All rights reserved.
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

#include "mverbs_ofi.h"

#define MVERBS_COMP_READ_FLAGS(ep, flags, context)                             \
    (((ep)->util_ep.tx_op_flags | (flags)) &                                   \
             (FI_COMPLETION | FI_TRANSMIT_COMPLETE | FI_DELIVERY_COMPLETE) ?   \
         context :                                                             \
         MVERBS_NO_COMP_FLAG)

#define MVERBS_COMP_READ(ep, context) MVERBS_COMP_READ_FLAGS(ep, 0, context)

static ssize_t mverbs_msg_ep_rma_write(struct fid_ep *ep_fid, const void *buf,
                                       size_t len, void *desc,
                                       fi_addr_t dest_addr, uint64_t addr,
                                       uint64_t key, void *context)
{
    struct mverbs_ep *ep =
        container_of(ep_fid, struct mverbs_ep, util_ep.ep_fid);
    struct ibv_send_wr wr = {
        .wr_id = MVERBS_COMP(ep, (uintptr_t)context),
        .opcode = IBV_WR_RDMA_WRITE,
        .wr.rdma.remote_addr = addr,
        .wr.rdma.rkey = (uint32_t)key,
        .send_flags = MVERBS_INJECT(ep, len, desc),
    };

    return mverbs_send_buf(ep, &wr, buf, len, desc);
}

static ssize_t mverbs_msg_ep_rma_writev(struct fid_ep *ep_fid,
                                        const struct iovec *iov, void **desc,
                                        size_t count, fi_addr_t dest_addr,
                                        uint64_t addr, uint64_t key,
                                        void *context)
{
    struct mverbs_ep *ep =
        container_of(ep_fid, struct mverbs_ep, util_ep.ep_fid);
    struct ibv_send_wr wr = {
        .wr_id = (uintptr_t)context,
        .opcode = IBV_WR_RDMA_WRITE,
        .wr.rdma.remote_addr = addr,
        .wr.rdma.rkey = (uint32_t)key,
    };

    return mverbs_send_iov(ep, &wr, iov, desc, count, ep->util_ep.tx_op_flags);
}

static ssize_t mverbs_msg_ep_rma_writemsg(struct fid_ep *ep_fid,
                                          const struct fi_msg_rma *msg,
                                          uint64_t flags)
{
    struct mverbs_ep *ep =
        container_of(ep_fid, struct mverbs_ep, util_ep.ep_fid);
    struct ibv_send_wr wr = {
        .wr_id = (uintptr_t)msg->context,
        .wr.rdma.remote_addr = msg->rma_iov->addr,
        .wr.rdma.rkey = (uint32_t)msg->rma_iov->key,
    };

    assert(ofi_total_iov_len(msg->msg_iov, msg->iov_count) ==
           ofi_total_rma_iov_len(msg->rma_iov, msg->rma_iov_count));

    if (flags & FI_REMOTE_CQ_DATA) {
        wr.opcode = IBV_WR_RDMA_WRITE_WITH_IMM;
        wr.imm_data = htonl((uint32_t)msg->data);
    } else {
        wr.opcode = IBV_WR_RDMA_WRITE;
    }

    return mverbs_send_iov(ep, &wr, msg->msg_iov, msg->desc, msg->iov_count,
                           flags);
}

static ssize_t mverbs_msg_ep_rma_read(struct fid_ep *ep_fid, void *buf,
                                      size_t len, void *desc,
                                      fi_addr_t src_addr, uint64_t addr,
                                      uint64_t key, void *context)
{
    struct mverbs_ep *ep =
        container_of(ep_fid, struct mverbs_ep, util_ep.ep_fid);
    struct ibv_send_wr wr = {
        .wr_id = MVERBS_COMP_READ(ep, (uintptr_t)context),
        .opcode = IBV_WR_RDMA_READ,
        .wr.rdma.remote_addr = addr,
        .wr.rdma.rkey = (uint32_t)key,
    };

    return mverbs_send_buf(ep, &wr, buf, len, desc);
}

static ssize_t mverbs_msg_ep_rma_readv(struct fid_ep *ep_fid,
                                       const struct iovec *iov, void **desc,
                                       size_t count, fi_addr_t src_addr,
                                       uint64_t addr, uint64_t key,
                                       void *context)
{
    struct mverbs_ep *ep =
        container_of(ep_fid, struct mverbs_ep, util_ep.ep_fid);
    struct ibv_send_wr wr = {
        .wr_id = MVERBS_COMP_READ(ep, (uintptr_t)context),
        .opcode = IBV_WR_RDMA_READ,
        .wr.rdma.remote_addr = addr,
        .wr.rdma.rkey = (uint32_t)key,
        .num_sge = count,
    };

    mverbs_iov_dupa(wr.sg_list, iov, desc, count);
    return mverbs_post_send(ep, &wr, 0);
}

static ssize_t mverbs_msg_ep_rma_readmsg(struct fid_ep *ep_fid,
                                         const struct fi_msg_rma *msg,
                                         uint64_t flags)
{
    struct mverbs_ep *ep =
        container_of(ep_fid, struct mverbs_ep, util_ep.ep_fid);
    struct ibv_send_wr wr = {
        .wr_id = MVERBS_COMP_READ_FLAGS(ep, flags, (uintptr_t)msg->context),
        .opcode = IBV_WR_RDMA_READ,
        .wr.rdma.remote_addr = msg->rma_iov->addr,
        .wr.rdma.rkey = (uint32_t)msg->rma_iov->key,
        .num_sge = msg->iov_count,
    };

    assert(ofi_total_iov_len(msg->msg_iov, msg->iov_count) ==
           ofi_total_rma_iov_len(msg->rma_iov, msg->rma_iov_count));

    mverbs_iov_dupa(wr.sg_list, msg->msg_iov, msg->desc, msg->iov_count);
    return mverbs_post_send(ep, &wr, 0);
}

static ssize_t mverbs_msg_ep_rma_writedata(struct fid_ep *ep_fid,
                                           const void *buf, size_t len,
                                           void *desc, uint64_t data,
                                           fi_addr_t dest_addr, uint64_t addr,
                                           uint64_t key, void *context)
{
    struct mverbs_ep *ep =
        container_of(ep_fid, struct mverbs_ep, util_ep.ep_fid);
    struct ibv_send_wr wr = {
        .wr_id = MVERBS_COMP(ep, (uintptr_t)context),
        .opcode = IBV_WR_RDMA_WRITE_WITH_IMM,
        .wr.rdma.remote_addr = addr,
        .wr.rdma.rkey = (uint32_t)key,
        .send_flags = MVERBS_INJECT(ep, len, desc),
    };

    wr.imm_data = htonl((uint32_t)data);

    return mverbs_send_buf(ep, &wr, buf, len, desc);
}

static ssize_t mverbs_msg_ep_rma_inject_write(struct fid_ep *ep_fid,
                                              const void *buf, size_t len,
                                              fi_addr_t dest_addr,
                                              uint64_t addr, uint64_t key)
{
    struct mverbs_ep *ep =
        container_of(ep_fid, struct mverbs_ep, util_ep.ep_fid);
    struct ibv_send_wr wr = {
        .wr_id = MVERBS_NO_COMP_FLAG,
        .opcode = IBV_WR_RDMA_WRITE,
        .wr.rdma.remote_addr = addr,
        .wr.rdma.rkey = (uint32_t)key,
        .send_flags = IBV_SEND_INLINE,
    };

    return mverbs_send_buf(ep, &wr, buf, len, NULL);
}

static ssize_t mverbs_rma_write_fast(struct fid_ep *ep_fid, const void *buf,
                                     size_t len, fi_addr_t dest_addr,
                                     uint64_t addr, uint64_t key)
{
    struct mverbs_ep *ep;

    ep = container_of(ep_fid, struct mverbs_ep, util_ep.ep_fid);

    ep->wrs->rma_wr.wr.rdma.remote_addr = addr;
    ep->wrs->rma_wr.wr.rdma.rkey = (uint32_t)key;

    ep->wrs->sge.addr = (uintptr_t)buf;
    ep->wrs->sge.length = (uint32_t)len;

    return mverbs_post_send(ep, &ep->wrs->rma_wr, 0);
}

static ssize_t mverbs_msg_ep_rma_inject_writedata(struct fid_ep *ep_fid,
                                                  const void *buf, size_t len,
                                                  uint64_t data,
                                                  fi_addr_t dest_addr,
                                                  uint64_t addr, uint64_t key)
{
    struct mverbs_ep *ep =
        container_of(ep_fid, struct mverbs_ep, util_ep.ep_fid);
    struct ibv_send_wr wr = {
        .wr_id = MVERBS_NO_COMP_FLAG,
        .opcode = IBV_WR_RDMA_WRITE_WITH_IMM,
        .wr.rdma.remote_addr = addr,
        .wr.rdma.rkey = (uint32_t)key,
        .send_flags = IBV_SEND_INLINE,
    };

    wr.imm_data = htonl((uint32_t)data);

    return mverbs_send_buf(ep, &wr, buf, len, NULL);
}

static ssize_t mverbs_msg_ep_rma_inject_writedata_fast(
    struct fid_ep *ep_fid, const void *buf, size_t len, uint64_t data,
    fi_addr_t dest_addr, uint64_t addr, uint64_t key)
{
    ssize_t ret;
    struct mverbs_ep *ep =
        container_of(ep_fid, struct mverbs_ep, util_ep.ep_fid);
    ep->wrs->rma_wr.wr.rdma.remote_addr = addr;
    ep->wrs->rma_wr.wr.rdma.rkey = (uint32_t)key;

    ep->wrs->rma_wr.imm_data = htonl((uint32_t)data);
    ep->wrs->rma_wr.opcode = IBV_WR_RDMA_WRITE_WITH_IMM;

    ep->wrs->sge.addr = (uintptr_t)buf;
    ep->wrs->sge.length = (uint32_t)len;

    ret = mverbs_post_send(ep, &ep->wrs->rma_wr, 0);
    ep->wrs->rma_wr.opcode = IBV_WR_RDMA_WRITE;
    return ret;
}

struct fi_ops_rma mverbs_msg_ep_rma_ops_ts = {
    .size = sizeof(struct fi_ops_rma),
    .read = mverbs_msg_ep_rma_read,
    .readv = mverbs_msg_ep_rma_readv,
    .readmsg = mverbs_msg_ep_rma_readmsg,
    .write = mverbs_msg_ep_rma_write,
    .writev = mverbs_msg_ep_rma_writev,
    .writemsg = mverbs_msg_ep_rma_writemsg,
    .inject = mverbs_msg_ep_rma_inject_write,
    .writedata = mverbs_msg_ep_rma_writedata,
    .injectdata = mverbs_msg_ep_rma_inject_writedata,
};

struct fi_ops_rma mverbs_msg_ep_rma_ops = {
    .size = sizeof(struct fi_ops_rma),
    .read = mverbs_msg_ep_rma_read,
    .readv = mverbs_msg_ep_rma_readv,
    .readmsg = mverbs_msg_ep_rma_readmsg,
    .write = mverbs_msg_ep_rma_write,
    .writev = mverbs_msg_ep_rma_writev,
    .writemsg = mverbs_msg_ep_rma_writemsg,
    .inject = mverbs_rma_write_fast,
    .writedata = mverbs_msg_ep_rma_writedata,
    .injectdata = mverbs_msg_ep_rma_inject_writedata_fast,
};

static ssize_t mverbs_msg_xrc_ep_rma_write(struct fid_ep *ep_fid,
                                           const void *buf, size_t len,
                                           void *desc, fi_addr_t dest_addr,
                                           uint64_t addr, uint64_t key,
                                           void *context)
{
    struct mverbs_xrc_ep *ep =
        container_of(ep_fid, struct mverbs_xrc_ep, base_ep.util_ep.ep_fid);
    struct ibv_send_wr wr = {
        .wr_id = MVERBS_COMP(&ep->base_ep, (uintptr_t)context),
        .opcode = IBV_WR_RDMA_WRITE,
        .wr.rdma.remote_addr = addr,
        .wr.rdma.rkey = (uint32_t)key,
        .send_flags = MVERBS_INJECT(&ep->base_ep, len, desc),
    };

    VRB_SET_REMOTE_SRQN(wr, ep->peer_srqn);

    return mverbs_send_buf(&ep->base_ep, &wr, buf, len, desc);
}

static ssize_t mverbs_msg_xrc_ep_rma_writev(struct fid_ep *ep_fid,
                                            const struct iovec *iov,
                                            void **desc, size_t count,
                                            fi_addr_t dest_addr, uint64_t addr,
                                            uint64_t key, void *context)
{
    struct mverbs_xrc_ep *ep =
        container_of(ep_fid, struct mverbs_xrc_ep, base_ep.util_ep.ep_fid);
    struct ibv_send_wr wr = {
        .wr_id = (uintptr_t)context,
        .opcode = IBV_WR_RDMA_WRITE,
        .wr.rdma.remote_addr = addr,
        .wr.rdma.rkey = (uint32_t)key,
    };

    VRB_SET_REMOTE_SRQN(wr, ep->peer_srqn);

    return mverbs_send_iov(&ep->base_ep, &wr, iov, desc, count,
                           ep->base_ep.util_ep.tx_op_flags);
}

static ssize_t mverbs_msg_xrc_ep_rma_writemsg(struct fid_ep *ep_fid,
                                              const struct fi_msg_rma *msg,
                                              uint64_t flags)
{
    struct mverbs_xrc_ep *ep =
        container_of(ep_fid, struct mverbs_xrc_ep, base_ep.util_ep.ep_fid);
    struct ibv_send_wr wr = {
        .wr_id = (uintptr_t)msg->context,
        .wr.rdma.remote_addr = msg->rma_iov->addr,
        .wr.rdma.rkey = (uint32_t)msg->rma_iov->key,
    };

    VRB_SET_REMOTE_SRQN(wr, ep->peer_srqn);

    if (flags & FI_REMOTE_CQ_DATA) {
        wr.opcode = IBV_WR_RDMA_WRITE_WITH_IMM;
        wr.imm_data = htonl((uint32_t)msg->data);
    } else {
        wr.opcode = IBV_WR_RDMA_WRITE;
    }

    return mverbs_send_iov(&ep->base_ep, &wr, msg->msg_iov, msg->desc,
                           msg->iov_count, flags);
}

static ssize_t mverbs_msg_xrc_ep_rma_read(struct fid_ep *ep_fid, void *buf,
                                          size_t len, void *desc,
                                          fi_addr_t src_addr, uint64_t addr,
                                          uint64_t key, void *context)
{
    struct mverbs_xrc_ep *ep =
        container_of(ep_fid, struct mverbs_xrc_ep, base_ep.util_ep.ep_fid);
    struct ibv_send_wr wr = {
        .wr_id = MVERBS_COMP_READ(&ep->base_ep, (uintptr_t)context),
        .opcode = IBV_WR_RDMA_READ,
        .wr.rdma.remote_addr = addr,
        .wr.rdma.rkey = (uint32_t)key,
    };

    VRB_SET_REMOTE_SRQN(wr, ep->peer_srqn);

    return mverbs_send_buf(&ep->base_ep, &wr, buf, len, desc);
}

static ssize_t mverbs_msg_xrc_ep_rma_readv(struct fid_ep *ep_fid,
                                           const struct iovec *iov, void **desc,
                                           size_t count, fi_addr_t src_addr,
                                           uint64_t addr, uint64_t key,
                                           void *context)
{
    struct mverbs_xrc_ep *ep =
        container_of(ep_fid, struct mverbs_xrc_ep, base_ep.util_ep.ep_fid);
    struct ibv_send_wr wr = {
        .wr_id = MVERBS_COMP_READ(&ep->base_ep, (uintptr_t)context),
        .opcode = IBV_WR_RDMA_READ,
        .wr.rdma.remote_addr = addr,
        .wr.rdma.rkey = (uint32_t)key,
        .num_sge = count,
    };

    VRB_SET_REMOTE_SRQN(wr, ep->peer_srqn);

    mverbs_iov_dupa(wr.sg_list, iov, desc, count);
    return mverbs_post_send(&ep->base_ep, &wr, 0);
}

static ssize_t mverbs_msg_xrc_ep_rma_readmsg(struct fid_ep *ep_fid,
                                             const struct fi_msg_rma *msg,
                                             uint64_t flags)
{
    struct mverbs_xrc_ep *ep =
        container_of(ep_fid, struct mverbs_xrc_ep, base_ep.util_ep.ep_fid);
    struct ibv_send_wr wr = {
        .wr_id = MVERBS_COMP_READ_FLAGS(&ep->base_ep, flags,
                                        (uintptr_t)msg->context),
        .opcode = IBV_WR_RDMA_READ,
        .wr.rdma.remote_addr = msg->rma_iov->addr,
        .wr.rdma.rkey = (uint32_t)msg->rma_iov->key,
        .num_sge = msg->iov_count,
    };

    VRB_SET_REMOTE_SRQN(wr, ep->peer_srqn);

    mverbs_iov_dupa(wr.sg_list, msg->msg_iov, msg->desc, msg->iov_count);
    return mverbs_post_send(&ep->base_ep, &wr, flags);
}

static ssize_t mverbs_msg_xrc_ep_rma_writedata(struct fid_ep *ep_fid,
                                               const void *buf, size_t len,
                                               void *desc, uint64_t data,
                                               fi_addr_t dest_addr,
                                               uint64_t addr, uint64_t key,
                                               void *context)
{
    struct mverbs_xrc_ep *ep =
        container_of(ep_fid, struct mverbs_xrc_ep, base_ep.util_ep.ep_fid);
    struct ibv_send_wr wr = {
        .wr_id = MVERBS_COMP(&ep->base_ep, (uintptr_t)context),
        .opcode = IBV_WR_RDMA_WRITE_WITH_IMM,
        .wr.rdma.remote_addr = addr,
        .wr.rdma.rkey = (uint32_t)key,
        .send_flags = MVERBS_INJECT(&ep->base_ep, len, desc),
    };

    wr.imm_data = htonl((uint32_t)data);

    VRB_SET_REMOTE_SRQN(wr, ep->peer_srqn);

    return mverbs_send_buf(&ep->base_ep, &wr, buf, len, desc);
}

static ssize_t mverbs_msg_xrc_ep_rma_inject_write(struct fid_ep *ep_fid,
                                                  const void *buf, size_t len,
                                                  fi_addr_t dest_addr,
                                                  uint64_t addr, uint64_t key)
{
    struct mverbs_xrc_ep *ep =
        container_of(ep_fid, struct mverbs_xrc_ep, base_ep.util_ep.ep_fid);
    struct ibv_send_wr wr = {
        .wr_id = MVERBS_NO_COMP_FLAG,
        .opcode = IBV_WR_RDMA_WRITE,
        .wr.rdma.remote_addr = addr,
        .wr.rdma.rkey = (uint32_t)key,
        .send_flags = IBV_SEND_INLINE,
    };

    VRB_SET_REMOTE_SRQN(wr, ep->peer_srqn);

    return mverbs_send_buf(&ep->base_ep, &wr, buf, len, NULL);
}

static ssize_t mverbs_xrc_rma_write_fast(struct fid_ep *ep_fid, const void *buf,
                                         size_t len, fi_addr_t dest_addr,
                                         uint64_t addr, uint64_t key)
{
    struct mverbs_xrc_ep *ep =
        container_of(ep_fid, struct mverbs_xrc_ep, base_ep.util_ep.ep_fid);

    ep->base_ep.wrs->rma_wr.wr.rdma.remote_addr = addr;
    ep->base_ep.wrs->rma_wr.wr.rdma.rkey = (uint32_t)key;
    VRB_SET_REMOTE_SRQN(ep->base_ep.wrs->rma_wr, ep->peer_srqn);
    ep->base_ep.wrs->sge.addr = (uintptr_t)buf;
    ep->base_ep.wrs->sge.length = (uint32_t)len;

    return mverbs_post_send(&ep->base_ep, &ep->base_ep.wrs->rma_wr, 0);
}

static ssize_t mverbs_msg_xrc_ep_rma_inject_writedata(
    struct fid_ep *ep_fid, const void *buf, size_t len, uint64_t data,
    fi_addr_t dest_addr, uint64_t addr, uint64_t key)
{
    struct mverbs_xrc_ep *ep =
        container_of(ep_fid, struct mverbs_xrc_ep, base_ep.util_ep.ep_fid);

    struct ibv_send_wr wr = {
        .wr_id = MVERBS_NO_COMP_FLAG,
        .opcode = IBV_WR_RDMA_WRITE_WITH_IMM,
        .wr.rdma.remote_addr = addr,
        .wr.rdma.rkey = (uint32_t)key,
        .send_flags = IBV_SEND_INLINE,
    };

    wr.imm_data = htonl((uint32_t)data);

    VRB_SET_REMOTE_SRQN(wr, ep->peer_srqn);

    return mverbs_send_buf(&ep->base_ep, &wr, buf, len, NULL);
}

static ssize_t mverbs_msg_xrc_ep_rma_inject_writedata_fast(
    struct fid_ep *ep_fid, const void *buf, size_t len, uint64_t data,
    fi_addr_t dest_addr, uint64_t addr, uint64_t key)
{
    ssize_t ret;
    struct mverbs_xrc_ep *ep =
        container_of(ep_fid, struct mverbs_xrc_ep, base_ep.util_ep.ep_fid);
    ep->base_ep.wrs->rma_wr.wr.rdma.remote_addr = addr;
    ep->base_ep.wrs->rma_wr.wr.rdma.rkey = (uint32_t)key;
    VRB_SET_REMOTE_SRQN(ep->base_ep.wrs->rma_wr, ep->peer_srqn);

    ep->base_ep.wrs->rma_wr.imm_data = htonl((uint32_t)data);
    ep->base_ep.wrs->rma_wr.opcode = IBV_WR_RDMA_WRITE_WITH_IMM;

    ep->base_ep.wrs->sge.addr = (uintptr_t)buf;
    ep->base_ep.wrs->sge.length = (uint32_t)len;

    ret = mverbs_post_send(&ep->base_ep, &ep->base_ep.wrs->rma_wr, 0);
    ep->base_ep.wrs->rma_wr.opcode = IBV_WR_RDMA_WRITE;
    return ret;
}

struct fi_ops_rma mverbs_msg_xrc_ep_rma_ops_ts = {
    .size = sizeof(struct fi_ops_rma),
    .read = mverbs_msg_xrc_ep_rma_read,
    .readv = mverbs_msg_xrc_ep_rma_readv,
    .readmsg = mverbs_msg_xrc_ep_rma_readmsg,
    .write = mverbs_msg_xrc_ep_rma_write,
    .writev = mverbs_msg_xrc_ep_rma_writev,
    .writemsg = mverbs_msg_xrc_ep_rma_writemsg,
    .inject = mverbs_msg_xrc_ep_rma_inject_write,
    .writedata = mverbs_msg_xrc_ep_rma_writedata,
    .injectdata = mverbs_msg_xrc_ep_rma_inject_writedata,
};

struct fi_ops_rma mverbs_msg_xrc_ep_rma_ops = {
    .size = sizeof(struct fi_ops_rma),
    .read = mverbs_msg_xrc_ep_rma_read,
    .readv = mverbs_msg_xrc_ep_rma_readv,
    .readmsg = mverbs_msg_xrc_ep_rma_readmsg,
    .write = mverbs_msg_xrc_ep_rma_write,
    .writev = mverbs_msg_xrc_ep_rma_writev,
    .writemsg = mverbs_msg_xrc_ep_rma_writemsg,
    .inject = mverbs_xrc_rma_write_fast,
    .writedata = mverbs_msg_xrc_ep_rma_writedata,
    .injectdata = mverbs_msg_xrc_ep_rma_inject_writedata_fast,
};
