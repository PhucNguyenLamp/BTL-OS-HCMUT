#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t * q) {
        if (q == NULL) return 1;
	return (q->size == 0);
}
void enqueue(struct queue_t * q, struct pcb_t * proc) {
        /* TODO: put a new process to queue [q] */
        // pthread_mutex_lock(&queue_lock);
        if (q->size == 0){
                q->proc[0] = proc;
                q->size++;
        }
        q->size++;
        q->proc[q->size-1] = proc;
        // pthread_mutex_unlock(&queue_lock);
}
struct pcb_t * dequeue(struct queue_t * q) {
        /* TODO: return a pcb whose priority is the highest
         * in the queue [q] and remember to remove it from q
         * */
        // pthread_mutex_lock(&queue_lock);
        struct pcb_t* proc = NULL;
        if(q->proc[0] != NULL){
                proc = q->proc[0];
                for(int i = 0; i < q->size - 1; i++){
                        q->proc[i] = q->proc[i+1];
                }
                q->size --;
        }
        // pthread_mutex_unlock(&queue_lock)
	return proc;
}

