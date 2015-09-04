program translate
  include 'shmem.fh'

  integer :: shmem_my_pe, shmem_n_pes
  integer :: me, npes

  integer, parameter :: target_pe = 1

  integer*8, save :: symvar
  integer*8       :: remote_symvar
  integer*8       :: shmemx_lookup_remote_addr

  call shmem_init ()
  me = shmem_my_pe ()
  npes = shmem_n_pes ()

  write (*, *) 'PE ', me, 'symvar @ ', loc(symvar)
  call sleep(1)

  if (me .eq. 0) then
     remote_symvar = shmemx_lookup_remote_addr (symvar, target_pe);
     
     write (*, *) remote_symvar
  end if

end program translate
