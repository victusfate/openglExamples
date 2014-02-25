# g++ -o test test.cc -framework OpenGL -framework GLUT -framework Cocoa
# g++ $(pkg-config --cflags glfw3) $(pkg-config --cflags glew) -o working_glfwSample working_glfwSample.cc $(pkg-config --libs glfw3) $(pkg-config --libs glew) -framework OpenGL -framework Cocoa
# g++ $(pkg-config --cflags glfw3) $(pkg-config --cflags glew) -o glfwSample glfwSample.cc $(pkg-config --libs glfw3) $(pkg-config --libs glew) -framework OpenGL -framework Cocoa
g++ $(pkg-config --cflags glfw3) $(pkg-config --cflags glew) -o main main.cc $(pkg-config --libs glfw3) $(pkg-config --libs glew) -framework OpenGL -framework Cocoa
