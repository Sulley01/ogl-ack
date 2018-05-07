// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <algorithm>

// Include libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/norm.hpp>
#include <common/shader.hpp>
#include <common/controls.hpp>
#include <common/texture.hpp>

// Global variables
GLFWwindow* window;

struct Particle {
	glm::vec3 pos, speed;
	unsigned char r, g, b, a;
	float size, angle, weight;
	float life;
	float cameradistance;

	bool operator<(const Particle& that) const {
		// Sort in reverse order : far particles drawn first.
		return this->cameradistance > that.cameradistance;
	}
};

const int MaxParticles = 100000;
Particle ParticlesContainer[MaxParticles];
int LastUsedParticle = 0;

int FindUnusedParticle() {
	for (int i = LastUsedParticle; i < MaxParticles; i++) {
		if (ParticlesContainer[i].life < 0) {
			LastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < LastUsedParticle; i++) {
		if (ParticlesContainer[i].life < 0) {
			LastUsedParticle = i;
			return i;
		}
	}

	return 0;
}

void SortParticles() {
	std::sort(&ParticlesContainer[0], &ParticlesContainer[MaxParticles]);
}

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

	glEnable(GL_DEPTH_TEST);

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
		-0.85f, 0.2f, 0 - half_car_width - 0.01f,
		-0.75f, 0.2f, 0 - half_car_width - 0.01f,
		-0.75f, 0.55f, 0 - half_car_width - 0.01f,
		0.2f, 0.55f, 0 - half_car_width - 0.01f,
		0.2f, 0.2f, 0 - half_car_width - 0.01f,
		0.4f, 0.2f, 0 - half_car_width - 0.01f,
		-0.7f, 0.55f, 0 - half_car_width - 0.01f,
		0.15f, 0.55f, 0 - half_car_width - 0.01f,
		0.15f, 0.2f, 0 - half_car_width - 0.01f,
		-0.7f, 0.2f, 0 - half_car_width - 0.01f,
		// Kaca Depan
		0.275f, 0.55f, 0.25f,
		0.275f, 0.55f, -0.25f,
		0.775f, 0.2f, -0.25f,
		0.775f, 0.2f, 0.25f,
		// Kaca Belakang
		-0.815f, 0.55f, 0.25f,
		-0.91f, 0.2f, 0.25f,
		-0.91f, 0.2f, -0.25f,
		-0.815f, 0.55f, -0.25f
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
		-0.6f, -0.2f, half_car_width + 0.05f,
		-0.45f, -0.25f, half_car_width + 0.05f,
		-0.4f, -0.4f, half_car_width + 0.05f,
		-0.45f, -0.55f, half_car_width + 0.05f,
		-0.6f, -0.6f, half_car_width + 0.05f,
		-0.75f, -0.55f, half_car_width + 0.05f,
		-0.8f, -0.4f, half_car_width + 0.05f,
		-0.75f, -0.25f, half_car_width + 0.05f,
		// Z+ in
		-0.6f, -0.2f, half_car_width - half_wheel_width + 0.05f,
		-0.45f, -0.25f, half_car_width - half_wheel_width + 0.05f,
		-0.4f, -0.4f, half_car_width - half_wheel_width + 0.05f,
		-0.45f, -0.55f, half_car_width - half_wheel_width + 0.05f,
		-0.6f, -0.6f, half_car_width - half_wheel_width + 0.05f,
		-0.75f, -0.55f, half_car_width - half_wheel_width + 0.05f,
		-0.8f, -0.4f, half_car_width - half_wheel_width + 0.05f,
		-0.75f, -0.25f, half_car_width - half_wheel_width + 0.05f,
		// Z- out
		-0.6f, -0.2f, 0 - half_car_width - 0.05f,
		-0.45f, -0.25f, 0 - half_car_width - 0.05f,
		-0.4f, -0.4f, 0 - half_car_width - 0.05f,
		-0.45f, -0.55f, 0 - half_car_width - 0.05f,
		-0.6f, -0.6f, 0 - half_car_width - 0.05f,
		-0.75f, -0.55f, 0 - half_car_width - 0.05f,
		-0.8f, -0.4f, 0 - half_car_width - 0.05f,
		-0.75f, -0.25f, 0 - half_car_width - 0.05f,
		// Z- in
		-0.6f, -0.2f, 0 - half_car_width + half_wheel_width - 0.05f,
		-0.45f, -0.25f, 0 - half_car_width + half_wheel_width - 0.05f,
		-0.4f, -0.4f, 0 - half_car_width + half_wheel_width - 0.05f,
		-0.45f, -0.55f, 0 - half_car_width + half_wheel_width - 0.05f,
		-0.6f, -0.6f, 0 - half_car_width + half_wheel_width - 0.05f,
		-0.75f, -0.55f, 0 - half_car_width + half_wheel_width - 0.05f,
		-0.8f, -0.4f, 0 - half_car_width + half_wheel_width - 0.05f,
		-0.75f, -0.25f, 0 - half_car_width + half_wheel_width - 0.05f
	};
	GLfloat frontwheel_vertexes[] = {
		// Z+ out
		0.6f, -0.2f, half_car_width + 0.05f,
		0.45f, -0.25f, half_car_width + 0.05f,
		0.4f, -0.4f, half_car_width + 0.05f,
		0.45f, -0.55f, half_car_width + 0.05f,
		0.6f, -0.6f, half_car_width + 0.05f,
		0.75f, -0.55f, half_car_width + 0.05f,
		0.8f, -0.4f, half_car_width + 0.05f,
		0.75f, -0.25f, half_car_width + 0.05f,
		// Z+ in
		0.6f, -0.2f, half_car_width - half_wheel_width + 0.05f,
		0.45f, -0.25f, half_car_width - half_wheel_width + 0.05f,
		0.4f, -0.4f, half_car_width - half_wheel_width + 0.05f,
		0.45f, -0.55f, half_car_width - half_wheel_width + 0.05f,
		0.6f, -0.6f, half_car_width - half_wheel_width + 0.05f,
		0.75f, -0.55f, half_car_width - half_wheel_width + 0.05f,
		0.8f, -0.4f, half_car_width - half_wheel_width + 0.05f,
		0.75f, -0.25f, half_car_width - half_wheel_width + 0.05f,
		// Z- out
		0.6f, -0.2f, 0 - half_car_width - 0.05f,
		0.45f, -0.25f, 0 - half_car_width - 0.05f,
		0.4f, -0.4f, 0 - half_car_width - 0.05f,
		0.45f, -0.55f, 0 - half_car_width - 0.05f,
		0.6f, -0.6f, 0 - half_car_width - 0.05f,
		0.75f, -0.55f, 0 - half_car_width - 0.05f,
		0.8f, -0.4f, 0 - half_car_width - 0.05f,
		0.75f, -0.25f, 0 - half_car_width - 0.05f,
		// Z- in
		0.6f, -0.2f, 0 - half_car_width + half_wheel_width - 0.05f,
		0.45f, -0.25f, 0 - half_car_width + half_wheel_width - 0.05f,
		0.4f, -0.4f, 0 - half_car_width + half_wheel_width - 0.05f,
		0.45f, -0.55f, 0 - half_car_width + half_wheel_width - 0.05f,
		0.6f, -0.6f, 0 - half_car_width + half_wheel_width - 0.05f,
		0.75f, -0.55f, 0 - half_car_width + half_wheel_width - 0.05f,
		0.8f, -0.4f, 0 - half_car_width + half_wheel_width - 0.05f,
		0.75f, -0.25f, 0 - half_car_width + half_wheel_width - 0.05f
	};
	GLfloat sun_dimension = 0.1f;
	GLfloat sun_vertexes[] = {
		-1 * sun_dimension, -1 * sun_dimension, sun_dimension,
		-1 * sun_dimension, sun_dimension, sun_dimension,
		sun_dimension, sun_dimension, sun_dimension,
		sun_dimension, -1 * sun_dimension, sun_dimension,

		-1 * sun_dimension, -1 * sun_dimension, -1 * sun_dimension,
		-1 * sun_dimension, sun_dimension, -1 * sun_dimension,
		sun_dimension, sun_dimension, -1 * sun_dimension,
		sun_dimension, -1 * sun_dimension, -1 * sun_dimension,
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
		11, 6, 0
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
		6,8,9
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
	GLuint sun_elements[] = {
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		0, 1, 4,
		1, 2, 5,
		2, 3, 6,
		3, 4, 7,

		4, 5, 1,
		5, 6, 2,
		6, 7, 3,
		7, 4, 0
	};
	GLfloat car_uv[] = {
		0.1f, 0.1f,
		0.1f, 0.58f,
		0.157f, 0.9f,
		0.443f, 0.9f,
		0.9f, 0.5f,
		0.9f, 0.1f,

		0.1f, 0.1f,
		0.1f, 0.58f,
		0.157f, 0.9f,
		0.443f, 0.9f,
		0.9f, 0.5f,
		0.9f, 0.1f
	};
	GLfloat car_n[] = {
		-1 / sqrt(3), -1 / sqrt(3), 1 / sqrt(3),
		-0.88638, 0.109119, 0.449908,
		0.611748, 0.477639, 0.630576,
		0.270198, 0.843144, 0.464867,
		0.308923, -0.6003, 0.737706,
		1 / sqrt(3), -1 / sqrt(3), 1 / sqrt(3),

		-1 / sqrt(3), -1 / sqrt(3), -1 / sqrt(3),
		-0.88638, 0.109119, -0.449908,
		0.611748, 0.477639, -0.630576,
		0.270198, 0.843144, -0.464867,
		0.308923, -0.6003, -0.737706,
		1 / sqrt(3), -1 / sqrt(3), -1 / sqrt(3)
	};

	static GLfloat* smoke_position = new GLfloat[MaxParticles * 4];
	static GLubyte* smoke_color = new GLubyte[MaxParticles * 4];
	for (int i = 0; i < MaxParticles; i++) {
		ParticlesContainer[i].life = -1.0f;
		ParticlesContainer[i].cameradistance = -1.0f;
	}
	static const GLfloat smoke_vertexes[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f,  0.5f, 0.0f,
		0.5f,  0.5f, 0.0f,
	};

	/* CAR */
	// Create Vertex Array Object
	GLuint CarVAO;
	glGenVertexArrays(1, &CarVAO);
	glBindVertexArray(CarVAO);
	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint CarVBO;
	glGenBuffers(1, &CarVBO);
	glBindBuffer(GL_ARRAY_BUFFER, CarVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(car_vertexes), car_vertexes, GL_STATIC_DRAW);
	GLuint CarUV;
	glGenBuffers(1, &CarUV);
	glBindBuffer(GL_ARRAY_BUFFER, CarUV);
	glBufferData(GL_ARRAY_BUFFER, sizeof(car_uv), car_uv, GL_STATIC_DRAW);
	GLuint CarN;
	glGenBuffers(1, &CarN);
	glBindBuffer(GL_ARRAY_BUFFER, CarN);
	glBufferData(GL_ARRAY_BUFFER, sizeof(car_n), car_n, GL_STATIC_DRAW);
	// Create an element array
	GLuint CarEBO;
	glGenBuffers(1, &CarEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CarEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(car_elements), car_elements, GL_STATIC_DRAW);

	/* FRONT WINDOW */
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

	/* GREY WINDOW*/
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

	/* BACK WHEEL */
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

	/* FRONT WHEEL */
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

	/* SUN */
	// Create Vertex Array Object
	GLuint SunVAO;
	glGenVertexArrays(1, &SunVAO);
	glBindVertexArray(SunVAO);
	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint SunVBO;
	glGenBuffers(1, &SunVBO);
	glBindBuffer(GL_ARRAY_BUFFER, SunVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sun_vertexes), sun_vertexes, GL_STATIC_DRAW);
	// Create an element array
	GLuint SunEBO;
	glGenBuffers(1, &SunEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SunEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sun_elements), sun_elements, GL_STATIC_DRAW);

	/* SMOKE */
	// Create Vertex Array Object
	GLuint SmokeVAO;
	glGenVertexArrays(1, &SmokeVAO);
	glBindVertexArray(SmokeVAO);
	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint SmokeVBO;
	glGenBuffers(1, &SmokeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, SmokeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(smoke_vertexes), smoke_vertexes, GL_STATIC_DRAW);
	GLuint SmokePositionVBO;
	glGenBuffers(1, &SmokePositionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, SmokePositionVBO);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	GLuint SmokeColorVBO;
	glGenBuffers(1, &SmokeColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, SmokeColorVBO);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

	// Create and compile our GLSL program from the shaders
	GLuint CarProgram = LoadShaders("CarVertexShader.vertexshader", "CarFragmentShader.fragmentshader");
	GLuint BackwheelProgram = LoadShaders("BackWheelVertexShader.vertexshader", "WheelFragmentShader.fragmentshader");
	GLuint FrontwheelProgram = LoadShaders("FrontWheelVertexShader.vertexshader", "WheelFragmentShader.fragmentshader");
	GLuint FrontWindowProgram = LoadShaders("FrontWindowVertexShader.vertexshader", "WindowFragmentShader.fragmentshader");
	GLuint GreyWindowProgram = LoadShaders("GreyWindowVertexShader.vertexshader", "GreyWindowFragmentShader.fragmentshader");
	GLuint SunProgram = LoadShaders("SunVertexShader.vertexshader", "SunFragmentShader.fragmentshader");
	GLuint SmokeProgram = LoadShaders("SmokeVertexShader.vertexshader", "SmokeFragmentShader.fragmentshader");
	// Get a handle for our "MVP" uniform
	GLuint CarCameraMatrix = glGetUniformLocation(CarProgram, "CarCameraMVP");
	GLuint CarViewMatrix = glGetUniformLocation(CarProgram, "CarCameraV");
	GLuint CarModelMatrix = glGetUniformLocation(CarProgram, "CarCameraM");
	GLuint BackwheelRotationMatrix = glGetUniformLocation(BackwheelProgram, "BackwheelRotationMVP");
	GLuint BackwheelCameraMatrix = glGetUniformLocation(BackwheelProgram, "BackwheelCameraMVP");
	GLuint FrontwheelRotationMatrix = glGetUniformLocation(FrontwheelProgram, "FrontwheelRotationMVP");
	GLuint FrontwheelCameraMatrix = glGetUniformLocation(FrontwheelProgram, "FrontwheelCameraMVP");
	GLuint FrontWindowCameraMatrix = glGetUniformLocation(FrontWindowProgram, "FrontWindowCameraMVP");
	GLuint GreyWindowCameraMatrix = glGetUniformLocation(GreyWindowProgram, "GreyWindowCameraMVP");
	GLuint SunMatrix = glGetUniformLocation(SunProgram, "SunMVP");
	GLuint SunCameraMatrix = glGetUniformLocation(SunProgram, "SunCameraMVP");
	GLuint SmokeCameraRightMatrix = glGetUniformLocation(SmokeProgram, "SmokeCameraRight");
	GLuint SmokeCameraUpMatrix = glGetUniformLocation(SmokeProgram, "SmokeCameraUp");
	GLuint SmokeVPMatrix = glGetUniformLocation(SmokeProgram, "SmokeVP");

	// Load the texture using any two methods
	GLuint Texture = loadBMP_custom("car.bmp");
	GLuint SmokeTexture = loadBMP_custom("smoke.bmp");
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(CarProgram, "carTextureSampler");
	GLuint SmokeTextureID = glGetUniformLocation(SmokeProgram, "smokeTextureSampler");
	// Get a handle for our "LightPosition" uniform
	GLuint LightID = glGetUniformLocation(CarProgram, "LightPosition_worldspace");

	// Variables
	float angle = 0;
	double lastTime = glfwGetTime();

	do {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

		// Light
		GLint lightPosX = 1;
		GLint lightPosY = 1;
		GLint lightPosZ = 1;
		glm::vec3 lightPos = glm::vec3(lightPosX, lightPosY, lightPosZ);

		// MVP sun
		glm::mat4 SunRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 SunMVP = SunRotation * glm::translate(glm::mat4(1.0f), lightPos);

		// Update lightPos
		lightPos = glm::vec3(SunMVP[0][0], SunMVP[1][1], SunMVP[2][2]);

		/* CAR */
		// Use our shader
		glUseProgram(CarProgram);
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(CarCameraMatrix, 1, GL_FALSE, &CameraMVP[0][0]);
		glUniformMatrix4fv(CarViewMatrix, 1, GL_FALSE, &CameraView[0][0]);
		glUniformMatrix4fv(CarModelMatrix, 1, GL_FALSE, &CameraModel[0][0]);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);
		// Draw object
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
		// Texture object
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, CarUV);
		glVertexAttribPointer(
			1,					// attribute. No particular reason for 1, but must match the layout in the shader.
			2,					// size : U+V => 2
			GL_FLOAT,			// type
			GL_FALSE,			// normalized?
			0,					// stride
			(void*)0			// array buffer offset
		);
		// Light object
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, CarN);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CarEBO);
		glDrawElements(GL_TRIANGLES, sizeof(car_elements), GL_UNSIGNED_INT, 0);

		/* BACK WHEEL */
		// Use our shader
		glUseProgram(BackwheelProgram);
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(BackwheelRotationMatrix, 1, GL_FALSE, &BackwheelMVP[0][0]);
		glUniformMatrix4fv(BackwheelCameraMatrix, 1, GL_FALSE, &CameraMVP[0][0]);
		// Draw object
		glBindVertexArray(BackwheelVAO);
		glBindBuffer(GL_ARRAY_BUFFER, BackwheelVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(backwheel_vertexes), backwheel_vertexes, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BackwheelEBO);
		glDrawElements(GL_TRIANGLES, sizeof(backwheel_elements), GL_UNSIGNED_INT, 0);

		/* GREY WINDOW */
		glUseProgram(GreyWindowProgram);
		glUniformMatrix4fv(GreyWindowCameraMatrix, 1, GL_FALSE, &CameraMVP[0][0]);
		glBindVertexArray(jendelaBelakangGreyVAO);
		glBindBuffer(GL_ARRAY_BUFFER, jendelaBelakangGreyVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(window_grey), window_grey, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, jendelaBelakangGreyEBO);
		glDrawElements(GL_TRIANGLES, sizeof(greyWindowElements), GL_UNSIGNED_INT, 0);

		/* FRONT WINDOW */
		glUseProgram(FrontWindowProgram);
		glUniformMatrix4fv(FrontWindowCameraMatrix, 1, GL_FALSE, &CameraMVP[0][0]);
		glBindVertexArray(jendelaBelakangVAO);
		glBindBuffer(GL_ARRAY_BUFFER, jendelaBelakangVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(front_window_vertexes), front_window_vertexes, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, jendelaBelakangEBO);
		glDrawElements(GL_TRIANGLES, sizeof(backWindowElements), GL_UNSIGNED_INT, 0);

		/* FRONT WHEEL */
		// Use our shader
		glUseProgram(FrontwheelProgram);
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(FrontwheelRotationMatrix, 1, GL_FALSE, &FrontwheelMVP[0][0]);
		glUniformMatrix4fv(FrontwheelCameraMatrix, 1, GL_FALSE, &CameraMVP[0][0]);
		// Draw object
		glBindVertexArray(FrontwheelVAO);
		glBindBuffer(GL_ARRAY_BUFFER, FrontwheelVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(frontwheel_vertexes), frontwheel_vertexes, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FrontwheelEBO);
		glDrawElements(GL_TRIANGLES, sizeof(frontwheel_elements), GL_UNSIGNED_INT, 0);

		/* SUN */
		// Use our shader
		glUseProgram(SunProgram);
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(SunMatrix, 1, GL_FALSE, &SunMVP[0][0]);
		glUniformMatrix4fv(SunCameraMatrix, 1, GL_FALSE, &CameraMVP[0][0]);
		// Draw object
		glBindVertexArray(SunVAO);
		glBindBuffer(GL_ARRAY_BUFFER, SunVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(sun_vertexes), sun_vertexes, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SunEBO);
		glDrawElements(GL_TRIANGLES, sizeof(sun_elements), GL_UNSIGNED_INT, 0);

		/* SMOKE */
		// Count time
		double currentTime = glfwGetTime();
		double delta = currentTime - lastTime;
		lastTime = currentTime;
		// Setup camera matrix
		computeMatricesFromInputs();
		glm::mat4 SmokeProjectionMatrix = getProjectionMatrix();
		glm::mat4 SmokeViewMatrix = getViewMatrix();
		glm::vec3 CameraPosition(glm::inverse(SmokeViewMatrix)[3]);
		glm::mat4 SmokeViewProjectionMatrix = SmokeProjectionMatrix * SmokeViewMatrix;
		// Generate 10 new particule each millisecond but limit to 60 fps
		int newparticles = (int)(delta*10000.0);
		if (newparticles > (int)(0.016f*10000.0)) {
			newparticles = (int)(0.016f*10000.0);
		}
		for (int i = 0; i<newparticles; i++) {
			int particleIndex = FindUnusedParticle();
			ParticlesContainer[particleIndex].life = 5.0f;
			ParticlesContainer[particleIndex].pos = glm::vec3(0, 0, -20.0f);
			float spread = 1.5f;
			glm::vec3 maindir = glm::vec3(0.0f, 10.0f, 0.0f);
			// Random direction
			glm::vec3 randomdir = glm::vec3(
				(rand() % 2000 - 1000.0f) / 1000.0f,
				(rand() % 2000 - 1000.0f) / 1000.0f,
				(rand() % 2000 - 1000.0f) / 1000.0f
			);
			ParticlesContainer[particleIndex].speed = maindir + randomdir * spread;
			// Random color
			ParticlesContainer[particleIndex].r = rand() % 256;
			ParticlesContainer[particleIndex].g = rand() % 256;
			ParticlesContainer[particleIndex].b = rand() % 256;
			ParticlesContainer[particleIndex].a = (rand() % 256) / 3;
			ParticlesContainer[particleIndex].size = (rand() % 1000) / 2000.0f + 0.1f;
		}
		// Simulate all particles
		int ParticlesCount = 0;
		for (int i = 0; i < MaxParticles; i++) {
			Particle& p = ParticlesContainer[i];
			if (p.life > 0.0f) {
				// Decrease life
				p.life -= delta;
				if (p.life > 0.0f) {
					// Simulate simple physics : gravity only, no collisions
					p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)delta * 0.5f;
					p.pos += p.speed * (float)delta;
					p.cameradistance = glm::length2(p.pos - CameraPosition);
					// Fill the GPU buffer
					smoke_position[4 * ParticlesCount + 0] = p.pos.x;
					smoke_position[4 * ParticlesCount + 1] = p.pos.y;
					smoke_position[4 * ParticlesCount + 2] = p.pos.z;
					smoke_position[4 * ParticlesCount + 3] = p.size;
					smoke_color[4 * ParticlesCount + 0] = p.r;
					smoke_color[4 * ParticlesCount + 1] = p.g;
					smoke_color[4 * ParticlesCount + 2] = p.b;
					smoke_color[4 * ParticlesCount + 3] = p.a;
				}
				else {
					// Particles that just died will be put at the end of the buffer in SortParticles()
					p.cameradistance = -1.0f;
				}
				ParticlesCount++;
			}
		}
		SortParticles();
		// Use our shader
		glUseProgram(SmokeProgram);
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, SmokeTexture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(SmokeTextureID, 0);
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniform3f(SmokeCameraRightMatrix, SmokeViewMatrix[0][0], SmokeViewMatrix[1][0], SmokeViewMatrix[2][0]);
		glUniform3f(SmokeCameraUpMatrix, SmokeViewMatrix[0][1], SmokeViewMatrix[1][1], SmokeViewMatrix[2][1]);
		glUniformMatrix4fv(SmokeVPMatrix, 1, GL_FALSE, &SmokeViewProjectionMatrix[0][0]);
		// Draw object
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindBuffer(GL_ARRAY_BUFFER, SmokePositionVBO);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLfloat) * 4, smoke_position);
		glBindBuffer(GL_ARRAY_BUFFER, SmokeColorVBO);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCount * sizeof(GLubyte) * 4, smoke_color);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, SmokeVBO);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		// Position object
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, SmokePositionVBO);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : x + y + z + size => 4
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : particles' colors
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, SmokeColorVBO);
		glVertexAttribPointer(
			2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : r + g + b + a => 4
			GL_UNSIGNED_BYTE,                 // type
			GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
			0,                                // stride
			(void*)0                          // array buffer offset
		);
		glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
		glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
		glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCount);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Rotate
		angle -= 1.0f;

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &CarEBO);
	glDeleteBuffers(1, &BackwheelEBO);
	glDeleteBuffers(1, &FrontwheelEBO);
	glDeleteBuffers(1, &jendelaBelakangEBO);
	glDeleteBuffers(1, &jendelaBelakangGreyEBO);
	glDeleteBuffers(1, &SunEBO);
	glDeleteBuffers(1, &CarVBO);
	glDeleteBuffers(1, &CarUV);
	glDeleteBuffers(1, &CarN);
	glDeleteBuffers(1, &BackwheelVBO);
	glDeleteBuffers(1, &FrontwheelVBO);
	glDeleteBuffers(1, &jendelaBelakangVBO);
	glDeleteBuffers(1, &jendelaBelakangGreyVBO);
	glDeleteBuffers(1, &SunVBO);
	glDeleteProgram(CarProgram);
	glDeleteProgram(BackwheelProgram);
	glDeleteProgram(FrontwheelProgram);
	glDeleteProgram(FrontWindowProgram);
	glDeleteProgram(GreyWindowProgram);
	glDeleteProgram(SunProgram);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &CarVAO);
	glDeleteVertexArrays(1, &BackwheelVAO);
	glDeleteVertexArrays(1, &FrontwheelVAO);
	glDeleteVertexArrays(1, &jendelaBelakangVAO);
	glDeleteVertexArrays(1, &jendelaBelakangGreyVAO);
	glDeleteVertexArrays(1, &SunVAO);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}