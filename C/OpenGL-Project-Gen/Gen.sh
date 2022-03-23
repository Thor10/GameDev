cd ./glad
gcc *.c -c && ar rcs libglad.a *.o && rm *.o
cd ..

mkdir Project && cd Project

mkdir Data

touch main.c

cd Data && git clone https://github.com/glfw/glfw.git GLFW
cd GLFW && mkdir build && cd build

cmake .. && make

cd src

mv libglfw3.a ../../ 

cd ../../include/GLFW

mv * ../../

cd ../../

rm CMakeLists.txt
rm LICENSE.md
rm -rf CMake
rm -rf build
rm -rf include 
rm -rf src

ls -al
cd .. && mkdir GLAD
ls -al
cp -r ../../glad/* GLAD
cd GLAD && mv *.a ../

cd ..
mkdir Libs
cd GLFW && mv libglfw3.a ../
cd ..
mv *.a Libs


cd ..
echo '
#include <stdio.h>
#include "Data/GLAD/gl.h"
#include "Data/GLFW/glfw3.h"

int main(void) {
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    # ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GL_TRUE);
    # endif

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    /* Make the context current */
    if (!gladLoadGL(glfwGetProcAddress)) {		
        printf("Error Initiating GLAD\n");
        glfwTerminate();
    }

    printf("%s\n", glGetString(GL_VERSION));
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.2f, 0.3f, 0.6f, 1.0f);
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
' > main.c

echo '
CC = gcc
output: main.o
	CC main.o -o output -L Data/Libs -lglfw3 -lglad -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo 
main.o : main.c
	CC -c main.c -lglfw -lglad
clean:
	rm *.o output
' > makefile

echo '
export DYLD_LIBRARY_PATH=Data/Libs
make
./output
' > run.sh

echo ""
echo ""
echo "Don't forget to 'cd Project && export DYLD_LIBRARY_PATH=Data/Libs'"
echo "Happy Coding!"

