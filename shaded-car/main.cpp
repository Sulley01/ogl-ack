// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <shader.hpp>

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Shaded Car", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Create Vertex Array Object
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);

	static const GLfloat g_vertex_buffer_data[] = {
		-0.9f, -0.4f, 0.0f, 0.1f, 0.0f, 0.0f,
		-0.9f, 0.2f, 0.0f, 0.5f, 0.0f, 0.0f,
		-0.8f, 0.6f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.2f, 0.6f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.9f, 0.1f, 0.0f, 0.5f, 0.0f, 0.0f,
		0.9f, -0.4f, 0.0f, 0.1f, 0.0f, 0.0f
	};

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// Create an element array
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);

	GLuint g_element_buffer_data[] = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_element_buffer_data), g_element_buffer_data, GL_STATIC_DRAW);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,								// attribute 0
		3,								// size
		GL_FLOAT,						// type
		GL_FALSE,						// normalized?
		sizeof(GL_FLOAT) * 6,			// stride
		(void*)0						// array buffer offset
	);

	// 2nd attribute buffer : vertices color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,								// attribute 1
		3,								// size
		GL_FLOAT,						// type
		GL_FALSE,						// normalized?
		sizeof(GL_FLOAT) * 6,			// stride
		(void*)(sizeof(GL_FLOAT) * 3)	// array buffer offset
	);

	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	glDisableVertexAttribArray(0);

	// Cleanup VBO
	glDeleteBuffers(1, &elementbuffer);
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}