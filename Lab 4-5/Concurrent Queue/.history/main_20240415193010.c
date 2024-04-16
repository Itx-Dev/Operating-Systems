#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct __node_t {
    int value;
    struct __node_t *next;
} node_t;

typedef struct __queue_t {
    node_t *head;
    node_t *tail;
    pthread_mutex_t head_lock, tail_lock;
} queue_t;

void Queue_Init(queue_t *q) {
    node_t *tmp = malloc(sizeof(node_t));
    tmp->next = NULL;
    q->head = q->tail = tmp;
    pthread_mutex_init(&q->head_lock, NULL);
    pthread_mutex_init(&q->tail_lock, NULL);
}

void Queue_Enqueue(queue_t *q, int value) {
    node_t *tmp = malloc(sizeof(node_t));
    assert(tmp != NULL);
    tmp->value = value;
    tmp->next = NULL;

    pthread_mutex_lock(&q->tail_lock);
    q->tail->next = tmp;
    q->tail = tmp;
    pthread_mutex_unlock(&q->tail_lock);
}

int Queue_Dequeue(queue_t *q, int *value) {
    pthread_mutex_lock(&q->head_lock);
    node_t *tmp = q->head;
    node_t *new_head = tmp->next;
    if (new_head == NULL) {
        pthread_mutex_unlock(&q->head_lock);
        return -1; // queue was empty
    }
    *value = new_head->value;
    q->head = new_head;
    pthread_mutex_unlock(&q->head_lock);
    free(tmp);
    return 0;
}


int Queue_Empty(queue_t *q) {
    if (q->head == NULL) {
        return 1;
    } else {
        return 0;
    }
}

void Empty_Queue(queue_t *q) {
    int newHead = 0;
    while (Queue_Empty(q) == 0) {
        Queue_Dequeue(q, &newHead);
        printf("New Head: %d\n", newHead);
    }
}

void* threadEnqueueDequeue(void *argt) {
    queue_t *queue = (queue_t *)argt;

    int val = 1;
    int val2 = 2;

    Queue_Enqueue(queue, val);
    printf("Enqueue %d\n", queue->head->value);
    Empty_Queue(queue);
    printf("Dequeue %d\n", queue->head->value);

}


int main(int argc, char* argv[]) {
    int numThreads = 5;
    pthread_t threads[numThreads];
    queue_t *queue = malloc(sizeof(queue_t));


    Queue_Init(queue);

    for (int i = 0; i < numThreads; i++) {
        pthread_create(&threads[i], NULL, threadEnqueueDequeue, (void*)queue);
    }




    free(queue);
}