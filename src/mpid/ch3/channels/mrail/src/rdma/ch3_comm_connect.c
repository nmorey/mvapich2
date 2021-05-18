/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 *
 * Copyright (c) 2001-2021, The Ohio State University. All rights
 * reserved.
 *
 * This file is part of the MVAPICH2 software package developed by the
 * team members of The Ohio State University's Network-Based Computing
 * Laboratory (NBCL), headed by Professor Dhabaleswar K. (DK) Panda.
 *
 * For detailed copyright and licensing information, please refer to the
 * copyright file COPYRIGHT in the top level MVAPICH2 directory.
 */

#include "mpidi_ch3_impl.h"
#include "upmi.h"

#undef FUNCNAME
#define FUNCNAME MPIDI_CH3_Comm_connect
#undef FCNAME
#define FCNAME MPL_QUOTE(FUNCNAME)
int MPIDI_CH3_Comm_connect(char *port_name, int root, MPID_Comm * comm_ptr,
                           MPID_Comm ** newcomm)
{
    MPIDI_STATE_DECL(MPID_STATE_MPIDI_CH3_COMM_CONNECT);
    MPIDI_FUNC_ENTER(MPID_STATE_MPIDI_CH3_COMM_CONNECT);
    int mpi_errno =
        MPIR_Err_create_code(MPI_SUCCESS, MPIR_ERR_RECOVERABLE, FCNAME,
                             __LINE__, MPI_ERR_OTHER, "**notimpl", 0);
    MPIDI_FUNC_EXIT(MPID_STATE_MPIDI_CH3_COMM_CONNECT);
    return mpi_errno;
}
