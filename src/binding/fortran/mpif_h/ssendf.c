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
extern FORT_DLL_SPEC void FORT_CALL MPI_SSEND( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend__( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend_( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#if defined(F77_NAME_UPPER)
#pragma weak MPI_SSEND = PMPI_SSEND
#pragma weak mpi_ssend__ = PMPI_SSEND
#pragma weak mpi_ssend_ = PMPI_SSEND
#pragma weak mpi_ssend = PMPI_SSEND
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma weak MPI_SSEND = pmpi_ssend__
#pragma weak mpi_ssend__ = pmpi_ssend__
#pragma weak mpi_ssend_ = pmpi_ssend__
#pragma weak mpi_ssend = pmpi_ssend__
#elif defined(F77_NAME_LOWER_USCORE)
#pragma weak MPI_SSEND = pmpi_ssend_
#pragma weak mpi_ssend__ = pmpi_ssend_
#pragma weak mpi_ssend_ = pmpi_ssend_
#pragma weak mpi_ssend = pmpi_ssend_
#else
#pragma weak MPI_SSEND = pmpi_ssend
#pragma weak mpi_ssend__ = pmpi_ssend
#pragma weak mpi_ssend_ = pmpi_ssend
#pragma weak mpi_ssend = pmpi_ssend
#endif



#elif defined(HAVE_PRAGMA_WEAK)

#if defined(F77_NAME_UPPER)
extern FORT_DLL_SPEC void FORT_CALL MPI_SSEND( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#pragma weak MPI_SSEND = PMPI_SSEND
#elif defined(F77_NAME_LOWER_2USCORE)
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend__( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#pragma weak mpi_ssend__ = pmpi_ssend__
#elif !defined(F77_NAME_LOWER_USCORE)
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#pragma weak mpi_ssend = pmpi_ssend
#else
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend_( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#pragma weak mpi_ssend_ = pmpi_ssend_
#endif

#elif defined(HAVE_PRAGMA_HP_SEC_DEF)
#if defined(F77_NAME_UPPER)
#pragma _HP_SECONDARY_DEF PMPI_SSEND  MPI_SSEND
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma _HP_SECONDARY_DEF pmpi_ssend__  mpi_ssend__
#elif !defined(F77_NAME_LOWER_USCORE)
#pragma _HP_SECONDARY_DEF pmpi_ssend  mpi_ssend
#else
#pragma _HP_SECONDARY_DEF pmpi_ssend_  mpi_ssend_
#endif

#elif defined(HAVE_PRAGMA_CRI_DUP)
#if defined(F77_NAME_UPPER)
#pragma _CRI duplicate MPI_SSEND as PMPI_SSEND
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma _CRI duplicate mpi_ssend__ as pmpi_ssend__
#elif !defined(F77_NAME_LOWER_USCORE)
#pragma _CRI duplicate mpi_ssend as pmpi_ssend
#else
#pragma _CRI duplicate mpi_ssend_ as pmpi_ssend_
#endif

#elif defined(HAVE_WEAK_ATTRIBUTE)
#if defined(F77_NAME_UPPER)
extern FORT_DLL_SPEC void FORT_CALL MPI_SSEND( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_SSEND")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend__( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_SSEND")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend_( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_SSEND")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_SSEND")));

#elif defined(F77_NAME_LOWER_2USCORE)
extern FORT_DLL_SPEC void FORT_CALL MPI_SSEND( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend__")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend__( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend__")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend_( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend__")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend__")));

#elif defined(F77_NAME_LOWER_USCORE)
extern FORT_DLL_SPEC void FORT_CALL MPI_SSEND( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend_")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend__( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend_")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend_( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend_")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend_")));

#else
extern FORT_DLL_SPEC void FORT_CALL MPI_SSEND( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend__( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend_( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend")));

#endif
#endif /* HAVE_PRAGMA_WEAK */
#endif /* USE_WEAK_SYMBOLS */
/* End MPI profiling block */


/* These definitions are used only for generating the Fortran wrappers */
#if defined(USE_WEAK_SYMBOLS) && defined(USE_ONLY_MPI_NAMES)
#if defined(HAVE_MULTIPLE_PRAGMA_WEAK)
extern FORT_DLL_SPEC void FORT_CALL MPI_SSEND( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend__( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend_( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#if defined(F77_NAME_UPPER)
#pragma weak mpi_ssend__ = MPI_SSEND
#pragma weak mpi_ssend_ = MPI_SSEND
#pragma weak mpi_ssend = MPI_SSEND
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma weak MPI_SSEND = mpi_ssend__
#pragma weak mpi_ssend_ = mpi_ssend__
#pragma weak mpi_ssend = mpi_ssend__
#elif defined(F77_NAME_LOWER_USCORE)
#pragma weak MPI_SSEND = mpi_ssend_
#pragma weak mpi_ssend__ = mpi_ssend_
#pragma weak mpi_ssend = mpi_ssend_
#else
#pragma weak MPI_SSEND = mpi_ssend
#pragma weak mpi_ssend__ = mpi_ssend
#pragma weak mpi_ssend_ = mpi_ssend
#endif
#elif defined(HAVE_WEAK_ATTRIBUTE)
#if defined(F77_NAME_UPPER)
extern FORT_DLL_SPEC void FORT_CALL MPI_SSEND( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend__( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("MPI_SSEND")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend_( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("MPI_SSEND")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("MPI_SSEND")));

#elif defined(F77_NAME_LOWER_2USCORE)
extern FORT_DLL_SPEC void FORT_CALL MPI_SSEND( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_ssend__")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend__( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend_( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_ssend__")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_ssend__")));

#elif defined(F77_NAME_LOWER_USCORE)
extern FORT_DLL_SPEC void FORT_CALL MPI_SSEND( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_ssend_")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend__( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_ssend_")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend_( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_ssend_")));

#else
extern FORT_DLL_SPEC void FORT_CALL MPI_SSEND( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_ssend")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend__( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_ssend")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend_( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("mpi_ssend")));
extern FORT_DLL_SPEC void FORT_CALL mpi_ssend( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#endif
#endif

#endif

/* Map the name to the correct form */
#ifndef MPICH_MPI_FROM_PMPI
#if defined(USE_WEAK_SYMBOLS)
#if defined(HAVE_MULTIPLE_PRAGMA_WEAK)
/* Define the weak versions of the PMPI routine*/
#ifndef F77_NAME_UPPER
extern FORT_DLL_SPEC void FORT_CALL PMPI_SSEND( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
#endif
#ifndef F77_NAME_LOWER_2USCORE
extern FORT_DLL_SPEC void FORT_CALL pmpi_ssend__( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
#endif
#ifndef F77_NAME_LOWER_USCORE
extern FORT_DLL_SPEC void FORT_CALL pmpi_ssend_( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );
#endif
#ifndef F77_NAME_LOWER
extern FORT_DLL_SPEC void FORT_CALL pmpi_ssend( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * );

#endif

#if defined(F77_NAME_UPPER)
#pragma weak pmpi_ssend__ = PMPI_SSEND
#pragma weak pmpi_ssend_ = PMPI_SSEND
#pragma weak pmpi_ssend = PMPI_SSEND
#elif defined(F77_NAME_LOWER_2USCORE)
#pragma weak PMPI_SSEND = pmpi_ssend__
#pragma weak pmpi_ssend_ = pmpi_ssend__
#pragma weak pmpi_ssend = pmpi_ssend__
#elif defined(F77_NAME_LOWER_USCORE)
#pragma weak PMPI_SSEND = pmpi_ssend_
#pragma weak pmpi_ssend__ = pmpi_ssend_
#pragma weak pmpi_ssend = pmpi_ssend_
#else
#pragma weak PMPI_SSEND = pmpi_ssend
#pragma weak pmpi_ssend__ = pmpi_ssend
#pragma weak pmpi_ssend_ = pmpi_ssend
#endif /* Test on name mapping */

#elif defined(HAVE_WEAK_ATTRIBUTE)
#if defined(F77_NAME_UPPER)
extern FORT_DLL_SPEC void FORT_CALL pmpi_ssend__( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_SSEND")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_ssend_( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_SSEND")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_ssend( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("PMPI_SSEND")));

#elif defined(F77_NAME_LOWER_2USCORE)
extern FORT_DLL_SPEC void FORT_CALL PMPI_SSEND( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend__")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_ssend_( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend__")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_ssend( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend__")));

#elif defined(F77_NAME_LOWER_USCORE)
extern FORT_DLL_SPEC void FORT_CALL PMPI_SSEND( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend_")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_ssend__( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend_")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_ssend( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend_")));

#else
extern FORT_DLL_SPEC void FORT_CALL PMPI_SSEND( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_ssend__( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend")));
extern FORT_DLL_SPEC void FORT_CALL pmpi_ssend_( void*, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint *, MPI_Fint * ) __attribute__((weak,alias("pmpi_ssend")));

#endif /* Test on name mapping */
#endif /* HAVE_MULTIPLE_PRAGMA_WEAK */
#endif /* USE_WEAK_SYMBOLS */

#ifdef F77_NAME_UPPER
#define mpi_ssend_ PMPI_SSEND
#elif defined(F77_NAME_LOWER_2USCORE)
#define mpi_ssend_ pmpi_ssend__
#elif !defined(F77_NAME_LOWER_USCORE)
#define mpi_ssend_ pmpi_ssend
#else
#define mpi_ssend_ pmpi_ssend_
#endif /* Test on name mapping */

#ifdef F77_USE_PMPI
/* This defines the routine that we call, which must be the PMPI version
   since we're renaming the Fortran entry as the pmpi version.  The MPI name
   must be undefined first to prevent any conflicts with previous renamings. */
#undef MPI_Ssend
#define MPI_Ssend PMPI_Ssend 
#endif

#else

#ifdef F77_NAME_UPPER
#define mpi_ssend_ MPI_SSEND
#elif defined(F77_NAME_LOWER_2USCORE)
#define mpi_ssend_ mpi_ssend__
#elif !defined(F77_NAME_LOWER_USCORE)
#define mpi_ssend_ mpi_ssend
/* Else leave name alone */
#endif


#endif /* MPICH_MPI_FROM_PMPI */

/* Prototypes for the Fortran interfaces */
#include "fproto.h"
FORT_DLL_SPEC void FORT_CALL mpi_ssend_ ( void*v1, MPI_Fint *v2, MPI_Fint *v3, MPI_Fint *v4, MPI_Fint *v5, MPI_Fint *v6, MPI_Fint *ierr ){
    *ierr = MPI_Ssend( v1, (int)*v2, (MPI_Datatype)(*v3), (int)*v4, (int)*v5, (MPI_Comm)(*v6) );
}
