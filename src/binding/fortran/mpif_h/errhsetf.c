/* -*- Mode: C; c-basic-offset:4 ; -*- */
/*  
 *  (C) 2001 by Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 *
 * This file is automatically generated by buildiface 
 * DO NOT EDIT
 */
#include "mpi_fortimpl.h"


/* Begin MPI profiling block */
#if defined(USE_WEAK_SYMBOLS) && !defined(USE_ONLY_MPI_NAMES) 
#if defined(HAVE_MULTIPLE_PRAGMA_WEAK)
extern FORT_DLL_SPEC void FORT_CALL MPI_ERRHANDLER_SET( MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set__( MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set( MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set_( MPI_Fint *, MPI_Fint *, MPI_Fint * );

#if defined(F77_NAME_UPPER)
#pragma weak MPI_ERRHANDLER_SET = PMPI_ERRHANDLER_SET
#pragma weak mpi_errhandler_set__ = PMPI_ERRHANDLER_SET
#pragma weak mpi_errhandler_set_ = PMPI_ERRHANDLER_SET
#pragma weak mpi_errhandler_set = PMPI_ERRHANDLER_SET
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma weak MPI_ERRHANDLER_SET = pmpi_errhandler_set__
#pragma weak mpi_errhandler_set__ = pmpi_errhandler_set__
#pragma weak mpi_errhandler_set_ = pmpi_errhandler_set__
#pragma weak mpi_errhandler_set = pmpi_errhandler_set__
#elif defined(F77_NAME_LOWER_USCORE)
#pragma weak MPI_ERRHANDLER_SET = pmpi_errhandler_set_
#pragma weak mpi_errhandler_set__ = pmpi_errhandler_set_
#pragma weak mpi_errhandler_set_ = pmpi_errhandler_set_
#pragma weak mpi_errhandler_set = pmpi_errhandler_set_
#else
#pragma weak MPI_ERRHANDLER_SET = pmpi_errhandler_set
#pragma weak mpi_errhandler_set__ = pmpi_errhandler_set
#pragma weak mpi_errhandler_set_ = pmpi_errhandler_set
#pragma weak mpi_errhandler_set = pmpi_errhandler_set
#endif



#elif defined(HAVE_PRAGMA_WEAK)

#if defined(F77_NAME_UPPER)
extern FORT_DLL_SPEC void FORT_CALL MPI_ERRHANDLER_SET( MPI_Fint *, MPI_Fint *, MPI_Fint * );

#pragma weak MPI_ERRHANDLER_SET = PMPI_ERRHANDLER_SET
#elif defined(F77_NAME_LOWER_2USCORE)
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set__( MPI_Fint *, MPI_Fint *, MPI_Fint * );

#pragma weak mpi_errhandler_set__ = pmpi_errhandler_set__
#elif !defined(F77_NAME_LOWER_USCORE)
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set( MPI_Fint *, MPI_Fint *, MPI_Fint * );

#pragma weak mpi_errhandler_set = pmpi_errhandler_set
#else
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set_( MPI_Fint *, MPI_Fint *, MPI_Fint * );

#pragma weak mpi_errhandler_set_ = pmpi_errhandler_set_
#endif

#elif defined(HAVE_PRAGMA_HP_SEC_DEF)
#if defined(F77_NAME_UPPER)
#pragma _HP_SECONDARY_DEF PMPI_ERRHANDLER_SET  MPI_ERRHANDLER_SET
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma _HP_SECONDARY_DEF pmpi_errhandler_set__  mpi_errhandler_set__
#elif !defined(F77_NAME_LOWER_USCORE)
#pragma _HP_SECONDARY_DEF pmpi_errhandler_set  mpi_errhandler_set
#else
#pragma _HP_SECONDARY_DEF pmpi_errhandler_set_  mpi_errhandler_set_
#endif

#elif defined(HAVE_PRAGMA_CRI_DUP)
#if defined(F77_NAME_UPPER)
#pragma _CRI duplicate MPI_ERRHANDLER_SET as PMPI_ERRHANDLER_SET
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma _CRI duplicate mpi_errhandler_set__ as pmpi_errhandler_set__
#elif !defined(F77_NAME_LOWER_USCORE)
#pragma _CRI duplicate mpi_errhandler_set as pmpi_errhandler_set
#else
#pragma _CRI duplicate mpi_errhandler_set_ as pmpi_errhandler_set_
#endif

#elif defined(HAVE_WEAK_ATTRIBUTE)
#if defined(F77_NAME_UPPER)
extern FORT_DLL_SPEC void FORT_CALL MPI_ERRHANDLER_SET( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_ERRHANDLER_SET")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set__( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_ERRHANDLER_SET")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set_( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_ERRHANDLER_SET")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_ERRHANDLER_SET")));

#elif defined(F77_NAME_LOWER_2USCORE)
extern FORT_DLL_SPEC void FORT_CALL MPI_ERRHANDLER_SET( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set__")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set__( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set__")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set_( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set__")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set__")));

#elif defined(F77_NAME_LOWER_USCORE)
extern FORT_DLL_SPEC void FORT_CALL MPI_ERRHANDLER_SET( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set_")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set__( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set_")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set_( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set_")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set_")));

#else
extern FORT_DLL_SPEC void FORT_CALL MPI_ERRHANDLER_SET( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set__( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set_( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set")));

#endif
#endif /* HAVE_PRAGMA_WEAK */
#endif /* USE_WEAK_SYMBOLS */
/* End MPI profiling block */


/* These definitions are used only for generating the Fortran wrappers */
#if defined(USE_WEAK_SYMBOLS) && defined(USE_ONLY_MPI_NAMES)
#if defined(HAVE_MULTIPLE_PRAGMA_WEAK)
extern FORT_DLL_SPEC void FORT_CALL MPI_ERRHANDLER_SET( MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set__( MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set( MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set_( MPI_Fint *, MPI_Fint *, MPI_Fint * );

#if defined(F77_NAME_UPPER)
#pragma weak mpi_errhandler_set__ = MPI_ERRHANDLER_SET
#pragma weak mpi_errhandler_set_ = MPI_ERRHANDLER_SET
#pragma weak mpi_errhandler_set = MPI_ERRHANDLER_SET
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma weak MPI_ERRHANDLER_SET = mpi_errhandler_set__
#pragma weak mpi_errhandler_set_ = mpi_errhandler_set__
#pragma weak mpi_errhandler_set = mpi_errhandler_set__
#elif defined(F77_NAME_LOWER_USCORE)
#pragma weak MPI_ERRHANDLER_SET = mpi_errhandler_set_
#pragma weak mpi_errhandler_set__ = mpi_errhandler_set_
#pragma weak mpi_errhandler_set = mpi_errhandler_set_
#else
#pragma weak MPI_ERRHANDLER_SET = mpi_errhandler_set
#pragma weak mpi_errhandler_set__ = mpi_errhandler_set
#pragma weak mpi_errhandler_set_ = mpi_errhandler_set
#endif
#elif defined(HAVE_WEAK_ATTRIBUTE)
#if defined(F77_NAME_UPPER)
extern FORT_DLL_SPEC void FORT_CALL MPI_ERRHANDLER_SET( MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set__( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("MPI_ERRHANDLER_SET")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set_( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("MPI_ERRHANDLER_SET")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("MPI_ERRHANDLER_SET")));

#elif defined(F77_NAME_LOWER_2USCORE)
extern FORT_DLL_SPEC void FORT_CALL MPI_ERRHANDLER_SET( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_errhandler_set__")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set__( MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set_( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_errhandler_set__")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_errhandler_set__")));

#elif defined(F77_NAME_LOWER_USCORE)
extern FORT_DLL_SPEC void FORT_CALL MPI_ERRHANDLER_SET( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_errhandler_set_")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set__( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_errhandler_set_")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set_( MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_errhandler_set_")));

#else
extern FORT_DLL_SPEC void FORT_CALL MPI_ERRHANDLER_SET( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_errhandler_set")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set__( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_errhandler_set")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set_( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_errhandler_set")));
extern FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set( MPI_Fint *, MPI_Fint *, MPI_Fint * );

#endif
#endif

#endif

/* Map the name to the correct form */
#ifndef MPICH_MPI_FROM_PMPI
#if defined(USE_WEAK_SYMBOLS)
#if defined(HAVE_MULTIPLE_PRAGMA_WEAK)
/* Define the weak versions of the PMPI routine*/
#ifndef F77_NAME_UPPER
extern FORT_DLL_SPEC void FORT_CALL PMPI_ERRHANDLER_SET( MPI_Fint *, MPI_Fint *, MPI_Fint * );
#endif
#ifndef F77_NAME_LOWER_2USCORE
extern FORT_DLL_SPEC void FORT_CALL pmpi_errhandler_set__( MPI_Fint *, MPI_Fint *, MPI_Fint * );
#endif
#ifndef F77_NAME_LOWER_USCORE
extern FORT_DLL_SPEC void FORT_CALL pmpi_errhandler_set_( MPI_Fint *, MPI_Fint *, MPI_Fint * );
#endif
#ifndef F77_NAME_LOWER
extern FORT_DLL_SPEC void FORT_CALL pmpi_errhandler_set( MPI_Fint *, MPI_Fint *, MPI_Fint * );

#endif

#if defined(F77_NAME_UPPER)
#pragma weak pmpi_errhandler_set__ = PMPI_ERRHANDLER_SET
#pragma weak pmpi_errhandler_set_ = PMPI_ERRHANDLER_SET
#pragma weak pmpi_errhandler_set = PMPI_ERRHANDLER_SET
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma weak PMPI_ERRHANDLER_SET = pmpi_errhandler_set__
#pragma weak pmpi_errhandler_set_ = pmpi_errhandler_set__
#pragma weak pmpi_errhandler_set = pmpi_errhandler_set__
#elif defined(F77_NAME_LOWER_USCORE)
#pragma weak PMPI_ERRHANDLER_SET = pmpi_errhandler_set_
#pragma weak pmpi_errhandler_set__ = pmpi_errhandler_set_
#pragma weak pmpi_errhandler_set = pmpi_errhandler_set_
#else
#pragma weak PMPI_ERRHANDLER_SET = pmpi_errhandler_set
#pragma weak pmpi_errhandler_set__ = pmpi_errhandler_set
#pragma weak pmpi_errhandler_set_ = pmpi_errhandler_set
#endif /* Test on name mapping */

#elif defined(HAVE_WEAK_ATTRIBUTE)
#if defined(F77_NAME_UPPER)
extern FORT_DLL_SPEC void FORT_CALL pmpi_errhandler_set__( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_ERRHANDLER_SET")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_errhandler_set_( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_ERRHANDLER_SET")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_errhandler_set( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_ERRHANDLER_SET")));

#elif defined(F77_NAME_LOWER_2USCORE)
extern FORT_DLL_SPEC void FORT_CALL PMPI_ERRHANDLER_SET( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set__")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_errhandler_set_( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set__")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_errhandler_set( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set__")));

#elif defined(F77_NAME_LOWER_USCORE)
extern FORT_DLL_SPEC void FORT_CALL PMPI_ERRHANDLER_SET( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set_")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_errhandler_set__( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set_")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_errhandler_set( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set_")));

#else
extern FORT_DLL_SPEC void FORT_CALL PMPI_ERRHANDLER_SET( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_errhandler_set__( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_errhandler_set_( MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_errhandler_set")));

#endif /* Test on name mapping */
#endif /* HAVE_MULTIPLE_PRAGMA_WEAK */
#endif /* USE_WEAK_SYMBOLS */

#ifdef F77_NAME_UPPER
#define mpi_errhandler_set_ PMPI_ERRHANDLER_SET
#elif defined(F77_NAME_LOWER_2USCORE)
#define mpi_errhandler_set_ pmpi_errhandler_set__
#elif !defined(F77_NAME_LOWER_USCORE)
#define mpi_errhandler_set_ pmpi_errhandler_set
#else
#define mpi_errhandler_set_ pmpi_errhandler_set_
#endif /* Test on name mapping */

#ifdef F77_USE_PMPI
/* This defines the routine that we call, which must be the PMPI version
   since we're renaming the Fortran entry as the pmpi version.  The MPI name
   must be undefined first to prevent any conflicts with previous renamings. */
#undef MPI_Errhandler_set
#define MPI_Errhandler_set PMPI_Errhandler_set 
#endif

#else

#ifdef F77_NAME_UPPER
#define mpi_errhandler_set_ MPI_ERRHANDLER_SET
#elif defined(F77_NAME_LOWER_2USCORE)
#define mpi_errhandler_set_ mpi_errhandler_set__
#elif !defined(F77_NAME_LOWER_USCORE)
#define mpi_errhandler_set_ mpi_errhandler_set
/* Else leave name alone */
#endif


#endif /* MPICH_MPI_FROM_PMPI */

/* Prototypes for the Fortran interfaces */
#include "fproto.h"
FORT_DLL_SPEC void FORT_CALL mpi_errhandler_set_ ( MPI_Fint *v1, MPI_Fint *v2, MPI_Fint *ierr ){
    *ierr = MPI_Errhandler_set( (MPI_Comm)(*v1), (MPI_Errhandler)*v2 );
}
