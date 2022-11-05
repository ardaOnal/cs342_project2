#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "commonDefs.c"

static const double prio_to_weight[40] = {
/* -20 */ 88761, 71755, 56483, 46273, 36291,
/* -15 */ 29154, 23254, 18705, 14949, 11916,
/* -10 */ 9548, 7620, 6100, 4904, 3906,
/* -5 */ 3121, 2501, 1991, 1586, 1277,
/* 0 */ 1024, 820, 655, 526, 423,
/* 5 */ 335, 272, 215, 172, 137,
/* 10 */ 110, 87, 70, 56, 45,
/* 15 */ 36, 29, 23, 18, 15,
};
int runqueueSize = 0;

struct Node{
    struct PCB* pcb;
    struct Node* next;
};

void insert( struct Node** head, struct PCB* pcb){
    struct Node* newNode = malloc(sizeof(struct Node));

    newNode->pcb = pcb;

    newNode->next = *head;
    *head = newNode;
    runqueueSize++;
}

int getMinCFS( struct Node* head, struct PCB** pcb){
    struct Node* cur = head;
    if(!head)
        return -1;
    int minIndex = 1;
    int min = head->pcb->vruntime;

    *pcb = cur->pcb;
    
    int curIndex = 1;

    while( cur){
        if(cur->pcb->vruntime < min){
            min = cur->pcb->vruntime;
            minIndex = curIndex;
        
            *pcb = cur->pcb;
        }
        curIndex++;
        cur = cur->next;
    }
    return minIndex; 
}
double getAllWeights(struct Node* head){
    double sum = 0;
    while(head){
        sum = sum + prio_to_weight[head->pcb->priority + 20];
        head = head->next;
    }
    return sum;
}
struct Node* find(struct Node* head, int index){
    if(index < 1)
        return NULL;
    else{
        struct Node* cur = head;
        for(int i = 1; i < index; i++)
            cur = cur->next;
        return cur;
    }
}
int findIndexByPid(struct Node* head, int pid){
    if( pid < 1)
        return -1;
    else{
        struct Node* cur = head;
        int index = 1;
        while(cur){
            if( cur->pcb->pid == pid)
                return index;
            index++;
            cur = cur->next;
        }
        return -1;
    }
}
struct Node* dequeueNode(struct Node** head, int index){
    struct Node* cur;

    if(index < 1)
        return NULL;

    if(index == 1){
        cur = *head;
        *head = (*head)->next;
    }
    else{
        struct Node* prev = find(*head, index-1);
        cur = prev->next;
        prev->next = cur->next;
    }
    cur->next = NULL;
    runqueueSize--;
    return cur;
    
}   
int deleteNode(struct Node** cur){
    (*cur)->next = NULL;
    pthread_cond_destroy(&((*cur)->pcb->cv));
    free((*cur)->pcb);
    free(*cur);
    *cur = NULL;
    return 1;
}


void printList( struct Node* head){
    while(head){
        printf("%d -> ", head->pcb->pid);
        head = head->next;
    }
    printf("\n");
}
