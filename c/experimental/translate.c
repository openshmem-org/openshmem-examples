#include <stdio.h>
#include <unistd.h>

#include <shmem.h>
#include <shmemx.h>

static const int target_pe = 1;

int
main()
{
    int me;
    long *symvar;

    shmem_init ();
    me = shmem_my_pe ();

    symvar = shmem_malloc (sizeof (*symvar) * 4);

    printf("PE %d: symvar @ %p\n", me, symvar);
    fflush (stdout);
    sleep (1);

    if (me == 0) {
        long *remote_symvar = shmemx_lookup_remote_addr (symvar, target_pe);

        if (symvar == remote_symvar) {
            printf ("symvar @ same address on PE %d\n", target_pe);
        }
        else {
            printf("symvar @ %p is at %p on PE %d\n",
                   symvar, remote_symvar, target_pe);
        }
    }

    shmem_barrier_all ();
    shmem_free (symvar);

    return 0;
}
