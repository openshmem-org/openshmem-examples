/*
 *
 * Copyright (c) 2011 - 2015
 *   University of Houston System and Oak Ridge National Laboratory.
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * o Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * 
 * o Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 
 * o Neither the name of the University of Houston System, Oak Ridge
 *   National Laboratory nor the names of its contributors may be used to
 *   endorse or promote products derived from this software without specific
 *   prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */



/*
 * reduce [0,1,2] + shmem_my_pe() across 4 PEs with MAX()
 *
 *
 */

#include <stdio.h>
#include <string.h>

#include <shmem.h>

long pSync[_SHMEM_REDUCE_SYNC_SIZE];
long pWrk[_SHMEM_REDUCE_SYNC_SIZE];

#define N 3

long src[N];
long dst[N];

int
main ()
{
  int i;

  for (i = 0; i < _SHMEM_REDUCE_SYNC_SIZE; i += 1)
    {
      pSync[i] = _SHMEM_SYNC_VALUE;
    }

  shmem_init ();

  for (i = 0; i < N; i += 1)
    {
      src[i] = shmem_my_pe () + i;
    }
  shmem_barrier_all ();

  shmem_long_max_to_all (dst, src, 3, 0, 0, 4, pWrk, pSync);

  printf ("%d/%d   dst =", shmem_my_pe (), shmem_n_pes ());
  for (i = 0; i < N; i += 1)
    {
      printf (" %d", dst[i]);
    }
  printf ("\n");

  return 0;
}
