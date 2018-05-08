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
Particle SmokeParticlesContainer[MaxParticles];
Particle RainParticlesContainer[MaxParticles];
Particle SplashParticlesContainer[MaxParticles];
int LastUsedSmokeParticle = 0;
int LastUsedRainParticle = 0;
int LastUsedSplashParticle = 0;

int FindUnusedSmokeParticle() {
	for (int i = LastUsedSmokeParticle; i < MaxParticles; i++) {
		if (SmokeParticlesContainer[i].life < 0) {
			LastUsedSmokeParticle = i;
			return i;
		}
	}

	for (int i = 0; i < LastUsedSmokeParticle; i++) {
		if (SmokeParticlesContainer[i].life < 0) {
			LastUsedSmokeParticle = i;
			return i;
		}
	}

	return 0;
}

int FindUnusedRainParticle() {
	for (int i = LastUsedRainParticle; i < MaxParticles; i++) {
		if (RainParticlesContainer[i].life < 0) {
			LastUsedRainParticle = i;
			return i;
		}
	}

	for (int i = 0; i < LastUsedRainParticle; i++) {
		if (RainParticlesContainer[i].life < 0) {
			LastUsedRainParticle = i;
			return i;
		}
	}

	return 0;
}

int FindUnusedSplashParticle() {
	for (int i = LastUsedSplashParticle; i < MaxParticles; i++) {
		if (SplashParticlesContainer[i].life < 0) {
			LastUsedSplashParticle = i;
			return i;
		}
	}

	for (int i = 0; i < LastUsedSplashParticle; i++) {
		if (SplashParticlesContainer[i].life < 0) {
			LastUsedSplashParticle = i;
			return i;
		}
	}

	return 0;
}

void SortSmokeParticles() {
	std::sort(&SmokeParticlesContainer[0], &SmokeParticlesContainer[MaxParticles]);
}

void SortRainParticles() {
	std::sort(&RainParticlesContainer[0], &RainParticlesContainer[MaxParticles]);
}

void SortSplashParticles() {
	std::sort(&SplashParticlesContainer[0], &SplashParticlesContainer[MaxParticles]);
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
		SmokeParticlesContainer[i].life = -1.0f;
		SmokeParticlesContainer[i].cameradistance = -1.0f;
	}
	static const GLfloat smoke_vertexes[] = {
		-0.1f, -0.1f, 0.0f,
		0.1f, -0.1f, 0.0f,
		-0.1f, 0.1f, 0.0f,
		0.1f, 0.1f, 0.0f,
	};

	static GLfloat* rain_position = new GLfloat[MaxParticles * 4];
	static GLubyte* rain_color = new GLubyte[MaxParticles * 4];
	for (int i = 0; i < MaxParticles; i++) {
		RainParticlesContainer[i].life = -1.0f;
		RainParticlesContainer[i].cameradistance = -1.0f;
	}
	static const GLfloat rain_vertexes[] = {
		-0.01f, -0.1f, 0.0f,
		0.01f, -0.1f, 0.0f,
		-0.01f, 0.1f, 0.0f,
		0.01f, 0.1f, 0.0f,
	};

	static GLfloat* splash_position = new GLfloat[MaxParticles * 4];
	static GLubyte* splash_color = new GLubyte[MaxParticles * 4];
	for (int i = 0; i < MaxParticles; i++) {
		SplashParticlesContainer[i].life = -1.0f;
		SplashParticlesContainer[i].cameradistance = -1.0f;
	}
	static const GLfloat splash_vertexes[] = {
		0.0f, 0.0f, 0.0f,
		-0.2f, 0.1f, 0.0f,
		0.2f, 0.1f, 0.0f,
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

	/* RAIN */
	// Create Vertex Array Object
	GLuint RainVAO;
	glGenVertexArrays(1, &RainVAO);
	glBindVertexArray(RainVAO);
	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint RainVBO;
	glGenBuffers(1, &RainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, RainVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rain_vertexes), rain_vertexes, GL_STATIC_DRAW);
	GLuint RainPositionVBO;
	glGenBuffers(1, &RainPositionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, RainPositionVBO);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	GLuint RainColorVBO;
	glGenBuffers(1, &RainColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, RainColorVBO);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

	/* SPLASH */
	// Create Vertex Array Object
	GLuint SplashVAO;
	glGenVertexArrays(1, &SplashVAO);
	glBindVertexArray(SplashVAO);
	// Create a Vertex Buffer Object and copy the vertex data to it
	GLuint SplashVBO;
	glGenBuffers(1, &SplashVBO);
	glBindBuffer(GL_ARRAY_BUFFER, SplashVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(splash_vertexes), splash_vertexes, GL_STATIC_DRAW);
	GLuint SplashPositionVBO;
	glGenBuffers(1, &SplashPositionVBO);
	glBindBuffer(GL_ARRAY_BUFFER, SplashPositionVBO);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	GLuint SplashColorVBO;
	glGenBuffers(1, &SplashColorVBO);
	glBindBuffer(GL_ARRAY_BUFFER, SplashColorVBO);
	glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

	// Create and compile our GLSL program from the shaders
	GLuint CarProgram = LoadShaders("CarVertexShader.vertexshader", "CarFragmentShader.fragmentshader");
	GLuint BackwheelProgram = LoadShaders("BackWheelVertexShader.vertexshader", "WheelFragmentShader.fragmentshader");
	GLuint FrontwheelProgram = LoadShaders("FrontWheelVertexShader.vertexshader", "WheelFragmentShader.fragmentshader");
	GLuint FrontWindowProgram = LoadShaders("FrontWindowVertexShader.vertexshader", "WindowFragmentShader.fragmentshader");
	GLuint GreyWindowProgram = LoadShaders("GreyWindowVertexShader.vertexshader", "GreyWindowFragmentShader.fragmentshader");
	GLuint SunProgram = LoadShaders("SunVertexShader.vertexshader", "SunFragmentShader.fragmentshader");
	GLuint SmokeProgram = LoadShaders("SmokeVertexShader.vertexshader", "SmokeFragmentShader.fragmentshader");
	GLuint RainProgram = LoadShaders("RainVertexShader.vertexshader", "RainFragmentShader.fragmentshader");
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
	GLuint RainCameraRightMatrix = glGetUniformLocation(RainProgram, "RainCameraRight");
	GLuint RainCameraUpMatrix = glGetUniformLocation(RainProgram, "RainCameraUp");
	GLuint RainVPMatrix = glGetUniformLocation(RainProgram, "RainVP");

	// Load the texture using any two methods
	GLuint Texture = loadBMP_custom("car.bmp");
	GLuint SmokeTexture = loadBMP_custom("smoke.bmp");
	GLuint RainTexture = loadBMP_custom("rain.bmp");
	// Get a handle for our "myTextureSampler" uniform
	GLuint TextureID = glGetUniformLocation(CarProgram, "carTextureSampler");
	GLuint SmokeTextureID = glGetUniformLocation(SmokeProgram, "smokeTextureSampler");
	GLuint RainTextureID = glGetUniformLocation(RainProgram, "rainTextureSampler");
	// Get a handle for our "LightPosition" uniform
	GLuint LightID = glGetUniformLocation(CarProgram, "LightPosition_worldspace");

	// Variables
	float angle = 0;
	double lastTime = glfwGetTime();
	double lastTimeFPS = glfwGetTime();
	int nbFrames = 0;

	do {
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_BLEND);

		// Count time
		double currentTime = glfwGetTime();
		double delta = currentTime - lastTime;
		lastTime = currentTime;

		// FPS
		nbFrames++;
		if (currentTime - lastTimeFPS >= 1.0) {
			printf("FPS : %f (%f ms/frame)\n", double(nbFrames), 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTimeFPS += 1.0; 
		}

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
		// Setup camera matrix
		computeMatricesFromInputs();
		glm::mat4 SmokeProjectionMatrix = getProjectionMatrix();
		glm::mat4 SmokeViewMatrix = getViewMatrix();
		glm::vec3 SmokeCameraPosition(glm::inverse(SmokeViewMatrix)[3]);
		glm::mat4 SmokeViewProjectionMatrix = SmokeProjectionMatrix * SmokeViewMatrix;
		// Generate 10 new particule each millisecond but limit to 60 fps
		int smokeNewparticles = (int)(delta*10000.0);
		if (smokeNewparticles > (int)(0.016f*10000.0)) {
			smokeNewparticles = (int)(0.016f*10000.0);
		}
		for (int i = 0; i < smokeNewparticles; i++) {
			int smokeParticleIndex = FindUnusedSmokeParticle();
			SmokeParticlesContainer[smokeParticleIndex].life = 0.2f;
			SmokeParticlesContainer[smokeParticleIndex].pos = glm::vec3(-0.9f, -0.4f, 0.0f);
			float smokeSpread = 1.5f;
			glm::vec3 smokeMaindir = glm::vec3(-10.0f, 0.0f, 0.0f);
			// Random direction
			glm::vec3 smokeRandomdir = glm::vec3(
				(rand() % 2000 - 1000.0f) / 1000.0f,
				(rand() % 2000 - 1000.0f) / 1000.0f,
				(rand() % 2000 - 1000.0f) / 1000.0f
			);
			SmokeParticlesContainer[smokeParticleIndex].speed = smokeMaindir + smokeRandomdir * smokeSpread;
			// Random color
			SmokeParticlesContainer[smokeParticleIndex].r = rand() % 256;
			SmokeParticlesContainer[smokeParticleIndex].g = rand() % 256;
			SmokeParticlesContainer[smokeParticleIndex].b = rand() % 256;
			SmokeParticlesContainer[smokeParticleIndex].a = (rand() % 256) / 3;
			SmokeParticlesContainer[smokeParticleIndex].size = (rand() % 1000) / 2000.0f + 0.1f;
		}
		// Simulate all particles
		int smokeParticlesCount = 0;
		for (int i = 0; i < MaxParticles; i++) {
			Particle& p = SmokeParticlesContainer[i];
			if (p.life > 0.0f) {
				// Decrease life
				p.life -= delta;
				if (p.life > 0.0f) {
					// Simulate simple physics : gravity only, no collisions
					p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)delta * 0.5f;
					p.pos += p.speed * (float)delta;
					p.cameradistance = glm::length2(p.pos - SmokeCameraPosition);
					// Fill the GPU buffer
					smoke_position[4 * smokeParticlesCount + 0] = p.pos.x;
					smoke_position[4 * smokeParticlesCount + 1] = p.pos.y;
					smoke_position[4 * smokeParticlesCount + 2] = p.pos.z;
					smoke_position[4 * smokeParticlesCount + 3] = p.size;
					smoke_color[4 * smokeParticlesCount + 0] = p.r;
					smoke_color[4 * smokeParticlesCount + 1] = p.g;
					smoke_color[4 * smokeParticlesCount + 2] = p.b;
					smoke_color[4 * smokeParticlesCount + 3] = p.a;
				}
				else {
					// Particles that just died will be put at the end of the buffer in SortParticles()
					p.cameradistance = -1.0f;
				}
				smokeParticlesCount++;
			}
		}
		SortSmokeParticles();
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
		glBufferSubData(GL_ARRAY_BUFFER, 0, smokeParticlesCount * sizeof(GLfloat) * 4, smoke_position);
		glBindBuffer(GL_ARRAY_BUFFER, SmokeColorVBO);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, smokeParticlesCount * sizeof(GLubyte) * 4, smoke_color);
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
		// Color object
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
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, smokeParticlesCount);

		/* RAIN */
		// Setup camera matrix
		computeMatricesFromInputs();
		glm::mat4 RainProjectionMatrix = getProjectionMatrix();
		glm::mat4 RainViewMatrix = getViewMatrix();
		glm::vec3 RainCameraPosition(glm::inverse(RainViewMatrix)[3]);
		glm::mat4 RainViewProjectionMatrix = RainProjectionMatrix * RainViewMatrix;
		// Generate 10 new particule each millisecond but limit to 60 fps
		int rainNewparticles = (int)(delta*10000.0);
		if (rainNewparticles > (int)(0.016f*10000.0)) {
			rainNewparticles = (int)(0.016f*10000.0);
		}
		for (int i = 0; i < rainNewparticles; i++) {
			int rainParticleIndex = FindUnusedRainParticle();
			RainParticlesContainer[rainParticleIndex].life = 0.2f;
			RainParticlesContainer[rainParticleIndex].pos = glm::vec3(0.0f, 2.0f, 0.0f);
			float rainSpread = 5.0f;
			glm::vec3 rainMaindir = glm::vec3(0.0f, -10.0f, 0.0f);
			// Random direction
			glm::vec3 rainRandomdir = glm::vec3(
				(rand() % 2000 - 1000.0f) / 1000.0f,
				(rand() % 2000 - 1000.0f) / 1000.0f,
				(rand() % 2000 - 1000.0f) / 1000.0f
			);
			RainParticlesContainer[rainParticleIndex].speed = rainMaindir + rainRandomdir * rainSpread;
			// Random color
			RainParticlesContainer[rainParticleIndex].r = rand() % 256;
			RainParticlesContainer[rainParticleIndex].g = rand() % 256;
			RainParticlesContainer[rainParticleIndex].b = rand() % 256;
			RainParticlesContainer[rainParticleIndex].a = (rand() % 256) / 3;
			RainParticlesContainer[rainParticleIndex].size = (rand() % 1000) / 2000.0f + 0.1f;
		}
		// Simulate all particles
		int rainParticlesCount = 0;
		for (int i = 0; i < MaxParticles; i++) {
			Particle& p = RainParticlesContainer[i];
			if (p.life > 0.0f) {
				// Decrease life
				p.life -= delta;
				if (p.life > 0.0f) {
					// Simulate simple physics : gravity only, no collisions
					p.speed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)delta * 0.5f;
					p.pos += p.speed * (float)delta;
					p.cameradistance = glm::length2(p.pos - RainCameraPosition);
					// Fill the GPU buffer
					rain_position[4 * rainParticlesCount + 0] = p.pos.x;
					rain_position[4 * rainParticlesCount + 1] = p.pos.y;
					rain_position[4 * rainParticlesCount + 2] = p.pos.z;
					rain_position[4 * rainParticlesCount + 3] = p.size;
					rain_color[4 * rainParticlesCount + 0] = p.r;
					rain_color[4 * rainParticlesCount + 1] = p.g;
					rain_color[4 * rainParticlesCount + 2] = p.b;
					rain_color[4 * rainParticlesCount + 3] = p.a;
					// Collision
					if (p.pos.x >= -0.9f && p.pos.x <= 0.9f && p.pos.y >= 0.55f && p.pos.y <= 0.65f && p.pos.z >= -0.5f && p.pos.z <= 0.5f) {
						int splashParticleIndex = FindUnusedSplashParticle();
						SplashParticlesContainer[splashParticleIndex].life = 0.1f;
						SplashParticlesContainer[splashParticleIndex].pos = glm::vec3(p.pos.x, p.pos.y, p.pos.z);
						SplashParticlesContainer[splashParticleIndex].speed = glm::vec3(0.0f, 0.0f, 0.0f);
						// Random color
						SplashParticlesContainer[splashParticleIndex].r = rand() % 256;
						SplashParticlesContainer[splashParticleIndex].g = rand() % 256;
						SplashParticlesContainer[splashParticleIndex].b = rand() % 256;
						SplashParticlesContainer[splashParticleIndex].a = (rand() % 256) / 3;
						SplashParticlesContainer[splashParticleIndex].size = (rand() % 1000) / 2000.0f + 0.1f;
					}
				}
				else {
					// Particles that just died will be put at the end of the buffer in SortParticles()
					p.cameradistance = -1.0f;
				}
				rainParticlesCount++;
			}
		}
		SortRainParticles();
		// Use our shader
		glUseProgram(RainProgram);
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, RainTexture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(RainTextureID, 0);
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniform3f(RainCameraRightMatrix, RainViewMatrix[0][0], RainViewMatrix[1][0], RainViewMatrix[2][0]);
		glUniform3f(RainCameraUpMatrix, RainViewMatrix[0][1], RainViewMatrix[1][1], RainViewMatrix[2][1]);
		glUniformMatrix4fv(RainVPMatrix, 1, GL_FALSE, &RainViewProjectionMatrix[0][0]);
		// Draw object
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindBuffer(GL_ARRAY_BUFFER, RainPositionVBO);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, rainParticlesCount * sizeof(GLfloat) * 4, rain_position);
		glBindBuffer(GL_ARRAY_BUFFER, RainColorVBO);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, rainParticlesCount * sizeof(GLubyte) * 4, rain_color);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, RainVBO);
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
		glBindBuffer(GL_ARRAY_BUFFER, RainPositionVBO);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : x + y + z + size => 4
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
		// Color object
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, RainColorVBO);
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
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, rainParticlesCount);

		/* SPLASH */
		// Simulate all particles
		int splashParticlesCount = 0;
		for (int i = 0; i < MaxParticles; i++) {
			Particle& p = SplashParticlesContainer[i];
			if (p.life > 0.0f) {
				// Decrease life
				p.life -= delta;
				if (p.life > 0.0f) {
					p.cameradistance = glm::length2(p.pos - RainCameraPosition);
					// Fill the GPU buffer
					splash_position[4 * splashParticlesCount + 0] = p.pos.x;
					splash_position[4 * splashParticlesCount + 1] = p.pos.y;
					splash_position[4 * splashParticlesCount + 2] = p.pos.z;
					splash_position[4 * splashParticlesCount + 3] = p.size;
					splash_color[4 * splashParticlesCount + 0] = p.r;
					splash_color[4 * splashParticlesCount + 1] = p.g;
					splash_color[4 * splashParticlesCount + 2] = p.b;
					splash_color[4 * splashParticlesCount + 3] = p.a;
				}
				else {
					// Particles that just died will be put at the end of the buffer in SortParticles()
					p.cameradistance = -1.0f;
				}
				splashParticlesCount++;
			}
		}
		SortSplashParticles();
		// Use our shader
		glUseProgram(RainProgram);
		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, RainTexture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(RainTextureID, 0);
		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniform3f(RainCameraRightMatrix, RainViewMatrix[0][0], RainViewMatrix[1][0], RainViewMatrix[2][0]);
		glUniform3f(RainCameraUpMatrix, RainViewMatrix[0][1], RainViewMatrix[1][1], RainViewMatrix[2][1]);
		glUniformMatrix4fv(RainVPMatrix, 1, GL_FALSE, &RainViewProjectionMatrix[0][0]);
		// Draw object
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindBuffer(GL_ARRAY_BUFFER, SplashPositionVBO);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, splashParticlesCount * sizeof(GLfloat) * 4, splash_position);
		glBindBuffer(GL_ARRAY_BUFFER, SplashColorVBO);
		glBufferData(GL_ARRAY_BUFFER, MaxParticles * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, splashParticlesCount * sizeof(GLubyte) * 4, splash_color);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, SplashVBO);
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
		glBindBuffer(GL_ARRAY_BUFFER, SplashPositionVBO);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			4,                                // size : x + y + z + size => 4
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);
		// Color object
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, SplashColorVBO);
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
		glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, splashParticlesCount);

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
	glDeleteBuffers(1, &SmokeVBO);
	glDeleteBuffers(1, &SmokePositionVBO);
	glDeleteBuffers(1, &SmokeColorVBO);
	glDeleteBuffers(1, &RainVBO);
	glDeleteBuffers(1, &RainPositionVBO);
	glDeleteBuffers(1, &RainColorVBO);
	glDeleteBuffers(1, &SplashVBO);
	glDeleteBuffers(1, &SplashPositionVBO);
	glDeleteBuffers(1, &SplashColorVBO);
	glDeleteProgram(CarProgram);
	glDeleteProgram(BackwheelProgram);
	glDeleteProgram(FrontwheelProgram);
	glDeleteProgram(FrontWindowProgram);
	glDeleteProgram(GreyWindowProgram);
	glDeleteProgram(SunProgram);
	glDeleteProgram(SmokeProgram);
	glDeleteProgram(RainProgram);
	glDeleteTextures(1, &Texture);
	glDeleteTextures(1, &SmokeTexture);
	glDeleteTextures(1, &RainTexture);
	glDeleteVertexArrays(1, &CarVAO);
	glDeleteVertexArrays(1, &BackwheelVAO);
	glDeleteVertexArrays(1, &FrontwheelVAO);
	glDeleteVertexArrays(1, &jendelaBelakangVAO);
	glDeleteVertexArrays(1, &jendelaBelakangGreyVAO);
	glDeleteVertexArrays(1, &SunVAO);
	glDeleteVertexArrays(1, &SmokeVAO);
	glDeleteVertexArrays(1, &RainVAO);
	glDeleteVertexArrays(1, &SplashVAO);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}