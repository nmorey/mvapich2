/*
 * Copyright (C) by Argonne National Laboratory
 *     See COPYRIGHT in top-level directory
 */

/* -- THIS FILE IS AUTO-GENERATED -- */

#include "mpiimpl.h"

/* -- Begin Profiling Symbol Block for routine MPI_Sendrecv */
#if defined(HAVE_PRAGMA_WEAK)
#pragma weak MPI_Sendrecv = PMPI_Sendrecv
#elif defined(HAVE_PRAGMA_HP_SEC_DEF)
#pragma _HP_SECONDARY_DEF PMPI_Sendrecv  MPI_Sendrecv
#elif defined(HAVE_PRAGMA_CRI_DUP)
#pragma _CRI duplicate MPI_Sendrecv as PMPI_Sendrecv
#elif defined(HAVE_WEAK_ATTRIBUTE)
int MPI_Sendrecv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag,
                 void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag,
                 MPI_Comm comm, MPI_Status *status)
                  __attribute__ ((weak, alias("PMPI_Sendrecv")));
#endif
/* -- End Profiling Symbol Block */

/* Define MPICH_MPI_FROM_PMPI if weak symbols are not supported to build
   the MPI routines */
#ifndef MPICH_MPI_FROM_PMPI
#undef MPI_Sendrecv
#define MPI_Sendrecv PMPI_Sendrecv
#endif /* MPICH_MPI_FROM_PMPI */

static int internal_Sendrecv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest,
                             int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype,
                             int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
    int mpi_errno = MPI_SUCCESS;
    MPIR_Comm *comm_ptr ATTRIBUTE((unused)) = NULL;

    MPIR_ERRTEST_INITIALIZED_ORDIE();

    MPID_THREAD_CS_ENTER(GLOBAL, MPIR_THREAD_GLOBAL_ALLFUNC_MUTEX);
    MPIR_FUNC_TERSE_ENTER;

#ifdef HAVE_ERROR_CHECKING
    {
        MPID_BEGIN_ERROR_CHECKS;
        {
            MPIR_ERRTEST_COMM(comm, mpi_errno);
        }
        MPID_END_ERROR_CHECKS;
    }
#endif /* HAVE_ERROR_CHECKING */

    MPIR_Comm_get_ptr(comm, comm_ptr);

#ifdef HAVE_ERROR_CHECKING
    {
        MPID_BEGIN_ERROR_CHECKS;
        {
            MPIR_Comm_valid_ptr(comm_ptr, mpi_errno, FALSE);
            if (mpi_errno) {
                goto fn_fail;
            }
            MPIR_ERRTEST_COUNT(sendcount, mpi_errno);
            if (sendcount > 0) {
                MPIR_ERRTEST_DATATYPE(sendtype, "datatype", mpi_errno);
                if (!HANDLE_IS_BUILTIN(sendtype)) {
                    MPIR_Datatype *datatype_ptr ATTRIBUTE((unused)) = NULL;
                    MPIR_Datatype_get_ptr(sendtype, datatype_ptr);
                    MPIR_Datatype_valid_ptr(datatype_ptr, mpi_errno);
                    if (mpi_errno) {
                        goto fn_fail;
                    }
                    MPIR_Datatype_committed_ptr(datatype_ptr, mpi_errno);
                    if (mpi_errno) {
                        goto fn_fail;
                    }
                }
                MPIR_ERRTEST_USERBUFFER(sendbuf, sendcount, sendtype, mpi_errno);
            }
            MPIR_ERRTEST_SEND_RANK(comm_ptr, dest, mpi_errno);
            MPIR_ERRTEST_SEND_TAG(sendtag, mpi_errno);
            MPIR_ERRTEST_COUNT(recvcount, mpi_errno);
            if (recvcount > 0) {
                MPIR_ERRTEST_DATATYPE(recvtype, "datatype", mpi_errno);
                if (!HANDLE_IS_BUILTIN(recvtype)) {
                    MPIR_Datatype *datatype_ptr ATTRIBUTE((unused)) = NULL;
                    MPIR_Datatype_get_ptr(recvtype, datatype_ptr);
                    MPIR_Datatype_valid_ptr(datatype_ptr, mpi_errno);
                    if (mpi_errno) {
                        goto fn_fail;
                    }
                    MPIR_Datatype_committed_ptr(datatype_ptr, mpi_errno);
                    if (mpi_errno) {
                        goto fn_fail;
                    }
                }
                MPIR_ERRTEST_USERBUFFER(recvbuf, recvcount, recvtype, mpi_errno);
            }
            MPIR_ERRTEST_RECV_RANK(comm_ptr, source, mpi_errno);
            MPIR_ERRTEST_RECV_TAG(recvtag, mpi_errno);
            if (status != MPI_STATUS_IGNORE) {
                MPIR_ERRTEST_ARGNULL(status, "status", mpi_errno);
            }
        }
        MPID_END_ERROR_CHECKS;
    }
#endif /* HAVE_ERROR_CHECKING */

    /* ... body of routine ... */
    mpi_errno = MPIR_Sendrecv_impl(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount,
                                   recvtype, source, recvtag, comm_ptr, status);
    if (mpi_errno) {
        goto fn_fail;
    }
    /* ... end of body of routine ... */

  fn_exit:
    MPIR_FUNC_TERSE_EXIT;
    MPID_THREAD_CS_EXIT(GLOBAL, MPIR_THREAD_GLOBAL_ALLFUNC_MUTEX);
    return mpi_errno;

  fn_fail:
    /* --BEGIN ERROR HANDLINE-- */
#ifdef HAVE_ERROR_CHECKING
    mpi_errno = MPIR_Err_create_code(mpi_errno, MPIR_ERR_RECOVERABLE, __func__, __LINE__, MPI_ERR_OTHER,
                                     "**mpi_sendrecv",
                                     "**mpi_sendrecv %p %d %D %i %t %p %d %D %i %t %C %p", sendbuf,
                                     sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype,
                                     source, recvtag, comm, status);
#endif
    mpi_errno = MPIR_Err_return_comm(comm_ptr, __func__, mpi_errno);
    /* --END ERROR HANDLING-- */
    goto fn_exit;
}

#ifdef ENABLE_QMPI
#ifndef MPICH_MPI_FROM_PMPI
int QMPI_Sendrecv(QMPI_Context context, int tool_id, const void *sendbuf, int sendcount,
                  MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount,
                  MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm,
                  MPI_Status *status)
{
    return internal_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
}
#endif /* MPICH_MPI_FROM_PMPI */
int MPI_Sendrecv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag,
                 void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag,
                 MPI_Comm comm, MPI_Status *status)
{
    QMPI_Context context;
    QMPI_Sendrecv_t *fn_ptr;

    context.storage_stack = NULL;

    if (MPIR_QMPI_num_tools == 0)
        return QMPI_Sendrecv(context, 0, sendbuf, sendcount, sendtype, dest, sendtag, recvbuf,
                             recvcount, recvtype, source, recvtag, comm, status);

    fn_ptr = (QMPI_Sendrecv_t *) MPIR_QMPI_first_fn_ptrs[MPI_SENDRECV_T];

    return (*fn_ptr) (context, MPIR_QMPI_first_tool_ids[MPI_SENDRECV_T], sendbuf, sendcount,
            sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
}
#else /* ENABLE_QMPI */
int MPI_Sendrecv(const void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag,
                 void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag,
                 MPI_Comm comm, MPI_Status *status)
{
    return internal_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
}

#endif /* ENABLE_QMPI */

/* -- Begin Profiling Symbol Block for routine MPI_Sendrecv_c */
#if defined(HAVE_PRAGMA_WEAK)
#pragma weak MPI_Sendrecv_c = PMPI_Sendrecv_c
#elif defined(HAVE_PRAGMA_HP_SEC_DEF)
#pragma _HP_SECONDARY_DEF PMPI_Sendrecv_c  MPI_Sendrecv_c
#elif defined(HAVE_PRAGMA_CRI_DUP)
#pragma _CRI duplicate MPI_Sendrecv_c as PMPI_Sendrecv_c
#elif defined(HAVE_WEAK_ATTRIBUTE)
int MPI_Sendrecv_c(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype, int dest,
                   int sendtag, void *recvbuf, MPI_Count recvcount, MPI_Datatype recvtype,
                   int source, int recvtag, MPI_Comm comm, MPI_Status *status)
                    __attribute__ ((weak, alias("PMPI_Sendrecv_c")));
#endif
/* -- End Profiling Symbol Block */

/* Define MPICH_MPI_FROM_PMPI if weak symbols are not supported to build
   the MPI routines */
#ifndef MPICH_MPI_FROM_PMPI
#undef MPI_Sendrecv_c
#define MPI_Sendrecv_c PMPI_Sendrecv_c
#endif /* MPICH_MPI_FROM_PMPI */

static int internal_Sendrecv_c(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype,
                               int dest, int sendtag, void *recvbuf, MPI_Count recvcount,
                               MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm,
                               MPI_Status *status)
{
    int mpi_errno = MPI_SUCCESS;
    MPIR_Comm *comm_ptr ATTRIBUTE((unused)) = NULL;

    MPIR_ERRTEST_INITIALIZED_ORDIE();

    MPID_THREAD_CS_ENTER(GLOBAL, MPIR_THREAD_GLOBAL_ALLFUNC_MUTEX);
    MPIR_FUNC_TERSE_ENTER;

#ifdef HAVE_ERROR_CHECKING
    {
        MPID_BEGIN_ERROR_CHECKS;
        {
            MPIR_ERRTEST_COMM(comm, mpi_errno);
        }
        MPID_END_ERROR_CHECKS;
    }
#endif /* HAVE_ERROR_CHECKING */

    MPIR_Comm_get_ptr(comm, comm_ptr);

#ifdef HAVE_ERROR_CHECKING
    {
        MPID_BEGIN_ERROR_CHECKS;
        {
            MPIR_Comm_valid_ptr(comm_ptr, mpi_errno, FALSE);
            if (mpi_errno) {
                goto fn_fail;
            }
            MPIR_ERRTEST_COUNT(sendcount, mpi_errno);
            if (sendcount > 0) {
                MPIR_ERRTEST_DATATYPE(sendtype, "datatype", mpi_errno);
                if (!HANDLE_IS_BUILTIN(sendtype)) {
                    MPIR_Datatype *datatype_ptr ATTRIBUTE((unused)) = NULL;
                    MPIR_Datatype_get_ptr(sendtype, datatype_ptr);
                    MPIR_Datatype_valid_ptr(datatype_ptr, mpi_errno);
                    if (mpi_errno) {
                        goto fn_fail;
                    }
                    MPIR_Datatype_committed_ptr(datatype_ptr, mpi_errno);
                    if (mpi_errno) {
                        goto fn_fail;
                    }
                }
                MPIR_ERRTEST_USERBUFFER(sendbuf, sendcount, sendtype, mpi_errno);
            }
            MPIR_ERRTEST_SEND_RANK(comm_ptr, dest, mpi_errno);
            MPIR_ERRTEST_SEND_TAG(sendtag, mpi_errno);
            MPIR_ERRTEST_COUNT(recvcount, mpi_errno);
            if (recvcount > 0) {
                MPIR_ERRTEST_DATATYPE(recvtype, "datatype", mpi_errno);
                if (!HANDLE_IS_BUILTIN(recvtype)) {
                    MPIR_Datatype *datatype_ptr ATTRIBUTE((unused)) = NULL;
                    MPIR_Datatype_get_ptr(recvtype, datatype_ptr);
                    MPIR_Datatype_valid_ptr(datatype_ptr, mpi_errno);
                    if (mpi_errno) {
                        goto fn_fail;
                    }
                    MPIR_Datatype_committed_ptr(datatype_ptr, mpi_errno);
                    if (mpi_errno) {
                        goto fn_fail;
                    }
                }
                MPIR_ERRTEST_USERBUFFER(recvbuf, recvcount, recvtype, mpi_errno);
            }
            MPIR_ERRTEST_RECV_RANK(comm_ptr, source, mpi_errno);
            MPIR_ERRTEST_RECV_TAG(recvtag, mpi_errno);
            if (status != MPI_STATUS_IGNORE) {
                MPIR_ERRTEST_ARGNULL(status, "status", mpi_errno);
            }
        }
        MPID_END_ERROR_CHECKS;
    }
#endif /* HAVE_ERROR_CHECKING */

    /* ... body of routine ... */
    if (sizeof(MPI_Count) == sizeof(MPI_Aint)) {
        mpi_errno = MPIR_Sendrecv_impl(sendbuf, (MPI_Aint) sendcount, sendtype, dest, sendtag, recvbuf,
                                       (MPI_Aint) recvcount, recvtype, source, recvtag, comm_ptr, status);
        if (mpi_errno) {
            goto fn_fail;
        }
    } else {
        /* MPI_Count is bigger than MPI_Aint */
        if (sendcount > MPIR_AINT_MAX) {
            mpi_errno = MPIR_Err_create_code(mpi_errno, MPIR_ERR_RECOVERABLE,
                                             __func__, __LINE__, MPI_ERR_OTHER,
                                             "**too_big_for_input",
                                             "**too_big_for_input %s", "sendcount");
            goto fn_fail;
        }
        if (recvcount > MPIR_AINT_MAX) {
            mpi_errno = MPIR_Err_create_code(mpi_errno, MPIR_ERR_RECOVERABLE,
                                             __func__, __LINE__, MPI_ERR_OTHER,
                                             "**too_big_for_input",
                                             "**too_big_for_input %s", "recvcount");
            goto fn_fail;
        }
        mpi_errno = MPIR_Sendrecv_impl(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount,
                                       recvtype, source, recvtag, comm_ptr, status);
        if (mpi_errno) {
            goto fn_fail;
        }
    }
    /* ... end of body of routine ... */

  fn_exit:
    MPIR_FUNC_TERSE_EXIT;
    MPID_THREAD_CS_EXIT(GLOBAL, MPIR_THREAD_GLOBAL_ALLFUNC_MUTEX);
    return mpi_errno;

  fn_fail:
    /* --BEGIN ERROR HANDLINE-- */
#ifdef HAVE_ERROR_CHECKING
    mpi_errno = MPIR_Err_create_code(mpi_errno, MPIR_ERR_RECOVERABLE, __func__, __LINE__, MPI_ERR_OTHER,
                                     "**mpi_sendrecv_c",
                                     "**mpi_sendrecv_c %p %c %D %i %t %p %c %D %i %t %C %p", sendbuf,
                                     sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype,
                                     source, recvtag, comm, status);
#endif
    mpi_errno = MPIR_Err_return_comm(comm_ptr, __func__, mpi_errno);
    /* --END ERROR HANDLING-- */
    goto fn_exit;
}

#ifdef ENABLE_QMPI
#ifndef MPICH_MPI_FROM_PMPI
int QMPI_Sendrecv_c(QMPI_Context context, int tool_id, const void *sendbuf, MPI_Count sendcount,
                    MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf,
                    MPI_Count recvcount, MPI_Datatype recvtype, int source, int recvtag,
                    MPI_Comm comm, MPI_Status *status)
{
    return internal_Sendrecv_c(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
}
#endif /* MPICH_MPI_FROM_PMPI */
int MPI_Sendrecv_c(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype, int dest,
                   int sendtag, void *recvbuf, MPI_Count recvcount, MPI_Datatype recvtype,
                   int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
    QMPI_Context context;
    QMPI_Sendrecv_c_t *fn_ptr;

    context.storage_stack = NULL;

    if (MPIR_QMPI_num_tools == 0)
        return QMPI_Sendrecv_c(context, 0, sendbuf, sendcount, sendtype, dest, sendtag, recvbuf,
                               recvcount, recvtype, source, recvtag, comm, status);

    fn_ptr = (QMPI_Sendrecv_c_t *) MPIR_QMPI_first_fn_ptrs[MPI_SENDRECV_C_T];

    return (*fn_ptr) (context, MPIR_QMPI_first_tool_ids[MPI_SENDRECV_C_T], sendbuf, sendcount,
            sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
}
#else /* ENABLE_QMPI */
int MPI_Sendrecv_c(const void *sendbuf, MPI_Count sendcount, MPI_Datatype sendtype, int dest,
                   int sendtag, void *recvbuf, MPI_Count recvcount, MPI_Datatype recvtype,
                   int source, int recvtag, MPI_Comm comm, MPI_Status *status)
{
    return internal_Sendrecv_c(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
}

#endif /* ENABLE_QMPI */
