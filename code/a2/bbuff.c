#include "bbuff.h"
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>

void* buffer[BUFFER_SIZE];
sem_t mutex;
sem_t full;
sem_t empty;
int in;
int out;
int length;

void wait(int *value){
  while((*value)<=0);
  (*value)--;
}

void signal(int *value){
  (*value)++;
}

void bbuff_init(){
  sem_init(&mutex,0,1);
	sem_init(&full,0,0);
	sem_init(&empty,0,BUFFER_SIZE);
  in = 0;
  out = 0;
  length = 0;
}

void bbuff_blocking_insert(void* item){
  sem_wait(&empty);/////////////////////////////////////////////////////////////////////
  sem_wait(&mutex);
  buffer [in] = item;
  in = (in + 1) % BUFFER_SIZE;
  length++;
  sem_post(&mutex);
  sem_post(&full);
}

void* bbuff_blocking_extract(void){
  void* candy;
  sem_wait(&full);
  sem_wait(&mutex);
  candy = buffer[out];
  out = (out + 1) % BUFFER_SIZE;
  length--;
  sem_post(&mutex);
  sem_post(&empty);
  return candy;
}

_Bool bbuff_is_empty(void){
  if(length == 0){
    return true;
  }
  return false;
}
