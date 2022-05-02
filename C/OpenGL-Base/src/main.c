// Including the needed 
// libraries
#include <GLAD/gl.h>
#include <GLFW/glfw3.h>
#include <STBI/stb_image.h>
#include <string.h>
#include <stdio.h>


// Pre-defined values for the window
#define WIDTH 640
#define HEIGHT 480
#define TITLE "OpenGL"

const char * v_shader = "\
#version 330 core\n\
layout (location = 0) in vec2 pos;\n\
void main() { \n\
	gl_Position = vec4(pos, 1.0f, 1.0f);\n\
}";

const char * f_shader = "\
#version 330 core\n\
out vec4 FragColor;\n\
uniform vec4 u_color;\n\
void main() { \n\
	FragColor = u_color;\n\
}";


int main(void) {
    // Initializing the GLFW library so we can make windows
    glfwInit();
    
    // Certain hints for the Window so that we a window that
    // uses a newer version of OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, 0);
   
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

	// Compile shaders
	//
	int success;
	char info_log[512] = {0};
		// Vertex shader
	unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertex_shader, 1, &v_shader, NULL);
	glCompileShader(vertex_shader);

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
		printf("%s\n", info_log);
	}

		// Frag shader
	unsigned int frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &f_shader, NULL);
	glCompileShader(frag_shader);

	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		glGetShaderInfoLog(frag_shader, 512, NULL, info_log);
		printf("%s\n", info_log);
	}

		// Link shaders
	unsigned int shader_prog = glCreateProgram();
	glAttachShader(shader_prog, vertex_shader);
	glAttachShader(shader_prog, frag_shader);
	glLinkProgram(shader_prog);

	glGetProgramiv(shader_prog, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_prog, 512, NULL, info_log);
		printf("%s\n", info_log);
	}

	glUseProgram(shader_prog);
	glDeleteShader(vertex_shader);
	glDeleteShader(frag_shader);
	

	// Let's render a square!
		// First we will need to render a single triangle, that will be 3 vertices
		// Multiply with 2 since, each vertex has 2 points, an x and y
	float triangle[3 * 2] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f
	};

	unsigned int indices[3] = {
		0, 1, 2
	};

	// . (0, 1)    . (1, 1)
	//  
	// Since I don't really remember things off by hand, lets learnopengl.com
	// . (0, 0)
	//
	//
		// Make a VAO now
	unsigned int vao; 
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

		// Make a VBO now
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *) 0);
	glEnableVertexAttribArray(0);
		// Make a IBO now
	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		// Lets set uniform
	int loc = glGetUniformLocation(shader_prog, "u_color");


	float r = 0.0f;
	float g = 0.0f; 
	float i = 0.01f;

	// While loop, everything like rendering, events and 
	// logic happens here
	while (!glfwWindowShouldClose(win)) {
		// Clearing the screen
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
		
		if (r > 1.0f) {
			i = -0.01f;
		} else if (r < 0.0f) {
			i = 0.01f;
		}

		r += i;
		g += i;

		glUniform4f(loc, r, g, 1.0f, 1.0f);
		// Rendering here
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

		// Swapping the screen buffers
		glfwSwapBuffers(win);

		// Getting events
		glfwPollEvents();       
	}


}


