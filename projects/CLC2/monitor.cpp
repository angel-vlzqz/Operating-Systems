#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>

using namespace std;

const int BUFFER_SIZE = 5;

class Monitor {
  private:
    queue<int> buffer;
    mutex mtx;
    condition_variable buffer_not_full, buffer_not_empty;

  public:
    void produce(int item) {
      unique_lock<mutex> lock(mtx);
      while (buffer.size() >= BUFFER_SIZE) {
        buffer_not_full.wait(lock);
      }
      buffer.push(item);
      cout << "Produced item: " << item << endl;
      buffer_not_empty.notify_one();
    }

    int consume() {
      unique_lock<mutex> lock(mtx);
      while (buffer.empty()) {
        buffer_not_empty.wait(lock);
      }
      int item = buffer.front();
      buffer.pop();
      cout << "Consumed item: " << item << endl;
      buffer_not_full.notify_one();
      return item;
    }
};

int main() {
  Monitor monitor;

  // Producer-consumer loop
  for (int i = 0; i < 10; ++i) {
    // Produce if i is even, consume if i is odd
    if (i % 2 == 0) {
      monitor.produce(i);
    } else {
      int item = monitor.consume();
    }
  }

  return 0;
}

