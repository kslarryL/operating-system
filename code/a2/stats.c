#include "stats.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

stats* myStats;

void stats_init(int num_producers){
  myStats = malloc(sizeof(stats) * num_producers);
  for(int i=0; i<num_producers; i++){
    myStats[i].factory_num = i;
    myStats[i].made = 0;
    myStats[i].eaten = 0;
    myStats[i].min_delay = DBL_MAX;///
    myStats[i].avg_delay = 0;
    myStats[i].max_delay = 0;
  }
}

void stats_cleanup(void){
  free(myStats);
}

void stats_record_produced(int factory_number){
  myStats[factory_number].made++;
}

void stats_record_consumed(int factory_number, double delay_in_ms){
  if(delay_in_ms < myStats[factory_number].min_delay){
    myStats[factory_number].min_delay = delay_in_ms;
  }
  if(delay_in_ms > myStats[factory_number].max_delay){
    myStats[factory_number].max_delay = delay_in_ms;
  }
  myStats[factory_number].avg_delay = (myStats[factory_number].avg_delay * myStats[factory_number].eaten + delay_in_ms)/(myStats[factory_number].eaten+1);
  myStats[factory_number].eaten++;
}

void stats_display(int factory_number){
  printf("Statistics:\n");
  printf("%10s%10s%10s%18s%18s%18s\n","Factory#","#Made","#Eaten","Min Delay[ms]","Avg Delay[ms]","Max Delay[ms]");
  for(int i=0; i<factory_number; i++){
    printf("%10d%10d%10d%18.5f%18.5f%18.5f\n",myStats[i].factory_num,myStats[i].made,myStats[i].eaten,myStats[i].min_delay,myStats[i].avg_delay,myStats[i].max_delay);
  }
  for(int i=0; i<factory_number; i++){
    if(myStats[i].made != myStats[i].eaten){
      printf("ERROR: Mismatch between number made and eaten in Factory %d.\n", i);
    }
  }
}
