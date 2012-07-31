#pragma once
double get_time();
void sleep_sec(double time);

#include <sstream>
#include<vector>
int mod(int x, int m);

std::vector<std::string> split_str(std::string str);

template<typename to, typename from>to lexical_cast(from const &x) {
  std::stringstream os;
  to ret;

  os << x;
  os >> ret;

  return ret;  
}
