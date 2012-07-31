#g++ -I/opt/vc/include/ -I/opt/vc/include/interface/vcos/pthreads main.cpp GLProgram.cpp texture.cpp buffer_objects.cpp egl_init.c  -L/opt/vc/lib/ -I/usr/include/python2.6/  -lEGL -lGLESv2 -lbcm_host -lvcos -Wall -lpython2.6 test_wrap.c 

#g++ -I/opt/vc/include/ -I/opt/vc/include/interface/vcos/pthreads main.cpp CollisionDetection.cpp GLProgram.cpp texture.cpp buffer_objects.cpp egl_init.c  -L/opt/vc/lib/  -lEGL -lGLESv2 -lbcm_host -lvcos -Wall -o test.o

