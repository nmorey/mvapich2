/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
/* Copyright (c) 2001-2020, The Ohio State University. All rights
 * reserved.
 *
 * This file is part of the MVAPICH2 software package developed by the
 * team members of The Ohio State University's Network-Based Computing
 * Laboratory (NBCL), headed by Professor Dhabaleswar K. (DK) Panda.
 *
 * For detailed copyright and licensing information, please refer to the
 * copyright file COPYRIGHT in the top level MVAPICH2 directory.
 *
 */

#include "mpidimpl.h"

/* FIXME: HOMOGENEOUS SYSTEMS ONLY -- no data conversion is performed */

/*
 * MPID_Ssend()
 */
#undef FUNCNAME
#define FUNCNAME MPID_Ssend
#undef FCNAME
#define FCNAME MPL_QUOTE(FUNCNAME)
int MPID_Ssend(const void * buf, MPI_Aint count, MPI_Datatype datatype, int rank, int tag, MPID_Comm * comm, int context_offset,
	       MPID_Request ** request)
{
    MPIDI_msg_sz_t data_sz;
    int dt_contig;
    MPI_Aint dt_true_lb;
    MPID_Datatype * dt_ptr;
    MPID_Request * sreq = NULL;
    MPIDI_VC_t * vc;
#if !defined(CHANNEL_MRAIL)
    int eager_threshold = -1;
#endif
    int mpi_errno = MPI_SUCCESS;
    MPIDI_STATE_DECL(MPID_STATE_MPID_SSEND);

    MPIDI_FUNC_ENTER(MPID_STATE_MPID_SSEND);

    MPIU_DBG_MSG_FMT(CH3_OTHER,VERBOSE,(MPIU_DBG_FDEST,
              "rank=%d, tag=%d, context=%d", 
              rank, tag, comm->context_id + context_offset));

    /* Check to make sure the communicator hasn't already been revoked */
    if (comm->revoked &&
            MPIR_AGREE_TAG != MPIR_TAG_MASK_ERROR_BITS(tag & ~MPIR_Process.tagged_coll_mask) &&
            MPIR_SHRINK_TAG != MPIR_TAG_MASK_ERROR_BITS(tag & ~MPIR_Process.tagged_coll_mask)) {
        MPIR_ERR_SETANDJUMP(mpi_errno,MPIX_ERR_REVOKED,"**revoked");
    }

    if (rank == comm->rank && comm->comm_kind != MPID_INTERCOMM)
    {
#if defined(CHANNEL_PSM)
    goto skip_self_send;     /* PSM internally, will optimize self-send */
#endif
	mpi_errno = MPIDI_Isend_self(buf, count, datatype, rank, tag, comm, 
				     context_offset, MPIDI_REQUEST_TYPE_SSEND, 
				     &sreq);
	/* In the single threaded case, sending to yourself will cause 
	   deadlock.  Note that in the runtime-thread case, this check
	   will not be made (long-term FIXME) */
#       ifndef MPICH_IS_THREADED
	{
	    /* --BEGIN ERROR HANDLING-- */
	    if (sreq != NULL && MPID_cc_get(sreq->cc) != 0)
	    {
		mpi_errno = MPIR_Err_create_code(MPI_SUCCESS, MPIR_ERR_RECOVERABLE, FCNAME, __LINE__, MPI_ERR_OTHER,
						 "**dev|selfsenddeadlock", 0);
		goto fn_exit;
	    }
	    /* --END ERROR HANDLING-- */
	}
#	endif
	goto fn_exit;
    }
#if defined (CHANNEL_PSM)
skip_self_send:
#endif        
    if (rank == MPI_PROC_NULL)
    {
	goto fn_exit;
    }

    MPIDI_Comm_get_vc_set_active(comm, rank, &vc);

#ifdef ENABLE_COMM_OVERRIDES
    if (vc->comm_ops && vc->comm_ops->ssend)
    {
	mpi_errno = vc->comm_ops->ssend( vc, buf, count, datatype, rank, tag, comm, context_offset, &sreq);
	goto fn_exit;
    }
#endif

    
    MPIDI_Datatype_get_info(count, datatype, dt_contig, data_sz, dt_ptr, dt_true_lb);

    MPIDI_Request_create_sreq(sreq, mpi_errno, goto fn_exit);
    MPIDI_Request_set_type(sreq, MPIDI_REQUEST_TYPE_SSEND);

    if (data_sz == 0)
    {
#if defined (CHANNEL_PSM)
    goto psm_ssend;
#endif
	mpi_errno = MPIDI_CH3_EagerSyncZero( &sreq, rank, tag, comm, 
					     context_offset );
	goto fn_exit;
    }
#if defined (CHANNEL_PSM)
psm_ssend:
    sreq->psm_flags |= PSM_SYNC_SEND;
    if(dt_contig) {
        PSMSG(fprintf(stderr, "psm Sync send\n"));
        mpi_errno = MPIDI_CH3_EagerContigSend(&sreq, MPIDI_CH3_PKT_EAGER_SEND,
                       (char *)buf + dt_true_lb, data_sz, rank, tag, comm,
                        context_offset);
    } else {
        PSMSG(fprintf(stderr, "psm NC-Sync send\n"));
        mpi_errno = MPIDI_CH3_EagerNoncontigSend(&sreq,
                        MPIDI_CH3_PKT_EAGER_SEND, buf, count, datatype, data_sz,
                        rank, tag, comm, context_offset);
    }
    goto fn_exit;
#endif /* CHANNEL_PSM */
    
#if defined(CHANNEL_MRAIL)
    if (data_sz + sizeof(MPIDI_CH3_Pkt_eager_sync_send_t) <= vc->eager_max_msg_sz
        && ! vc->force_rndv)
#else /* defined(CHANNEL_MRAIL) */
    MPIDI_CH3_GET_EAGER_THRESHOLD(&eager_threshold, comm, vc);

    if (data_sz + sizeof(MPIDI_CH3_Pkt_eager_sync_send_t) <= eager_threshold)
#endif /* defined(CHANNEL_MRAIL) */
    {
	mpi_errno = MPIDI_CH3_EagerSyncNoncontigSend( &sreq, buf, count,
                                                      datatype, data_sz, 
                                                      dt_contig, dt_true_lb,
                                                      rank, tag, comm, 
                                                      context_offset );
    }
    else
    {
	/* Note that the sreq was created above */
	mpi_errno = vc->rndvSend_fn( &sreq, buf, count, datatype, dt_contig,
                                     data_sz, dt_true_lb, rank, tag, comm, 
                                     context_offset );
	/* Note that we don't increase the ref cound on the datatype
	   because this is a blocking call, and the calling routine 
	   must wait until sreq completes */
    }

  fn_fail:
  fn_exit:
    *request = sreq;
    
    MPIU_DBG_STMT(CH3_OTHER,VERBOSE,{if (sreq!=NULL) {
            MPIU_DBG_MSG_P(CH3_OTHER,VERBOSE,
			   "request allocated, handle=0x%08x", sreq->handle);}});
    
    MPIDI_FUNC_EXIT(MPID_STATE_MPID_SSEND);
    return mpi_errno;
}
