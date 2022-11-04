#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "linkedList.c"

static const int prio_to_weight[40] = {
/* -20 */ 88761, 71755, 56483, 46273, 36291,
/* -15 */ 29154, 23254, 18705, 14949, 11916,
/* -10 */ 9548, 7620, 6100, 4904, 3906,
/* -5 */ 3121, 2501, 1991, 1586, 1277,
/* 0 */ 1024, 820, 655, 526, 423,
/* 5 */ 335, 272, 215, 172, 137,
/* 10 */ 110, 87, 70, 56, 45,
/* 15 */ 36, 29, 23, 18, 15,
};

struct Node* runqueue = NULL;
int runqueueSize = 0;



void* processThread( void* arg_ptr){

    printf("In pthread: %ld\n", pthread_self());
}

void* generatorThread(void* arg_ptr){



    int* processLengths = ((struct generatorArgs *) arg_ptr)->processLengths;
    int* priorityValues = ((struct generatorArgs *) arg_ptr)->priorityValues;
    int plCount = ((struct generatorArgs *) arg_ptr)->plCount;
    int* interarrivalTimes = ((struct generatorArgs *) arg_ptr)->interarrivalTimes;

    for(int i = 0; i < plCount; i++){
        printf("PL %d %d %d\n", i, processLengths[i], priorityValues[i]);
        if( i != plCount - 1)
            printf("IAT %d %d\n", i, interarrivalTimes[i]);
    }
    pthread_t processThreadIds[plCount];
    for(int i = 0; i < plCount; i++){
        struct processThreadArgs processThreadArgs;
        int ret = pthread_create(&processThreadIds[i], NULL, processThread, (void*) &processThreadArgs);

        if( ret){
            printf("ERROR creating thread\n");
        }
        if( i != plCount - 1)
            usleep(1000 * interarrivalTimes[i]);
    }
    pthread_exit(NULL);
    
}

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
            
            lineNumber = lineNumber + 1;
            char type[10]; // PL or IAT
            int value1;
            int value2;
            int lineInt = fscanf(fp,"%s %d %d",type,&value1,&value2);

            // Values as arrays from input file
            int processLengths[(lineNumber+1)/2 ];
            int plCount = 0;
            int priorityValues[(lineNumber+1)/2];
            int pvCount = 0;
            int interarrivalTimes[(lineNumber-1)/2];
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
                if( i != (lineNumber+1)/2 - 1)
                    printf("IAT %d %d\n",i,interarrivalTimes[i]);

            }
            

            
            printf("\nThe file %s has %d lines\n", INFILE, lineNumber);
            fclose(fp);
            
            struct generatorArgs generator_args;
            generator_args.processLengths = processLengths;
            generator_args.interarrivalTimes = interarrivalTimes;
            generator_args.priorityValues = priorityValues;
            generator_args.plCount = plCount;
            
            pthread_t thr_id[2];
            int ret = pthread_create(&thr_id[0], NULL, generatorThread, (void*) &generator_args);

            if( ret){
                printf("ERROR creating thread\n");
            }

            pthread_join(thr_id[0], NULL);
            

            /*
            struct PCB myPcb;
            myPcb.priority = 12;

            struct PCB myPcb1;
            myPcb1.priority = 16;

            struct PCB myPcb2;
            myPcb2.priority = 1;

            struct PCB myPcb3;
            myPcb3.priority = 20;

            struct PCB myPcb4;
            myPcb4.priority = -2;

            struct PCB myPcb5;
            myPcb5.priority = -200;

            struct Node* head = NULL;


            insert(&head, myPcb);
            insert(&head, myPcb1);
            insert(&head, myPcb2);
            insert(&head, myPcb5);
            insert(&head, myPcb3);
            insert(&head, myPcb4);
        
            
            printList(head);


            struct PCB minPcb;
            int myMinIndexFornow = getMin(head, &minPcb);

            printf("Index: %d, Min priority: %d\n", myMinIndexFornow, minPcb.priority);

            printList(head);

            deleteNode( &head, myMinIndexFornow);

            printList(head);

            */


        }

    }


}