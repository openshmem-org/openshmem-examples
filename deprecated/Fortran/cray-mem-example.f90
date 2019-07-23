!
! This example adapted from Cray's man pages:
!
!   http://docs.cray.com/cgi-bin/craydoc.cgi?mode=View;id=sw_releases-dj1kd1jh-1382363518;idx=man_search;this_sort=title;q=;type=man;title=Message%20Passing%20Toolkit%20%28MPT%29%206.2%20Man%20Pages
!
! Original copyright indicated at above URL.
!
! Changes here are to make it OpenSHMEM compliant (e.g. can't use
! shmem_my_pe as variable because it conflicts with API).
!
! Requires "cray pointer" compatibility (e.g. -fcray-pointer with GCC).
!

program shpalloc_example
  implicit none
  include "shmem.fh"
  integer :: shmem_n_pes    ! OpenSHMEM routine

  integer :: n_pes
  integer :: imax, jmax
  call shmem_init ()
  n_pes = shmem_n_pes()
  imax  = n_pes
  jmax  = n_pes
  call sub(imax,jmax)
  call shmem_finalize()
end program shpalloc_example

subroutine sub(imax,jmax)
  implicit none
  include "shmem.fh"
  integer :: shmem_my_pe, shmem_n_pes    ! OpenSHMEM routines

  integer imax, jmax
  integer :: mype, n_pes
  integer :: pe, err, idefault, ibytes
  pointer (pA,A)
  integer(kind=8) A(imax,jmax)
  mype = shmem_my_pe()
  n_pes = shmem_n_pes()
  pe = n_pes - mype - 1
  ibytes=kind(idefault)
  !          want to create matrix of size A(imax,jmax)
  call shpalloc(pA,imax*jmax*8/ibytes,err,0)
  A(imax,jmax) = mype
  call shmem_barrier_all()
  call shmem_put64(A(1,1), A(imax,jmax), 1, pe)
  call shmem_barrier_all()
  print *, mype, A(1,1)
  call shmem_barrier_all()
  call shpdeallc(pA,err,0)
  return
end subroutine sub
