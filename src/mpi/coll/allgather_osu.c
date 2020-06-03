/* -*- Mode: C; c-basic-offset:4 ; -*- */
/* Copyright (c) 2001-2020, The Ohio State University. All rights
* reserved.
*
* This file is part of the MVAPICH2 software package developed by the
* team members of The Ohio State University's Network-Based Computing
* Laboratory (NBCL), headed by Professor Dhabaleswar K. (DK) Panda.
*
* For detailed copyright and licensing information, please refer to the
* copyright file COPYRIGHT in the top level MVAPICH2 directory.
*/
/*
*
*  (C) 2001 by Argonne National Laboratory.
*      See COPYRIGHT in top-level directory.
*/

#include "mpiimpl.h"

#include "datatype.h"
#include "common_tuning.h"
#include "coll_shmem.h"
#include "allgather_tuning.h"
extern struct coll_runtime mv2_coll_param;
extern int allgather_tuning_algo;
extern int allgather_algo_num;
extern int use_2lvl_allgather;


MPIR_T_PVAR_DOUBLE_TIMER_DECL_EXTERN(MV2, mv2_coll_timer_allgather_rd);
MPIR_T_PVAR_DOUBLE_TIMER_DECL_EXTERN(MV2, mv2_coll_timer_allgather_bruck);
MPIR_T_PVAR_DOUBLE_TIMER_DECL_EXTERN(MV2, mv2_coll_timer_allgather_ring);
MPIR_T_PVAR_DOUBLE_TIMER_DECL_EXTERN(MV2, mv2_coll_timer_allgather_direct);
MPIR_T_PVAR_DOUBLE_TIMER_DECL_EXTERN(MV2, mv2_coll_timer_allgather_directspread);
MPIR_T_PVAR_DOUBLE_TIMER_DECL_EXTERN(MV2, mv2_coll_timer_allgather_gather_bcast);
MPIR_T_PVAR_DOUBLE_TIMER_DECL_EXTERN(MV2, mv2_coll_timer_allgather_2lvl_nonblocked);
MPIR_T_PVAR_DOUBLE_TIMER_DECL_EXTERN(MV2, mv2_coll_timer_allgather_2lvl_ring_nonblocked);
MPIR_T_PVAR_DOUBLE_TIMER_DECL_EXTERN(MV2, mv2_coll_timer_allgather_2lvl_direct);
MPIR_T_PVAR_DOUBLE_TIMER_DECL_EXTERN(MV2, mv2_coll_timer_allgather_2lvl_ring);

MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_rd_allgather_comm);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_rd);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_bruck);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_ring);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_direct);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_directspread);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_gather_bcast);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_2lvl_nonblocked);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_2lvl_ring_nonblocked);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_2lvl_direct);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_2lvl_ring);

MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_rd_bytes_send);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_bruck_bytes_send);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_ring_bytes_send);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_direct_bytes_send);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_directspread_bytes_send);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_2lvl_ring_nonblocked_bytes_send);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_2lvl_direct_bytes_send);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_2lvl_ring_bytes_send);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_rd_bytes_recv);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_bruck_bytes_recv);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_ring_bytes_recv);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_direct_bytes_recv);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_directspread_bytes_recv);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_2lvl_ring_nonblocked_bytes_recv);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_2lvl_direct_bytes_recv);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_2lvl_ring_bytes_recv);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_rd_count_send);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_bruck_count_send);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_ring_count_send);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_direct_count_send);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_directspread_count_send);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_2lvl_ring_nonblocked_count_send);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_2lvl_direct_count_send);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_2lvl_ring_count_send);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_rd_count_recv);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_bruck_count_recv);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_ring_count_recv);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_direct_count_recv);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_directspread_count_recv);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_2lvl_ring_nonblocked_count_recv);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_2lvl_direct_count_recv);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_2lvl_ring_count_recv);

MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_bytes_send);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_bytes_recv);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_count_send);
MPIR_T_PVAR_ULONG2_COUNTER_DECL_EXTERN(MV2, mv2_coll_allgather_count_recv);

#define FGP_SWITCH_FACTOR 4     /*Used to determine switch between naive and FGP
                                   design */

int (*MV2_Allgather_function)(const void *sendbuf,
                             int sendcount,
                             MPI_Datatype sendtype,
                             void *recvbuf,
                             int recvcount,
                             MPI_Datatype recvtype, MPID_Comm * comm_ptr,
                             MPIR_Errflag_t *errflag);

int MPIR_Allgather_RD_Allgather_Comm_MV2(const void *sendbuf,
                                 int sendcount,
                                 MPI_Datatype sendtype,
                                 void *recvbuf,
                                 int recvcount,
                                 MPI_Datatype recvtype, MPID_Comm * comm_ptr,
                                 MPIR_Errflag_t *errflag)
{
    MPIR_T_PVAR_COUNTER_INC(MV2, mv2_coll_allgather_rd_allgather_comm, 1);

    return 0;
}

int allgather_tuning(int comm_size, int pof2)
{

    char *value;
    if (pof2 == 1 && (value = getenv("MV2_ALLGATHER_RD_THRESHOLD")) != NULL) {
        /* pof2 case. User has set the run-time parameter
           "MV2_ALLGATHER_RD_THRESHOLD".
           * Just use that value */
        return mv2_coll_param.allgather_rd_threshold;
    }
    if (pof2 == 0 && (value = getenv("MV2_ALLGATHER_BRUCK_THRESHOLD")) != NULL) {
        /* Non-pof2 case. User has set the run-time parameter
           "MV2_ALLGATHER_BRUCK_THRESHOLD".
           * Just use that value */
        return mv2_coll_param.allgather_bruck_threshold;
    } else {
        /* User has not used any run-time parameters.
         */
        if (comm_size <= MV2_ALLGATHER_SMALL_SYSTEM_SIZE) {
            return mv2_tuning_table[ALLGATHER_ID][SMALL];
        } else if (comm_size > MV2_ALLGATHER_SMALL_SYSTEM_SIZE
                   && comm_size <= MV2_ALLGATHER_MEDIUM_SYSTEM_SIZE) {
            return mv2_tuning_table[ALLGATHER_ID][MEDIUM];
        } else {
            return mv2_tuning_table[ALLGATHER_ID][LARGE];
        }
    }
}

/* This is the default implementation of allgather. The algorithm is:

Algorithm: MPI_Allgather

For short messages and non-power-of-two no. of processes, we use
the algorithm from the Jehoshua Bruck et al IEEE TPDS Nov 97
paper. It is a variant of the disemmination algorithm for
barrier. It takes ceiling(lg p) steps.

Cost = lgp.alpha + n.((p-1)/p).beta
where n is total size of data gathered on each process.

For short or medium-size messages and power-of-two no. of
processes, we use the recursive doubling algorithm.

Cost = lgp.alpha + n.((p-1)/p).beta

TODO: On TCP, we may want to use recursive doubling instead of the Bruck
algorithm in all cases because of the pairwise-exchange property of
recursive doubling (see Benson et al paper in Euro PVM/MPI
2003).

It is interesting to note that either of the above algorithms for
MPI_Allgather has the same cost as the tree algorithm for MPI_Gather!

For long messages or medium-size messages and non-power-of-two
no. of processes, we use a ring algorithm. In the first step, each
process i sends its contribution to process i+1 and receives
the contribution from process i-1 (with wrap-around). From the
second step onwards, each process i forwards to process i+1 the
data it received from process i-1 in the previous step. This takes
a total of p-1 steps.

Cost = (p-1).alpha + n.((p-1)/p).beta

We use this algorithm instead of recursive doubling for long
messages because we find that this communication pattern (nearest
neighbor) performs twice as fast as recursive doubling for long
messages (on Myrinet and IBM SP).

Possible improvements:

End Algorithm: MPI_Allgather
*/
/* begin:nested */
/* not declared static because a machine-specific function may call this
one in some cases */

/* This implements an allgather via direct method, in which each
 * process sends directly to every other process.  All processes
 * start sending to rank 0 and work up in order. This is meant as
 * a base case */
#undef FUNCNAME
#define FUNCNAME MPIR_Allgather_Direct_MV2
#undef FCNAME
#define FCNAME MPL_QUOTE(FUNCNAME)
int MPIR_Allgather_Direct_MV2(
    const void *sendbuf, int sendcnt, MPI_Datatype sendtype,
          void *recvbuf, int recvcnt, MPI_Datatype recvtype,
    MPID_Comm * comm_ptr, MPIR_Errflag_t *errflag)
{
    MPIR_TIMER_START(coll,allgather,direct);
    int i;
    int mpi_errno = MPI_SUCCESS;
    int mpi_errno_ret = MPI_SUCCESS;
    MPIU_CHKLMEM_DECL(2);

    MPIR_T_PVAR_COUNTER_INC(MV2, mv2_coll_allgather_direct, 1);

    if (recvcnt == 0) {
        return MPI_SUCCESS;
    }

    /* get our rank and the size of this communicator */
    int rank = comm_ptr->rank;
    int size = comm_ptr->local_size;

    /* get extent of receive type */
    MPI_Aint recvtype_extent;
    MPID_Datatype_get_extent_macro(recvtype, recvtype_extent);

    /* allocate an array of request objects */
    MPID_Request **reqarray = NULL;
    MPIU_CHKLMEM_MALLOC(reqarray, MPID_Request **,
                        2 * size * sizeof (MPID_Request*),
                        mpi_errno, "reqarray");

    /* allocate an array of status objects */
    MPI_Status *starray = NULL;
    MPIU_CHKLMEM_MALLOC(starray, MPI_Status *,
                        2 * size * sizeof (MPI_Status),
                        mpi_errno, "starray");

    /* initialize our active request counter to 0 */
    int reqs = 0;

    /* copy our data to our receive buffer if needed */
    if (sendbuf != MPI_IN_PLACE) {
        /* compute location in receive buffer for our data */
        void* rbuf = (void*)((char*) recvbuf + rank * recvcnt * recvtype_extent);

        /* copy data from send buffer to receive buffer */
        mpi_errno = MPIR_Localcopy(
            sendbuf, sendcnt, sendtype,
            rbuf,    recvcnt, recvtype
        );
        if (mpi_errno) {
            MPIR_ERR_POP(mpi_errno);
        }
    }

    /* post receives */
    for (i = 0; i < size; i++) {
        /* our data is already in the receive buffer */
        if (i == rank) {
            continue;
        }

        /* compute pointer in receive buffer for incoming data from this rank */
        void* rbuf = (void*)((char*) recvbuf + i * recvcnt * recvtype_extent);

        /* post receive for data from this rank */
        MPIR_PVAR_INC(allgather, direct, recv, recvcnt, recvtype);
        mpi_errno = MPIC_Irecv(rbuf, recvcnt, recvtype,
            i, MPIR_ALLGATHER_TAG, comm_ptr, &reqarray[reqs++]
        );
        if (mpi_errno) {
            /* for communication errors, just record the error but continue */
            *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
            MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
            MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
        }
    }

    /* TODO: consider placing a barrier here to ensure
     * receives are posted before sends, especially for large messages */
    //MPIR_Barrier_impl(comm_ptr);

    /* get parameters for sending data */
    const void* sbuf   = sendbuf;
    int scnt           = sendcnt;
    MPI_Datatype stype = sendtype;
    if (sendbuf == MPI_IN_PLACE) {
        /* use receive params if IN_PLACE */
        sbuf   = (void*)((char*) recvbuf + rank * recvcnt * recvtype_extent);
        scnt   = recvcnt;
        stype  = recvtype;
    }

    /* post sends */
    for (i = 0; i < size; i++) {
        /* no need to send to ourself */
        if (i == rank) {
            continue;
        }

        /* send data to this rank */
        MPIR_PVAR_INC(allgather, direct, send, scnt, stype); 
        mpi_errno = MPIC_Isend(sbuf, scnt, stype,
            i, MPIR_ALLGATHER_TAG, comm_ptr, &reqarray[reqs++], errflag
        );
        if (mpi_errno) {
            /* for communication errors, just record the error but continue */
            *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
            MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
            MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
        }
    }

    /* wait for all outstanding requests to complete */
    mpi_errno = MPIC_Waitall(reqs, reqarray, starray, errflag);
    if (mpi_errno && mpi_errno != MPI_ERR_IN_STATUS) {
        MPIR_ERR_POP(mpi_errno);
    }

    /* --BEGIN ERROR HANDLING-- */
    if (mpi_errno == MPI_ERR_IN_STATUS) {
        for (i = 0; i < reqs; i++) {
            if (starray[i].MPI_ERROR != MPI_SUCCESS) {
                mpi_errno = starray[i].MPI_ERROR;
                if (mpi_errno) {
                    /* for communication errors, just record the error but continue */
                    *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
                    MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
                    MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
                }
            }
        }
    }

    MPIU_CHKLMEM_FREEALL();
  fn_fail:
    MPIR_TIMER_END(coll,allgather,direct);
    return (mpi_errno);
}

/* This implements an allgather via direct method, in which each
 * process sends directly to every other process.  To spread the
 * load and avoid hot spots, processes starting by sending to the
 * rank one higher than their own.  This is meant as a base case
 * allgather, but it may actually be the fastest method in some cases. */
#undef FUNCNAME
#define FUNCNAME MPIR_Allgather_DirectSpread_MV2
#undef FCNAME
#define FCNAME MPL_QUOTE(FUNCNAME)
int MPIR_Allgather_DirectSpread_MV2(
    const void *sendbuf, int sendcnt, MPI_Datatype sendtype,
          void *recvbuf, int recvcnt, MPI_Datatype recvtype,
    MPID_Comm * comm_ptr, MPIR_Errflag_t *errflag)
{
    MPIR_TIMER_START(coll,allgather,directspread);
    int i;
    int mpi_errno = MPI_SUCCESS;
    int mpi_errno_ret = MPI_SUCCESS;
    MPIU_CHKLMEM_DECL(2);

    MPIR_T_PVAR_COUNTER_INC(MV2, mv2_coll_allgather_directspread, 1);

    if (recvcnt == 0) {
        return MPI_SUCCESS;
    }

    /* get our rank and the size of this communicator */
    int rank = comm_ptr->rank;
    int size = comm_ptr->local_size;

    /* get extent of receive type */
    MPI_Aint recvtype_extent;
    MPID_Datatype_get_extent_macro(recvtype, recvtype_extent);

    /* allocate an array of request objects */
    MPID_Request **reqarray = NULL;
    MPIU_CHKLMEM_MALLOC(reqarray, MPID_Request **,
                        2 * size * sizeof (MPID_Request*),
                        mpi_errno, "reqarray");

    /* allocate an array of status objects */
    MPI_Status *starray = NULL;
    MPIU_CHKLMEM_MALLOC(starray, MPI_Status *,
                        2 * size * sizeof (MPI_Status),
                        mpi_errno, "starray");

    /* initialize our active request counter to 0 */
    int reqs = 0;

    /* copy our data to our receive buffer if needed */
    if (sendbuf != MPI_IN_PLACE) {
        /* compute location in receive buffer for our data */
        void* rbuf = (void*)((char*) recvbuf + rank * recvcnt * recvtype_extent);

        /* copy data from send buffer to receive buffer */
        mpi_errno = MPIR_Localcopy(
            sendbuf, sendcnt, sendtype,
            rbuf,    recvcnt, recvtype
        );
        if (mpi_errno) {
            MPIR_ERR_POP(mpi_errno);
        }
    }

    /* post receives */
    for (i = 1; i < size; i++) {
        /* compute source rank sending to us in this step */
        int src = rank - i;
        if (src < 0) {
            src += size;
        }

        /* get pointer to receive buffer for this rank */
        void* rbuf = (void*)((char*) recvbuf + src * recvcnt * recvtype_extent);

        /* post receive */
        MPIR_PVAR_INC(allgather, directspread, recv, recvcnt, recvtype);
        mpi_errno = MPIC_Irecv(rbuf, recvcnt, recvtype,
            src, MPIR_ALLGATHER_TAG, comm_ptr, &reqarray[reqs++]
        );
        if (mpi_errno) {
            /* for communication errors, just record the error but continue */
            *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
            MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
            MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
        }
    }

    /* TODO: consider placing a barrier here to ensure
     * receives are posted before sends, especially for large messages */
    //MPIR_Barrier_impl(comm_ptr);

    /* get parameters for sending data */
    const void* sbuf   = sendbuf;
    int scnt           = sendcnt;
    MPI_Datatype stype = sendtype;
    if (sendbuf == MPI_IN_PLACE) {
        /* use receive params if IN_PLACE */
        sbuf   = (void*)((char*) recvbuf + rank * recvcnt * recvtype_extent);
        scnt   = recvcnt;
        stype  = recvtype;
    }

    for (i = 1; i < size; i++) {
        /* compute destination rank for this step */
        int dst = rank + i;
        if (dst >= size) {
            dst -= size;
        }

        /* post send to this destination rank */
        MPIR_PVAR_INC(allgather, directspread, send, scnt, stype); 
        mpi_errno = MPIC_Isend(sbuf, scnt, stype,
            dst, MPIR_ALLGATHER_TAG, comm_ptr, &reqarray[reqs++], errflag
        );
        if (mpi_errno) {
            /* for communication errors, just record the error but continue */
            *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
            MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
            MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
        }
    }

    /* wait on all outstanding requests */
    mpi_errno = MPIC_Waitall(reqs, reqarray, starray, errflag);
    if (mpi_errno && mpi_errno != MPI_ERR_IN_STATUS) {
        MPIR_ERR_POP(mpi_errno);
    }

    /* --BEGIN ERROR HANDLING-- */
    if (mpi_errno == MPI_ERR_IN_STATUS) {
        for (i = 0; i < reqs; i++) {
            if (starray[i].MPI_ERROR != MPI_SUCCESS) {
                mpi_errno = starray[i].MPI_ERROR;
                if (mpi_errno) {
                    /* for communication errors, just record the error but continue */
                    *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
                    MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
                    MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
                }
            }
        }
    }

    MPIU_CHKLMEM_FREEALL();
  fn_fail:
    MPIR_TIMER_END(coll,allgather,directspread);
    return (mpi_errno);
}

#undef FUNCNAME
#define FUNCNAME MPIR_Allgather_RD_MV2
#undef FCNAME
#define FCNAME MPL_QUOTE(FUNCNAME)
int MPIR_Allgather_RD_MV2(const void *sendbuf,
                          int sendcount,
                          MPI_Datatype sendtype,
                          void *recvbuf,
                          int recvcount,
                          MPI_Datatype recvtype, MPID_Comm * comm_ptr,
                          MPIR_Errflag_t *errflag)
{
    MPIR_TIMER_START(coll,allgather,rd);
    int comm_size, rank;
    int mpi_errno = MPI_SUCCESS;
    int mpi_errno_ret = MPI_SUCCESS;
    MPI_Aint recvtype_extent;
    int j, i;
    int curr_cnt, dst;
    MPI_Status status;
    int mask, dst_tree_root, my_tree_root, is_homogeneous,
        send_offset, recv_offset, last_recv_cnt = 0, nprocs_completed, k,
        offset, tmp_mask, tree_root;
#ifdef MPID_HAS_HETERO
    int position, tmp_buf_size, nbytes;
#endif

    MPIR_T_PVAR_COUNTER_INC(MV2, mv2_coll_allgather_rd, 1);

    comm_size = comm_ptr->local_size;
    rank = comm_ptr->rank;

    MPID_Datatype_get_extent_macro(recvtype, recvtype_extent);

    is_homogeneous = 1;
#ifdef MPID_HAS_HETERO
    if (comm_ptr->is_hetero) {
        is_homogeneous = 0;
    }
#endif

    if (is_homogeneous) {
        /* homogeneous. no need to pack into tmp_buf on each node. copy
         * local data into recvbuf */
        if (sendbuf != MPI_IN_PLACE) {
            mpi_errno = MPIR_Localcopy(sendbuf, sendcount, sendtype,
                                       ((char *) recvbuf +
                                        rank * recvcount * recvtype_extent),
                                       recvcount, recvtype);
            if (mpi_errno) {
                MPIR_ERR_POP(mpi_errno);
            }
        }

        curr_cnt = recvcount;

        mask = 0x1;
        i = 0;
        while (mask < comm_size) {
            dst = rank ^ mask;

            /* find offset into send and recv buffers. zero out
             * the least significant "i" bits of rank and dst to
             * find root of src and dst subtrees. Use ranks of
             * roots as index to send from and recv into buffer */

            dst_tree_root = dst >> i;
            dst_tree_root <<= i;

            my_tree_root = rank >> i;
            my_tree_root <<= i;

            /* FIXME: saving an MPI_Aint into an int */
            send_offset = my_tree_root * recvcount * recvtype_extent;
            recv_offset = dst_tree_root * recvcount * recvtype_extent;

            if (dst < comm_size) {
                MPIR_PVAR_INC(allgather, rd, send, curr_cnt, recvtype); 
                MPIR_PVAR_INC(allgather, rd, recv, (comm_size - dst_tree_root) * recvcount, recvtype); 
                mpi_errno =
                    MPIC_Sendrecv(((char *) recvbuf + send_offset),
                                     curr_cnt, recvtype, dst,
                                     MPIR_ALLGATHER_TAG,
                                     ((char *) recvbuf + recv_offset),
                                     (comm_size -
                                      dst_tree_root) * recvcount, recvtype,
                                     dst, MPIR_ALLGATHER_TAG, comm_ptr, &status, errflag);
                if (mpi_errno) {
                    /* for communication errors, just record the error but
                       continue */
                    *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
                    MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
                    MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
                    last_recv_cnt = 0;
                }

                MPIR_Get_count_impl(&status, recvtype, &last_recv_cnt);
                curr_cnt += last_recv_cnt;
            }

            /* if some processes in this process's subtree in this step
             * did not have any destination process to communicate with
             * because of non-power-of-two, we need to send them the
             * data that they would normally have received from those
             * processes. That is, the haves in this subtree must send to
             * the havenots. We use a logarithmic recursive-halfing algorithm
             * for this. */

            /* This part of the code will not currently be
             * executed because we are not using recursive
             * doubling for non power of two. Mark it as experimental
             * so that it doesn't show up as red in the coverage
             * tests. */

            /* --BEGIN EXPERIMENTAL-- */
            if (dst_tree_root + mask > comm_size) {
                nprocs_completed = comm_size - my_tree_root - mask;
                /* nprocs_completed is the number of processes in this
                 * subtree that have all the data. Send data to others
                 * in a tree fashion. First find root of current tree
                 * that is being divided into two. k is the number of
                 * least-significant bits in this process's rank that
                 * must be zeroed out to find the rank of the root */
                j = mask;
                k = 0;
                while (j) {
                    j >>= 1;
                    k++;
                }
                k--;

                /* FIXME: saving an MPI_Aint into an int */
                offset = recvcount * (my_tree_root + mask) * recvtype_extent;
                tmp_mask = mask >> 1;

                while (tmp_mask) {
                    dst = rank ^ tmp_mask;

                    tree_root = rank >> k;
                    tree_root <<= k;

                    /* send only if this proc has data and destination
                     * doesn't have data. at any step, multiple processes
                     * can send if they have the data */
                    if ((dst > rank) && (rank < tree_root + nprocs_completed)
                        && (dst >= tree_root + nprocs_completed)) {
                        MPIR_PVAR_INC(allgather, rd, send, last_recv_cnt, recvtype); 
                        mpi_errno =
                            MPIC_Send(((char *) recvbuf + offset),
                                         last_recv_cnt, recvtype, dst,
                                         MPIR_ALLGATHER_TAG, comm_ptr, errflag);
                        /* last_recv_cnt was set in the previous
                         * receive. that's the amount of data to be
                         * sent now. */
                        if (mpi_errno) {
                            /* for communication errors, just record the error
                               but continue */
                            *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
                            MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
                            MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
                        }
                    }
                    /* recv only if this proc. doesn't have data and sender
                     * has data */
                    else if ((dst < rank) &&
                             (dst < tree_root + nprocs_completed) &&
                             (rank >= tree_root + nprocs_completed)) {
                        MPIR_PVAR_INC(allgather, rd, recv, (comm_size - (my_tree_root + mask)) * recvcount, recvtype);
                        mpi_errno =
                            MPIC_Recv(((char *) recvbuf + offset),
                                         (comm_size -
                                          (my_tree_root +
                                           mask)) * recvcount, recvtype,
                                         dst, MPIR_ALLGATHER_TAG, comm_ptr, &status, errflag);
                        /* nprocs_completed is also equal to the
                         * no. of processes whose data we don't have */
                        if (mpi_errno) {
                            /* for communication errors, just record the error
                               but continue */
                            *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
                            MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
                            MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
                            last_recv_cnt = 0;
                        }
                        MPIR_Get_count_impl(&status, recvtype, &last_recv_cnt);
                        curr_cnt += last_recv_cnt;
                    }
                    tmp_mask >>= 1;
                    k--;
                }
            }
            /* --END EXPERIMENTAL-- */

            mask <<= 1;
            i++;
        }
    }
#ifdef MPID_HAS_HETERO
    else {
        /* heterogeneous. need to use temp. buffer. */

        MPIR_Pack_size_impl(recvcount * comm_size, recvtype, &tmp_buf_size);

        tmp_buf = MPIU_Malloc(tmp_buf_size);
        /* --BEGIN ERROR HANDLING-- */
        if (!tmp_buf) {
            mpi_errno =
                MPIR_Err_create_code(MPI_SUCCESS, MPIR_ERR_RECOVERABLE,
                                     FCNAME, __LINE__, MPI_ERR_OTHER, "**nomem", 0);
            return mpi_errno;
        }
        /* --END ERROR HANDLING-- */

        /* calculate the value of nbytes, the number of bytes in packed
         * representation that each process contributes. We can't simply divide
         * tmp_buf_size by comm_size because tmp_buf_size is an upper
         * bound on the amount of memory required. (For example, for
         * a single integer, MPICH-1 returns pack_size=12.) Therefore, we
         * actually pack some data into tmp_buf and see by how much
         * 'position' is incremented. */

        position = 0;
        MPIR_Pack_impl(recvbuf, 1, recvtype, tmp_buf, tmp_buf_size, &position);
        nbytes = position * recvcount;

        /* pack local data into right location in tmp_buf */
        position = rank * nbytes;
        if (sendbuf != MPI_IN_PLACE) {
            MPIR_Pack_impl(sendbuf, sendcount, sendtype, tmp_buf,
                           tmp_buf_size, &position);
        } else {
            /* if in_place specified, local data is found in recvbuf */
            MPIR_Pack_impl(((char *) recvbuf + recvtype_extent * rank),
                           recvcount, recvtype, tmp_buf, tmp_buf_size, &position);
        }

        curr_cnt = nbytes;

        mask = 0x1;
        i = 0;
        while (mask < comm_size) {
            dst = rank ^ mask;

            /* find offset into send and recv buffers. zero out
             * the least significant "i" bits of rank and dst to
             * find root of src and dst subtrees. Use ranks of
             * roots as index to send from and recv into buffer. */

            dst_tree_root = dst >> i;
            dst_tree_root <<= i;

            my_tree_root = rank >> i;
            my_tree_root <<= i;

            send_offset = my_tree_root * nbytes;
            recv_offset = dst_tree_root * nbytes;

            if (dst < comm_size) {
                MPIR_PVAR_INC(allgather, rd, send, curr_cnt, MPI_BYTE); 
                MPIR_PVAR_INC(allgather, rd, recv, (tmp_buf_size - recv_offset), MPI_BYTE); 
                mpi_errno =
                    MPIC_Sendrecv(((char *) tmp_buf + send_offset),
                                     curr_cnt, MPI_BYTE, dst,
                                     MPIR_ALLGATHER_TAG,
                                     ((char *) tmp_buf + recv_offset),
                                     tmp_buf_size - recv_offset, MPI_BYTE,
                                     dst, MPIR_ALLGATHER_TAG, comm_ptr, &status, errflag);
                if (mpi_errno) {
                    /* for communication errors, just record the error but
                       continue */
                    *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
                    MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
                    MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
                    last_recv_cnt = 0;
                }

                MPIR_Get_count_impl(&status, MPI_BYTE, &last_recv_cnt);
                curr_cnt += last_recv_cnt;
            }

            /* if some processes in this process's subtree in this step
             * did not have any destination process to communicate with
             * because of non-power-of-two, we need to send them the
             * data that they would normally have received from those
             * processes. That is, the haves in this subtree must send to
             * the havenots. We use a logarithmic recursive-halfing
             * algorithm for this. */

            if (dst_tree_root + mask > comm_size) {
                nprocs_completed = comm_size - my_tree_root - mask;
                /* nprocs_completed is the number of processes in this
                 * subtree that have all the data. Send data to others
                 * in a tree fashion. First find root of current tree
                 * that is being divided into two. k is the number of
                 * least-significant bits in this process's rank that
                 * must be zeroed out to find the rank of the root */
                j = mask;
                k = 0;
                while (j) {
                    j >>= 1;
                    k++;
                }
                k--;

                offset = nbytes * (my_tree_root + mask);
                tmp_mask = mask >> 1;

                while (tmp_mask) {
                    dst = rank ^ tmp_mask;
                    tree_root = rank >> k;
                    tree_root <<= k;

                    /* send only if this proc has data and destination
                     * doesn't have data. at any step, multiple processes
                     * can send if they have the data */
                    if ((dst > rank) && (rank < tree_root + nprocs_completed)
                        && (dst >= tree_root + nprocs_completed)) {
                        MPIR_PVAR_INC(allgather, rd, send, last_recv_cnt, MPI_BYTE); 
                        mpi_errno =
                            MPIC_Send(((char *) tmp_buf + offset),
                                         last_recv_cnt, MPI_BYTE, dst,
                                         MPIR_ALLGATHER_TAG, comm_ptr, errflag);
                        /* last_recv_cnt was set in the previous
                         * receive. that's the amount of data to be
                         * sent now. */
                        if (mpi_errno) {
                            /* for communication errors, just record the error
                               but continue */
                            *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
                            MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
                            MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
                        }
                    }
                    /* recv only if this proc. doesn't have data and sender
                     * has data */
                    else if ((dst < rank) &&
                             (dst < tree_root + nprocs_completed) &&
                             (rank >= tree_root + nprocs_completed)) {
                        MPIR_PVAR_INC(allgather, rd, recv, (tmp_buf_size - offset), MPI_BYTE);
                        mpi_errno = MPIC_Recv(((char *) tmp_buf + offset),
                                              tmp_buf_size - offset,
                                              MPI_BYTE, dst,
                                              MPIR_ALLGATHER_TAG, comm_ptr, &status, errflag);
                        /* nprocs_completed is also equal to the
                         * no. of processes whose data we don't have */
                        if (mpi_errno) {
                            /* for communication errors, just record the error
                               but continue */
                            *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
                            MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
                            MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
                            last_recv_cnt = 0;
                        }
                        MPIR_Get_count_impl(&status, MPI_BYTE, &last_recv_cnt);
                        curr_cnt += last_recv_cnt;
                    }
                    tmp_mask >>= 1;
                    k--;
                }
            }
            mask <<= 1;
            i++;
        }

        position = 0;
        MPIR_Unpack_impl(tmp_buf, tmp_buf_size, &position, recvbuf,
                         recvcount * comm_size, recvtype);

        MPIU_Free(tmp_buf);
    }
#endif                          /* MPID_HAS_HETERO */

  fn_fail:
    MPIR_TIMER_END(coll,allgather,rd);
    return (mpi_errno);
}

#undef FUNCNAME
#define FUNCNAME MPIR_Allgather_Bruck_MV2
#undef FCNAME
#define FCNAME MPL_QUOTE(FUNCNAME)
int MPIR_Allgather_Bruck_MV2(const void *sendbuf,
                             int sendcount,
                             MPI_Datatype sendtype,
                             void *recvbuf,
                             int recvcount,
                             MPI_Datatype recvtype, MPID_Comm * comm_ptr,
                             MPIR_Errflag_t *errflag)
{
    MPIR_TIMER_START(coll,allgather,bruck);
    int comm_size, rank;
    int mpi_errno = MPI_SUCCESS;
    int mpi_errno_ret = MPI_SUCCESS;
    MPI_Aint recvtype_extent;
    MPI_Aint recvtype_true_extent, recvbuf_extent, recvtype_true_lb;
    int src, rem;
    void *tmp_buf;
    int curr_cnt, dst;
    int pof2 = 0;

    MPIR_T_PVAR_COUNTER_INC(MV2, mv2_coll_allgather_bruck, 1);

    comm_size = comm_ptr->local_size;
    rank = comm_ptr->rank;

    MPID_Datatype_get_extent_macro(recvtype, recvtype_extent);

    /* get true extent of recvtype */
    MPIR_Type_get_true_extent_impl(recvtype, &recvtype_true_lb, &recvtype_true_extent);
    recvbuf_extent =
        recvcount * comm_size * (MPIR_MAX(recvtype_true_extent, recvtype_extent));
    /* allocate a temporary buffer of the same size as recvbuf. */
    tmp_buf = MPIU_Malloc(recvbuf_extent);
    /* --BEGIN ERROR HANDLING-- */
    if (!tmp_buf) {
        mpi_errno =
            MPIR_Err_create_code(MPI_SUCCESS, MPIR_ERR_RECOVERABLE, FCNAME,
                                 __LINE__, MPI_ERR_OTHER, "**nomem", 0);
        return mpi_errno;
    }
    /* --END ERROR HANDLING-- */

    /* adjust for potential negative lower bound in datatype */
    tmp_buf = (void *) ((char *) tmp_buf - recvtype_true_lb);

    /* copy local data to the top of tmp_buf */
    if (sendbuf != MPI_IN_PLACE) {
        mpi_errno = MPIR_Localcopy(sendbuf, sendcount, sendtype,
                                   tmp_buf, recvcount, recvtype);
        if (mpi_errno) {
            MPIR_ERR_POP(mpi_errno);
        }
    } else {
        mpi_errno = MPIR_Localcopy(((char *) recvbuf +
                                    rank * recvcount * recvtype_extent),
                                   recvcount, recvtype, tmp_buf, recvcount, recvtype);
        if (mpi_errno) {
            MPIR_ERR_POP(mpi_errno);
        }
    }

    /* do the first \floor(\lg p) steps */

    curr_cnt = recvcount;
    pof2 = 1;
    while (pof2 <= comm_size / 2) {
        src = (rank + pof2) % comm_size;
        dst = (rank - pof2 + comm_size) % comm_size;
        MPIR_PVAR_INC(allgather, bruck, send, curr_cnt, recvtype); 
        MPIR_PVAR_INC(allgather, bruck, recv, curr_cnt, recvtype); 
        mpi_errno = MPIC_Sendrecv(tmp_buf, curr_cnt, recvtype, dst,
                                     MPIR_ALLGATHER_TAG,
                                     ((char *) tmp_buf +
                                      curr_cnt * recvtype_extent), curr_cnt,
                                     recvtype, src, MPIR_ALLGATHER_TAG,
                                     comm_ptr, MPI_STATUS_IGNORE, errflag);
        if (mpi_errno) {
            /* for communication errors, just record the error but continue */
            *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
            MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
            MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
        }
        curr_cnt *= 2;
        pof2 *= 2;
    }

    /* if comm_size is not a power of two, one more step is needed */

    rem = comm_size - pof2;
    if (rem) {
        src = (rank + pof2) % comm_size;
        dst = (rank - pof2 + comm_size) % comm_size;
        MPIR_PVAR_INC(allgather, bruck, send, rem * recvcount, recvtype); 
        MPIR_PVAR_INC(allgather, bruck, recv, rem * recvcount, recvtype); 
        mpi_errno = MPIC_Sendrecv(tmp_buf, rem * recvcount, recvtype,
                                     dst, MPIR_ALLGATHER_TAG,
                                     ((char *) tmp_buf +
                                      curr_cnt * recvtype_extent),
                                     rem * recvcount, recvtype, src,
                                     MPIR_ALLGATHER_TAG, comm_ptr,
                                     MPI_STATUS_IGNORE, errflag);
        if (mpi_errno) {
            /* for communication errors, just record the error but continue */
            *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
            MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
            MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
        }
    }

    /* Rotate blocks in tmp_buf down by (rank) blocks and store
     * result in recvbuf. */

    mpi_errno = MPIR_Localcopy(tmp_buf, (comm_size - rank) * recvcount,
                               recvtype,
                               (char *) recvbuf +
                               rank * recvcount * recvtype_extent,
                               (comm_size - rank) * recvcount, recvtype);
    if (mpi_errno) {
        MPIR_ERR_POP(mpi_errno);
    }

    if (rank) {
        mpi_errno = MPIR_Localcopy((char *) tmp_buf +
                                   (comm_size -
                                    rank) * recvcount * recvtype_extent,
                                   rank * recvcount, recvtype, recvbuf,
                                   rank * recvcount, recvtype);
        if (mpi_errno) {
            MPIR_ERR_POP(mpi_errno);
        }
    }

    void *tmp = (void*)(tmp_buf + recvtype_true_lb);
    MPIU_Free(tmp);

  fn_fail:
    MPIR_TIMER_END(coll,allgather,bruck);
    return (mpi_errno);
}

#undef FUNCNAME
#define FUNCNAME MPIR_Allgather_Ring_MV2
#undef FCNAME
#define FCNAME MPL_QUOTE(FUNCNAME)
int MPIR_Allgather_Ring_MV2(const void *sendbuf,
                            int sendcount,
                            MPI_Datatype sendtype,
                            void *recvbuf,
                            int recvcount,
                            MPI_Datatype recvtype, MPID_Comm * comm_ptr,
                            MPIR_Errflag_t *errflag)
{
    MPIR_TIMER_START(coll,allgather,ring);
    int comm_size, rank;
    int mpi_errno = MPI_SUCCESS;
    int mpi_errno_ret = MPI_SUCCESS;
    MPI_Aint recvtype_extent;
    int j, i;
    int left, right, jnext;

    MPIR_T_PVAR_COUNTER_INC(MV2, mv2_coll_allgather_ring, 1);

    comm_size = comm_ptr->local_size;
    rank = comm_ptr->rank;

    MPID_Datatype_get_extent_macro(recvtype, recvtype_extent);

    /* First, load the "local" version in the recvbuf. */
    if (sendbuf != MPI_IN_PLACE) {
        mpi_errno = MPIR_Localcopy(sendbuf, sendcount, sendtype,
                                   ((char *) recvbuf +
                                    rank * recvcount * recvtype_extent),
                                   recvcount, recvtype);
        if (mpi_errno) {
            MPIR_ERR_POP(mpi_errno);
        }
    }

    /*
     * Now, send left to right.  This fills in the receive area in
     * reverse order.
     */
    left = (comm_size + rank - 1) % comm_size;
    right = (rank + 1) % comm_size;

    j = rank;
    jnext = left;
    for (i = 1; i < comm_size; i++) {
        MPIR_PVAR_INC(allgather, ring, send, recvcount, recvtype); 
        MPIR_PVAR_INC(allgather, ring, recv, recvcount, recvtype); 
        mpi_errno = MPIC_Sendrecv(((char *) recvbuf +
                                      j * recvcount * recvtype_extent),
                                     recvcount, recvtype, right,
                                     MPIR_ALLGATHER_TAG,
                                     ((char *) recvbuf +
                                      jnext * recvcount * recvtype_extent),
                                     recvcount, recvtype, left,
                                     MPIR_ALLGATHER_TAG, comm_ptr,
                                     MPI_STATUS_IGNORE, errflag);
        if (mpi_errno) {
            /* for communication errors, just record the error but continue */
            *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
            MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
            MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
        }
        j = jnext;
        jnext = (comm_size + jnext - 1) % comm_size;
    }

  fn_fail:
    MPIR_TIMER_END(coll,allgather,ring);
    return (mpi_errno);
}

/* executes an allgather as a gather followed by a broadcast */
#undef FUNCNAME
#define FUNCNAME MPIR_Allgather_gather_bcast_MV2
#undef FCNAME
#define FCNAME MPL_QUOTE(FUNCNAME)
int MPIR_Allgather_gather_bcast_MV2(
    const void *sendbuf, int sendcount, MPI_Datatype sendtype,
          void *recvbuf, int recvcount, MPI_Datatype recvtype,
    MPID_Comm * comm_ptr, MPIR_Errflag_t *errflag)
{
    MPIR_TIMER_START(coll,allgather,gather_bcast);
    int comm_size;
    int mpi_errno = MPI_SUCCESS;
    int gather_bcast_root = -1;

    MPIR_T_PVAR_COUNTER_INC(MV2, mv2_coll_allgather_gather_bcast, 1);

    comm_size = comm_ptr->local_size;

    MPIU_Assert(comm_ptr->dev.ch.shmem_coll_ok == 1);

    //Call gather (Calling mv2 version so that gather is tuned)
    //If IN_PLACE is used, gather is expected to handle it
    gather_bcast_root = 0;
    mpi_errno = MPIR_Gather_MV2(sendbuf, sendcount, sendtype,
				recvbuf, recvcount, recvtype,
				gather_bcast_root, comm_ptr, errflag);
    if (mpi_errno) {
	MPIR_ERR_POP(mpi_errno);
    }

    //gather_bcast_root has all data at this point

    //call bcast on the receive buffer
    mpi_errno = MPIR_Bcast_MV2(recvbuf, recvcount * comm_size, recvtype,
			       gather_bcast_root, comm_ptr, errflag);
    if (mpi_errno) {
	MPIR_ERR_POP(mpi_errno);
    }

  fn_fail:
    MPIR_TIMER_END(coll,allgather,gather_bcast);
    return (mpi_errno);
}

#undef FUNCNAME
#define FUNCNAME MPIR_Allgather_intra_MV2
#undef FCNAME
#define FCNAME MPL_QUOTE(FUNCNAME)
int MPIR_Allgather_intra_MV2(const void *sendbuf,
                             int sendcount,
                             MPI_Datatype sendtype,
                             void *recvbuf,
                             int recvcount,
                             MPI_Datatype recvtype, MPID_Comm * comm_ptr, MPIR_Errflag_t *errflag)
{
    int comm_size;
    int mpi_errno = MPI_SUCCESS;
    int type_size;
    int comm_size_is_pof2 = 0;

    if (((sendcount == 0) && (sendbuf != MPI_IN_PLACE)) || (recvcount == 0)) {
        return MPI_SUCCESS;
    }

    comm_size = comm_ptr->local_size;

    MPID_Datatype_get_size_macro(recvtype, type_size);

    /* check if comm_size is a power of two */
    comm_size_is_pof2 = comm_ptr->dev.ch.is_pof2;

    /* check if multiple threads are calling this collective function */
    MPIDU_ERR_CHECK_MULTIPLE_THREADS_ENTER(comm_ptr);

    if ((recvcount * type_size <= allgather_tuning(comm_size, comm_size_is_pof2))
        && (comm_size_is_pof2 == 1)) {
        /* Short or medium size message and power-of-two no. of processes. Use
         * recursive doubling algorithm */
        mpi_errno = MPIR_Allgather_RD_MV2(sendbuf, sendcount, sendtype, recvbuf,
                                              recvcount, recvtype, comm_ptr, errflag);   
                    
    } else if (recvcount * type_size <= allgather_tuning(comm_size, comm_size_is_pof2)) {
        /* Short message and non-power-of-two no. of processes. Use
        * Bruck algorithm (see description above). */
        mpi_errno = MPIR_Allgather_Bruck_MV2(sendbuf, sendcount, sendtype, recvbuf,
                                                 recvcount, recvtype, comm_ptr, errflag);

    } else {                
        /* long message or medium-size message and non-power-of-two
        * no. of processes. use ring algorithm. */
        mpi_errno = MPIR_Allgather_Ring_MV2(sendbuf, sendcount, sendtype, recvbuf,
                                                recvcount, recvtype, comm_ptr, errflag);

    }
    
    /* check if multiple threads are calling this collective function */
    MPIDU_ERR_CHECK_MULTIPLE_THREADS_EXIT(comm_ptr);

    return (mpi_errno);
}

#undef FUNCNAME
#define FUNCNAME MPIR_2lvl_Allgather_MV2
#undef FCNAME
#define FCNAME MPL_QUOTE(FUNCNAME)
int MPIR_2lvl_Allgather_MV2(const void *sendbuf,int sendcnt, MPI_Datatype sendtype,
                            void *recvbuf, int recvcnt,MPI_Datatype recvtype,
                            MPID_Comm * comm_ptr, MPIR_Errflag_t *errflag)
{
    int rank, size;
    int local_rank, local_size;
    int leader_comm_size = 0; 
    int mpi_errno = MPI_SUCCESS;
    MPI_Aint recvtype_extent = 0;  /* Datatype extent */
    MPI_Comm shmem_comm, leader_comm;
    MPID_Comm *shmem_commptr=NULL, *leader_commptr = NULL;

    if (recvcnt == 0) {
        return MPI_SUCCESS;
    }

    rank = comm_ptr->rank;
    size = comm_ptr->local_size; 

    /* extract the rank,size information for the intra-node
     * communicator */
    MPID_Datatype_get_extent_macro(recvtype, recvtype_extent);
    
    shmem_comm = comm_ptr->dev.ch.shmem_comm;
    MPID_Comm_get_ptr(shmem_comm, shmem_commptr);
    local_rank = shmem_commptr->rank;
    local_size = shmem_commptr->local_size;

    if (local_rank == 0) {
        /* Node leader. Extract the rank, size information for the leader
         * communicator */
        leader_comm = comm_ptr->dev.ch.leader_comm;
        MPID_Comm_get_ptr(leader_comm, leader_commptr);
        leader_comm_size = leader_commptr->local_size;
    }

    /*If there is just one node, after gather itself,
     * root has all the data and it can do bcast*/
    if(local_rank == 0) {
        mpi_errno = MPIR_Gather_impl(sendbuf, sendcnt,sendtype, 
                                    (void*)((char*)recvbuf + (rank * recvcnt * recvtype_extent)), 
                                     recvcnt, recvtype,
                                     0, shmem_commptr, errflag);
    } else {
        /*Since in allgather all the processes could have 
         * its own data in place*/
        if(sendbuf == MPI_IN_PLACE) {
            mpi_errno = MPIR_Gather_impl((void*)((char*)recvbuf + (rank * recvcnt * recvtype_extent)), 
                                         recvcnt , recvtype, 
                                         recvbuf, recvcnt, recvtype,
                                         0, shmem_commptr, errflag);
        } else {
            mpi_errno = MPIR_Gather_impl(sendbuf, sendcnt,sendtype, 
                                         recvbuf, recvcnt, recvtype,
                                         0, shmem_commptr, errflag);
        }
    }

    if (mpi_errno) {
        MPIR_ERR_POP(mpi_errno);
    }

    /* Exchange the data between the node leaders*/
    if (local_rank == 0 && (leader_comm_size > 1)) {
        /*When data in each socket is different*/
        if (comm_ptr->dev.ch.is_uniform != 1) {

            int *displs = NULL;
            int *recvcnts = NULL;
            int *node_sizes;
            int i = 0;

            node_sizes = comm_ptr->dev.ch.node_sizes;

            displs = MPIU_Malloc(sizeof (int) * leader_comm_size);
            recvcnts = MPIU_Malloc(sizeof (int) * leader_comm_size);
            if (!displs || !recvcnts) {
                mpi_errno = MPIR_Err_create_code(MPI_SUCCESS,
                        MPIR_ERR_RECOVERABLE,
                        FCNAME, __LINE__,
                        MPI_ERR_OTHER,
                        "**nomem", 0);
                return mpi_errno;
            }
            recvcnts[0] = node_sizes[0] * recvcnt;
            displs[0] = 0;

            for (i = 1; i < leader_comm_size; i++) {
                displs[i] = displs[i - 1] + node_sizes[i - 1] * recvcnt;
                recvcnts[i] = node_sizes[i] * recvcnt;
            }


            mpi_errno = MPIR_Allgatherv(MPI_IN_PLACE,
                                       (recvcnt*local_size),
                                       recvtype, 
                                       recvbuf, recvcnts,
                                       displs, recvtype,
                                       leader_commptr, errflag);
            MPIU_Free(displs);
            MPIU_Free(recvcnts);
        } else {
            mpi_errno = MV2_Allgather_function(MPI_IN_PLACE, 
                                               (recvcnt*local_size),
                                               recvtype,
                                               recvbuf, (recvcnt*local_size), recvtype,
                                             leader_commptr, errflag);

        }

        if (mpi_errno) {
            MPIR_ERR_POP(mpi_errno);
        }
    }

    /*Bcast the entire data from node leaders to all other cores*/
    mpi_errno = MPIR_Bcast_impl (recvbuf, recvcnt * size, recvtype, 0, shmem_commptr, errflag);
    if (mpi_errno) {
        MPIR_ERR_POP(mpi_errno);
    }
  
  fn_fail:
    return (mpi_errno);
}
/* end:nested */

/* This implements an allgather via a gather to leader processes,
 * an allgather across leaders, and a broadcast from each leader.
 * First all procs on a node execute a gather to the leader process.
 * Then the leaders exchange data via allgather/allgatherv.
 * Finally, the leaders broadcast the data to all procs on the node.
 * In this version, ranks do not need to be ordered, because the
 * leader will unpack data from a temporary buffer to the receive
 * buffer in the correct order before the broadcast. */
#undef FUNCNAME
#define FUNCNAME MPIR_2lvl_Allgather_nonblocked_MV2
#undef FCNAME
#define FCNAME MPL_QUOTE(FUNCNAME)
int MPIR_2lvl_Allgather_nonblocked_MV2(
    const void *sendbuf, int sendcnt, MPI_Datatype sendtype,
          void *recvbuf, int recvcnt, MPI_Datatype recvtype,
    MPID_Comm * comm_ptr, MPIR_Errflag_t *errflag)
{
    MPIR_TIMER_START(coll,allgather,2lvl_nonblocked);
    int i;
    int mpi_errno = MPI_SUCCESS;

    MPIR_T_PVAR_COUNTER_INC(MV2, mv2_coll_allgather_2lvl_nonblocked, 1);

    if (recvcnt == 0) {
        return MPI_SUCCESS;
    }

    /* get our rank and the size of this communicator */
    int rank = comm_ptr->rank;
    int size = comm_ptr->local_size;
    int* node_sizes = comm_ptr->dev.ch.node_sizes;

    /* get extent of receive type */
    MPI_Aint recvtype_extent;
    MPID_Datatype_get_extent_macro(recvtype, recvtype_extent);

    /* get true extent of recvtype */
    MPI_Aint recvtype_true_lb, recvtype_true_extent;
    MPIR_Type_get_true_extent_impl(recvtype, &recvtype_true_lb, &recvtype_true_extent);

    /* get info about communicator for ranks on the same node */
    MPID_Comm* shmem_commptr;
    MPI_Comm shmem_comm = comm_ptr->dev.ch.shmem_comm;
    MPID_Comm_get_ptr(shmem_comm, shmem_commptr);
    int local_rank = shmem_commptr->rank;
    int local_size = shmem_commptr->local_size;

    /* TODO: if ranks are in order, we can avoid the temp buffer and memcpy */
    int need_temp = 1;

    /* get info about communicator across node leaders, allocate temp buffer */
    MPID_Comm* leader_commptr = NULL;
    int leader_rank = -1;
    int leader_size = 0;
    void* tmpbuf = recvbuf;
    if (local_rank == 0) {
        /* Node leader. Extract the rank, size information for the leader
         * communicator */
        MPI_Comm leader_comm = comm_ptr->dev.ch.leader_comm;
        MPID_Comm_get_ptr(leader_comm, leader_commptr);
        leader_rank = leader_commptr->rank;
        leader_size = leader_commptr->local_size;

        /* allocate a temporary buffer */
        if (need_temp) {
            tmpbuf = MPIU_Malloc(size * recvcnt * recvtype_extent);
            if (!tmpbuf) {
                mpi_errno = MPIR_Err_create_code(MPI_SUCCESS, MPIR_ERR_RECOVERABLE,
                                                 FCNAME, __LINE__, MPI_ERR_OTHER,
                                                 "**nomem", 0);
                return mpi_errno;
            }

            /* adjust for potential negative lower bound in datatype */
            tmpbuf = (void*)((char*) tmpbuf - recvtype_true_lb);
        }
    }

    /* ----------------------------------------------
     * Gather data from procs on same node to leader
     * ---------------------------------------------- */

    /* gather data to leaders on each node */
    if (local_rank == 0) {
        /* compute number of items to receive ahead of our spot in the buffer */
        MPI_Aint preceding_count = 0;
        for (i=0; i < leader_rank; i++) {
            preceding_count += node_sizes[i] * recvcnt;
        }

        /* compute location to receive data from procs on our node */
        void* rbuf = (void*)((char*)tmpbuf + (preceding_count * recvtype_extent));

        /* gather data from procs on our node */
        if (sendbuf == MPI_IN_PLACE) {
            /* data is in our receive buffer indexed by our rank */
            void* sbuf = (void*)((char*)recvbuf + (rank * recvcnt * recvtype_extent));
            mpi_errno = MPIR_Gather_impl(sbuf, recvcnt, recvtype,
                                         rbuf, recvcnt, recvtype,
                                         0, shmem_commptr, errflag);
        } else {
            mpi_errno = MPIR_Gather_impl(sendbuf, sendcnt, sendtype,
                                            rbuf, recvcnt, recvtype,
                                         0, shmem_commptr, errflag);
        }
    } else {
        /* send data to leader on our node */
        if (sendbuf == MPI_IN_PLACE) {
            /* data is in our receive buffer indexed by our rank */
            void* sbuf = (void*)((char*)recvbuf + (rank * recvcnt * recvtype_extent));
            mpi_errno = MPIR_Gather_impl(sbuf, recvcnt, recvtype,
                                         NULL, recvcnt, recvtype,
                                         0, shmem_commptr, errflag);
        } else {
            mpi_errno = MPIR_Gather_impl(sendbuf, sendcnt, sendtype,
                                         NULL,    recvcnt, recvtype,
                                         0, shmem_commptr, errflag);
        }
    }

    if (mpi_errno) {
        MPIR_ERR_POP(mpi_errno);
    }

    /* ----------------------------------------------
     * Execute allgather across leaders
     * ---------------------------------------------- */

    /*If there is just one node, after gather itself,
     * root has all the data and it can do bcast*/

    /* Exchange the data between the node leaders */
    if (local_rank == 0 && (leader_size > 1)) {
        /* When data in each socket is different */
        if (comm_ptr->dev.ch.is_uniform != 1) {
            /* allocate memory for counts and displacements arrays */
            int* displs = MPIU_Malloc(sizeof (int) * leader_size);
            int* counts = MPIU_Malloc(sizeof (int) * leader_size);
            if (!displs || !counts) {
                mpi_errno = MPIR_Err_create_code(MPI_SUCCESS,
                        MPIR_ERR_RECOVERABLE,
                        FCNAME, __LINE__,
                        MPI_ERR_OTHER,
                        "**nomem", 0);
                return mpi_errno;
            }

            /* set values in our counts and displacements arrays */
            displs[0] = 0;
            counts[0] = node_sizes[0] * recvcnt;
            for (i = 1; i < leader_size; i++) {
                displs[i] = displs[i - 1] + node_sizes[i - 1] * recvcnt;
                counts[i] = node_sizes[i] * recvcnt;
            }

            /* execute allgatherv across leader ranks */
            mpi_errno = MPIR_Allgatherv_impl(MPI_IN_PLACE, (recvcnt*local_size), recvtype,
                                             tmpbuf, counts, displs, recvtype,
                                             leader_commptr, errflag);

            /* free counts and displacements arrays */
            MPIU_Free(displs);
            MPIU_Free(counts);
        } else {
            /* execute allgather across leader ranks */
            mpi_errno = MPIR_Allgather_impl(MPI_IN_PLACE, (recvcnt*local_size), recvtype,
                                            tmpbuf, (recvcnt*local_size), recvtype,
                                            leader_commptr, errflag);

        }

        if (mpi_errno) {
            MPIR_ERR_POP(mpi_errno);
        }
    }

    /* ----------------------------------------------
     * Unpack data into receive buffer in correct order
     * ---------------------------------------------- */

    /* ----------------------------------------------
     * TODO: can avoid temp buffer and mem copy when in block
     * ---------------------------------------------- */
    /* ----------------------------------------------
     * TODO: faster memcpy available?
     * ---------------------------------------------- */

    /* leaders copy data from temp buffer to receive buffer in correct order */
    if (local_rank == 0 && need_temp) {
        /* point to start of temp buffer */
        char* sbuf = (char*)tmpbuf;

        /* copy data for each rank from temp buffer to receive buffer */
        for (i = 0; i < size; i++) {
            /* get next rank in list */
            int dstrank = comm_ptr->dev.ch.rank_list[i];

            /* compute position in receive buffer for this rank */
            void* rbuf = (void*)((char*)recvbuf + dstrank * recvcnt * recvtype_extent);

            /* copy data to its correct place */
            mpi_errno = MPIR_Localcopy(sbuf, recvcnt, recvtype,
                                       rbuf, recvcnt, recvtype);
            if (mpi_errno) {
                MPIR_ERR_POP(mpi_errno);
            }

            /* update pointer to next spot in temp buffer */
            sbuf += recvcnt * recvtype_extent;
        }

        /* free the temporary buffer if we allocated one */
        tmpbuf = (void*)((char*)tmpbuf + recvtype_true_lb);
        MPIU_Free(tmpbuf);
    }

    /* ----------------------------------------------
     * Broadcast receive buffer from leader to all procs on the node
     * ---------------------------------------------- */

    /* Bcast the entire data from node leaders to other ranks on node */
    mpi_errno = MPIR_Bcast_impl(recvbuf, recvcnt * size, recvtype, 0, shmem_commptr, errflag);
    if (mpi_errno) {
        MPIR_ERR_POP(mpi_errno);
    }

  fn_fail:
    MPIR_TIMER_END(coll,allgather,2lvl_nonblocked);
    return (mpi_errno);
}

/* Execute an allgather by forwarding data through a ring of
 * processes.  This implementation uses the two-level data
 * structures to account for how procs are assigned to nodes
 * to ensure data is only sent into and out of each node once. */
#undef FUNCNAME
#define FUNCNAME MPIR_2lvl_Allgather_Ring_nonblocked_MV2
#undef FCNAME
#define FCNAME MPL_QUOTE(FUNCNAME)
int MPIR_2lvl_Allgather_Ring_nonblocked_MV2(
    const void *sendbuf, int sendcount, MPI_Datatype sendtype,
          void *recvbuf, int recvcount, MPI_Datatype recvtype,
    MPID_Comm * comm_ptr, MPIR_Errflag_t *errflag)
{
    MPIR_TIMER_START(coll,allgather,2lvl_ring_nonblocked);
    int mpi_errno = MPI_SUCCESS;
    int mpi_errno_ret = MPI_SUCCESS;
    int i;

    /* get our rank and the size of this communicator */
    int rank = comm_ptr->rank;
    int size = comm_ptr->local_size;

    /* get extent of receive type */
    MPI_Aint recvtype_extent;
    MPID_Datatype_get_extent_macro(recvtype, recvtype_extent);

    MPIR_T_PVAR_COUNTER_INC(MV2, mv2_coll_allgather_2lvl_ring_nonblocked, 1);

    /* First, load the "local" version in the recvbuf. */
    if (sendbuf != MPI_IN_PLACE) {
        /* compute location in receive buffer for our data */
        void* rbuf = (void*)((char*) recvbuf + rank * recvcount * recvtype_extent);

        /* copy data from send buffer to receive buffer */
        mpi_errno = MPIR_Localcopy(
            sendbuf, sendcount, sendtype,
            rbuf,    recvcount, recvtype
        );
        if (mpi_errno) {
            MPIR_ERR_POP(mpi_errno);
        }
    }

    /* Now, send left to right. */

    /* lookup our index in the rank list */
    int rank_index = comm_ptr->dev.ch.rank_list_index;

    /* compute the left and right neighbor ranks in the rank_list */
    int left_index  = (size + rank_index - 1) % size;
    int right_index = (size + rank_index + 1) % size;
    int left  = comm_ptr->dev.ch.rank_list[left_index];
    int right = comm_ptr->dev.ch.rank_list[right_index];

    /* execute ring exchange, start by sending our own data to the right
     * and receiving the data from the rank to our left */
    int send_index = rank_index;
    int recv_index = left_index;
    for (i = 1; i < size; i++) {
        /* compute ranks whose data we'll send and receive in this step */
        int send_rank = comm_ptr->dev.ch.rank_list[send_index];
        int recv_rank = comm_ptr->dev.ch.rank_list[recv_index];

        /* compute position within buffer to send from and receive into */
        void* sbuf = (void*)((char*) recvbuf + send_rank * recvcount * recvtype_extent);
        void* rbuf = (void*)((char*) recvbuf + recv_rank * recvcount * recvtype_extent);

        /* exchange data with our neighbors in the ring */
        MPIR_PVAR_INC(allgather, 2lvl_ring_nonblocked, send, recvcount, recvtype); 
        MPIR_PVAR_INC(allgather, 2lvl_ring_nonblocked, recv, recvcount, recvtype); 
        mpi_errno = MPIC_Sendrecv(
           sbuf, recvcount, recvtype, right, MPIR_ALLGATHER_TAG,
           rbuf, recvcount, recvtype, left,  MPIR_ALLGATHER_TAG,
           comm_ptr, MPI_STATUS_IGNORE, errflag
        );
        if (mpi_errno) {
            /* for communication errors, just record the error but continue */
            *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
            MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
            MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
        }

        /* update index values to account for data we just received */
        send_index = recv_index;
        recv_index = (size + recv_index - 1) % size;
    }

  fn_fail:
    MPIR_TIMER_END(coll,allgather,2lvl_ring_nonblocked);
    return (mpi_errno);
}

/* In this implementation, we "gather" data from all procs on a node
 * to their leader.  This is done with direct send/recv and we write
 * the data directly to the receive buffer on the leader process.
 * The leaders then execute an "allgather" by directly sending each
 * of these messages. Finally, we broadcast the final receive buffer
 * to the procs on the node. */
#undef FUNCNAME
#define FUNCNAME MPIR_2lvl_Allgather_Direct_MV2
#undef FCNAME
#define FCNAME MPL_QUOTE(FUNCNAME)
int MPIR_2lvl_Allgather_Direct_MV2(
    const void *sendbuf, int sendcnt, MPI_Datatype sendtype,
          void *recvbuf, int recvcnt, MPI_Datatype recvtype,
    MPID_Comm *comm_ptr, MPIR_Errflag_t *errflag)
{
    MPIR_TIMER_START(coll,allgather,2lvl_direct);
    int i, j;
    int mpi_errno = MPI_SUCCESS;
    int mpi_errno_ret = MPI_SUCCESS;

    MPIR_T_PVAR_COUNTER_INC(MV2, mv2_coll_allgather_2lvl_direct, 1);

    if (recvcnt == 0) {
        return MPI_SUCCESS;
    }

    MPIU_CHKLMEM_DECL(2);

    /* get our rank and the size of this communicator */
    int rank = comm_ptr->rank;
    int size = comm_ptr->local_size;

    /* get extent of receive type */
    MPI_Aint recvtype_extent;
    MPID_Datatype_get_extent_macro(recvtype, recvtype_extent);

    /* get info about communicator for ranks on the same node */
    MPID_Comm* shmem_commptr;
    MPI_Comm shmem_comm = comm_ptr->dev.ch.shmem_comm;
    MPID_Comm_get_ptr(shmem_comm, shmem_commptr);
    int local_rank = shmem_commptr->rank;
    int local_size = shmem_commptr->local_size;

    /* get info about communicator across node leaders, allocate temp buffer */
    MPID_Comm* leader_commptr = NULL;
    int leader_rank = -1;
    int leader_size = 0;
    if (local_rank == 0) {
        /* Node leader. Extract the rank, size information for the leader
         * communicator */
        MPI_Comm leader_comm = comm_ptr->dev.ch.leader_comm;
        MPID_Comm_get_ptr(leader_comm, leader_commptr);
        leader_rank = leader_commptr->rank;
        leader_size = leader_commptr->local_size;
    }

    int gather_msgs    = 0;
    int allgather_msgs = 0;

    /* request/status object allocation is different for 
     * leader vs. non-leader */
    if (local_rank == 0) {
        gather_msgs    = local_size - 1; 
        allgather_msgs = (local_size * (leader_size - 1)) + (size - local_size);
    } else {
        /* if non-leader only send one msg in gather step */
        gather_msgs = 1;
    }

    /* now get the max of these two because the gather
     * completes before the allgather to save space */ 
    int max = allgather_msgs; 
    if (gather_msgs > max) {
        max = gather_msgs;
    }

    /* allocate memory for request objects */
    MPID_Request **reqarray = NULL;
    MPIU_CHKLMEM_MALLOC(reqarray, MPID_Request **,
                        max * sizeof (MPID_Request*),
                        mpi_errno, "reqarray");

    
    /* allocate memory for status objects */
    MPI_Status *starray = NULL;
    MPIU_CHKLMEM_MALLOC(starray, MPI_Status *,
                        max * sizeof (MPI_Status),
                        mpi_errno, "starray");

    /****************************
     * Gather data to leaders using direct send/recv
     ****************************/

    /* track number of requests */
    int reqs = 0;

    /* gather data to leaders on each node */
    int rank_index = comm_ptr->dev.ch.rank_list_index;
    if (local_rank == 0) {
        /* post receives & point i at leader rank in the rank_list */
        for (i = 1; i < local_size; i++) {
            /* get global rank of incoming data */
            int dstrank = comm_ptr->dev.ch.rank_list[rank_index + i];

            /* compute pointer in receive buffer for incoming data from this rank */
            void* rbuf = (void*)((char*) recvbuf + dstrank * recvcnt * recvtype_extent);

            /* post receive for data from this rank */
            MPIR_PVAR_INC(allgather, 2lvl_direct, recv, recvcnt, recvtype);
            mpi_errno = MPIC_Irecv(rbuf, recvcnt, recvtype,
                i, MPIR_ALLGATHER_TAG, shmem_commptr, &reqarray[reqs++]
            );

            if (mpi_errno) {
                /* for communication errors, just record the error but continue */
                *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
                MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
                MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
            }
       }

       /* copy our data to our receive buffer if needed */
       if (sendbuf != MPI_IN_PLACE) {
           /* compute location in receive buffer for our data */
           void* rbuf = (void*)((char*) recvbuf + rank * recvcnt * recvtype_extent);
           mpi_errno = MPIR_Localcopy(
               sendbuf, sendcnt, sendtype,
               rbuf, recvcnt, recvtype
           );
           if (mpi_errno) {
               MPIR_ERR_POP(mpi_errno);
           }
       }
    } else {
        /* get parameters for sending data */
        const void* sbuf   = sendbuf;
        int scnt           = sendcnt;
        MPI_Datatype stype = sendtype;
        if (sendbuf == MPI_IN_PLACE) {
            /* use receive params if IN_PLACE */
            sbuf   = (void*)((char*) recvbuf + rank * recvcnt * recvtype_extent);
            scnt   = recvcnt;
            stype  = recvtype;
        }

        /* send data to the leader process */
        MPIR_PVAR_INC(allgather, 2lvl_direct, send, scnt, stype); 
        mpi_errno = MPIC_Isend(sbuf, scnt, stype,
           0, MPIR_ALLGATHER_TAG, shmem_commptr, &reqarray[reqs++], errflag
        );
        if (mpi_errno) {
            /* for communication errors, just record the error but continue */
            *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
            MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
            MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
        }
   }

    /* wait for all outstanding requests to complete */
    mpi_errno = MPIC_Waitall(reqs, reqarray, starray, errflag);
    if (mpi_errno && mpi_errno != MPI_ERR_IN_STATUS) {
           MPIR_ERR_POP(mpi_errno);
    }

    /* --BEGIN ERROR HANDLING-- */
    if (mpi_errno == MPI_ERR_IN_STATUS) {
        for (i = 0; i < reqs; i++) {
            if (starray[i].MPI_ERROR != MPI_SUCCESS) {
                mpi_errno = starray[i].MPI_ERROR;
                if (mpi_errno) {
                    /* for communication errors, just record the error but continue */
                    *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
                    MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
                    MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
                }
            }
        }
    }

    /****************************
     * Execute direct-send allgather across leaders
     ****************************/

    /*If there is just one node, after gather itself,
     * root has all the data and it can do bcast*/

    /* Exchange the data between the node leaders */
    if (local_rank == 0) {
        /* initialize the active request counter */
        reqs = 0;

        /* post receives */
        for (i = 1; i < leader_size; i++) {
            /* compute source rank sending to us in this step */
            int src = leader_rank - i;
            if (src < 0) {
                src += leader_size;
            }

            /* receive all messages from each rank on src node */
            int recv_count = comm_ptr->dev.ch.node_sizes[src];
            int recv_displ = comm_ptr->dev.ch.node_disps[src];
            for (j = 0; j < recv_count; j++) {
                /* get rank of incoming data */
                int srcrank = comm_ptr->dev.ch.rank_list[recv_displ + j];

                /* get pointer to receive buffer for this rank */
                void* rbuf = (void*)((char*) recvbuf + srcrank * recvcnt * recvtype_extent);

                /* post receive */
                MPIR_PVAR_INC(allgather, 2lvl_direct, recv, recvcnt, recvtype);
                mpi_errno = MPIC_Irecv(rbuf, recvcnt, recvtype,
                    src, MPIR_ALLGATHER_TAG, leader_commptr, &reqarray[reqs++]
                );
                if (mpi_errno) {
                    /* for communication errors, just record the error but continue */
                    *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
                    MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
                    MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
                }
            }
        }

        /* post sends */
        for (i = 1; i < leader_size; i++) {
            /* compute destination rank for this step */
            int dst = leader_rank + i;
            if (dst >= leader_size) {
                dst -= leader_size;
            }

            /* send all messages for this leader to each other leader rank */
            int send_count = comm_ptr->dev.ch.node_sizes[leader_rank];
            for (j = 0; j < send_count; j++) {
                /* get source rank for data we'll send */
                int dstrank = comm_ptr->dev.ch.rank_list[rank_index + j];

                /* get pointer into buffer for this rank */
                void* sbuf = (void*)((char*) recvbuf + dstrank * recvcnt * recvtype_extent);

                /* post send to this destination rank */
                MPIR_PVAR_INC(allgather, 2lvl_direct, send, sendcnt, sendtype); 
                mpi_errno = MPIC_Isend(sbuf, sendcnt, sendtype,
                    dst, MPIR_ALLGATHER_TAG, leader_commptr, &reqarray[reqs++], errflag
                );

                if (mpi_errno) {
                    /* for communication errors, just record the error but continue */
                    *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
                    MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
                    MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
                }
            }
        }

        /* wait for all outstanding requests to complete */
        mpi_errno = MPIC_Waitall(reqs, reqarray, starray, errflag);
        if (mpi_errno && mpi_errno != MPI_ERR_IN_STATUS) {
           MPIR_ERR_POP(mpi_errno);
        }

        /* --BEGIN ERROR HANDLING-- */
        if (mpi_errno == MPI_ERR_IN_STATUS) {
            for (i = 0; i < reqs; i++) {
                if (starray[i].MPI_ERROR != MPI_SUCCESS) {
                    mpi_errno = starray[i].MPI_ERROR;
                    if (mpi_errno) {
                        /* for communication errors, just record the error but continue */
                        *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
                        MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
                        MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
                    }
                }
            }
        }
    }

    /****************************
     * Broadcast data from leaders to procs on the node
     ****************************/

    /* Bcast the entire data from node leaders to other ranks on node */
    mpi_errno = MPIR_Bcast_impl(recvbuf, recvcnt * size, recvtype, 0, shmem_commptr, errflag);
    if (mpi_errno) {
        MPIR_ERR_POP(mpi_errno);
    }

  fn_exit:
    MPIU_CHKLMEM_FREEALL();

  fn_fail:
    MPIR_TIMER_END(coll,allgather,2lvl_direct);
    return (mpi_errno);
}

/* In this implementation, we "gather" data from all procs on a node
 * to their leader.  This is done with direct send/recv and we write
 * the data directly to the receive buffer on the leader process.
 * The leaders then execute an "allgather" by directly sending each
 * of these messages.  We use a ring algorithm to forward data through
 * leaders.  Finally, we broadcast the final receive buffer to the
 * procs on the node. */
#undef FUNCNAME
#define FUNCNAME MPIR_2lvl_Allgather_Ring_MV2
#undef FCNAME
#define FCNAME MPL_QUOTE(FUNCNAME)
int MPIR_2lvl_Allgather_Ring_MV2(
    const void *sendbuf, int sendcnt, MPI_Datatype sendtype,
          void *recvbuf, int recvcnt, MPI_Datatype recvtype,
    MPID_Comm *comm_ptr, MPIR_Errflag_t *errflag)
{
    MPIR_TIMER_START(coll,allgather,2lvl_ring);
    int i, j;
    int mpi_errno = MPI_SUCCESS;
    int mpi_errno_ret = MPI_SUCCESS;

    MPIR_T_PVAR_COUNTER_INC(MV2, mv2_coll_allgather_2lvl_ring, 1);

    if (recvcnt == 0) {
        return MPI_SUCCESS;
    }

    MPIU_CHKLMEM_DECL(2);

    /* get our rank and the size of this communicator */
    int rank = comm_ptr->rank;
    int size = comm_ptr->local_size;

    /* get extent of receive type */
    MPI_Aint recvtype_extent;
    MPID_Datatype_get_extent_macro(recvtype, recvtype_extent);

    /* get info about communicator for ranks on the same node */
    MPID_Comm* shmem_commptr;
    MPI_Comm shmem_comm = comm_ptr->dev.ch.shmem_comm;
    MPID_Comm_get_ptr(shmem_comm, shmem_commptr);
    int local_rank = shmem_commptr->rank;
    int local_size = shmem_commptr->local_size;

    /* get info about communicator across node leaders */
    MPID_Comm* leader_commptr = NULL;
    int leader_rank = -1;
    int leader_size = 0;
    if (local_rank == 0) {
        /* Node leader. Extract the rank, size information for the leader
         * communicator */
        MPI_Comm leader_comm = comm_ptr->dev.ch.leader_comm;
        MPID_Comm_get_ptr(leader_comm, leader_commptr);
        leader_rank = leader_commptr->rank;
        leader_size = leader_commptr->local_size;
    }

    int gather_msgs    = 0;
    int allgather_msgs = 0;
    int max_local_size = 0;
    int *node_sizes    = comm_ptr->dev.ch.node_sizes;

    /* request/status object allocation is different for 
     * leader vs. non-leader */
    if (local_rank == 0) {
        gather_msgs = local_size - 1; 
        for (i = 0; i < leader_size; i++) {
            if (node_sizes[i] > max_local_size) {
                max_local_size = node_sizes[i];
            } 
        }
        allgather_msgs = 2 * max_local_size;
    } else {
        /* if non-leader only send one msg in gather step */
        gather_msgs = 1;
    }

    /* now get the max of these two because the gather
     * completes before the allgather to save space */ 
    int max = allgather_msgs; 
    if (gather_msgs > max) {
        max = gather_msgs;
    }

    /* allocate array of request objects */
    MPID_Request **reqarray = NULL;
    MPIU_CHKLMEM_MALLOC(reqarray, MPID_Request **,
                        max * sizeof (MPID_Request*),
                        mpi_errno, "reqarray");

    /* allocate array of status objects */
    MPI_Status *starray = NULL;
    MPIU_CHKLMEM_MALLOC(starray, MPI_Status *,
                        max * sizeof (MPI_Status),
                        mpi_errno, "starray");

    /****************************
     * Gather data to leaders using direct send/recv
     ****************************/

    int reqs = 0;

    /* gather data to leaders on each node */
    int rank_index = comm_ptr->dev.ch.rank_list_index;
    if (local_rank == 0) {
        /* post receives for incoming data from procs on our node */
        for (i = 1; i < local_size; i++) {
            /* get global rank of this process */
            int srcrank = comm_ptr->dev.ch.rank_list[rank_index + i];

            /* compute pointer in receive buffer for incoming data from this rank */
            void* rbuf = (void*)((char*) recvbuf + srcrank * recvcnt * recvtype_extent);

            /* post receive for data from this rank on shared mem comm */
            MPIR_PVAR_INC(allgather, 2lvl_ring, recv, recvcnt, recvtype);
            mpi_errno = MPIC_Irecv(rbuf, recvcnt, recvtype,
                i, MPIR_ALLGATHER_TAG, shmem_commptr, &reqarray[reqs++]
            );
            if (mpi_errno) {
                /* for communication errors, just record the error but continue */
                *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
                MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
                MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
            }
       }

       /* copy our data to our receive buffer if needed */
       if (sendbuf != MPI_IN_PLACE) {
           /* compute location in receive buffer for our data */
           void* rbuf = (void*)((char*) recvbuf + rank * recvcnt * recvtype_extent);
           mpi_errno = MPIR_Localcopy(
               sendbuf, sendcnt, sendtype,
               rbuf, recvcnt, recvtype
           );
           if (mpi_errno) {
               MPIR_ERR_POP(mpi_errno);
           }
       }
    } else {
        /* get parameters for sending data */
        const void* sbuf   = sendbuf;
        int scnt           = sendcnt;
        MPI_Datatype stype = sendtype;
        if (sendbuf == MPI_IN_PLACE) {
            /* use receive params if IN_PLACE */
            sbuf   = (void*)((char*) recvbuf + rank * recvcnt * recvtype_extent);
            scnt   = recvcnt;
            stype  = recvtype;
        }

        /* send data to leader of our node */
        MPIR_PVAR_INC(allgather, 2lvl_ring, send, scnt, stype); 
        mpi_errno = MPIC_Isend(sbuf, scnt, stype,
           0, MPIR_ALLGATHER_TAG, shmem_commptr, &reqarray[reqs++], errflag
        );
        if (mpi_errno) {
            /* for communication errors, just record the error but continue */
            *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
            MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
            MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
        }
   }

   /* wait for all outstanding requests to complete */
   mpi_errno = MPIC_Waitall(reqs, reqarray, starray, errflag);
   if (mpi_errno && mpi_errno != MPI_ERR_IN_STATUS) {
          MPIR_ERR_POP(mpi_errno);
   }

   /* --BEGIN ERROR HANDLING-- */
   if (mpi_errno == MPI_ERR_IN_STATUS) {
       for (i = 0; i < reqs; i++) {
           if (starray[i].MPI_ERROR != MPI_SUCCESS) {
               mpi_errno = starray[i].MPI_ERROR;
               if (mpi_errno) {
                   /* for communication errors, just record the error but continue */
                   *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
                   MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
                   MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
               }
           }
       }
   }

   if (mpi_errno) {
       MPIR_ERR_POP(mpi_errno);
   }

    /****************************
     * Execute ring-based allgather across leaders
     ****************************/

    /* Exchange the data between the node leaders */
    if (local_rank == 0 && leader_size > 1) {
        /* get our left and right ranks in our leader comm */
        int left  = (leader_size + leader_rank - 1) % leader_size;
        int right = (leader_size + leader_rank + 1) % leader_size;

        /* start by sending our own data and receiving data
         * from the leader to our left */
        int send_index = leader_rank;
        int recv_index = left;
        for (i = 0; i < leader_size; i++) {
            /* initalize our request counter */
            reqs = 0;

            /* post receives for data coming from the left */
            int recv_count = comm_ptr->dev.ch.node_sizes[recv_index];
            int recv_displ = comm_ptr->dev.ch.node_disps[recv_index];
            for (j = 0; j < recv_count; j++) {
                /* get source rank for this message */
                int srcrank = comm_ptr->dev.ch.rank_list[recv_displ + j];

                /* compute pointer in receive buffer for incoming data from this rank */
                void* rbuf = (void*)((char*) recvbuf + srcrank * recvcnt * recvtype_extent);

                /* post receive for data from this rank */
                MPIR_PVAR_INC(allgather, 2lvl_ring, recv, recvcnt, recvtype);
                mpi_errno = MPIC_Irecv(rbuf, recvcnt, recvtype,
                    left, MPIR_ALLGATHER_TAG, leader_commptr, &reqarray[reqs++]
                );
                if (mpi_errno) {
                    /* for communication errors, just record the error but continue */
                    *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
                    MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
                    MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
                }
            }

            /* TODO: consider placing a barrier here to ensure
             * receives are posted before sends, especially for large messages */
            //MPIR_Barrier_impl(comm_ptr);

            /* post sends for data we're sending to the right */
            int send_count = comm_ptr->dev.ch.node_sizes[send_index];
            int send_displ = comm_ptr->dev.ch.node_disps[send_index];
            for (j = 0; j < send_count; j++) {
                /* get source rank for this message */
                int dstrank = comm_ptr->dev.ch.rank_list[send_displ + j];

                /* compute pointer in receive buffer for outgoing data from this rank */
                void* sbuf = (void*)((char*) recvbuf + dstrank * recvcnt * recvtype_extent);

                /* post send for data from this rank */
                MPIR_PVAR_INC(allgather, 2lvl_ring, send, recvcnt, recvtype); 
                mpi_errno = MPIC_Isend(sbuf, recvcnt, recvtype,
                    right, MPIR_ALLGATHER_TAG, leader_commptr, &reqarray[reqs++], errflag
                );
                if (mpi_errno) {
                    /* for communication errors, just record the error but continue */
                    *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
                    MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
                    MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
                }
            }

            /* wait for all outstanding requests to complete */
            mpi_errno = MPIC_Waitall(reqs, reqarray, starray, errflag);
            if (mpi_errno && mpi_errno != MPI_ERR_IN_STATUS) {
                   MPIR_ERR_POP(mpi_errno);
            }

            /* --BEGIN ERROR HANDLING-- */
            if (mpi_errno == MPI_ERR_IN_STATUS) {
                for (i = 0; i < reqs; i++) {
                    if (starray[i].MPI_ERROR != MPI_SUCCESS) {
                        mpi_errno = starray[i].MPI_ERROR;
                        if (mpi_errno) {
                            /* for communication errors, just record the error but continue */
                            *errflag = MPIR_ERR_GET_CLASS(mpi_errno);
                            MPIR_ERR_SET(mpi_errno, MPI_ERR_OTHER, "**fail");
                            MPIR_ERR_ADD(mpi_errno_ret, mpi_errno);
                        }
                    }
                }
            }

            /* update index values so in the next step, we send the data
             * that we just received */
            send_index = recv_index;
            recv_index = (leader_size + recv_index - 1) % leader_size;
        }
    }

    /****************************
     * Broadcast data from leaders to procs on the node
     ****************************/

    /* Bcast the entire data from node leaders to other ranks on node */
    mpi_errno = MPIR_Bcast_impl(recvbuf, recvcnt * size, recvtype, 0, shmem_commptr, errflag);
    if (mpi_errno) {
        MPIR_ERR_POP(mpi_errno);
    }

  fn_exit:
    MPIU_CHKLMEM_FREEALL();

  fn_fail:
    MPIR_TIMER_END(coll,allgather,2lvl_ring);
    return (mpi_errno);
}

#undef FUNCNAME
#define FUNCNAME MPIR_Allgather_index_tuned_intra_MV2
#undef FCNAME
#define FCNAME MPL_QUOTE(FUNCNAME)
int MPIR_Allgather_index_tuned_intra_MV2(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                       void *recvbuf, int recvcount, MPI_Datatype recvtype,
                       MPID_Comm * comm_ptr, MPIR_Errflag_t *errflag)
{

    int mpi_errno = MPI_SUCCESS;
    int nbytes = 0, comm_size, recvtype_size;
    int comm_size_index = 0;
    int inter_node_algo_index = 0;
    int local_size = 0;
    int partial_sub_ok = 0;
    int conf_index = 0;
    int table_min_comm_size = 0;
    int table_max_comm_size = 0;
    int table_min_inter_size = 0;
    int table_max_inter_size = 0;
    int last_inter;
    int lp2ltn; // largest power of 2 less than n
    int lp2ltn_min;
    MPI_Comm shmem_comm;
    MPID_Comm *shmem_commptr=NULL;

    /* Get the size of the communicator */
    comm_size = comm_ptr->local_size;

    MPID_Datatype_get_size_macro(recvtype, recvtype_size);
    nbytes = recvtype_size * recvcount;

    int i, rank;
    MPI_Aint recvtype_extent;
    MPID_Datatype_get_extent_macro(recvtype, recvtype_extent);
    mpi_errno = PMPI_Comm_rank(comm_ptr->handle, &rank);
    if (mpi_errno) {
        MPIR_ERR_POP(mpi_errno);
    }
#ifdef _ENABLE_CUDA_
    int send_mem_type = 0;
    int recv_mem_type = 0;
    int snbytes = INT_MAX;
    MPI_Aint sendtype_extent;
    if (rdma_enable_cuda) {
        send_mem_type = is_device_buffer(sendbuf);
        recv_mem_type = is_device_buffer(recvbuf);
    }

    /*Handling Non-contig datatypes */
    if (rdma_enable_cuda && (send_mem_type || recv_mem_type)) {
        cuda_coll_pack((void **)&sendbuf, &sendcount, &sendtype,
                       &recvbuf, &recvcount, &recvtype,
                       rank * recvcount * recvtype_extent, 1, comm_size);
    }

    MPID_Datatype_get_extent_macro(sendtype, sendtype_extent);
    MPID_Datatype_get_extent_macro(recvtype, recvtype_extent);
    if (sendbuf != MPI_IN_PLACE) {
        snbytes = sendtype_extent * sendcount;
    }
    MPID_Datatype_get_size_macro(recvtype, recvtype_size);
    nbytes = recvtype_size * recvcount;

    if (rdma_enable_cuda && rdma_cuda_allgather_fgp &&
        send_mem_type && recv_mem_type &&
        snbytes >
        rdma_cuda_allgather_naive_limit / (FGP_SWITCH_FACTOR * comm_size) &&
        nbytes > rdma_cuda_allgather_naive_limit / (FGP_SWITCH_FACTOR * comm_size)) {
        if (sendbuf != MPI_IN_PLACE) {
            mpi_errno =
                MPIR_Allgather_cuda_intra_MV2(sendbuf, sendcount, sendtype,
                                              recvbuf, recvcount, recvtype,
                                              comm_ptr, errflag);
        } else {
            mpi_errno =
                MPIR_Allgather_cuda_intra_MV2(recvbuf +
                                              rank * recvcount *
                                              recvtype_extent, recvcount,
                                              recvtype, recvbuf, recvcount,
                                              recvtype, comm_ptr, errflag);
        }
        if (mpi_errno) {
            MPIR_ERR_POP(mpi_errno);
        }
        goto fn_exit;
    } else if (rdma_enable_cuda && (send_mem_type || recv_mem_type) &&
               rdma_cuda_use_naive && (nbytes <= rdma_cuda_allgather_naive_limit)) {
        if (sendbuf != MPI_IN_PLACE) {
            mpi_errno = cuda_stage_alloc((void **)&sendbuf, sendcount * sendtype_extent,
                                         &recvbuf,
                                         recvcount * recvtype_extent *
                                         comm_size, send_mem_type, recv_mem_type, 0);
        } else {
            mpi_errno = cuda_stage_alloc((void **)&sendbuf, recvcount * recvtype_extent,
                                         &recvbuf,
                                         recvcount * recvtype_extent *
                                         comm_size, send_mem_type,
                                         recv_mem_type,
                                         rank * recvcount * recvtype_extent);
        }
        if (mpi_errno) {
            MPIR_ERR_POP(mpi_errno);
        }
    }
#endif                          /*#ifdef _ENABLE_CUDA_ */

    if (mv2_use_old_allgather == 1) {
	MPIR_Allgather_intra_MV2(sendbuf, sendcount, sendtype, recvbuf, recvcount,
				 recvtype, comm_ptr, errflag);
	goto fn_exit;
    }
    
    /* check if safe to use partial subscription mode */
    if (comm_ptr->dev.ch.shmem_coll_ok == 1 && comm_ptr->dev.ch.is_uniform) {
    
        shmem_comm = comm_ptr->dev.ch.shmem_comm;
        MPID_Comm_get_ptr(shmem_comm, shmem_commptr);
        local_size = shmem_commptr->local_size;
        i = 0;
        if (mv2_allgather_indexed_table_ppn_conf[0] == -1) {
            // Indicating user defined tuning
            conf_index = 0;
            goto conf_check_end;
        }
        if ((comm_ptr->dev.ch.allgather_comm_ok != 0) &&
                (comm_ptr->dev.ch.is_blocked == 0 &&
                (mv2_allgather_cyclic_algo_threshold <= nbytes ||
                 mv2_allgather_ring_algo_threshold <= nbytes))) {
                /* for large messages or nonblocked hostfiles, use ring-allgather algorithm. */
                mpi_errno = MPIR_2lvl_Allgather_Ring_nonblocked_MV2(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm_ptr, errflag);
                goto fn_cuda_exit;
        }

        FIND_PPN_INDEX  (allgather, local_size,conf_index, partial_sub_ok)


    }

    if (partial_sub_ok != 1) {
        conf_index = mv2_allgather_indexed_num_ppn_conf/2;
    }
        
conf_check_end:

    /* Search for the corresponding system size inside the tuning table */
    /*
     * Comm sizes progress in powers of 2. Therefore comm_size can just be indexed instead
     */
    table_min_comm_size = mv2_allgather_indexed_thresholds_table[conf_index][0].numproc;
    table_max_comm_size =
	mv2_allgather_indexed_thresholds_table[conf_index][mv2_size_allgather_indexed_tuning_table[conf_index] - 1].numproc;
    
    if (comm_size < table_min_comm_size) {
	/* Comm size smaller than smallest configuration in table: use smallest available */
	comm_size_index = 0;
    }
    else if (comm_size > table_max_comm_size) {
	/* Comm size larger than largest configuration in table: use largest available */
	comm_size_index = mv2_size_allgather_indexed_tuning_table[conf_index] - 1;
    }
    else {
	/* Comm size in between smallest and largest configuration: find closest match */
    lp2ltn_min = pow(2, (int)log2(table_min_comm_size));
	if (comm_ptr->dev.ch.is_pof2) {
	    comm_size_index = log2( comm_size / lp2ltn_min );
	}
	else {
	    lp2ltn = pow(2, (int)log2(comm_size));
        comm_size_index = (lp2ltn < lp2ltn_min) ? 0 : log2( lp2ltn / lp2ltn_min );
	}
    }

    last_inter = mv2_allgather_indexed_thresholds_table[conf_index][comm_size_index].size_inter_table - 1;
    table_min_inter_size = mv2_allgather_indexed_thresholds_table[conf_index][comm_size_index].inter_leader[0].msg_sz;
    table_max_inter_size = mv2_allgather_indexed_thresholds_table[conf_index][comm_size_index].inter_leader[last_inter].msg_sz;
    
    if (nbytes < table_min_inter_size) {
	/* Msg size smaller than smallest configuration in table: use smallest available */
	inter_node_algo_index = 0;
    }
    else if (nbytes > table_max_inter_size) {
	/* Msg size larger than largest configuration in table: use largest available */
	inter_node_algo_index = last_inter;
    }
    else {
	/* Msg size in between smallest and largest configuration: find closest match */
	if (pow(2, (int)log2(nbytes)) == nbytes) {
	    inter_node_algo_index = log2( nbytes / table_min_inter_size );
	}
	else {
	    lp2ltn = pow(2, (int)log2(nbytes));
	    inter_node_algo_index = (lp2ltn < table_min_inter_size) ? 0 : log2( lp2ltn / table_min_inter_size );
	}
    }

    /* Set inter-leader pt */
    MV2_Allgather_function =
                          mv2_allgather_indexed_thresholds_table[conf_index][comm_size_index].
	inter_leader[inter_node_algo_index].MV2_pt_Allgather_function;

    if(MV2_Allgather_function == &MPIR_Allgather_RD_Allgather_Comm_MV2) {
        if(comm_ptr->dev.ch.allgather_comm_ok == 1) {
            int sendtype_iscontig = 0, recvtype_iscontig = 0;
            void *tmp_recv_buf = NULL;
            MPIR_T_PVAR_COUNTER_INC(MV2, mv2_num_shmem_coll_calls, 1);
            if (sendtype != MPI_DATATYPE_NULL && recvtype != MPI_DATATYPE_NULL) {
                MPIR_Datatype_iscontig(sendtype, &sendtype_iscontig);
                MPIR_Datatype_iscontig(recvtype, &recvtype_iscontig);
            }

            MPID_Comm *allgather_comm_ptr;
            MPID_Comm_get_ptr(comm_ptr->dev.ch.allgather_comm, allgather_comm_ptr);

            /*creation of a temporary recvbuf */
            tmp_recv_buf = MPIU_Malloc(recvcount * comm_size * recvtype_extent);
            if (!tmp_recv_buf) {
                mpi_errno = MPIR_Err_create_code(MPI_SUCCESS, MPIR_ERR_RECOVERABLE,
                                                 FCNAME, __LINE__, MPI_ERR_OTHER,
                                                 "**nomem", 0);
                return mpi_errno;
            }
            /* Calling Allgather with temporary buffer and allgather communicator */
            if (sendbuf != MPI_IN_PLACE) {
                mpi_errno = MPIR_Allgather_RD_MV2(sendbuf, sendcount, sendtype,
                                                     tmp_recv_buf, recvcount,
                                                     recvtype, allgather_comm_ptr, errflag);
            } else {
                mpi_errno = MPIR_Allgather_RD_MV2(recvbuf + rank * recvcount *
                                                     recvtype_extent, recvcount,
                                                     recvtype, tmp_recv_buf,
                                                     recvcount, recvtype,
                                                     allgather_comm_ptr, errflag);
            }

            if (mpi_errno) {
                MPIR_ERR_POP(mpi_errno);
            }
            /* Reordering data into recvbuf */
            if (sendtype_iscontig == 1 && recvtype_iscontig == 1
#if defined(_ENABLE_CUDA_)
                && rdma_enable_cuda == 0
#endif
            ){
                for (i = 0; i < comm_size; i++) {
                    MPIUI_Memcpy((void *) ((char *) recvbuf +
                                           (comm_ptr->dev.ch.allgather_new_ranks[i]) *
                                           nbytes),
                                           (char *) tmp_recv_buf + i * nbytes, nbytes);
                }
            } else {
                for (i = 0; i < comm_size; i++) {
                    mpi_errno = MPIR_Localcopy((void *) ((char *) tmp_recv_buf +
                                                i * recvcount *
                                                recvtype_extent),
                                                recvcount, recvtype,
                                                (void *) ((char *) recvbuf +
                                                (comm_ptr->dev.ch.allgather_new_ranks[i])
                                                * recvcount * recvtype_extent),
                                           recvcount, recvtype);
                    if (mpi_errno) {
                        MPIR_ERR_POP(mpi_errno);
                    }
                }
            }
            MPIU_Free(tmp_recv_buf);
        } else {
            mpi_errno = MPIR_Allgather_RD_MV2(sendbuf, sendcount, sendtype,
                                                recvbuf, recvcount, recvtype,
                                                comm_ptr, errflag);
            if (mpi_errno) {
                MPIR_ERR_POP(mpi_errno);
            }
        } 
    } else if(MV2_Allgather_function == &MPIR_Allgather_Bruck_MV2 
            || MV2_Allgather_function == &MPIR_Allgather_RD_MV2
            || MV2_Allgather_function == &MPIR_Allgather_Ring_MV2
            || MV2_Allgather_function == &MPIR_Allgather_Direct_MV2
            || MV2_Allgather_function == &MPIR_Allgather_DirectSpread_MV2) {
            mpi_errno = MV2_Allgather_function(sendbuf, sendcount, sendtype,
                                          recvbuf, recvcount, recvtype,
                                          comm_ptr, errflag);
    } else if ((comm_ptr->dev.ch.shmem_coll_ok == 1 &&
                comm_ptr->dev.ch.rank_list != NULL) &&
                sendbuf != MPI_IN_PLACE &&
            (MV2_Allgather_function == &MPIR_Allgather_gather_bcast_MV2
            || MV2_Allgather_function == &MPIR_2lvl_Allgather_nonblocked_MV2
            || MV2_Allgather_function == &MPIR_2lvl_Allgather_Ring_nonblocked_MV2
            || MV2_Allgather_function == &MPIR_2lvl_Allgather_Direct_MV2
            || MV2_Allgather_function == &MPIR_2lvl_Allgather_Ring_MV2)) {
            mpi_errno = MV2_Allgather_function(sendbuf, sendcount, sendtype,
                                          recvbuf, recvcount, recvtype,
                                          comm_ptr, errflag);
    } else {
        mpi_errno = MPIR_Allgather_intra(sendbuf, sendcount, sendtype,
                                         recvbuf, recvcount, recvtype, comm_ptr, errflag);
    }

fn_cuda_exit:
#ifdef _ENABLE_CUDA_
    if (rdma_enable_cuda && ((send_mem_type == 1) || (recv_mem_type == 1)) &&
        rdma_cuda_use_naive && (nbytes <= rdma_cuda_allgather_naive_limit)) {
        cuda_stage_free((void **)&sendbuf,
                        &recvbuf, recvcount * recvtype_extent * comm_size,
                        send_mem_type, recv_mem_type);
    }
#endif                          /*#ifdef _ENABLE_CUDA_ */

    if (mpi_errno) {
        MPIR_ERR_POP(mpi_errno);
    }

  fn_exit:
#ifdef _ENABLE_CUDA_
    /*Handling Non-Contig datatypes */
    if (rdma_enable_cuda && (send_mem_type || recv_mem_type)) {
        cuda_coll_unpack(&recvcount, comm_size);
    }
#endif                          /*#ifdef _ENABLE_CUDA_ */
    return mpi_errno;
  fn_fail:
    goto fn_exit;
}

#undef FUNCNAME
#define FUNCNAME MPIR_Allgather_MV2
#undef FCNAME
#define FCNAME MPL_QUOTE(FUNCNAME)
int MPIR_Allgather_MV2(const void *sendbuf, int sendcount, MPI_Datatype sendtype,
                       void *recvbuf, int recvcount, MPI_Datatype recvtype,
                       MPID_Comm * comm_ptr, MPIR_Errflag_t *errflag)
{

    int mpi_errno = MPI_SUCCESS;
    int nbytes = 0, comm_size, recvtype_size;
    int range = 0;
    int partial_sub_ok = 0;
    int conf_index = 0;
    int range_threshold = 0;
    int is_two_level = 0;
    int local_size = -1;
    MPI_Comm shmem_comm;
    MPID_Comm *shmem_commptr=NULL;
    
    if (mv2_use_indexed_tuning || mv2_use_indexed_allgather_tuning) {
	    return MPIR_Allgather_index_tuned_intra_MV2(sendbuf, sendcount,
                            sendtype, recvbuf, recvcount,
				            recvtype, comm_ptr, errflag);
    }

    /* Get the size of the communicator */
    comm_size = comm_ptr->local_size;

    MPID_Datatype_get_size_macro(recvtype, recvtype_size);
    nbytes = recvtype_size * recvcount;

    int i, rank;
    MPI_Aint recvtype_extent;
    MPID_Datatype_get_extent_macro(recvtype, recvtype_extent);
    mpi_errno = PMPI_Comm_rank(comm_ptr->handle, &rank);
    if (mpi_errno) {
        MPIR_ERR_POP(mpi_errno);
    }
#ifdef _ENABLE_CUDA_
    int send_mem_type = 0;
    int recv_mem_type = 0;
    int snbytes = INT_MAX;
    MPI_Aint sendtype_extent;
    if (rdma_enable_cuda) {
        send_mem_type = is_device_buffer(sendbuf);
        recv_mem_type = is_device_buffer(recvbuf);
    }

    /*Handling Non-contig datatypes */
    if (rdma_enable_cuda && (send_mem_type || recv_mem_type)) {
        cuda_coll_pack((void **)&sendbuf, &sendcount, &sendtype,
                       &recvbuf, &recvcount, &recvtype,
                       rank * recvcount * recvtype_extent, 1, comm_size);
    }

    MPID_Datatype_get_extent_macro(sendtype, sendtype_extent);
    MPID_Datatype_get_extent_macro(recvtype, recvtype_extent);
    if (sendbuf != MPI_IN_PLACE) {
        snbytes = sendtype_extent * sendcount;
    }
    MPID_Datatype_get_size_macro(recvtype, recvtype_size);
    nbytes = recvtype_size * recvcount;

    if (rdma_enable_cuda && rdma_cuda_allgather_fgp &&
        send_mem_type && recv_mem_type &&
        snbytes >
        rdma_cuda_allgather_naive_limit / (FGP_SWITCH_FACTOR * comm_size) &&
        nbytes > rdma_cuda_allgather_naive_limit / (FGP_SWITCH_FACTOR * comm_size)) {
        if (sendbuf != MPI_IN_PLACE) {
            mpi_errno =
                MPIR_Allgather_cuda_intra_MV2(sendbuf, sendcount, sendtype,
                                              recvbuf, recvcount, recvtype,
                                              comm_ptr, errflag);
        } else {
            mpi_errno =
                MPIR_Allgather_cuda_intra_MV2(recvbuf +
                                              rank * recvcount *
                                              recvtype_extent, recvcount,
                                              recvtype, recvbuf, recvcount,
                                              recvtype, comm_ptr, errflag);
        }
        if (mpi_errno) {
            MPIR_ERR_POP(mpi_errno);
        }
        goto fn_exit;
    } else if (rdma_enable_cuda && (send_mem_type || recv_mem_type) &&
               rdma_cuda_use_naive && (nbytes <= rdma_cuda_allgather_naive_limit)) {
        if (sendbuf != MPI_IN_PLACE) {
            mpi_errno = cuda_stage_alloc((void **)&sendbuf, sendcount * sendtype_extent,
                                         &recvbuf,
                                         recvcount * recvtype_extent *
                                         comm_size, send_mem_type, recv_mem_type, 0);
        } else {
            mpi_errno = cuda_stage_alloc((void **)&sendbuf, recvcount * recvtype_extent,
                                         &recvbuf,
                                         recvcount * recvtype_extent *
                                         comm_size, send_mem_type,
                                         recv_mem_type,
                                         rank * recvcount * recvtype_extent);
        }
        if (mpi_errno) {
            MPIR_ERR_POP(mpi_errno);
        }
    }
#endif                          /*#ifdef _ENABLE_CUDA_ */

    if (mv2_use_old_allgather == 1) {
	MPIR_Allgather_intra_MV2(sendbuf, sendcount, sendtype, recvbuf, recvcount,
				 recvtype, comm_ptr, errflag);
	goto fn_exit;
    }
    
    /* check if safe to use partial subscription mode */
    if (comm_ptr->dev.ch.shmem_coll_ok == 1 && comm_ptr->dev.ch.is_uniform) {
    
        shmem_comm = comm_ptr->dev.ch.shmem_comm;
        MPID_Comm_get_ptr(shmem_comm, shmem_commptr);
        local_size = shmem_commptr->local_size;
        i = 0;
        if (mv2_allgather_table_ppn_conf[0] == -1) {
            // Indicating user defined tuning
            conf_index = 0;
            goto conf_check_end;
        }
        

        FIND_PPN_INDEX  (allgather, local_size,conf_index, partial_sub_ok)

        
    }

    if (partial_sub_ok != 1) {
        conf_index = mv2_allgather_num_ppn_conf/2;
    }

conf_check_end:

    /* Search for the corresponding system size inside the tuning table */
    while ((range < (mv2_size_allgather_tuning_table[conf_index] - 1)) &&
           (comm_size >
            mv2_allgather_thresholds_table[conf_index][range].numproc)) {
        range++;
    }
    /* Search for corresponding inter-leader function */
    while ((range_threshold <
         (mv2_allgather_thresholds_table[conf_index][range].size_inter_table - 1))
           && (nbytes > mv2_allgather_thresholds_table[conf_index][range].inter_leader[range_threshold].max)
           && (mv2_allgather_thresholds_table[conf_index][range].inter_leader[range_threshold].max !=
               -1)) {
        range_threshold++;
    }

    /* Set inter-leader pt */
    MV2_Allgather_function =
                          mv2_allgather_thresholds_table[conf_index][range].inter_leader[range_threshold].
                          MV2_pt_Allgather_function;

    is_two_level =  mv2_allgather_thresholds_table[conf_index][range].two_level[range_threshold];

    /* intracommunicator */
    if(is_two_level ==1){
        
        if(comm_ptr->dev.ch.shmem_coll_ok == 1){
            MPIR_T_PVAR_COUNTER_INC(MV2, mv2_num_shmem_coll_calls, 1);
	   if (1 == comm_ptr->dev.ch.is_blocked) {
                mpi_errno = MPIR_2lvl_Allgather_MV2(sendbuf, sendcount, sendtype,
						    recvbuf, recvcount, recvtype,
						    comm_ptr, errflag);
	   }
	   else {
	       mpi_errno = MPIR_Allgather_intra(sendbuf, sendcount, sendtype,
						recvbuf, recvcount, recvtype,
						comm_ptr, errflag);
	   }
        } else {
            mpi_errno = MPIR_Allgather_RD_MV2(sendbuf, sendcount, sendtype,
                                                recvbuf, recvcount, recvtype,
                                                comm_ptr, errflag);
        }
    } else if(MV2_Allgather_function == &MPIR_Allgather_RD_Allgather_Comm_MV2){
        if(comm_ptr->dev.ch.allgather_comm_ok == 1) {
            int sendtype_iscontig = 0, recvtype_iscontig = 0;
            void *tmp_recv_buf = NULL;
            MPIR_T_PVAR_COUNTER_INC(MV2, mv2_num_shmem_coll_calls, 1);
            if (sendtype != MPI_DATATYPE_NULL && recvtype != MPI_DATATYPE_NULL) {
                MPIR_Datatype_iscontig(sendtype, &sendtype_iscontig);
                MPIR_Datatype_iscontig(recvtype, &recvtype_iscontig);
            }

            MPID_Comm *allgather_comm_ptr;
            MPID_Comm_get_ptr(comm_ptr->dev.ch.allgather_comm, allgather_comm_ptr);

            /*creation of a temporary recvbuf */
            tmp_recv_buf = MPIU_Malloc(recvcount * comm_size * recvtype_extent);
            if (!tmp_recv_buf) {
                mpi_errno = MPIR_Err_create_code(MPI_SUCCESS, MPIR_ERR_RECOVERABLE,
                                                 FCNAME, __LINE__, MPI_ERR_OTHER,
                                                 "**nomem", 0);
                return mpi_errno;
            }
            /* Calling Allgather with temporary buffer and allgather communicator */
            if (sendbuf != MPI_IN_PLACE) {
                mpi_errno = MPIR_Allgather_RD_MV2(sendbuf, sendcount, sendtype,
                                                     tmp_recv_buf, recvcount,
                                                     recvtype, allgather_comm_ptr, errflag);
            } else {
                mpi_errno = MPIR_Allgather_RD_MV2(recvbuf + rank * recvcount *
                                                     recvtype_extent, recvcount,
                                                     recvtype, tmp_recv_buf,
                                                     recvcount, recvtype,
                                                     allgather_comm_ptr, errflag);
            }

            if (mpi_errno) {
                MPIR_ERR_POP(mpi_errno);
            }
            /* Reordering data into recvbuf */
            if (sendtype_iscontig == 1 && recvtype_iscontig == 1
#if defined(_ENABLE_CUDA_)
                && rdma_enable_cuda == 0
#endif
            ){
                for (i = 0; i < comm_size; i++) {
                    MPIUI_Memcpy((void *) ((char *) recvbuf +
                                           (comm_ptr->dev.ch.allgather_new_ranks[i]) *
                                           nbytes),
                                           (char *) tmp_recv_buf + i * nbytes, nbytes);
                }
            } else {
                for (i = 0; i < comm_size; i++) {
                    mpi_errno = MPIR_Localcopy((void *) ((char *) tmp_recv_buf +
                                                i * recvcount *
                                                recvtype_extent),
                                                recvcount, recvtype,
                                                (void *) ((char *) recvbuf +
                                                (comm_ptr->dev.ch.allgather_new_ranks[i])
                                                * recvcount * recvtype_extent),
                                           recvcount, recvtype);
                    if (mpi_errno) {
                        MPIR_ERR_POP(mpi_errno);
                    }
                }
            }
            MPIU_Free(tmp_recv_buf);
        } else {
            mpi_errno = MPIR_Allgather_RD_MV2(sendbuf, sendcount, sendtype,
                                                recvbuf, recvcount, recvtype,
                                                comm_ptr, errflag);
            if (mpi_errno) {
                MPIR_ERR_POP(mpi_errno);
            }
        } 
    } else if(MV2_Allgather_function == &MPIR_Allgather_Bruck_MV2 
            || MV2_Allgather_function == &MPIR_Allgather_RD_MV2
            || MV2_Allgather_function == &MPIR_Allgather_Ring_MV2) {
            mpi_errno = MV2_Allgather_function(sendbuf, sendcount, sendtype,
                                          recvbuf, recvcount, recvtype,
                                          comm_ptr, errflag);
    } else {
        mpi_errno = MPIR_Allgather_intra(sendbuf, sendcount, sendtype,
                                         recvbuf, recvcount, recvtype, comm_ptr, errflag);
    }

#ifdef _ENABLE_CUDA_
    if (rdma_enable_cuda && (send_mem_type || recv_mem_type) &&
        rdma_cuda_use_naive && (nbytes <= rdma_cuda_allgather_naive_limit)) {
        cuda_stage_free((void **)&sendbuf,
                        &recvbuf, recvcount * recvtype_extent * comm_size,
                        send_mem_type, recv_mem_type);
    }
#endif                          /*#ifdef _ENABLE_CUDA_ */

    if (mpi_errno) {
        MPIR_ERR_POP(mpi_errno);
    }

  fn_exit:
#ifdef _ENABLE_CUDA_
    /*Handling Non-Contig datatypes */
    if (rdma_enable_cuda && (send_mem_type || recv_mem_type)) {
        cuda_coll_unpack(&recvcount, comm_size);
    }
#endif                          /*#ifdef _ENABLE_CUDA_ */
    return mpi_errno;
  fn_fail:
    goto fn_exit;
}
