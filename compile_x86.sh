g++ main.cpp CollisionDetection.cpp GLProgram.cpp texture.cpp buffer_objects.cpp gl_init.c -lGL -lglfw -Wall -o test.o

#g++ main.cpp CollisionDetection.cpp GLProgram.cpp texture.cpp buffer_objects.cpp gl_init.c -I/usr/include/python2.7/  -lGL -lglfw -Wall -lpython2.7 test_wrap.c -fPIC -export-dynamic -shared -DHAVE_CONFIG_H -lm -ldl -o _test.so
