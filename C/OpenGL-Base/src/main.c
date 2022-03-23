
// Including the needed 
// libraries
#include <GLAD/gl.h>
#include <GLFW/glfw3.h>

// Pre-defined values for the window
#define WIDTH 640
#define HEIGHT 480
#define TITLE "OpenGL"


int main(void) {
    // Initializing the GLFW library so we can make windows
    glfwInit();
    
    // Certain hints for the Window so that we a window that
    // uses a newer version of OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif  


    // Creating window pointer, if it is NULL, the program ends
    GLFWwindow * win = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
    if (!win) {
        glfwTerminate();
        return -1;
    }

    // Making the window's context current so that the current window 
    // is the one being focused on and also setting the swap interval
    // so that there are no screen tears, aka vsync
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1);


    // Initializing GLAD, which is the OpenGL loader / OpenGL function
    // library
    if (!gladLoadGL(glfwGetProcAddress)) {
        glfwTerminate();
    }

    // Enabling some OpenGL blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // While loop, everything like rendering, events and 
    // logic happens here
    while (!glfwWindowShouldClose(win)) {
        // Clearing the screen
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 1.0f, 1.0f, 1.0f);

            // Rendering here


        // Swapping the screen buffers
        glfwSwapBuffers(win);

        // Getting events
        glfwPollEvents();       
    }


}

