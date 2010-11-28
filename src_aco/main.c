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
    printf ("\n");
    printf ("Instance: %s\n", file_name);
    printf ("Problem size: %d\n", prob->size);
    printf ("Seed: %d\n", seed);
    printf ("Number of ants: %d\n", prob->nants);
    printf ("Number of Generations: %d\n", prob->ngenerations);
    printf ("Pheromone persistence: %0.3f\n", prob->phero_pers);
    printf ("Options:%s\n", flags_msg);
    printf ("Time Elepsed: %.2lf\n", current_user_time_secs());
}

void usage(const char *prog){
    fprintf (stderr, "Usage: %s [OPTIONS] qaplibinput.data\n", prog);
    fprintf (stderr, "\nOPTIONS:\n");
    fprintf (stderr, "  -v\t\t\t\tBe verbose\n"); //xxx implement
    fprintf (stderr, "  -S seed, --seed seed\t\tSet a seed\n");
    fprintf (stderr, "  -a, --asymmetric\t\tAsymmetric instance\n");
    fprintf (stderr, "  -s, --symmetric\t\tSymmetric instance\n");
    fprintf (stderr, "  -i, --iteration-best\t\tXXXXX\n");
    fprintf (stderr, "  -I, --best-so-far\t\tXXXXX\n");
    fprintf (stderr, "  -n num\t\t\tXXXXX\n");
    fprintf (stderr, "  --number-of-ants num\t\tXXXXX\n");
    fprintf (stderr, "  -p num\t\t\txxxx\n");
    fprintf (stderr, "  --pheromone-persistence num\txxxx\n");
    fprintf (stderr, "  -g num\t\t\txxxx\n");
    fprintf (stderr, "  --generations num\t\txxxx\n");
    fprintf (stderr, "  -h, --help\t\t\txxxx\n");
    printf ("\n");
    exit(1);
}

int main (int argc, char * const argv[])
{

    int c, nants = 10, i, j, seed, ngenerations = 250; // xxx seed precisa ser unsigned int?
    unsigned flags = 0;
    float phero_pers = 0.98; 
    char file_name[MAXNAME], flags_msg[100] = "";
    FILE *file;
    QAP_t *qap;
    QAP_solution_t *sol;

    struct option options[] = {
                    {"seed", 1, NULL, 'S'},
                    {"number-of-ants", 1, NULL, 'n'},
                    {"pheromone-persistence", 1, NULL, 'p'},
                    {"iteration-best", 0, NULL, 'i'},
                    {"best-so-far", 0, NULL, 'I'},
                    {"hybrid", 0, NULL, 'y'}, // xxx nao suportado ainda
                    {"generations", 1, NULL, 'g'},
                    {"asymmetric", 1, NULL, 'a'},
                    {"symmetric", 1, NULL, 's'},
                    {"help", 0, NULL, 'h'},
                    {0, 0, 0, 0}};

    seed = time(NULL);

    if (argc < 2){
        usage(argv[0]);       
    }

    /* Opções:
     *  S - seed
     *  n - number of ants
     *  p - pheromone persistence -- interval: [0,1] // xxx: fazer uma checagem do intervalo
     *  i - iteration best used to update pheromone
     *  I - best so for used to update pheromone
     *  h - hybrid algoritm used to update pheromone 
     *  g - number of generations
     *  a - Asymmetric QAP 
     *  s - Symmetric QAP
     */
    while ((c = getopt_long(argc, argv, ":S:n:p:iIysag:h", options, NULL)) != -1 ){
        switch (c){
            case 'i':
                if ((flags & PHEUP_BF) || (flags & PHEUP_HY)){
                    fprintf (stderr, "apenas um metodo para atualizar o feromonio\n"); /// xxxx traduzir depois  
                    _exit(1);
                }
                flags |= PHEUP_IB;
                strcat (flags_msg, " PHEUP_IB");
                break;
            case 'I':
                if ((flags & PHEUP_IB) || (flags & PHEUP_HY)){
                    fprintf (stderr, "apenas um metodo para atualizar o feromonio\n"); /// xxxx traduzir depois  
                    _exit(1);
                }
                flags |= PHEUP_BF;
                strcat (flags_msg, " PHEUP_BF");
                break;
            case 'y':
                flags |= PHEUP_HY; //xxx checar se mais de uma opção foram assinaladas
                fprintf (stderr, "nao implementado ainda\n");
                _exit(1);
                break;
            case 'g':
                ngenerations = atoi(optarg);
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
            case ':':
                printf("option required for '%c'\n", optopt);
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
            case 'h':
                usage(argv[0]);
                break;
            case '?':
                fprintf (stderr, "unknow option '%c'\n", optopt);
                break;
        }
    }

    if (!(flags & SYMMETRIC) && !(flags & ASYMMETRIC)){
        flags |= ASYMMETRIC;
    }

    if (!(flags & PHEUP_IB) && !(flags & PHEUP_BF)){
        flags |= PHEUP_IB;       
    }

    // xxx ler mais do que um arquivo 
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

    qap = Malloc(sizeof(QAP_t));
    qap->nants = nants;
    qap->phero_pers = phero_pers;
    qap->flags = flags;
    qap->ngenerations = ngenerations;

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
