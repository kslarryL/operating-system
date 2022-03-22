#include "bbuff.h"
#include "stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <stdbool.h>
#include <float.h>
#include <unistd.h>

_Bool makeStop;

typedef struct{
  int factory_number;
  double creation_ts_ms;
} candy_t;

double current_time_in_ms(void)
{
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}

int errInput(int input){
  if(input<=0){
    return 1;
  }
  return 0;
}


void* makeCandy(void* argv){
  int num = *((int*)argv);
  while(!makeStop){
    int time = rand() % 4;
    printf("Factory %d ships candy & waits %d s\n",num,time);
    candy_t *candy = malloc(sizeof(candy_t));
    candy->factory_number = num;
    candy->creation_ts_ms = current_time_in_ms();
    stats_record_produced(num);
    bbuff_blocking_insert(candy);
    sleep(time);
  }
  printf("Candy-factory %d done\n",num);
  return NULL;
}

void* eatCandy(void* argv){
  while(1){
    candy_t* candy = (candy_t*)bbuff_blocking_extract();
    double delay = current_time_in_ms() - candy->creation_ts_ms;
    stats_record_consumed(candy->factory_number, delay);
    //printf("EAT:Factory#%d creation_ts_ms: %f\n",candy->factory_number,candy->creation_ts_ms);////xxxx
    free(candy);
    int time = rand() % 2;
    sleep(time);
  }
  return NULL;
}


int main(int argc, char *argv[]){
  // 1.  Extract arguments /////////////////////////////////////////////////////
  printf("\n--------CANDYKIDS--------\n\n");

  int factories = atoi(argv[1]), kids = atoi(argv[2]), seconds = atoi(argv[3]);

  if(errInput(factories) || errInput(kids) || errInput(seconds)){
    printf("Wrong Argument Error: your input is out of range.\n");
    return 0;
  }
  printf("%d candy-factory threads will spawn.\n%d kid threads will spawn.\n%d seconds will be allowed the factory threads to run for.\n",factories,kids,seconds);

  // 2.  Initialize modules //////////////////////////////////////////////////// not completed
  printf("Initializing modules...\n");
  makeStop = false;
  stats_init(factories);
  bbuff_init();
  int* factoryArr = malloc(sizeof(int)*factories);
  for(int i=0;i<factories;i++){
    factoryArr[i] = i;
  }
  int* kidArr = malloc(sizeof(int)*factories);
  for(int i=0;i<kids;i++){
    kidArr[i] = i;
  }

  // 3.  Launch candy-factory threads //////////////////////////////////////////
  printf("Launching candy-factory threads...\n");
  pthread_t* factories_tid = malloc(sizeof(pthread_t)*factories);
  for(int i=0; i<factories; i++){
    pthread_create(&(factories_tid[i]),NULL,makeCandy,(void*)(&factoryArr[i]));
  }

  // 4.  Launch kid threads ////////////////////////////////////////////////////
  printf("Launching kid threads...\n");
  pthread_t* kid_tid = malloc(sizeof(pthread_t)*kids);
  for(int i=0; i<kids; i++){
    pthread_create(&(kid_tid[i]),NULL,eatCandy,NULL);
  }

  // 5.  Wait for requested time ///////////////////////////////////////////////
  printf("Waiting for requested time...\n");
  for(int i=0; i<seconds; i++){
    sleep(1);
    printf("Time %ds\n",i+1);
  }

  // 6.  Stop candy-factory threads ////////////////////////////////////////////
  printf("Stopping candy-factory threads...\n");
  makeStop = true;
  for(int i=0; i<factories; i++){
    pthread_join(factories_tid[i],NULL);
  }

  // 7.  Wait until no more candy //////////////////////////////////////////////
  while(!bbuff_is_empty()){
    printf("Waiting for all candy to be consumed...\n");
    sleep(1);
  }


  // 8.  Stop kid threads //////////////////////////////////////////////////////
  printf("Stopping kid threads...\n");
  for(int i=0; i<kids; i++){
    pthread_cancel(kid_tid[i]);
    pthread_join(kid_tid[i],NULL);
  }

  // 9.  Print statistics //////////////////////////////////////////////////////
  stats_display(factories);

  // 10. Cleanup any allocated memory //////////////////////////////////////////
  printf("Cleaning up any allocated memory...\n");
  stats_cleanup();
  free(factoryArr);
  free(kidArr);
  free(factories_tid);
  free(kid_tid);
  return 0;
}
