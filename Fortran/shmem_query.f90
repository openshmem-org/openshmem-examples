!
!
! Copyright (c) 2011 - 2015
!   University of Houston System and Oak Ridge National Laboratory.
!
! All rights reserved.
!
! Redistribution and use in source and binary forms, with or without
! modification, are permitted provided that the following conditions
! are met:
!
! o Redistributions of source code must retain the above copyright notice,
!   this list of conditions and the following disclaimer.
!
! o Redistributions in binary form must reproduce the above copyright
!   notice, this list of conditions and the following disclaimer in the
!   documentation and/or other materials provided with the distribution.
!
! o Neither the name of the University of Houston System, Oak Ridge
!   National Laboratory nor the names of its contributors may be used to
!   endorse or promote products derived from this software without specific
!   prior written permission.
!
! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
! "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
! LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
! A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
! HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
! SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
! TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
! PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
! LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
! NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
! SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
!

! For every PE:
!  PE<id> : Major ver=<..> (SUCCESS) Minor ver=<..>(SUCCESS) Name=<..>
!

program query

  implicit none
  include 'shmem.fh'

  character (len=SHMEM_MAX_NAME_LEN) name
  character (len=7) :: major_status, minor_status

  integer :: major_ver, minor_ver
  integer :: me, shmem_my_pe

  call shmem_init ()
  me = shmem_my_pe()

  call shmem_info_get_version(major_ver, minor_ver)
  call shmem_info_get_name(name)

  if (major_ver == SHMEM_MAJOR_VERSION) then
     major_status = "SUCCESS"
  else
     major_status = "FAIL"
  end if

  if (minor_ver == SHMEM_MINOR_VERSION) then
     minor_status = "SUCCESS"
  else
     minor_status = "FAIL"
  end if

  print *,'PE',me,':Major ver=',major_ver,'(',major_status,')',&
       ' Minor ver=',minor_ver,'(',minor_status,')',&
       ' Name="',name

end program query
