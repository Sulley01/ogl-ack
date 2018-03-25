#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

void drawCircle(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLint numberOfSides)
{
	int i;

	GLfloat twicePi = 2.0f * M_PI;

	glPushMatrix();
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y); // center of circle
	for (i = 0; i <= numberOfSides; i++) {
		glVertex2f(
			x + (radius * cos(i * twicePi / numberOfSides)),
			y + (radius * sin(i * twicePi / numberOfSides))
		);
	}
	glEnd();
	glPopMatrix();
}

int main(void)
{
	GLFWwindow *window;

	// Initialize the library
	if (!glfwInit())
	{
		return -1;
	}

	// Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Immediate Car", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(window);

	glViewport(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT); // specifies the part of the window to which OpenGL will draw (in pixels), convert from normalised to pixels
	glMatrixMode(GL_PROJECTION); // projection matrix defines the properties of the camera that views the objects in the world coordinate frame. Here you typically set the zoom factor, aspect ratio and the near and far clipping planes
	glLoadIdentity(); // replace the current matrix with the identity matrix and starts us a fresh because matrix transforms such as glOrpho and glRotate cumulate, basically puts us at (0, 0, 0)
	glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1); // essentially set coordinate system
	glMatrixMode(GL_MODELVIEW); // (default matrix mode) modelview matrix defines how your objects are transformed (meaning translation, rotation and scaling) in your world
	glLoadIdentity(); // same as above comment

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // polygon drawing mode (GL_POINT, GL_LINE, GL_FILL)
	
	float angle = 0;

	// Loop until the user closes the window
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		
		// Car

		glLoadIdentity();

		glRotatef(0.0, 0, 0, 1);
		
		glColor3f(0.75, 0.75, 0.75);
		glPushMatrix();
		glBegin(GL_POLYGON);
		glVertex3f(51.2, 230.4, 0);
		glVertex3f(51.2, 460.8, 0);
		glVertex3f(102.4, 614.4, 0);
		glVertex3f(614.4, 614.4, 0);
		glVertex3f(972.8, 422.4, 0);
		glVertex3f(972.8, 230.4, 0);
		glEnd();
		glPopMatrix();

		// Back wheel

		glLoadIdentity();

		glTranslatef(183.6, 230.4, 0);
		glRotatef(angle, 0, 0, 1);
		glTranslatef(-183.6, -230.4, 0);

		glColor3f(0.25, 0.25, 0.25);
		drawCircle(183.6, 230.4, 0, 102.4, 30);
		glColor3f(0, 0, 0);
		drawCircle(153.6, 230.4, 0, 10, 30);
		drawCircle(183.6, 260.4, 0, 10, 30);
		drawCircle(213.6, 230.4, 0, 10, 30);
		drawCircle(183.6, 200.4, 0, 10, 30);

		glLoadIdentity();

		glTranslatef(840.4, 230.4, 0);
		glRotatef(angle, 0, 0, 1);
		glTranslatef(-840.4, -230.4, 0);

		glColor3f(0.25, 0.25, 0.25);
		drawCircle(840.4, 230.4, 0, 102.4, 30);
		glColor3f(0, 0, 0);
		drawCircle(810.4, 230.4, 0, 10, 30);
		drawCircle(840.4, 260.4, 0, 10, 30);
		drawCircle(870.4, 230.4, 0, 10, 30);
		drawCircle(840.4, 200.4, 0, 10, 30);

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();

		angle-=0.1;
	}

	glfwTerminate();

	return 0;
}