CFLAGS = -Wall `pkg-config --cflags glfw3` `pkg-config --cflags glew`
LDFLAGS = `pkg-config --libs glfw3` `pkg-config --libs glew`

all:
	g++ -o test test.cc -framework OpenGL -framework GLUT -framework Cocoa
	g++ $(CFLAGS) -o working_glfwSample working_glfwSample.cc $(LDFLAGS) -framework OpenGL -framework Cocoa
	g++ $(CFLAGS) -o glfwSample glfwSample.cc $(LDFLAGS) -framework OpenGL -framework Cocoa
	g++ $(CFLAGS) -o main main.cc $(LDFLAGS) -framework OpenGL -framework Cocoa

clean:
	rm test working_glfwSample glfwSample main

