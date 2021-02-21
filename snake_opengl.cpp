#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>

void error_callback(int, const char *);
void framebuffer_size_callback(GLFWwindow*, int, int);

void init_gl();
void log_error(const char *);
void end(int);

const int MAP_WIDTH = 30;
const int MAP_HEIGHT = 30;
const int NODE_WIDTH = 20;
const int NODE_HEIGHT = 20;
const char *WINDOW_TITLE = "Snake OpenGL";
GLFWwindow *window;

const char *vertex_shader_source = "#version 330 core\n"
"in vec2 a_pos;\n"
"void main() {\n"
"	gl_Position = vec4(a_pos, 0.f, 1.f);\n"
"}";

const char *fragment_shader_source = "#version 330 core\n"
"out vec4 col;\n"
"void main() {\n"
"	col = vec4(0.4f, 0.5f, 0.9f, 1.f);\n"
"}\n";

float vertices[] = {
	-0.5f, -0.5f, 
	-0.5f, 0.5f, 
	0.5f, 0.5f, 
	0.5f, -0.5f
};

unsigned int indices[] = {
	0, 1, 2, 
	0, 2, 3
};

unsigned int vertex_array, vertex_buffer, element_buffer, vertex_shader, fragment_shader, shader_program;

int main() {
	glfwSetErrorCallback(error_callback);
	glfwInit();
	window = glfwCreateWindow(MAP_WIDTH * NODE_WIDTH, MAP_HEIGHT * NODE_HEIGHT, WINDOW_TITLE, NULL, NULL);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	init_gl();
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(shader_program);
		glBindVertexArray(vertex_array);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	end(0);
	return 0;
}
void init_gl() {
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);
	
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glGenBuffers(1, &element_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);
	glCompileShader(vertex_shader);
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);
	glCompileShader(fragment_shader);
	
	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);
	
	unsigned int location = glGetAttribLocation(shader_program, "a_pos");
	glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
	glEnableVertexAttribArray(location);
}

void log_error(const char *description) {
	fprintf(stderr, "\033[91m\033[1m%s\033[0m\n", description);
}
void end(int code) {
	glfwTerminate();
	exit(code);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	glViewport(0, 0, width, height);
}
void error_callback(int code, const char *description) {
	log_error(description);
	end(code);
}
