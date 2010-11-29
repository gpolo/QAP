/*
 * Created: Sáb 20 Nov 2010 19:22:27 BRST
 *
 * Author: Maycon Sambinelli, msambinelli@gmail.com
 *
 */
 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <string.h>

#include "QAP.h"
#include "ant.h"
#include "util.h"

#define MAXNAME 255

void run_info(QAP_t *prob, char *file_name, char *flags_msg, int seed) {
    printf ("\n\n");
    printf ("Instance: %s\n", file_name);
    printf ("Problem size: %d\n", prob->size);
    printf ("Seed: %d\n", seed);
    printf ("Number of ants: %d\n", prob->nants);
    printf ("Pheromone persistence: %0.3f\n", prob->phero_pers);
    printf ("Options:%s\n", flags_msg);
    if (prob->flags & MGENERATION){
        printf ("Number of Generations: %d\n", prob->ngenerations);
    }
    if (prob->flags & TIMESTOP){
        printf ("Time Limit: %f\n", prob->time_limit);
    }
    if (prob->flags & BESTSTOP){
        printf ("Stop in best know solution: %d\n", prob->best_know_solution);
    }
    printf("\n");
}

void usage(const char *prog){
    fprintf (stderr, "Usage: %s [OPTIONS] qaplibinput.data\n", prog);
    fprintf (stderr, "\nOPTIONS:\n");
    fprintf (stderr, "  -v\t\tBe verbose\n");
    fprintf (stderr, "  -h\t\tHelp\n");
    fprintf (stderr, "  -S seed\tSet a seed\n");
    fprintf (stderr, "  -a\t\tAsymmetric instance\n");
    fprintf (stderr, "  -s\t\tSymmetric instance\n");
    fprintf (stderr, "  -i\t\tUse iteration best solution to update the pheromone\n");
    fprintf (stderr, "  -I\t\tUse besto so far solution to update the pheromone\n");
    fprintf (stderr, "  -n num\tNumber of ants\n");
    fprintf (stderr, "  -p num\tPheromone persistance\n");
    fprintf (stderr, "  -g num\tMaximum number of generations\n");
    fprintf (stderr, "  -t secs\tTime limit\n");
    fprintf (stderr, "  -b cust\tStop when the cust of best known solution is found\n");
    printf ("\n");
    exit(1);
}

int main (int argc, char * const argv[])
{

    int c, nants = 10, i, j, ngenerations = 250, best_know_sol = 0;
    unsigned flags = 0;
    float phero_pers = 0.98, time_limit = 180;
    char file_name[MAXNAME], flags_msg[100] = "";
    FILE *file;
    QAP_t *qap;
    QAP_solution_t *sol;
    long int seed;

    seed = seed_prng();

    if (argc < 2){
        usage(argv[0]);       
    }

    /* Opções:
     *  S - seed
     *  n - number of ants
     *  p - pheromone persistence -- interval: [0,1]
     *  i - iteration best used to update pheromone
     *  I - best so for used to update pheromone
     *  a - Asymmetric QAP 
     *  s - Symmetric QAP
     *  g - number of generations
     *  h - help
     *  t - max time run
     *  v - verbose
     *  b - stop when it've found best solution
     */
    while ((c = getopt(argc, argv, ":S:n:p:iIsag:ht:vb:")) != -1 ){
        switch (c){
            case 'i':
                if (flags & PHEUP_BF){
                    fprintf (stderr, "Choose only one method for updating the pheromone\n");
                    _exit(1);
                }
                flags |= PHEUP_IB;
                strcat (flags_msg, " PHEUP_IB");
                break;
            case 'I':
                if (flags & PHEUP_IB){
                    fprintf (stderr, "Choose only one method for updating the pheromone\n");
                    _exit(1);
                }
                flags |= PHEUP_BF;
                strcat (flags_msg, " PHEUP_BF");
                break;
            case 'g':
                ngenerations = atoi(optarg);
                flags |= MGENERATION;
                strcat (flags_msg, " MGENERATION");
                break;
            case 'S':
                seed = atoi(optarg);
                break;
            case 'n':
                nants = atoi(optarg);
                break;
            case 'p':
                phero_pers = atof(optarg);
                break;
            case 's':
                if (flags & ASYMMETRIC){
                    fprintf(stderr, "CONFLICT: An instance is symmetric or asymmetric\n");
                    exit(1);
                }
                flags |= SYMMETRIC;
                strcat (flags_msg, " SYMMETRIC");
                break;
            case 'a':
                if (flags & SYMMETRIC){
                    fprintf(stderr, "CONFLICT: An instance is symmetric or asymmetric\n");
                    exit(1);
                }
                flags |= ASYMMETRIC;
                strcat (flags_msg, " ASYMMETRIC");
                break;
            case 't':
                time_limit = atof(optarg);
                flags |= TIMESTOP;
                strcat (flags_msg, " TIMESTOP");
                break;
            case 'b':
                best_know_sol = atoi(optarg);
                flags |= BESTSTOP;
                strcat (flags_msg, " BESTSTOP");
                break;
            case 'v':
                strcat (flags_msg, " VERBOSE");
                flags |= VERBOSE;
                break;
            case 'h':
                usage(argv[0]);
                break;
            case '?':
                fprintf (stderr, "unknow option '%c'\n", optopt);
                break;
            case ':':
                printf("option required for '%c'\n", optopt);
                break;
        }
    }

    if (!(flags & SYMMETRIC) && !(flags & ASYMMETRIC)){
        flags |= ASYMMETRIC;
    }

    if (!(flags & PHEUP_IB) && !(flags & PHEUP_BF)){
        flags |= PHEUP_IB;       
    }

    if (argc == optind){
        fprintf (stderr, "no input file\n");       
        _exit(0);
    }

    strcpy(file_name, argv[optind]);

    file = fopen (file_name, "r");
    if (!file){
        fprintf (stderr, "error during file opening\n");       
        _exit(0);
    }

    srand48(seed);

    qap = Malloc(sizeof(QAP_t));
    qap->nants = nants;
    qap->phero_pers = phero_pers;
    qap->flags = flags;
    qap->ngenerations = ngenerations;
    qap->best_know_solution = best_know_sol;
    qap->time_limit = time_limit;

    fscanf (file, "%d", &qap->size); 
    for (i = 0; i < qap->size; i++)
        for (j = 0; j < qap->size; j++)
            fscanf (file, "%d", &qap->dist[i][j]);

    for (i = 0; i < qap->size; i++)
        for (j = 0; j < qap->size; j++)
            fscanf (file, "%d", &qap->flow[i][j]);

    fclose(file);

    sol = aco(qap);
    run_info(qap, file_name, flags_msg, seed);
    QAP_show_solution(qap, sol);
    return 0;
}
