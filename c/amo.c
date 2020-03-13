/*****************************************************************************
 *  Copyright (C) 2015 Silicon Graphics International Corp.
 *  All rights reserved.

 ****************************************************************************/

/* Program contributed by SGI on the OpenSHMEM mailing list*/

#include <shmem.h>
#include <stdio.h>
#include <stdlib.h>

const int tries = 10000;

#ifdef TEST64BIT
typedef long locktype;

#else /*  */
typedef int locktype;

#endif /*  */
int
main()
{
    int tpe, other;
    long i;
    struct
    {
        locktype a;
        locktype b;
    } *twovars;
    int numfail = 0;
    shmem_init();
    tpe = 0;
    other = shmem_n_pes() - 1;
    twovars = shmem_malloc(sizeof(*twovars));
    if (shmem_my_pe() == 0) {
        printf("Element size: %ld bytes\n", sizeof(locktype));
        printf("Addresses: 1st element %p\n", (void *) &twovars->a);
        printf("           2nd element %p\n", (void *) &twovars->b);
        printf("Iterations: %d   target PE: %d   other active PE: %d\n",
               tries, tpe, other);
    }
    twovars->a = 0;
    twovars->b = 0;
    shmem_barrier_all();
    if (shmem_my_pe() == 0) {

        // put two values alternately to the 1st 32 bit word
        long expect, check;
        for (i = 0; i < tries; i++) {
            expect = 2 + i % 2;
            if (sizeof(locktype) == sizeof(int)) {
                shmem_int_p((void *) &twovars->a, expect, tpe);
                check = shmem_int_g((void *) &twovars->a, tpe);
            }
            else if (sizeof(locktype) == sizeof(long)) {
                shmem_long_p((void *) &twovars->a, expect, tpe);
                check = shmem_long_g((void *) &twovars->a, tpe);
            }
            if (check != expect) {
                printf("error: iter %ld get returned %ld expected %ld\n", i,
                       check, expect);
                numfail++;
                if (numfail > 10) {
                    printf("FAIL\n");
                    abort();
                }
            }
        }
        printf("PE %d done doing puts and gets\n", shmem_my_pe());
    }
    else if (shmem_my_pe() == other) {

        // keep on atomically incrementing the 2nd 32 bit word
        long oldval;
        for (i = 0; i < tries; i++) {
            if (sizeof(locktype) == sizeof(int)) {
                oldval =
                    shmem_int_atomic_fetch_inc((void *) &twovars->b, tpe);
            }
            else if (sizeof(locktype) == sizeof(long)) {
                oldval =
                    shmem_long_atomic_fetch_inc((void *) &twovars->b, tpe);
            }
            if (oldval != i) {
                printf("error: iter %ld finc got %ld expect %ld\n", i,
                       oldval, i);
                numfail++;
                if (numfail > 10) {
                    printf("FAIL\n");
                    abort();
                }
            }
        }
        printf("PE %d done doing fincs\n", shmem_my_pe());
    }
    shmem_barrier_all();
    if (numfail) {
        printf("FAIL\n");
    }
    shmem_barrier_all();
    if (shmem_my_pe() == 0) {
        printf("test complete\n");
    }
    shmem_finalize();
    return 0;
}
