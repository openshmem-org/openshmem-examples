/*
 *
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
 * SGI/SHMEM version of the C "pi" program that is part of the MPICH
 * distribution
 *
 * MPI version is:
 *  (C) 2001 by Argonne UT-Battelle, LLC.
 *      See COPYRIGHT in top-level directory (of MPICH distribution).
 */

#include <shmem.h>
#include <stdlib.h>
#include <sys/time.h>

#include <stdio.h>
#include <math.h>

static const double PI25DT = 3.141592653589793238462643;

static inline
double
f (double a)
{
  return (4.0 / (1.0 + a * a));
}

/*
 * these all need to be symmetric as shmem targets
 */
int n;
double mypi, pi;

long pSyncB[SHMEM_BCAST_SYNC_SIZE]; /* for broadcast */
long pSyncR[SHMEM_REDUCE_SYNC_SIZE]; /* for reduction */

static inline
long
max2(long a, long b)
{
    return (a > b) ? a : b;
}

/*
 *
 */

int
main (int argc, char *argv[])
{
  int myid, numprocs, i;
  double h, sum;
  struct timeval startwtime, endwtime;
  double *pWrkR;                /* symmetric reduction workspace */

  shmem_init ();
  numprocs = shmem_n_pes ();
  myid = shmem_my_pe ();

  if (myid == 0)
    {
      if (argc > 1)
	n = atoi (argv[1]);	/* # rectangles on command line */
      else
	n = 10000;		/* default # of rectangles */

      gettimeofday (&startwtime, NULL);
    }

  /* reduction of 1 value: size the workspace accordingly */
  {
      const long pWrkRSize = max2 (1/2 + 1, SHMEM_REDUCE_MIN_WRKDATA_SIZE);

      pWrkR = (double *) shmem_malloc (pWrkRSize * sizeof(*pWrkR));
  }

  /* initialize sync arrays */
  for (i = 0; i < SHMEM_BCAST_SYNC_SIZE; i += 1) {
    pSyncB[i] = SHMEM_SYNC_VALUE;
  }
  for (i = 0; i < SHMEM_REDUCE_SYNC_SIZE; i += 1) {
    pSyncR[i] = SHMEM_SYNC_VALUE;
  }
  shmem_barrier_all ();

  /* -=- set up done -=- */

  /* send "n" out to everyone */
  shmem_broadcast32 (&n, &n, 1, 0, 0, 0, numprocs, pSyncB);

  /* do partial computation */
  h = 1.0 / (double) n;
  sum = 0.0;
  /* A slightly better approach starts from large i and works back */
  for (i = myid + 1; i <= n; i += numprocs)
    {
      const double x = h * ((double) i - 0.5);
      sum += f (x);
    }
  mypi = h * sum;

  /* wait for everyone to finish */
  shmem_barrier_all ();

  /* add up partial pi computations into PI */
  shmem_double_sum_to_all (&pi, &mypi, 1, 0, 0, numprocs, pWrkR, pSyncR);

  /* "master" PE summarizes */
  if (myid == 0)
    {
      double elapsed;
      gettimeofday (&endwtime, NULL);
      elapsed = (endwtime.tv_sec - startwtime.tv_sec) * 1000.0;	/* sec to ms */
      elapsed += (endwtime.tv_usec - startwtime.tv_usec) / 1000.0;	/* us to ms */
      printf ("pi is approximately %.16f, Error is %.16f\n",
	      pi, fabs (pi - PI25DT));
      printf ("run time = %f ms\n", elapsed);
      fflush (stdout);
    }

  shmem_free (pWrkR);

  shmem_finalize ();

  return 0;
}
