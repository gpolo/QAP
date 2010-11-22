/*
 * Created:  22/11/2010 12:33:06
 *
 * Author:  Guilherme Polo, ggpolo@gmail.com
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "util.h"
#include "memetic.h"

#define VERBOSE     2
#define SYMMETRIC   4
#define ASYMMETRIC  8
#define MEMETIC    16
#define SET_SEED   64

/* Cool seeds:
 *  763879292910
 *  2983906046
 */

static void
usage(const char *prog)
{
    fprintf(stderr, "Usage: %s [OPTIONS] qaplibinput.data\n", prog);
    fprintf(stderr, "\nOPTIONS:\n");
    fprintf(stderr, "\t-v\t  Be verbose\n");
    fprintf(stderr, "\t-S seed\t  Set a seed\n");
    fprintf(stderr, "\t-a\t  Asymmetric instance\n");
    fprintf(stderr, "\t-s\t  Symmetric instance\n");
    fprintf(stderr, "\t-m\t  Use memetic algorithm [default]\n");
    exit(1);
}

static int
try_openinput(int argc, char *argv[], long int *seed)
{
#ifdef LEAVE
#undef LEAVE
#endif
#define LEAVE(msg) fprintf(stderr, msg); exit(1)

    int ch;
    int status = 0;

    if (argc < 2) {
        usage(argv[0]);
    }

    while ((ch = getopt(argc, argv, "vS:asm")) != -1) {
        switch (ch) {
        case 'v':
            status |= VERBOSE;
            break;

        case 'S':
            status |= SET_SEED;
            *seed = atoll(optarg);
            break;

        case 'a':
            if (status & SYMMETRIC) {
                LEAVE("CONFLICT: An instance is either symmetric or "
                        "asymmetric.\n");
            }
            status |= ASYMMETRIC;
            break;

        case 's':
            if (status & ASYMMETRIC) {
                LEAVE("CONFLICT: An instance is either symmetric or "
                        "asymmetric.\n");
            }
            status |= SYMMETRIC;
            break;

        case 'm':
            status |= MEMETIC;
            break;
        } /* switch */
    }

    if (!(status & SYMMETRIC) && !(status & ASYMMETRIC)) {
        LEAVE("MISSING: Is this instance symmetric or asymmetric ?\n");
    }

    if (!(status & MEMETIC)) {
        /* Default to Memetic Algorithm. */
        status |= MEMETIC;
    }

    argc -= optind;
    argv += optind;
    if (argc != 1) {
        usage((argv - optind)[0]);
    } else {
        if (freopen(*argv, "r", stdin) == NULL) {
            perror("freopen");
            exit(1);
        }
    }

    return status;

#undef LEAVE
}


int
main(int argc, char *argv[])
{
    int pinfo;
    long int seed;

    pinfo = try_openinput(argc, argv, &seed);

    if (seed_prng(seed, ((pinfo & SET_SEED) ? SEED_REUSE : SEED_NEW),
                &seed) < 0) {
        return 1;
    }

    printf("Seed: %ld\n", seed);

    if (pinfo & MEMETIC) {
        return memetic(pinfo & VERBOSE, pinfo & ASYMMETRIC);
    } else {
        fprintf(stderr, "Not supported.\n");
        return 1;
    }
}
