#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>


struct PCB{
    int pid;
    pthread_t threadId;
    int processLength;
    int totalTimeSpent;
    pthread_cond_t cv;
    int priority;
};

struct generatorArgs{
    int* processLengths;
    int* priorityValues;
    int plCount;
    int* interarrivalTimes;  
};

struct processThreadArgs{
    int pid;
    int processLength;
    int totalTimeSpent;
    int priority;
};