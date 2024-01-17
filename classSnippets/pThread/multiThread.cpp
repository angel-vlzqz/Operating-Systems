#include <stdio.h>
#include <pthread.h>

// use -pthread in compilation
void *busy(void *ptr)
{
  // ptr will point to "Hi"
  puts("Hello World");
  return NULL;
}

void *busy1(void *ptr)
{
  // ptr will point to "Hi"
  puts("Hello World 1");
  return NULL;
}

int main()
{
  pthread_t id;
  pthread_create(&id, NULL, busy, NULL);
  pthread_create(&id, NULL, busy1, NULL);
  while(1)
  {
    // infinite loop
  }
}
