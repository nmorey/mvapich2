/*
 * Copyright (c) 2022 Canon Medical Research USA, Inc.  All rights reserved.
 * Copyright (c) 2010-2014 Intel Corporation.  All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
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
 *
 * $Id: cm.c 3453 2005-09-15 21:43:21Z sean.hefty $
 */

/* Adapted for use on Windows after being taken from
 *   URL: https://github.com/linux-rdma/rdma-core.git
 *   commit: 4834dd4a0848a71b98a48c71deb733500d4d5f3a
 *   path: librdmacm/addrinfo.c
 */

#include <config.h>
#include <asm/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include <rdma/rdma_cma.h>
#include <infiniband/ib.h>

static inline int ERR(int err)
{
    errno = err;
    return -1;
}

__be16 ucma_get_port(struct sockaddr *addr)
{
    switch (addr->sa_family) {
        case AF_INET:
            return ((struct sockaddr_in *)addr)->sin_port;
        case AF_INET6:
            return ((struct sockaddr_in6 *)addr)->sin6_port;
        case AF_IB:
            return htons(
                (uint16_t)ntohll(((struct sockaddr_ib *)addr)->sib_sid));
        default:
            return 0;
    }
}

static struct rdma_addrinfo nohints;

static void ucma_convert_to_ai(struct addrinfo *ai,
                               const struct rdma_addrinfo *rai)
{
    memset(ai, 0, sizeof(*ai));
    if (rai->ai_flags & RAI_PASSIVE)
        ai->ai_flags = AI_PASSIVE;
    if (rai->ai_flags & RAI_NUMERICHOST)
        ai->ai_flags |= AI_NUMERICHOST;
    if (rai->ai_family != AF_IB)
        ai->ai_family = rai->ai_family;

    switch (rai->ai_qp_type) {
        case IBV_QPT_RC:
        case IBV_QPT_UC:
        case IBV_QPT_XRC_SEND:
        case IBV_QPT_XRC_RECV:
            ai->ai_socktype = SOCK_STREAM;
            break;
        case IBV_QPT_UD:
            ai->ai_socktype = SOCK_DGRAM;
            break;
    }

    switch (rai->ai_port_space) {
        case RDMA_PS_TCP:
            ai->ai_protocol = IPPROTO_TCP;
            break;
        case RDMA_PS_IPOIB:
        case RDMA_PS_UDP:
            ai->ai_protocol = IPPROTO_UDP;
            break;
        case RDMA_PS_IB:
            if (ai->ai_socktype == SOCK_STREAM)
                ai->ai_protocol = IPPROTO_TCP;
            else if (ai->ai_socktype == SOCK_DGRAM)
                ai->ai_protocol = IPPROTO_UDP;
            break;
    }
}

static int ucma_copy_addr(struct sockaddr **dst, socklen_t *dst_len,
                          struct sockaddr *src, socklen_t src_len)
{
    *dst = malloc(src_len);
    if (!(*dst))
        return ERR(ENOMEM);

    memcpy(*dst, src, src_len);
    *dst_len = src_len;
    return 0;
}

void ucma_set_sid(enum rdma_port_space ps, struct sockaddr *addr,
                  struct sockaddr_ib *sib)
{
    __be16 port;

    port = addr ? ucma_get_port(addr) : 0;
    sib->sib_sid = htonll(((uint64_t)ps << 16) + ntohs(port));

    if (ps)
        sib->sib_sid_mask = htonll(RDMA_IB_IP_PS_MASK);
    if (port)
        sib->sib_sid_mask |= htonll(RDMA_IB_IP_PORT_MASK);
}

/*
 *      IPv6 address structure
 */
struct __in6_addr {
    union {
        __u8 u6_addr8[16];
        __be16 u6_addr16[8];
        __be32 u6_addr32[4];
    } in6_u;
#define s6_addr32 in6_u.u6_addr32
};

struct __sockaddr_in6 {
    unsigned short int sin6_family; /* AF_INET6 */
    __be16 sin6_port;               /* Transport layer port # */
    __be32 sin6_flowinfo;           /* IPv6 flow information */
    struct __in6_addr sin6_addr;    /* IPv6 address */
    __u32 sin6_scope_id;            /* scope id (new in RFC2553) */
};

static int ucma_convert_in6(int ps, struct sockaddr_ib **dst,
                            socklen_t *dst_len, struct __sockaddr_in6 *src,
                            socklen_t src_len)
{
    *dst = calloc(1, sizeof(struct sockaddr_ib));
    if (!(*dst))
        return ERR(ENOMEM);

    (*dst)->sib_family = AF_IB;
    (*dst)->sib_pkey = htons(0xFFFF);
    (*dst)->sib_flowinfo = src->sin6_flowinfo;
    ib_addr_set(&(*dst)->sib_addr, src->sin6_addr.s6_addr32[0],
                src->sin6_addr.s6_addr32[1], src->sin6_addr.s6_addr32[2],
                src->sin6_addr.s6_addr32[3]);
    ucma_set_sid(ps, (struct sockaddr *)src, *dst);
    (*dst)->sib_scope_id = src->sin6_scope_id;

    *dst_len = sizeof(struct sockaddr_ib);
    return 0;
}

static int ucma_convert_to_rai(struct rdma_addrinfo *rai,
                               const struct rdma_addrinfo *hints,
                               const struct addrinfo *ai)
{
    int ret;

    if (hints->ai_qp_type) {
        rai->ai_qp_type = hints->ai_qp_type;
    } else {
        switch (ai->ai_socktype) {
            case SOCK_STREAM:
                rai->ai_qp_type = IBV_QPT_RC;
                break;
            case SOCK_DGRAM:
                rai->ai_qp_type = IBV_QPT_UD;
                break;
        }
    }

    if (hints->ai_port_space) {
        rai->ai_port_space = hints->ai_port_space;
    } else {
        switch (ai->ai_protocol) {
            case IPPROTO_TCP:
                rai->ai_port_space = RDMA_PS_TCP;
                break;
            case IPPROTO_UDP:
                rai->ai_port_space = RDMA_PS_UDP;
                break;
        }
    }

    if ((ai->ai_flags & AI_PASSIVE) ||
        (hints->ai_flags & RAI_PASSIVE)) { // Windows getaddrinfo doesn't set
                                           // AI_PASSIVE flag on output.
        rai->ai_flags = RAI_PASSIVE;
        if (ai->ai_canonname)
            rai->ai_src_canonname = strdup(ai->ai_canonname);

        if ((hints->ai_flags & RAI_FAMILY) && (hints->ai_family == AF_IB) &&
            (hints->ai_flags & RAI_NUMERICHOST)) {
            rai->ai_family = AF_IB;
            ret = ucma_convert_in6(
                rai->ai_port_space, (struct sockaddr_ib **)&rai->ai_src_addr,
                &rai->ai_src_len, (struct __sockaddr_in6 *)ai->ai_addr,
                (socklen_t)ai->ai_addrlen);
        } else {
            rai->ai_family = ai->ai_family;
            ret = ucma_copy_addr(&rai->ai_src_addr, &rai->ai_src_len,
                                 ai->ai_addr, (socklen_t)ai->ai_addrlen);
        }
    } else {
        if (ai->ai_canonname)
            rai->ai_dst_canonname = strdup(ai->ai_canonname);

        if ((hints->ai_flags & RAI_FAMILY) && (hints->ai_family == AF_IB) &&
            (hints->ai_flags & RAI_NUMERICHOST)) {
            rai->ai_family = AF_IB;
            ret = ucma_convert_in6(
                rai->ai_port_space, (struct sockaddr_ib **)&rai->ai_dst_addr,
                &rai->ai_dst_len, (struct __sockaddr_in6 *)ai->ai_addr,
                (socklen_t)ai->ai_addrlen);
        } else {
            rai->ai_family = ai->ai_family;
            ret = ucma_copy_addr(&rai->ai_dst_addr, &rai->ai_dst_len,
                                 ai->ai_addr, (socklen_t)ai->ai_addrlen);
        }
    }
    return ret;
}

static int ucma_getaddrinfo(const char *node, const char *service,
                            const struct rdma_addrinfo *hints,
                            struct rdma_addrinfo *rai)
{
    struct addrinfo ai_hints;
    struct addrinfo *ai;
    int ret;

    if (hints != &nohints) {
        ucma_convert_to_ai(&ai_hints, hints);
        ret = getaddrinfo(node, service, &ai_hints, &ai);
    } else {
        ret = getaddrinfo(node, service, NULL, &ai);
    }
    if (ret)
        return ret;

    ret = ucma_convert_to_rai(rai, hints, ai);
    freeaddrinfo(ai);
    return ret;
}

int rdma_getaddrinfo(const char *node, const char *service,
                     const struct rdma_addrinfo *hints,
                     struct rdma_addrinfo **res)
{
    struct rdma_addrinfo *rai;
    int ret = 0;

    if (!service && !node && !hints)
        return ERR(EINVAL);

    rai = calloc(1, sizeof(*rai));
    if (!rai)
        return ERR(ENOMEM);

    if (!hints)
        hints = &nohints;

    if (node || service) {
        ret = ucma_getaddrinfo(node, service, hints, rai);
    } else {
        rai->ai_flags = hints->ai_flags;
        rai->ai_family = hints->ai_family;
        rai->ai_qp_type = hints->ai_qp_type;
        rai->ai_port_space = hints->ai_port_space;
        if (hints->ai_dst_len) {
            ret = ucma_copy_addr(&rai->ai_dst_addr, &rai->ai_dst_len,
                                 hints->ai_dst_addr, hints->ai_dst_len);
        }
    }
    if (ret)
        goto err;

    if (!rai->ai_src_len && hints->ai_src_len) {
        ret = ucma_copy_addr(&rai->ai_src_addr, &rai->ai_src_len,
                             hints->ai_src_addr, hints->ai_src_len);
        if (ret)
            goto err;
    }

    *res = rai;
    return 0;

err:
    rdma_freeaddrinfo(rai);
    return ret;
}

void rdma_freeaddrinfo(struct rdma_addrinfo *res)
{
    struct rdma_addrinfo *rai;

    while (res) {
        rai = res;
        res = res->ai_next;

        if (rai->ai_connect)
            free(rai->ai_connect);

        if (rai->ai_route)
            free(rai->ai_route);

        if (rai->ai_src_canonname)
            free(rai->ai_src_canonname);

        if (rai->ai_dst_canonname)
            free(rai->ai_dst_canonname);

        if (rai->ai_src_addr)
            free(rai->ai_src_addr);

        if (rai->ai_dst_addr)
            free(rai->ai_dst_addr);

        free(rai);
    }
}
