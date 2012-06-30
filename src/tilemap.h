#include <vector>
#include <iostream>
template <class T>
class tilemap{
  std::vector<T> data;
  int size_x;
  int size_y;
  T def;
 public:
  tilemap(){

  }
  tilemap(int sizex, int sizey, T _default){
    size_x = sizex;
    size_y = sizey;
    def = _default;
    for(int i = 0; i < size_x*size_y;i++){
      data.push_back(def);
    }
    
  }
  T get_tile(int x, int y){
    if(x >= size_x || y >= size_y){
      return def;
    }
    if(x < 0 || y < 0){
      return def;
    }
    return data[x + size_x*y];
  }
  void set_tile(int x, int y, T value){
    data[x + size_x*y] = value;
  }
  T& tile(int x, int y){
    return data[x + size_x*y];
  }
  void get_tilechunk(int from_x, float to_x, float from_y, float to_y, T * data_out){

  }

};
