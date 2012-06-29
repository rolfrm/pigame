#include "misc.h"
#include <sys/time.h>
#include <stdlib.h>

double get_time(){
	timeval current_time;
	gettimeofday(&current_time,NULL);

	return (current_time.tv_sec)+(current_time.tv_usec ) *0.000001;
}
