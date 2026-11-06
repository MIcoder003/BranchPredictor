#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sim_bp.h"
#include "BranchPredictor.h"
#include "Hybrid.h"

/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim bimodal 6 gcc_trace.txt
    argc = 4
    argv[0] = "sim"
    argv[1] = "bimodal"
    argv[2] = "6"
    ... and so on
*/
int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
    char outcome;           // Variable holds branch outcome
    unsigned long int addr; // Variable holds the address read from input file

    if (!(argc == 4 || argc == 5 || argc == 7))
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }

    params.bp_name  = argv[1];
    params.K = 0;
    params.M1 = 0;
    params.M2 = 0;
    params.N = 0;

    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
    {
        if(argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M2       = strtoul(argv[2], NULL, 10);
        trace_file      = argv[3];
        printf("COMMAND\n %s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);
    }
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        if(argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M1       = strtoul(argv[2], NULL, 10);
        params.N        = strtoul(argv[3], NULL, 10);
        trace_file      = argv[4];
        printf("COMMAND\n %s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);

    }
    else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
    {
        if(argc != 7)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.K        = strtoul(argv[2], NULL, 10);
        params.M1       = strtoul(argv[3], NULL, 10);
        params.N        = strtoul(argv[4], NULL, 10);
        params.M2       = strtoul(argv[5], NULL, 10);
        trace_file      = argv[6];
        printf("COMMAND\n %s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);

    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }

    bool isBimodal = (strcmp(params.bp_name, "bimodal") == 0);
    bool isGshare  = (strcmp(params.bp_name, "gshare")  == 0);
    bool isHybrid  = (strcmp(params.bp_name, "hybrid")  == 0);

    // Single-predictor instance: bimodal is just gshare with N = 0.
    unsigned long m = isBimodal ? params.M2 : params.M1;
    unsigned long n = isBimodal ? 0UL       : params.N;
    BranchPredictor bp(m, n);

    // Hybrid instance (only used when the hybrid predictor is selected).
    Hybrid hb(params.K ? params.K : 1,
              params.M1 ? params.M1 : 1,
              params.N,
              params.M2 ? params.M2 : 1);

    unsigned long mispredictions = 0;
    unsigned long total = 0;
    double misprediction_rate = 0.0;

    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }

    char str[2];
    while(fscanf(FP, "%lx %s", &addr, str) != EOF)
    {
        outcome = str[0];
        bool actual = (outcome == 't');     // Convert char to bool
        bool taken;

        if (isHybrid)
        {
            taken = hb.predictBranch(addr);
            hb.update(addr, actual);
        }
        else
        {
            taken = bp.predictBranch(addr);
            bp.updateCounter(addr, actual);
        }

        total++;
        if (taken != actual)
        {
            mispredictions++;
        }
    }
    fclose(FP);

    misprediction_rate = (total == 0) ? 0.0 : (double)mispredictions / (double)total;

    printf("OUTPUT\n");
    printf(" number of predictions:    %lu\n", total);
    printf(" number of mispredictions: %lu\n", mispredictions);
    printf(" misprediction rate:       %.2f%%\n", misprediction_rate * 100);

    if (isBimodal)
    {
        printf("FINAL BIMODAL CONTENTS\n");
        bp.printContents();
    }
    else if (isGshare)
    {
        printf("FINAL GSHARE CONTENTS\n");
        bp.printContents();
    }
    else
    {
        hb.printContents();
    }

    return 0;
}