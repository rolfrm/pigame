#include "misc.h"
#include <time.h>
#include <stdlib.h>
timespec start = {0,-1};;
double get_time(){
	timespec time;
	clock_gettime(CLOCK_REALTIME, &time);
	if(start.tv_nsec == -1){
	  start = time;
	}

	double t = (time.tv_sec - start.tv_sec) + (double)(time.tv_nsec - start.tv_nsec)/1000000000.0;
	
	return t;
}


void sleep_sec(double t){
  timespec time = {0, (long) (t*1000000000.0)};
  timespec out;
  nanosleep(&time,&out);
}
