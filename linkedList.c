#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "commonDefs.c"

struct Node{
    struct PCB pcb;
    struct Node* next;
};

void insert( struct Node** head, struct PCB pcb){
    struct Node* newNode = malloc(sizeof(struct Node));
    
    newNode->pcb.cv = pcb.cv;
    newNode->pcb.pid = pcb.pid;
    newNode->pcb.processLength = pcb.processLength;
    newNode->pcb.threadId = pcb.threadId;
    newNode->pcb.totalTimeSpent = pcb.totalTimeSpent;
    newNode->pcb.priority = pcb.priority;

    newNode->next = *head;
    *head = newNode;
}

int getMin( struct Node* head, struct PCB* pcb){
    struct Node* cur = head;
    if(!head)
        return -1;
    int minIndex = 1;
    int min = head->pcb.priority;

    pcb->cv = cur->pcb.cv;
    pcb->pid = cur->pcb.pid;
    pcb->processLength = cur->pcb.processLength;
    pcb->threadId = cur->pcb.threadId;
    pcb->totalTimeSpent = cur->pcb.totalTimeSpent;
    pcb->priority = cur->pcb.priority;

    
    int curIndex = 1;
    while( cur){
        if(cur->pcb.priority < min){
            min = cur->pcb.priority;
            minIndex = curIndex;
        
            pcb->cv = cur->pcb.cv;
            pcb->pid = cur->pcb.pid;
            pcb->processLength = cur->pcb.processLength;
            pcb->threadId = cur->pcb.threadId;
            pcb->totalTimeSpent = cur->pcb.totalTimeSpent;
            pcb->priority = cur->pcb.priority;

        }
        curIndex++;
        cur = cur->next;
    }
    return minIndex; 
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
int deleteNode(struct Node** head, int index){
    struct Node* cur;

    if(index < 1)
        return -1;

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
    free(cur);
    cur = NULL;
    return 1;
}   



void printList( struct Node* head){
    while(head){
        printf("%d ->", head->pcb.priority);
        head = head->next;
    }
    printf("\n");
}
