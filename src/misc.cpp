#include "misc.h"
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <sstream>
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
  timespec time = {(long)t, (long) (fmod(t,1.0)*1000000000.0)};
  timespec out;
  nanosleep(&time,&out);
}

std::vector<std::string> split_str(std::string str){
  std::vector<std::string> out;
  while(true){
    if(str.size() == 0){
      return out;
    }
    int pos = str.find(' ');
    if(str[pos] == ' '){
      std::string st = str.substr(0,pos);
      if(st != ""){
	out.push_back(st);
      }
      str = str.substr(pos+1,str.size());
    }else{
      if(str != ""){
	out.push_back(str);
      }
      return out;
    }
  }
  return out; 
}

#include <iostream>
void split_test(){
  std::vector<std::string> st = split_str("   asd  dsa   w  d    a  sdsa   s ");
  for(int i = 0; i< st.size();i++){
    std::cout << "|" << st[i]<< "|" << "\n";
  }
} 

int mod(int x, int m) {
    return (x%m + m)%m;
}
