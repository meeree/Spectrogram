GDB=-ggdb 
GPROF=
CFLAGS=-std=c++11 $(GDB) $(GPROF)
OPENGL=-L/usr/local/lib -lGLEW -lGLU -lm -lglfw3 -lrt -lm -ldl -lXrandr -lXinerama -lXi -lXcursor -lXrender -lGL -lm -lpthread -ldl -ldrm -lXdamage -lXfixes -lXxf86vm -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp

graphics : graphics.o 
	g++ graphics.o -o graphics $(CFLAGS) $(OPENGL)

graphics.o : graphics.cpp
	g++ -c graphics.cpp $(CFLAGS) $(OPENGL)

clean : 
	rm *.o graphics 
