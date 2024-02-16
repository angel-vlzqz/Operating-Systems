#include <iostream>
#include <queue>
#include <semaphore.h>

using namespace std;

const int BUFFER_SIZE = 5;

class SharedData {
  public:
    queue<int> buffer;
    sem_t sem_buffer, sem_mutex;

    SharedData() {
      sem_init(&sem_buffer, 0, BUFFER_SIZE);
      sem_init(&sem_mutex, 0, 1);
    }

    // No need for a destructor, as sem_destroy is not called
};

void produce(SharedData &shared, int item) {
  sem_wait(&shared.sem_buffer);
  sem_wait(&shared.sem_mutex);
  shared.buffer.push(item);
  cout << "Produced item: " << item << endl;
  sem_post(&shared.sem_mutex);
}

int consume(SharedData &shared) {
  sem_wait(&shared.sem_mutex);
  while (shared.buffer.empty()) {
    sem_post(&shared.sem_mutex);
    sem_wait(&shared.sem_buffer);
    sem_wait(&shared.sem_mutex);
  }
  int item = shared.buffer.front();
  shared.buffer.pop();
  cout << "Consumed item: " << item << endl;
  sem_post(&shared.sem_mutex);
  sem_post(&shared.sem_buffer);
  return item;
}

int main() {
  SharedData shared;

  // Producer-consumer loop
  for (int i = 0; i < 10; ++i) {
    // Produce if i is even, consume if i is odd
    if (i % 2 == 0) {
      produce(shared, i);
    } else {
      int item = consume(shared);
    }
  }

  return 0;
}

