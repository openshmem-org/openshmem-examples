/*
 *
 * Copyright (c) 2016 - 2021
 *   Stony Brook University
 * Copyright (c) 2015 - 2018
 *   Los Alamos National Security, LLC.
 * Copyright (c) 2011 - 2015
 *   University of Houston System and UT-Battelle, LLC.
 * Copyright (c) 2009 - 2015
 *   Silicon Graphics International Corp.  SHMEM is copyrighted
 *   by Silicon Graphics International Corp. (SGI) The OpenSHMEM API
 *   (shmem) is released by Open Source Software Solutions, Inc., under an
 *   agreement with Silicon Graphics International Corp. (SGI).
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
 * o Neither the name of the University of Houston System,
 *   UT-Battelle, LLC. nor the names of its contributors may be used to
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
 * allow user to select global or malloc'ed lock; set or test lock to
 * grab the lock
 *
 * adapted from example in the OpenSHMEM specification:
 *
 * http://www.openshmem.org/site/sites/default/site_files/OpenSHMEM-1.5.pdf
 *
 * p. 140
 */

#include <shmem.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <getopt.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  static int count = 0;
  struct utsname u;
  int dynamic = 0;
  int use_test = 0;
  long *lock;

  while (1) {
      const int c = getopt(argc, argv, "dt");

      if (c == -1) {
          break;
          /* NOT REACHED */
      }

      switch (c) {
      case 'd':
          dynamic = 1;
          break;
      case 't':
          use_test = 1;
          break;
      default:
          exit(1);
          break;
      }
  }

  uname(&u);

  shmem_init();

  int mype = shmem_my_pe();

  if (dynamic) {
      if (mype == 0) fprintf(stderr, "Using dynamic lock\n");
      lock = shmem_malloc(sizeof(long));
      *lock = 0;
      shmem_barrier_all();
  }
  else {
      if (mype == 0) fprintf(stderr, "Using static lock\n");
      static long _lock  = 0;
      lock  = &_lock;
  }

  if (use_test) {
      if (mype == 0) fprintf(stderr, "Using test_lock\n");
      while (shmem_test_lock(lock));
  }
  else {
      if (mype == 0) fprintf(stderr, "Using set_lock\n");
      shmem_set_lock(lock);
  }

  int val = shmem_g(&count, 0);

  printf("%s: %d: count is %d\n", u.nodename, mype, val);

  val++;

  shmem_int_p(&count, val, 0);

  shmem_clear_lock(lock);

  shmem_finalize();

  return 0;
}
