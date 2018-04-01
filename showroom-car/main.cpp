// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <common/shader.hpp>
#include <common/controls.hpp>

#include <math.h>

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
	window = glfwCreateWindow(1024, 768, "Showroom Car", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	// Hide the mouse and enable unlimited mouvement
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the mouse at the center of the screen
	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	GLfloat half_car_width = 0.3f;

	GLfloat g_vertex_buffer_data[] = {
		-0.9f, -0.4f, half_car_width,
		-0.9f, 0.2f, half_car_width,
		-0.8f, 0.6f, half_car_width,
		0.2f, 0.6f, half_car_width,
		0.9f, 0.1f, half_car_width,
		0.9f, -0.4f, half_car_width,
		-0.9f, -0.4f, 0-half_car_width,
		-0.9f, 0.2f, 0-half_car_width,
		-0.8f, 0.6f, 0-half_car_width,
		0.2f, 0.6f, 0-half_car_width,
		0.9f, 0.1f, 0-half_car_width,
		0.9f, -0.4f, 0-half_car_width
	};

	GLfloat half_wheel_width = 0.1f;

	GLfloat g_vertex2_buffer_data[] = {
		-0.6f, -0.2f, half_car_width,
		-0.45f, -0.25f, half_car_width,
		-0.4f, -0.4f, half_car_width,
		-0.45f, -0.55f, half_car_width,
		-0.6f, -0.6f, half_car_width,
		-0.75f, -0.55f, half_car_width,
		-0.8f, -0.4f, half_car_width,
		-0.75f, -0.25f, half_car_width,
		-0.6f, -0.2f, half_car_width-half_wheel_width,
		-0.45f, -0.25f, half_car_width-half_wheel_width,
		-0.4f, -0.4f, half_car_width-half_wheel_width,
		-0.45f, -0.55f, half_car_width-half_wheel_width,
		-0.6f, -0.6f, half_car_width-half_wheel_width,
		-0.75f, -0.55f, half_car_width-half_wheel_width,
		-0.8f, -0.4f, half_car_width-half_wheel_width,
		-0.75f, -0.25f, half_car_width-half_wheel_width,

		-0.6f, -0.2f, 0-half_car_width,
		-0.45f, -0.25f, 0-half_car_width,
		-0.4f, -0.4f, 0-half_car_width,
		-0.45f, -0.55f, 0-half_car_width,
		-0.6f, -0.6f, 0-half_car_width,
		-0.75f, -0.55f, 0-half_car_width,
		-0.8f, -0.4f, 0-half_car_width,
		-0.75f, -0.25f, 0-half_car_width,
		-0.6f, -0.2f, 0-half_car_width + half_wheel_width,
		-0.45f, -0.25f, 0-half_car_width + half_wheel_width,
		-0.4f, -0.4f, 0-half_car_width + half_wheel_width,
		-0.45f, -0.55f, 0-half_car_width + half_wheel_width,
		-0.6f, -0.6f, 0-half_car_width + half_wheel_width,
		-0.75f, -0.55f, 0-half_car_width + half_wheel_width,
		-0.8f, -0.4f, 0-half_car_width + half_wheel_width,
		-0.75f, -0.25f, 0-half_car_width + half_wheel_width
	};

	GLfloat g_vertex3_buffer_data[] = {
		0.6f, -0.2f, half_car_width,
		0.45f, -0.25f, half_car_width,
		0.4f, -0.4f, half_car_width,
		0.45f, -0.55f, half_car_width,
		0.6f, -0.6f, half_car_width,
		0.75f, -0.55f, half_car_width,
		0.8f, -0.4f, half_car_width,
		0.75f, -0.25f, half_car_width,
		0.6f, -0.2f, half_car_width-half_wheel_width,
		0.45f, -0.25f, half_car_width-half_wheel_width,
		0.4f, -0.4f, half_car_width-half_wheel_width,
		0.45f, -0.55f, half_car_width-half_wheel_width,
		0.6f, -0.6f, half_car_width-half_wheel_width,
		0.75f, -0.55f, half_car_width-half_wheel_width,
		0.8f, -0.4f, half_car_width-half_wheel_width,
		0.75f, -0.25f, half_car_width-half_wheel_width,

		0.6f, -0.2f, 0-half_car_width,
		0.45f, -0.25f, 0-half_car_width,
		0.4f, -0.4f, 0-half_car_width,
		0.45f, -0.55f, 0-half_car_width,
		0.6f, -0.6f, 0-half_car_width,
		0.75f, -0.55f, 0-half_car_width,
		0.8f, -0.4f, 0-half_car_width,
		0.75f, -0.25f, 0-half_car_width,
		0.6f, -0.2f, 0-half_car_width + half_wheel_width,
		0.45f, -0.25f, 0-half_car_width + half_wheel_width,
		0.4f, -0.4f, 0-half_car_width + half_wheel_width,
		0.45f, -0.55f, 0-half_car_width + half_wheel_width,
		0.6f, -0.6f, 0-half_car_width + half_wheel_width,
		0.75f, -0.55f, 0-half_car_width + half_wheel_width,
		0.8f, -0.4f, 0-half_car_width + half_wheel_width,
		0.75f, -0.25f, 0-half_car_width + half_wheel_width
	};

	GLuint g_element_buffer_data[] = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		6, 7, 8,
		6, 8, 9,
		6, 9, 10,
		6, 10, 11,
		0, 1, 6,
		1, 2, 7,
		2, 3, 8,
		3, 4, 9,
		4, 5, 10,
		5, 0, 11,
		6, 7, 1,
		7, 8, 2,
		8, 9, 3,
		9, 10, 4,
		10, 11, 5,
		11, 6, 0,
	};

	GLuint g_element2_buffer_data[] = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 7,
		8, 9, 10,
		8, 10, 11,
		8, 11, 12,
		8, 12, 13,
		8, 13, 14,
		8, 14, 15,
		0, 1, 8,
		1, 2, 9,
		2, 3, 10,
		3, 4, 11,
		4, 5, 12,
		5, 6, 13,
		6, 7, 14,
		7, 0, 15,
		8, 9, 1,
		9, 10, 2,
		10, 11, 3,
		11, 12, 4,
		12, 13, 5,
		13, 14, 6,
		14, 15, 7,
		15, 8, 0,

		0 + 16, 1 + 16, 2 + 16,
		0 + 16, 2 + 16, 3 + 16,
		0 + 16, 3 + 16, 4 + 16,
		0 + 16, 4 + 16, 5 + 16,
		0 + 16, 5 + 16, 6 + 16,
		0 + 16, 6 + 16, 7 + 16,
		8 + 16, 9 + 16, 10 + 16,
		8 + 16, 10 + 16, 11 + 16,
		8 + 16, 11 + 16, 12 + 16,
		8 + 16, 12 + 16, 13 + 16,
		8 + 16, 13 + 16, 14 + 16,
		8 + 16, 14 + 16, 15 + 16,
		0 + 16, 1 + 16, 8 + 16,
		1 + 16, 2 + 16, 9 + 16,
		2 + 16, 3 + 16, 10 + 16,
		3 + 16, 4 + 16, 11 + 16,
		4 + 16, 5 + 16, 12 + 16,
		5 + 16, 6 + 16, 13 + 16,
		6 + 16, 7 + 16, 14 + 16,
		7 + 16, 0 + 16, 15 + 16,
		8 + 16, 9 + 16, 1 + 16,
		9 + 16, 10 + 16, 2 + 16,
		10 + 16, 11 + 16, 3 + 16,
		11 + 16, 12 + 16, 4 + 16,
		12 + 16, 13 + 16, 5 + 16,
		13 + 16, 14 + 16, 6 + 16,
		14 + 16, 15 + 16, 7 + 16,
		15 + 16, 8 + 16, 0 + 16
	};

	GLuint g_element3_buffer_data[] = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 7,
		8, 9, 10,
		8, 10, 11,
		8, 11, 12,
		8, 12, 13,
		8, 13, 14,
		8, 14, 15,
		0, 1, 8,
		1, 2, 9,
		2, 3, 10,
		3, 4, 11,
		4, 5, 12,
		5, 6, 13,
		6, 7, 14,
		7, 0, 15,
		8, 9, 1,
		9, 10, 2,
		10, 11, 3,
		11, 12, 4,
		12, 13, 5,
		13, 14, 6,
		14, 15, 7,
		15, 8, 0,

		0 + 16, 1 + 16, 2 + 16,
		0 + 16, 2 + 16, 3 + 16,
		0 + 16, 3 + 16, 4 + 16,
		0 + 16, 4 + 16, 5 + 16,
		0 + 16, 5 + 16, 6 + 16,
		0 + 16, 6 + 16, 7 + 16,
		8 + 16, 9 + 16, 10 + 16,
		8 + 16, 10 + 16, 11 + 16,
		8 + 16, 11 + 16, 12 + 16,
		8 + 16, 12 + 16, 13 + 16,
		8 + 16, 13 + 16, 14 + 16,
		8 + 16, 14 + 16, 15 + 16,
		0 + 16, 1 + 16, 8 + 16,
		1 + 16, 2 + 16, 9 + 16,
		2 + 16, 3 + 16, 10 + 16,
		3 + 16, 4 + 16, 11 + 16,
		4 + 16, 5 + 16, 12 + 16,
		5 + 16, 6 + 16, 13 + 16,
		6 + 16, 7 + 16, 14 + 16,
		7 + 16, 0 + 16, 15 + 16,
		8 + 16, 9 + 16, 1 + 16,
		9 + 16, 10 + 16, 2 + 16,
		10 + 16, 11 + 16, 3 + 16,
		11 + 16, 12 + 16, 4 + 16,
		12 + 16, 13 + 16, 5 + 16,
		13 + 16, 14 + 16, 6 + 16,
		14 + 16, 15 + 16, 7 + 16,
		15 + 16, 8 + 16, 0 + 16
	};

	// Object 1
	// Create Vertex Array Object
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Create an element array
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_element_buffer_data), g_element_buffer_data, GL_STATIC_DRAW);

	// Object 2
	// Create Vertex Array Object
	GLuint VertexArrayID2;
	glGenVertexArrays(1, &VertexArrayID2);
	glBindVertexArray(VertexArrayID2);

	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint vertexbuffer2;
	glGenBuffers(1, &vertexbuffer2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex2_buffer_data), g_vertex2_buffer_data, GL_STATIC_DRAW);

	// 2nd attribute buffer : vertices
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Create an element array
	GLuint elementbuffer2;
	glGenBuffers(1, &elementbuffer2);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_element2_buffer_data), g_element2_buffer_data, GL_STATIC_DRAW);

	// Object 3
	// Create Vertex Array Object
	GLuint VertexArrayID3;
	glGenVertexArrays(1, &VertexArrayID3);
	glBindVertexArray(VertexArrayID3);

	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint vertexbuffer3;
	glGenBuffers(1, &vertexbuffer3);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer3);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex3_buffer_data), g_vertex3_buffer_data, GL_STATIC_DRAW);

	// 3rd attribute buffer : vertices
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// Create an element array
	GLuint elementbuffer3;
	glGenBuffers(1, &elementbuffer3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(g_element3_buffer_data), g_element3_buffer_data, GL_STATIC_DRAW);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("CarVertexShader.vertexshader", "CarFragmentShader.fragmentshader");

	GLuint programID2 = LoadShaders("BackWheelVertexShader.vertexshader", "WheelFragmentShader.fragmentshader");
	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID2, "MVP1");

	GLuint programID3 = LoadShaders("FrontWheelVertexShader.vertexshader", "WheelFragmentShader.fragmentshader");
	// Get a handle for our "MVP" uniform
	GLuint MatrixID2 = glGetUniformLocation(programID3, "MVP2");

	// Get a handle for our "MVP" uniform
	GLuint MatrixID01 = glGetUniformLocation(programID, "MVP");
	GLuint MatrixID02 = glGetUniformLocation(programID2, "MVP");
	GLuint MatrixID03 = glGetUniformLocation(programID3, "MVP");

	float angle = 0;

	glm::mat4 translation1a, translation1b, translation2a, translation2b, rotation, translation2, MVP1, MVP2;

	// MVP back wheel
	translation1a = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, 0.4f, 0.0f));
	rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
	translation1b = glm::translate(glm::mat4(1.0f), glm::vec3(-0.6f, -0.4f, 0.0f));
	MVP1 = translation1b * rotation * translation1a;

	// MVP front wheel
	translation2a = glm::translate(glm::mat4(1.0f), glm::vec3(-0.6f, 0.4f, 0.0f));
	rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
	translation2b = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, -0.4f, 0.0f));
	MVP2 = translation2b * rotation * translation2a;

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP1[0][0]);

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP2[0][0]);

	do {
		// Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
		glClear(GL_COLOR_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID01, 1, GL_FALSE, &MVP[0][0]);

		// Draw object 1
		glBindVertexArray(VertexArrayID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
		glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0);

		// Use our shader
		glUseProgram(programID2);
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP1[0][0]);
		glUniformMatrix4fv(MatrixID02, 1, GL_FALSE, &MVP[0][0]);

		// Draw object 2
		glBindVertexArray(VertexArrayID2);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer2);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex2_buffer_data), g_vertex2_buffer_data, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer2);
		glDrawElements(GL_TRIANGLES, 168, GL_UNSIGNED_INT, 0);

		// Use our shader
		glUseProgram(programID3);
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP2[0][0]);
		glUniformMatrix4fv(MatrixID03, 1, GL_FALSE, &MVP[0][0]);

		// Draw object 3
		glBindVertexArray(VertexArrayID3);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer3);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex3_buffer_data), g_vertex3_buffer_data, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer3);
		glDrawElements(GL_TRIANGLES, 168, GL_UNSIGNED_INT, 0);

		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		angle -= 1.0f;

		rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
		// MVP back wheel
		MVP1 = translation1b * rotation * translation1a;
		// MVP front wheel
		MVP2 = translation2b * rotation * translation2a;

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &elementbuffer);
	glDeleteBuffers(1, &elementbuffer2);
	glDeleteBuffers(1, &elementbuffer3);
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &vertexbuffer2);
	glDeleteBuffers(1, &vertexbuffer3);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteVertexArrays(1, &VertexArrayID2);
	glDeleteVertexArrays(1, &VertexArrayID3);
	glDeleteProgram(programID);
	glDeleteProgram(programID2);
	glDeleteProgram(programID3);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}