#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){ 
    if ( argc > 5) {
        // Command Line
        if ( argv[1][0] == 'C') {
            int minPrio = atoi(argv[2]);
            int maxPrio = atoi(argv[3]);
            char* distPL = argv[4];
            int avgPL = atoi(argv[5]); 
            int minPL = atoi(argv[6]);
            int maxPL = atoi(argv[7]);
            char* distIAT = argv[8];
            int avgIAT = atoi(argv[9]); 
            int minIAT = atoi(argv[10]);
            int maxIAT = atoi(argv[11]);
            int rqLen = atoi(argv[12]);
            int ALLP = atoi(argv[13]);
            int OUTMODE = atoi(argv[14]);
            char* OUTFILE;
            if ( argc == 16) OUTFILE = argv[15];
            
            printf("\nCommand line\n");
            printf("Arguments:\n");
            printf("minPrio %d\n", minPrio);
            printf("maxPrio %d\n", maxPrio);
            printf("distPL %s\n", distPL);
            printf("avgPL %d\n", avgPL);
            printf("minPL %d\n", minPL);
            printf("maxPL %d\n", maxPL);
            printf("distIAT %s\n", distIAT);
            printf("avgIAT %d\n", avgIAT);
            printf("minIAT %d\n", minIAT);
            printf("maxIAT %d\n", maxIAT);
            printf("rqLen %d\n", rqLen);
            printf("ALLP %d\n", ALLP);
            printf("OUTMODE %d\n", OUTMODE);
            if ( argc == 16) printf("OUTFILE %s\n", OUTFILE);


        } 
        // Input file
        else if (argv[1][0] == 'F' ) {
            printf("\nInput file\n");
            int rqLen = atoi(argv[2]);
            int ALLP = atoi(argv[3]);
            int OUTMODE = atoi(argv[4]);
            char* INFILE = argv[5];
            char* OUTFILE;
            if ( argc == 7) OUTFILE = argv[6];


            printf("File input\n");
            printf("Arguments:\n");
            printf("rqLen %d\n", rqLen);
            printf("ALLP %d\n", ALLP);
            printf("OUTMODE %d\n", OUTMODE);
            printf("INFILE %s\n", INFILE);
            if ( argc == 7) printf("OUTFILE %s\n", OUTFILE);


            FILE* fp;
            char * line = NULL;
            size_t len = 0;
            ssize_t read;

            fp = fopen(INFILE, "r");

            if (fp == NULL)
            {
                printf("Could not open file %s", INFILE);
                return 0;
            }

            char c;
            int lineNumber = 0;
            for (c = getc(fp); c != EOF; c = getc(fp))
                if (c == '\n') // Increment count if this character is newline
                    lineNumber = lineNumber + 1;

            fclose(fp);
            fp = fopen(INFILE, "r");

            if (fp == NULL)
            {
                printf("Could not open file %s", INFILE);
                return 0;
            }
            
            
            char type[10]; // PL or IAT
            int value1;
            int value2;
            int lineInt = fscanf(fp,"%s %d %d",type,&value1,&value2);

            // Values as arrays from input file
            int processLengths[(lineNumber+1)/2];
            int plCount = 0;
            int priorityValues[(lineNumber+1)/2];
            int pvCount = 0;
            int interarrivalTimes[(lineNumber+1)/2];
            int iaCount = 0;

            while(lineInt != -1)  {
                if ( lineInt == 3) {
                    if ( strcmp(type,"PL") == 0) {
                        processLengths[plCount++] = value1;
                        priorityValues[pvCount++] = value2;
                    }
                } else if (lineInt == 2) {
                    if ( strcmp(type,"IAT") == 0) {
                        interarrivalTimes[iaCount++] = value1;
                    }
                }

                lineInt = fscanf(fp,"%s %d %d",type,&value1,&value2);
            }

            printf("\nResults in Array:\n");
            for ( int i = 0; i < (lineNumber+1)/2;i++) {
                printf("PL %d %d %d\n",i,processLengths[i],priorityValues[i]);
                printf("IAT %d %d\n",i,interarrivalTimes[i]);
            }

            
            printf("\nThe file %s has %d lines\n ", INFILE, lineNumber);
            fclose(fp);


        }

    }
}