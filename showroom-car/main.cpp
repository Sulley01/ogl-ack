// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Include libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <common/shader.hpp>
#include <common/controls.hpp>

// Global variables
GLFWwindow* window;

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

	// Vertexes data
	GLfloat half_car_width = 0.3f;
	GLfloat window_grey[]{
		-0.85f, 0.2f, half_car_width,
		-0.75f, 0.2f, half_car_width,
		-0.75f, 0.55f, half_car_width,
		0.2f, 0.55f, half_car_width,
		0.2f, 0.2f, half_car_width,
		0.4f, 0.2f, half_car_width,
		-0.7f, 0.55f, half_car_width,
		0.15f, 0.55f, half_car_width,
		0.15f, 0.2f, half_car_width,
		-0.7f, 0.2f, half_car_width
	};
	GLfloat front_window_vertexes[] = {
		-0.85f, 0.2f, half_car_width + 0.01f,
		-0.75f, 0.2f, half_car_width + 0.01f,
		-0.75f, 0.55f, half_car_width + 0.01f,
		0.2f, 0.55f, half_car_width + 0.01f,
		0.2f, 0.2f, half_car_width + 0.01f,
		0.4f, 0.2f, half_car_width + 0.01f,
		-0.7f, 0.55f, half_car_width + 0.01f,
		0.15f, 0.55f, half_car_width + 0.01f,
		0.15f, 0.2f, half_car_width + 0.01f,
		-0.7f, 0.2f, half_car_width + 0.01f,
		// Belakang
		-0.85f, 0.2f, 0 - half_car_width,
		-0.75f, 0.2f, 0 - half_car_width,
		-0.75f, 0.55f, 0 - half_car_width,
		0.2f, 0.55f, 0 - half_car_width,
		0.2f, 0.2f, 0 - half_car_width,
		0.4f, 0.2f, 0 - half_car_width,
		-0.7f, 0.55f, 0 - half_car_width,
		0.15f, 0.55f, 0 - half_car_width,
		0.15f, 0.2f, 0 - half_car_width,
		-0.7f, 0.2f, 0 - half_car_width,
		// Kaca Depan
		0.3f, 0.5f, 0.25f,
		0.3f, 0.5f, -0.25f,
		0.7f, 0.2f, -0.25f,
		0.7f, 0.2f, 0.25f,
		// Kaca Belakang
		-0.8f, 0.55f, 0.25f,
		-0.9f, 0.2f, 0.25f,
		-0.9f, 0.2f, -0.25f,
		-0.8f, 0.55f, -0.25f
	};
	GLfloat car_vertexes[] = {
		// Z+
		-0.9f, -0.4f, half_car_width,
		-0.9f, 0.2f, half_car_width,
		-0.8f, 0.6f, half_car_width,
		0.2f, 0.6f, half_car_width,
		0.9f, 0.1f, half_car_width,
		0.9f, -0.4f, half_car_width,
		// Z-
		-0.9f, -0.4f, 0 - half_car_width,
		-0.9f, 0.2f, 0 - half_car_width,
		-0.8f, 0.6f, 0 - half_car_width,
		0.2f, 0.6f, 0 - half_car_width,
		0.9f, 0.1f, 0 - half_car_width,
		0.9f, -0.4f, 0 - half_car_width
	};
	GLfloat half_wheel_width = 0.1f;
	GLfloat backwheel_vertexes[] = {
		// Z+ out
		-0.6f, -0.2f, half_car_width,
		-0.45f, -0.25f, half_car_width,
		-0.4f, -0.4f, half_car_width,
		-0.45f, -0.55f, half_car_width,
		-0.6f, -0.6f, half_car_width,
		-0.75f, -0.55f, half_car_width,
		-0.8f, -0.4f, half_car_width,
		-0.75f, -0.25f, half_car_width,
		// Z+ in
		-0.6f, -0.2f, half_car_width - half_wheel_width,
		-0.45f, -0.25f, half_car_width - half_wheel_width,
		-0.4f, -0.4f, half_car_width - half_wheel_width,
		-0.45f, -0.55f, half_car_width - half_wheel_width,
		-0.6f, -0.6f, half_car_width - half_wheel_width,
		-0.75f, -0.55f, half_car_width - half_wheel_width,
		-0.8f, -0.4f, half_car_width - half_wheel_width,
		-0.75f, -0.25f, half_car_width - half_wheel_width,
		// Z- out
		-0.6f, -0.2f, 0 - half_car_width,
		-0.45f, -0.25f, 0 - half_car_width,
		-0.4f, -0.4f, 0 - half_car_width,
		-0.45f, -0.55f, 0 - half_car_width,
		-0.6f, -0.6f, 0 - half_car_width,
		-0.75f, -0.55f, 0 - half_car_width,
		-0.8f, -0.4f, 0 - half_car_width,
		-0.75f, -0.25f, 0 - half_car_width,
		// Z- in
		-0.6f, -0.2f, 0 - half_car_width + half_wheel_width,
		-0.45f, -0.25f, 0 - half_car_width + half_wheel_width,
		-0.4f, -0.4f, 0 - half_car_width + half_wheel_width,
		-0.45f, -0.55f, 0 - half_car_width + half_wheel_width,
		-0.6f, -0.6f, 0 - half_car_width + half_wheel_width,
		-0.75f, -0.55f, 0 - half_car_width + half_wheel_width,
		-0.8f, -0.4f, 0 - half_car_width + half_wheel_width,
		-0.75f, -0.25f, 0 - half_car_width + half_wheel_width
	};
	GLfloat frontwheel_vertexes[] = {
		// Z+ out
		0.6f, -0.2f, half_car_width,
		0.45f, -0.25f, half_car_width,
		0.4f, -0.4f, half_car_width,
		0.45f, -0.55f, half_car_width,
		0.6f, -0.6f, half_car_width,
		0.75f, -0.55f, half_car_width,
		0.8f, -0.4f, half_car_width,
		0.75f, -0.25f, half_car_width,
		// Z+ in
		0.6f, -0.2f, half_car_width - half_wheel_width,
		0.45f, -0.25f, half_car_width - half_wheel_width,
		0.4f, -0.4f, half_car_width - half_wheel_width,
		0.45f, -0.55f, half_car_width - half_wheel_width,
		0.6f, -0.6f, half_car_width - half_wheel_width,
		0.75f, -0.55f, half_car_width - half_wheel_width,
		0.8f, -0.4f, half_car_width - half_wheel_width,
		0.75f, -0.25f, half_car_width - half_wheel_width,
		// Z- out
		0.6f, -0.2f, 0 - half_car_width,
		0.45f, -0.25f, 0 - half_car_width,
		0.4f, -0.4f, 0 - half_car_width,
		0.45f, -0.55f, 0 - half_car_width,
		0.6f, -0.6f, 0 - half_car_width,
		0.75f, -0.55f, 0 - half_car_width,
		0.8f, -0.4f, 0 - half_car_width,
		0.75f, -0.25f, 0 - half_car_width,
		// Z- in
		0.6f, -0.2f, 0 - half_car_width + half_wheel_width,
		0.45f, -0.25f, 0 - half_car_width + half_wheel_width,
		0.4f, -0.4f, 0 - half_car_width + half_wheel_width,
		0.45f, -0.55f, 0 - half_car_width + half_wheel_width,
		0.6f, -0.6f, 0 - half_car_width + half_wheel_width,
		0.75f, -0.55f, 0 - half_car_width + half_wheel_width,
		0.8f, -0.4f, 0 - half_car_width + half_wheel_width,
		0.75f, -0.25f, 0 - half_car_width + half_wheel_width
	};
	
	GLuint car_elements[] = {
		// Z+
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		// Z-
		6, 7, 8,
		6, 8, 9,
		6, 9, 10,
		6, 10, 11,
		// Z between
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
	GLuint one_wheel_size = 16;
	GLuint backwheel_elements[] = {
		// Z+ out
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 7,
		// Z+ in
		8, 9, 10,
		8, 10, 11,
		8, 11, 12,
		8, 12, 13,
		8, 13, 14,
		8, 14, 15,
		// Z+ between
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
		// Z- out
		0 + one_wheel_size, 1 + one_wheel_size, 2 + one_wheel_size,
		0 + one_wheel_size, 2 + one_wheel_size, 3 + one_wheel_size,
		0 + one_wheel_size, 3 + one_wheel_size, 4 + one_wheel_size,
		0 + one_wheel_size, 4 + one_wheel_size, 5 + one_wheel_size,
		0 + one_wheel_size, 5 + one_wheel_size, 6 + one_wheel_size,
		0 + one_wheel_size, 6 + one_wheel_size, 7 + one_wheel_size,
		// Z- in
		8 + one_wheel_size, 9 + one_wheel_size, 10 + one_wheel_size,
		8 + one_wheel_size, 10 + one_wheel_size, 11 + one_wheel_size,
		8 + one_wheel_size, 11 + one_wheel_size, 12 + one_wheel_size,
		8 + one_wheel_size, 12 + one_wheel_size, 13 + one_wheel_size,
		8 + one_wheel_size, 13 + one_wheel_size, 14 + one_wheel_size,
		8 + one_wheel_size, 14 + one_wheel_size, 15 + one_wheel_size,
		// Z- between
		0 + one_wheel_size, 1 + one_wheel_size, 8 + one_wheel_size,
		1 + one_wheel_size, 2 + one_wheel_size, 9 + one_wheel_size,
		2 + one_wheel_size, 3 + one_wheel_size, 10 + one_wheel_size,
		3 + one_wheel_size, 4 + one_wheel_size, 11 + one_wheel_size,
		4 + one_wheel_size, 5 + one_wheel_size, 12 + one_wheel_size,
		5 + one_wheel_size, 6 + one_wheel_size, 13 + one_wheel_size,
		6 + one_wheel_size, 7 + one_wheel_size, 14 + one_wheel_size,
		7 + one_wheel_size, 0 + one_wheel_size, 15 + one_wheel_size,
		8 + one_wheel_size, 9 + one_wheel_size, 1 + one_wheel_size,
		9 + one_wheel_size, 10 + one_wheel_size, 2 + one_wheel_size,
		10 + one_wheel_size, 11 + one_wheel_size, 3 + one_wheel_size,
		11 + one_wheel_size, 12 + one_wheel_size, 4 + one_wheel_size,
		12 + one_wheel_size, 13 + one_wheel_size, 5 + one_wheel_size,
		13 + one_wheel_size, 14 + one_wheel_size, 6 + one_wheel_size,
		14 + one_wheel_size, 15 + one_wheel_size, 7 + one_wheel_size,
		15 + one_wheel_size, 8 + one_wheel_size, 0 + one_wheel_size
	};
	GLuint greyWindowElements[] = {
		// Kaca Depan Bagian Belakang
		0,1,2,
		// Kaca Depan Bagian Depan
		3,4,5,
		// Kaca Depan Tengah
		6,7,8,
		6,8,9,
	};
	GLuint backWindowElements[] = {
		// Kaca Depan Bagian Belakang
		0,1,2,
		// Kaca Depan Bagian Depan
		3,4,5,
		// Kaca Depan Tengah
		6,7,8,
		6,8,9,
		// Kaca Belakang Bagian Belakang
		10,11,12,
		// Kaca Belakang Bagian Depan
		13,14,15,
		// Kaca Belakang Bagian Tengah
		16,17,18,
		16,18,19,
		// Kaca Depan
		20,21,22,
		20,22,23,
		// Kaca Belakang
		24,25,26,
		24,26,27
	};
	GLuint frontwheel_elements[] = {
		// Z+ out
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 7,
		// Z+ in
		8, 9, 10,
		8, 10, 11,
		8, 11, 12,
		8, 12, 13,
		8, 13, 14,
		8, 14, 15,
		// Z+ between
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
		// Z- out
		0 + one_wheel_size, 1 + one_wheel_size, 2 + one_wheel_size,
		0 + one_wheel_size, 2 + one_wheel_size, 3 + one_wheel_size,
		0 + one_wheel_size, 3 + one_wheel_size, 4 + one_wheel_size,
		0 + one_wheel_size, 4 + one_wheel_size, 5 + one_wheel_size,
		0 + one_wheel_size, 5 + one_wheel_size, 6 + one_wheel_size,
		0 + one_wheel_size, 6 + one_wheel_size, 7 + one_wheel_size,
		// Z- in
		8 + one_wheel_size, 9 + one_wheel_size, 10 + one_wheel_size,
		8 + one_wheel_size, 10 + one_wheel_size, 11 + one_wheel_size,
		8 + one_wheel_size, 11 + one_wheel_size, 12 + one_wheel_size,
		8 + one_wheel_size, 12 + one_wheel_size, 13 + one_wheel_size,
		8 + one_wheel_size, 13 + one_wheel_size, 14 + one_wheel_size,
		8 + one_wheel_size, 14 + one_wheel_size, 15 + one_wheel_size,
		// Z- between
		0 + one_wheel_size, 1 + one_wheel_size, 8 + one_wheel_size,
		1 + one_wheel_size, 2 + one_wheel_size, 9 + one_wheel_size,
		2 + one_wheel_size, 3 + one_wheel_size, 10 + one_wheel_size,
		3 + one_wheel_size, 4 + one_wheel_size, 11 + one_wheel_size,
		4 + one_wheel_size, 5 + one_wheel_size, 12 + one_wheel_size,
		5 + one_wheel_size, 6 + one_wheel_size, 13 + one_wheel_size,
		6 + one_wheel_size, 7 + one_wheel_size, 14 + one_wheel_size,
		7 + one_wheel_size, 0 + one_wheel_size, 15 + one_wheel_size,
		8 + one_wheel_size, 9 + one_wheel_size, 1 + one_wheel_size,
		9 + one_wheel_size, 10 + one_wheel_size, 2 + one_wheel_size,
		10 + one_wheel_size, 11 + one_wheel_size, 3 + one_wheel_size,
		11 + one_wheel_size, 12 + one_wheel_size, 4 + one_wheel_size,
		12 + one_wheel_size, 13 + one_wheel_size, 5 + one_wheel_size,
		13 + one_wheel_size, 14 + one_wheel_size, 6 + one_wheel_size,
		14 + one_wheel_size, 15 + one_wheel_size, 7 + one_wheel_size,
		15 + one_wheel_size, 8 + one_wheel_size, 0 + one_wheel_size
	};

	// Object 1
	// Create Vertex Array Object
	GLuint CarVAO;
	glGenVertexArrays(1, &CarVAO);
	glBindVertexArray(CarVAO);
	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint CarVBO;
	glGenBuffers(1, &CarVBO);
	glBindBuffer(GL_ARRAY_BUFFER, CarVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(car_vertexes), car_vertexes, GL_STATIC_DRAW);
	// Create an element array
	GLuint CarEBO;
	glGenBuffers(1, &CarEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CarEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(car_elements), car_elements, GL_STATIC_DRAW);

	//Test Window
	GLuint jendelaBelakangVAO;
	glGenVertexArrays(1, &jendelaBelakangVAO);
	glBindVertexArray(jendelaBelakangVAO);

	GLuint jendelaBelakangVBO;
	glGenBuffers(1, &jendelaBelakangVBO);
	glBindBuffer(GL_ARRAY_BUFFER, jendelaBelakangVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(front_window_vertexes), front_window_vertexes, GL_STATIC_DRAW);

	GLuint jendelaBelakangEBO;
	glGenBuffers(1, &jendelaBelakangEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, jendelaBelakangEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(backWindowElements), backWindowElements, GL_STATIC_DRAW);

	//Test Window Grey
	GLuint jendelaBelakangGreyVAO;
	glGenVertexArrays(1, &jendelaBelakangGreyVAO);
	glBindVertexArray(jendelaBelakangGreyVAO);

	GLuint jendelaBelakangGreyVBO;
	glGenBuffers(1, &jendelaBelakangGreyVBO);
	glBindBuffer(GL_ARRAY_BUFFER, jendelaBelakangGreyVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(window_grey), window_grey, GL_STATIC_DRAW);

	GLuint jendelaBelakangGreyEBO;
	glGenBuffers(1, &jendelaBelakangGreyEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, jendelaBelakangGreyEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(greyWindowElements), greyWindowElements, GL_STATIC_DRAW);

	// Object 2
	// Create Vertex Array Object
	GLuint BackwheelVAO;
	glGenVertexArrays(1, &BackwheelVAO);
	glBindVertexArray(BackwheelVAO);
	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint BackwheelVBO;
	glGenBuffers(1, &BackwheelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, BackwheelVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(backwheel_vertexes), backwheel_vertexes, GL_STATIC_DRAW);
	// Create an element array
	GLuint BackwheelEBO;
	glGenBuffers(1, &BackwheelEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BackwheelEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(backwheel_elements), backwheel_elements, GL_STATIC_DRAW);

	// Object 3
	// Create Vertex Array Object
	GLuint FrontwheelVAO;
	glGenVertexArrays(1, &FrontwheelVAO);
	glBindVertexArray(FrontwheelVAO);
	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint FrontwheelVBO;
	glGenBuffers(1, &FrontwheelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, FrontwheelVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(frontwheel_vertexes), frontwheel_vertexes, GL_STATIC_DRAW);
	// Create an element array
	GLuint FrontwheelEBO;
	glGenBuffers(1, &FrontwheelEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FrontwheelEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(frontwheel_elements), frontwheel_elements, GL_STATIC_DRAW);

	// Create and compile our GLSL program from the shaders
	GLuint CarProgram = LoadShaders("CarVertexShader.vertexshader", "CarFragmentShader.fragmentshader");
	GLuint BackwheelProgram = LoadShaders("BackWheelVertexShader.vertexshader", "WheelFragmentShader.fragmentshader");
	GLuint FrontwheelProgram = LoadShaders("FrontWheelVertexShader.vertexshader", "WheelFragmentShader.fragmentshader");
	GLuint FrontWindowProgram = LoadShaders("FrontWindowVertexShader.vertexshader", "WindowFragmentShader.fragmentshader");
	GLuint GreyWindowProgram = LoadShaders("GreyWindowVertexShader.vertexshader", "GreyWindowFragmentShader.fragmentshader");
	// Get a handle for our "MVP" uniform
	GLuint CarCameraMatrix = glGetUniformLocation(CarProgram, "CarCameraMVP");
	GLuint BackwheelRotationMatrix = glGetUniformLocation(BackwheelProgram, "BackwheelRotationMVP");
	GLuint BackwheelCameraMatrix = glGetUniformLocation(BackwheelProgram, "BackwheelCameraMVP");
	GLuint FrontwheelRotationMatrix = glGetUniformLocation(FrontwheelProgram, "FrontwheelRotationMVP");
	GLuint FrontwheelCameraMatrix = glGetUniformLocation(FrontwheelProgram, "FrontwheelCameraMVP");
	GLuint FrontWindowCameraMatrix = glGetUniformLocation(FrontWindowProgram, "FrontWindowCameraMVP");
	GLuint GreyWindowCameraMatrix = glGetUniformLocation(GreyWindowProgram, "GreyWindowCameraMVP");

	// Variables
	float angle = 0;

	do {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 CameraProjection = getProjectionMatrix();
		glm::mat4 CameraView = getViewMatrix();
		glm::mat4 CameraModel = glm::mat4(1.0);
		glm::mat4 CameraMVP = CameraProjection * CameraView * CameraModel;

		// MVP backwheel
		glm::mat4 BackwheelSubTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, 0.4f, 0.0f));
		glm::mat4 BackwheelRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 BackwheelAddTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(-0.6f, -0.4f, 0.0f));
		glm::mat4 BackwheelMVP = BackwheelAddTranslation * BackwheelRotation * BackwheelSubTranslation;

		// MVP frontwheel
		glm::mat4 FrontwheelSubTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(-0.6f, 0.4f, 0.0f));
		glm::mat4 FrontwheelRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 FrontwheelAddTranslation = glm::translate(glm::mat4(1.0f), glm::vec3(0.6f, -0.4f, 0.0f));
		glm::mat4 FrontwheelMVP = FrontwheelAddTranslation * FrontwheelRotation * FrontwheelSubTranslation;

		// Use our shader
		glUseProgram(CarProgram);
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(CarCameraMatrix, 1, GL_FALSE, &CameraMVP[0][0]);
		// Draw object 1
		glBindVertexArray(CarVAO);
		glBindBuffer(GL_ARRAY_BUFFER, CarVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(car_vertexes), car_vertexes, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CarEBO);
		glDrawElements(GL_TRIANGLES, sizeof(car_elements), GL_UNSIGNED_INT, 0);

		// Use our shader
		glUseProgram(BackwheelProgram);
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(BackwheelRotationMatrix, 1, GL_FALSE, &BackwheelMVP[0][0]);
		glUniformMatrix4fv(BackwheelCameraMatrix, 1, GL_FALSE, &CameraMVP[0][0]);
		// Draw object 2
		glBindVertexArray(BackwheelVAO);
		glBindBuffer(GL_ARRAY_BUFFER, BackwheelVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backwheel_vertexes), backwheel_vertexes, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BackwheelEBO);
		glDrawElements(GL_TRIANGLES, sizeof(backwheel_elements), GL_UNSIGNED_INT, 0);

		//Test gambar window grey
		glUseProgram(GreyWindowProgram);
		glUniformMatrix4fv(GreyWindowCameraMatrix, 1, GL_FALSE, &CameraMVP[0][0]);
		glBindVertexArray(jendelaBelakangGreyVAO);
		glBindBuffer(GL_ARRAY_BUFFER, jendelaBelakangGreyVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(window_grey), window_grey, GL_STATIC_DRAW);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(
			3,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, jendelaBelakangGreyEBO);
		glDrawElements(GL_TRIANGLES, sizeof(greyWindowElements), GL_UNSIGNED_INT, 0);

		//Test gambar window
		glUseProgram(FrontWindowProgram);
		glUniformMatrix4fv(FrontWindowCameraMatrix, 1, GL_FALSE, &CameraMVP[0][0]);
		glBindVertexArray(jendelaBelakangVAO);
		glBindBuffer(GL_ARRAY_BUFFER, jendelaBelakangVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(front_window_vertexes), front_window_vertexes, GL_STATIC_DRAW);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(
			3,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, jendelaBelakangEBO);
		glDrawElements(GL_TRIANGLES, sizeof(backWindowElements), GL_UNSIGNED_INT, 0);




		// Use our shader
		glUseProgram(FrontwheelProgram);
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(FrontwheelRotationMatrix, 1, GL_FALSE, &FrontwheelMVP[0][0]);
		glUniformMatrix4fv(FrontwheelCameraMatrix, 1, GL_FALSE, &CameraMVP[0][0]);
		// Draw object 3
		glBindVertexArray(FrontwheelVAO);
		glBindBuffer(GL_ARRAY_BUFFER, FrontwheelVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(frontwheel_vertexes), frontwheel_vertexes, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FrontwheelEBO);
		glDrawElements(GL_TRIANGLES, sizeof(frontwheel_elements), GL_UNSIGNED_INT, 0);

		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Rotate
		angle -= 1.0f;
		// MVP backwheel
		BackwheelRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
		BackwheelMVP = BackwheelAddTranslation * BackwheelRotation * BackwheelSubTranslation;
		// MVP frontwheel
		FrontwheelRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
		FrontwheelMVP = FrontwheelAddTranslation * FrontwheelRotation * FrontwheelSubTranslation;

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &CarEBO);
	glDeleteBuffers(1, &BackwheelEBO);
	glDeleteBuffers(1, &FrontwheelEBO);
	glDeleteBuffers(1, &jendelaBelakangEBO);
	glDeleteBuffers(1, &CarVBO);
	glDeleteBuffers(1, &BackwheelVBO);
	glDeleteBuffers(1, &FrontwheelVBO);
	glDeleteBuffers(1, &jendelaBelakangVBO);
	glDeleteVertexArrays(1, &CarVAO);
	glDeleteVertexArrays(1, &BackwheelVAO);
	glDeleteVertexArrays(1, &FrontwheelVAO);
	glDeleteVertexArrays(1, &jendelaBelakangVAO);
	glDeleteProgram(CarProgram);
	glDeleteProgram(BackwheelProgram);
	glDeleteProgram(FrontwheelProgram);
	glDeleteProgram(FrontWindowProgram);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}