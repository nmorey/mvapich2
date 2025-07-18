! This file created from f77/pt2pt/bsendf.f with f77tof90
!
! Copyright (C) by Argonne National Laboratory
!     See COPYRIGHT in top-level directory
!

! Basic test for MPI_Bsend
!     We test a basic buffered send of 10 INTEGERs and assume a buffer
!     of 400 CHARACTERs are sufficient to account for MPI_BSEND_OVERHEAD

      program bsend
      use mpi
      integer ierr, errs, comm
      character dummy_buf(400)
      INTEGER dummy_size
!
      errs = 0
      comm = MPI_COMM_WORLD;
      call MTest_Init( ierr )
      call mpi_buffer_attach(dummy_buf, 400, ierr )
      call test_bsend( comm, errs )
      call mpi_buffer_detach(dummy_buf, dummy_size, ierr )
      call MTest_Finalize( errs )
      end
!
      subroutine test_bsend( comm, errs )
      use mpi
      integer comm, errs
      integer rank, size, ierr, tag
      integer i
      integer N
      parameter (N=10)
      integer buf(N)

      tag = 1123
      call mpi_comm_rank( comm, rank, ierr )
      call mpi_comm_size( comm, size, ierr )
!
      if (rank .eq. 0) then
         do i = 1, N
            buf(i) = i
         end do
         call MPI_Bsend(buf, N, MPI_INTEGER, 1, tag, comm, ierr)
      else if (rank .eq. 1)  then
         do i = 1, N
            buf(i) = 0.0
         end do
         call MPI_Recv(buf, N, MPI_INTEGER, 0, tag, comm, &
      &                 MPI_STATUS_IGNORE, ierr)
         do i = 1, N
            if (buf(i) .ne. i) then
                print *, i, buf(i)
                errs = errs + 1
            end if
         end do
      end if
!
      end
