/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 */

#ifndef CH4_RECV_H_INCLUDED
#define CH4_RECV_H_INCLUDED

#include "ch4_impl.h"

#ifndef MPIDI_CH4_DIRECT_NETMOD
MPL_STATIC_INLINE_PREFIX int anysource_irecv(void *buf, MPI_Aint count, MPI_Datatype datatype,
                                             int rank, int tag, MPIR_Comm * comm,
                                             int attr, MPIDI_av_entry_t * av,
                                             MPIR_Request ** request)
{
    int mpi_errno = MPI_SUCCESS;

    /* Need critical section to prevent shmmem progress in-between
     * 1. Enter VCI lock and pre-allocate request for shm receive.
     * 2. MPIDI_SHM_mpi_irecv with pre-allocated request
     * 3. MPIDI_NM_mpi_irecv need use recursive locking in case it share the shm vci lock
     */
#if MPICH_THREAD_GRANULARITY == MPICH_THREAD_GRANULARITY__VCI
    int vci;
    MPIDI_POSIX_RECV_VSI(vci);
    MPID_THREAD_CS_ENTER(VCI, MPIDI_VCI(vci).lock);

    MPIDI_CH4_REQUEST_CREATE(*request, MPIR_REQUEST_KIND__RECV, vci, 1);
    MPIR_Assert(*request);
#endif

    mpi_errno = MPIDI_SHM_mpi_irecv(buf, count, datatype, rank, tag, comm, attr, request);
    MPIR_ERR_CHECK(mpi_errno);

    if (!MPIR_Request_is_complete(*request) && !MPIDIG_REQUEST_IN_PROGRESS(*request)) {
        MPIR_Request *nm_rreq = NULL;
        mpi_errno = MPIDI_NM_mpi_irecv(buf, count, datatype, rank, tag, comm, attr,
                                       av, &nm_rreq, *request);
        MPIR_ERR_CHECK(mpi_errno);
        /* if netmod recv is not matched yet, attach it to the shm request's partner */
        if (nm_rreq->dev.anysrc_partner) {
            (*request)->dev.anysrc_partner = nm_rreq;
        }
    }
  fn_exit:
#if MPICH_THREAD_GRANULARITY == MPICH_THREAD_GRANULARITY__VCI
    MPID_THREAD_CS_EXIT(VCI, MPIDI_VCI(vci).lock);
#endif
    return mpi_errno;
  fn_fail:
    goto fn_exit;
}
#endif

MPL_STATIC_INLINE_PREFIX int MPIDI_cancel_recv_unsafe(MPIR_Request * rreq)
{
    int mpi_errno;
    MPIR_FUNC_ENTER;

#ifdef MPIDI_CH4_DIRECT_NETMOD
    mpi_errno = MPIDI_NM_mpi_cancel_recv(rreq, false);
#else
    if (MPIDI_REQUEST(rreq, is_local)) {
        MPIR_Request *partner_rreq = rreq->dev.anysrc_partner;
        if (unlikely(partner_rreq)) {
            /* Canceling MPI_ANY_SOURCE receive -- first cancel NM recv, then SHM */
            mpi_errno = MPIDI_NM_mpi_cancel_recv(partner_rreq, false);
            MPIR_ERR_CHECK(mpi_errno);
            MPIDI_CH4_REQUEST_FREE(partner_rreq);
        }
        mpi_errno = MPIDI_SHM_mpi_cancel_recv(rreq);
    } else {
        mpi_errno = MPIDI_NM_mpi_cancel_recv(rreq, false);
    }
#endif
    MPIR_ERR_CHECK(mpi_errno);

  fn_exit:
    MPIR_FUNC_EXIT;
    return mpi_errno;
  fn_fail:
    goto fn_exit;
}

MPL_STATIC_INLINE_PREFIX int MPIDI_irecv(void *buf,
                                         MPI_Aint count,
                                         MPI_Datatype datatype,
                                         int rank,
                                         int tag,
                                         MPIR_Comm * comm,
                                         int attr, MPIDI_av_entry_t * av, MPIR_Request ** req)
{
    int mpi_errno = MPI_SUCCESS;
    MPIR_FUNC_ENTER;

    *(req) = NULL;
#ifdef MPIDI_CH4_DIRECT_NETMOD
    mpi_errno = MPIDI_NM_mpi_irecv(buf, count, datatype, rank, tag, comm, attr, av, req, NULL);
#else
    if (unlikely(rank == MPI_ANY_SOURCE)) {
        mpi_errno = anysource_irecv(buf, count, datatype, rank, tag, comm, attr, av, req);

    } else {
        if (MPIDI_av_is_local(av))
            mpi_errno = MPIDI_SHM_mpi_irecv(buf, count, datatype, rank, tag, comm, attr, req);
        else
            mpi_errno =
                MPIDI_NM_mpi_irecv(buf, count, datatype, rank, tag, comm, attr, av, req, NULL);
    }
#endif
    MPIR_ERR_CHECK(mpi_errno);

  fn_exit:
    MPIR_FUNC_EXIT;
    return mpi_errno;

  fn_fail:
    goto fn_exit;
}

MPL_STATIC_INLINE_PREFIX int MPIDI_imrecv(void *buf,
                                          MPI_Aint count, MPI_Datatype datatype,
                                          MPIR_Request * message)
{
    int mpi_errno = MPI_SUCCESS;
    MPIR_FUNC_ENTER;

#ifdef MPIDI_CH4_DIRECT_NETMOD
    mpi_errno = MPIDI_NM_mpi_imrecv(buf, count, datatype, message);
#else
    if (MPIDI_REQUEST(message, is_local))
        mpi_errno = MPIDI_SHM_mpi_imrecv(buf, count, datatype, message);
    else
        mpi_errno = MPIDI_NM_mpi_imrecv(buf, count, datatype, message);
#endif
    MPIR_ERR_CHECK(mpi_errno);

  fn_exit:
    MPIR_FUNC_EXIT;
    return mpi_errno;

  fn_fail:
    goto fn_exit;
}

MPL_STATIC_INLINE_PREFIX int MPIDI_cancel_recv_safe(MPIR_Request * rreq)
{
    int mpi_errno = MPI_SUCCESS;
    MPIR_FUNC_ENTER;

#if MPICH_THREAD_GRANULARITY == MPICH_THREAD_GRANULARITY__VCI
    int vci = MPIDI_Request_get_vci(rreq);
#endif
    /* MPIDI_NM_mpi_cancel_recv is used both externally and internally. For internal
     * usage it's often used inside a critical section (e.g. progress and anysource
     * receive). Therefore, we allow recursive lock usage here.
     */
    MPID_THREAD_CS_ENTER(VCI, MPIDI_VCI(vci).lock);
    mpi_errno = MPIDI_cancel_recv_unsafe(rreq);
    MPID_THREAD_CS_EXIT(VCI, MPIDI_VCI(vci).lock);
    MPIR_ERR_CHECK(mpi_errno);

  fn_exit:
    MPIR_FUNC_EXIT;
    return mpi_errno;

  fn_fail:
    goto fn_exit;
}


MPL_STATIC_INLINE_PREFIX int MPID_Recv(void *buf,
                                       MPI_Aint count,
                                       MPI_Datatype datatype,
                                       int rank,
                                       int tag,
                                       MPIR_Comm * comm,
                                       int attr, MPI_Status * status, MPIR_Request ** request)
{
    return MPID_Irecv(buf, count, datatype, rank, tag, comm, attr, request);
}

MPL_STATIC_INLINE_PREFIX int MPID_Mrecv(void *buf,
                                        MPI_Aint count,
                                        MPI_Datatype datatype, MPIR_Request * message,
                                        MPI_Status * status, MPIR_Request ** rreq)
{
    return MPID_Imrecv(buf, count, datatype, message, rreq);
}

MPL_STATIC_INLINE_PREFIX int MPID_Imrecv(void *buf, MPI_Aint count, MPI_Datatype datatype,
                                         MPIR_Request * message, MPIR_Request ** rreqp)
{
    int mpi_errno = MPI_SUCCESS;

    MPIR_FUNC_ENTER;

    MPIR_Assert(message->kind == MPIR_REQUEST_KIND__MPROBE);
    message->kind = MPIR_REQUEST_KIND__RECV;

    if (message->comm && MPIR_is_self_comm(message->comm)) {
        mpi_errno = MPIDI_Self_imrecv(buf, count, datatype, message, rreqp);
    } else {
        *rreqp = message;
        mpi_errno = MPIDI_imrecv(buf, count, datatype, message);
    }
    MPIR_ERR_CHECK(mpi_errno);

    MPII_RECVQ_REMEMBER(message, message->status.MPI_SOURCE, message->status.MPI_TAG,
                        message->comm->recvcontext_id, buf, count);

  fn_exit:
    MPIR_FUNC_EXIT;
    return mpi_errno;
  fn_fail:
    goto fn_exit;
}

MPL_STATIC_INLINE_PREFIX int MPID_Irecv(void *buf,
                                        MPI_Aint count,
                                        MPI_Datatype datatype,
                                        int rank,
                                        int tag,
                                        MPIR_Comm * comm, int attr, MPIR_Request ** request)
{
    int mpi_errno = MPI_SUCCESS;
    MPIR_FUNC_ENTER;

    if (MPIR_is_self_comm(comm)) {
        mpi_errno = MPIDI_Self_irecv(buf, count, datatype, rank, tag, comm, attr, request);
    } else {
        MPIDI_av_entry_t *av = (rank == MPI_ANY_SOURCE ? NULL : MPIDIU_comm_rank_to_av(comm, rank));
        mpi_errno = MPIDI_irecv(buf, count, datatype, rank, tag, comm, attr, av, request);
    }

    MPIR_ERR_CHECK(mpi_errno);

    MPII_RECVQ_REMEMBER(*request, rank, tag, comm->recvcontext_id, buf, count);
  fn_exit:
    MPIR_FUNC_EXIT;
    return mpi_errno;
  fn_fail:
    goto fn_exit;
}

MPL_STATIC_INLINE_PREFIX int MPID_Irecv_unsafe(void *buf,
                                        MPI_Aint count,
                                        MPI_Datatype datatype,
                                        int rank,
                                        int tag,
                                        MPIR_Comm * comm, int attr, MPIR_Request ** request)
{
    int mpi_errno = MPI_SUCCESS;
    MPIR_FUNC_ENTER;

    if (rank == comm->rank && comm->comm_kind != MPIR_COMM_KIND__INTERCOMM) {
        mpi_errno = MPIDI_Self_irecv(buf, count, datatype, rank, tag, MPIR_Process.comm_self, attr, request);
    } else {
        MPIDI_av_entry_t *av = (rank == MPI_ANY_SOURCE ? NULL : MPIDIU_comm_rank_to_av(comm, rank));
        mpi_errno = MPIDI_irecv(buf, count, datatype, rank, tag, comm, attr, av, request);
    }

    MPIR_ERR_CHECK(mpi_errno);

    MPII_RECVQ_REMEMBER(*request, rank, tag, comm->recvcontext_id, buf, count);
  fn_exit:
    MPIR_FUNC_EXIT;
    return mpi_errno;
  fn_fail:
    goto fn_exit;
}

MPL_STATIC_INLINE_PREFIX int MPID_Cancel_recv(MPIR_Request * rreq)
{
    int mpi_errno;
    MPIR_FUNC_ENTER;

    if (rreq->comm && MPIR_is_self_comm(rreq->comm)) {
        mpi_errno = MPIDI_Self_cancel(rreq);
    } else {
        mpi_errno = MPIDI_cancel_recv_safe(rreq);
    }

    MPIR_ERR_CHECK(mpi_errno);
  fn_exit:
    MPIR_FUNC_EXIT;
    return mpi_errno;
  fn_fail:
    goto fn_exit;
}

#endif /* CH4_RECV_H_INCLUDED */
