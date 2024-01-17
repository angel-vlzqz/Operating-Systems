#include <stdio.h>
#include <pthread.h>

//int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
  //                 void *(*start_routine)(void *), void *arg) 
//{
  // first points to a variable that will hold the id of the newly 
  // created thread
  // second points to attributes that we can use to tweak and tune some
  // of the advanced features of pthreads
  // third points to a function that we want to run
  // fourth pointer will be given to our function
  //return 0;
//}

void *busy(void *ptr)
{
  // ptr will goint to "HI"
  puts("hello world");
  return NULL;
}

int main()
{
  pthread_t id;
  pthread_create(&id, NULL, busy, NULL); // create a thread/process
  while(1)
  {
  }
}
