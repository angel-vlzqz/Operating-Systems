/*
Nathan Dilla / Angel Velazquez
CST - 315
Professor Richardo Citro
January 28, 2024
CLC Assignment 1: Producer and Consumer
*/

#include <stdio.h>
// include a library for queueing
#include <queue>
// include a library for sleeping
#include <unistd.h>
// include a library for threads
#include <pthread.h>

using namespace std;

int theProduct = 0;

int produce() {
    printf("Produced: %d\n", theProduct);
    return theProduct++;
}

void consume(int i) {
    printf("Consumed: %d\n", i);
}

// create a queue
queue<int> buffer;

void put(int i) {
    buffer.push(i);
}

void get(int& i) {
    if (!buffer.empty()) {
        i = buffer.front();
        buffer.pop();
    } else {
        i = -1; // return -1 or some invalid value when the queue is empty
    }
}

void* producer(void* arg) {
    int i;
    while(1) {
        i = produce();
        put(i);
        sleep(1);
    }
}

void* consumer(void* arg) {
    int i;
    while (1) {
        get(i);
        if (i != -1) {
            consume(i);
        }
        sleep(1);
    }
}

int main() {
    pthread_t producerThread, consumerThread;

    // create a thread for producer
    pthread_create(&producerThread, NULL, producer, NULL);

    // create a thread for consumer
    pthread_create(&consumerThread, NULL, consumer, NULL);

    // join the threads
    pthread_join(producerThread, NULL);
    pthread_join(consumerThread, NULL);

    return 0;
}
