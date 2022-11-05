#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "linkedList.c"


const int MINIMUM_GRANULARITY = 10;
const int SCHED_LATENCY = 100;

int ALLP;

struct Node* runqueue = NULL;
pthread_mutex_t mutex_lock;

int finishedProcessCount = 0;
pthread_cond_t scheduler_cv;
int counter = 0;

void* schedulerThread( void* arg_ptr){
    pthread_mutex_lock(&mutex_lock);
    while(finishedProcessCount < ALLP){
        //printf("Scheduler waiting...\n");
        printList(runqueue);
        pthread_cond_wait(&scheduler_cv, &mutex_lock);
        //printf("Scheduler woken up\n");
        if(runqueue){
            struct PCB* minVruntime;
            getMinCFS(runqueue, &minVruntime);

            pthread_cond_signal(&(minVruntime->cv));
        }       
    }
    printf("BİTTİ\n");
    pthread_mutex_unlock(&mutex_lock);

    
}

void* processThread( void* arg_ptr){

    printf("In pthread: %ld\n", pthread_self());

    struct PCB* pcb = malloc(sizeof(struct PCB));

    pthread_cond_init(&pcb->cv, NULL);
    pcb->pid = ((struct processThreadArgs *) arg_ptr)->pid;
    pcb->priority = ((struct processThreadArgs *) arg_ptr)->priority;
    pcb->processLength = ((struct processThreadArgs *) arg_ptr)->processLength;
    pcb->totalTimeSpent = 0;
    pcb->vruntime = 0;

    pthread_mutex_lock(&mutex_lock);
    insert(&runqueue, pcb);
    printList(runqueue);

    printf("Wake up signal sent to scheduler due to insert\n");
    pthread_cond_signal(&scheduler_cv);


    // if ( counter == 2) {
    //     struct Node* dequeuedNode = dequeueNode(&runqueue, 2);
    //     printf("After dequeue:");
    //     printList(runqueue);
    //     printf("Dequeued Node pid %d\n", dequeuedNode->pcb->pid);
    //     insert(&runqueue,dequeuedNode->pcb);
    //     printf("After insert:");
    //     printList(runqueue);
    // }
    // counter++;
    
    while(pcb->totalTimeSpent < pcb->processLength){
        pthread_cond_wait(&(pcb->cv), &(mutex_lock));
        

        double weightDenominator = getAllWeights(runqueue);
        /*printf("Priority: %d ", pcb->priority);
        printf("Weight: %f ", prio_to_weight[pcb->priority + 20]);
        printf("Denom: %f\n", weightDenominator);*/

        int timeslice = (prio_to_weight[pcb->priority + 20]) / weightDenominator * SCHED_LATENCY;
        printf("Timeslice: %d\n", timeslice);
        //dequeue
        struct Node* dequeuedNode = dequeueNode(&runqueue, findIndexByPid(runqueue, pcb->pid));
        if(timeslice < MINIMUM_GRANULARITY)
            timeslice = MINIMUM_GRANULARITY;
        if(pcb->processLength - pcb->totalTimeSpent < timeslice){
            timeslice = pcb->processLength - pcb->totalTimeSpent;
        }
        printf("Pid: %d is sleeping for: %d, process length : %d, totalTimeSpent: %d\n", pcb->pid, 
                                                    timeslice, pcb->processLength, pcb->totalTimeSpent);
        usleep(timeslice * 1000);

        //increment total time spent
        pcb->totalTimeSpent = pcb->totalTimeSpent + timeslice;

        //vruntime
        pcb->vruntime = pcb->vruntime + (prio_to_weight[20] / prio_to_weight[pcb->priority + 20] * (double)(timeslice));


        if( pcb->totalTimeSpent >= pcb->processLength){
            deleteNode(&dequeuedNode);
            finishedProcessCount++;
            pthread_cond_signal(&scheduler_cv);
            break;
        }
        else{
            //SONDA DEQUEUED NODE'U FREE LEMEYİ DENE
            insert(&runqueue, dequeuedNode->pcb);
        }
        pthread_cond_signal(&scheduler_cv);
    }
    
    pthread_mutex_unlock(&mutex_lock);

    printf("AAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n");
    
}

void* generatorThread(void* arg_ptr){



    int* processLengths = ((struct generatorArgs *) arg_ptr)->processLengths;
    int* priorityValues = ((struct generatorArgs *) arg_ptr)->priorityValues;
    int plCount = ((struct generatorArgs *) arg_ptr)->plCount;
    int* interarrivalTimes = ((struct generatorArgs *) arg_ptr)->interarrivalTimes;

    // for(int i = 0; i < plCount; i++){
    //     printf("PL %d %d %d\n", i, processLengths[i], priorityValues[i]);
    //     if( i != plCount - 1)
    //         printf("IAT %d %d\n", i, interarrivalTimes[i]);
    // }
    pthread_t processThreadIds[plCount];
    for(int i = 0; i < plCount; i++){
        struct processThreadArgs processThreadArgs;
        processThreadArgs.pid = i+1;
        processThreadArgs.priority = priorityValues[i];
        processThreadArgs.processLength = processLengths[i];
        processThreadArgs.totalTimeSpent = 0;

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





        pthread_mutex_init(&mutex_lock, NULL);
        pthread_cond_init(&scheduler_cv, NULL);







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
            ALLP = atoi(argv[13]);
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
            ALLP = atoi(argv[3]);
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
            /*
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
            */
            fp = fopen(INFILE, "r");

            if (fp == NULL)
            {
                printf("Could not open file %s", INFILE);
                return 0;
            }
            
            //lineNumber = lineNumber + 1;
            char type[20]; // PL or IAT
            int value1;
            int value2;
            int lineInt = fscanf(fp,"%s %d %d",type,&value1,&value2);

            // Values as arrays from input file
            int processLengths[ALLP ];
            int plCount = 0;
            int priorityValues[ALLP];
            int pvCount = 0;
            int interarrivalTimes[ALLP-1];
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
            for ( int i = 0; i < ALLP;i++) {
                printf("PL %d %d %d\n",i,processLengths[i],priorityValues[i]);
                if( i != ALLP - 1)
                    printf("IAT %d %d\n",i,interarrivalTimes[i]);

            }
            

            
            printf("\nThe file %s has %d processes\n", INFILE, ALLP);
            fclose(fp);
            
            struct generatorArgs generator_args;
            generator_args.processLengths = processLengths;
            generator_args.interarrivalTimes = interarrivalTimes;
            generator_args.priorityValues = priorityValues;
            generator_args.plCount = plCount;
            
            pthread_t thr_id[2];
            int ret = pthread_create(&thr_id[0], NULL, schedulerThread, (void*) NULL);

            if( ret){
                printf("ERROR creating schedular thread\n");
            }
            ret = pthread_create(&thr_id[1], NULL, generatorThread, (void*) &generator_args);

            if( ret){
                printf("ERROR creating generator thread\n");
            }

            

            pthread_join(thr_id[0], NULL);
            pthread_join(thr_id[1], NULL);
            



        }

    }


}