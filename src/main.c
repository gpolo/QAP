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
#include "flags.h"

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
    /* Artificial: */
    fprintf(stderr, "\t-A cost\t  Stop when 'cost' is found\n");
    fprintf(stderr, "\t-S seed\t  Set a seed\n");
    fprintf(stderr, "\t-T time\t  Set a time limit in floating point seconds\n");
    fprintf(stderr, "\t-a\t  Asymmetric instance\n");
    fprintf(stderr, "\t-s\t  Symmetric instance\n");
    fprintf(stderr, "\t-m\t  Use memetic algorithm [default]\n");
    exit(1);
}

static struct ParseResult *
try_openinput(int argc, char *argv[])//, long int *seed, double *time_limit)
{
#ifdef LEAVE
#undef LEAVE
#endif
#define LEAVE(msg) fprintf(stderr, msg); exit(1)

    struct ParseResult *result = Malloc(sizeof(struct ParseResult));
    int ch;

    result->flags = 0;

    if (argc < 2) {
        usage(argv[0]);
    }

    while ((ch = getopt(argc, argv, "vA:S:T:asm")) != -1) {
        switch (ch) {
        case 'v':
            result->flags |= VERBOSE;
            break;

        case 'A':
            result->flags |= ARTIFICIAL;
            result->artificial_cost_limit = atoi(optarg);
            break;

        case 'S':
            result->flags |= SET_SEED;
            result->seed = atoll(optarg);
            break;

        case 'T':
            result->flags |= TIME_LIMIT;
            result->time_limit = atof(optarg);
            break;

        case 'a':
            if (result->flags & SYMMETRIC) {
                LEAVE("CONFLICT: An instance is either symmetric or "
                        "asymmetric.\n");
            }
            result->flags |= ASYMMETRIC;
            break;

        case 's':
            if (result->flags & ASYMMETRIC) {
                LEAVE("CONFLICT: An instance is either symmetric or "
                        "asymmetric.\n");
            }
            result->flags |= SYMMETRIC;
            break;

        case 'm':
            result->flags |= MEMETIC;
            break;

        default:
            usage(argv[0]);
            break;
        } /* switch */
    }

    if (!(result->flags & SYMMETRIC) && !(result->flags & ASYMMETRIC)) {
        LEAVE("MISSING: Is this instance symmetric or asymmetric ?\n");
    }

    if (!(result->flags & MEMETIC)) {
        /* Default to Memetic Algorithm. */
        result->flags |= MEMETIC;
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

    return result;

#undef LEAVE
}


int
main(int argc, char *argv[])
{
    int result;
    struct ParseResult *pinfo;

    pinfo = try_openinput(argc, argv);

    if (seed_prng(pinfo->seed,
                ((pinfo->flags & SET_SEED) ? SEED_REUSE : SEED_NEW),
                &(pinfo->seed)) < 0) {
        return 1;
    }

    printf("Seed: %ld\n", pinfo->seed);

    if (pinfo->flags & MEMETIC) {
        result = memetic(pinfo);
    } else {
        fprintf(stderr, "Not supported.\n");
        result = 1;
    }

    free(pinfo);
    return result;
}
