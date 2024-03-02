// Created by: Nathan Dilla, Angel Velazquez
// This program demonstrates a deadlock scenario and how to avoid it using
// the trylock function.
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

pthread_mutex_t lock1, lock2;

void attempt_lock(pthread_mutex_t *firstLock, pthread_mutex_t *secondLock) {
  while (1) {
    pthread_mutex_lock(firstLock);
    printf("Locked first resource.\n");

    // Try to lock the second resource
    if (pthread_mutex_trylock(secondLock) == 0) {
      // Success, both resources are locked
      printf("Locked second resource.\n");
      break;
    }

    // Failed to lock the second resource, unlock the first and retry
    pthread_mutex_unlock(firstLock);
    printf("Could not lock second resource, retrying...\n");
    sleep(1); // Introduce a delay to reduce CPU usage and give the other thread a chance to unlock
  }
}

void *resource1() {
  attempt_lock(&lock1, &lock2);
  // Simulate work
  sleep(1);
  // Unlock resources
  pthread_mutex_unlock(&lock2);
  pthread_mutex_unlock(&lock1);
  pthread_exit(NULL);
}

void *resource2() {
  attempt_lock(&lock2, &lock1);
  // Simulate work
  sleep(1);
  // Unlock resources
  pthread_mutex_unlock(&lock1);
  pthread_mutex_unlock(&lock2);
  pthread_exit(NULL);
}

int main() {
  pthread_mutex_init(&lock1, NULL);
  pthread_mutex_init(&lock2, NULL);

  pthread_t t1, t2;

  pthread_create(&t1, NULL, resource1, NULL);
  pthread_create(&t2, NULL, resource2, NULL);

  pthread_join(t1, NULL);
  pthread_join(t2, NULL);

  pthread_mutex_destroy(&lock1);
  pthread_mutex_destroy(&lock2);

  return 0;
}
