#include <GL/glut.h>
#include <stdio.h>  
#include <stdlib.h>
#include <time.h>   
#include <iostream>

using namespace std;

int windowWidth, windowHeight;

void init() {
	windowWidth = 640; windowHeight = 480;
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(10, 10);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutCreateWindow("Simple NURBS");
	glEnable(GL_DEPTH_TEST);
}

GLUnurbsObj *nurbs;

// resize
void resize(int width, int height) {
	windowWidth = width; windowHeight = height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (GLfloat)width / (GLfloat)height, 1.0f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_FLAT);
	glEnable(GL_MAP1_VERTEX_3); 
	nurbs = gluNewNurbsRenderer();
}

// display
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);

	GLfloat ctlpoints[][3] = { 
		{ -3.0, -2.0, 0.0 },{ -2.0, 2.0, 0.0 },
		{ 2.0, -2.0, 0.0 },{ 3.0, 2.0, 0.0 },
		{ 3.0, -2.0, 0.0 },{ 2.0, 2.0, 0.0 },
		{ -2.0, -2.0, 0.0 },{ -3.0, 2.0, 0.0 },
		{ 1.0, 1.0, 0.0 } 
	};

	glColor3f(0, 1, 0);

	GLfloat knots[8] = {
		0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0
	};
	gluBeginCurve(nurbs);
	gluNurbsCurve(nurbs,
		8, knots,
		3,
		&ctlpoints[0][0],
		4,
		GL_MAP1_VERTEX_3);
	gluEndCurve(nurbs);

	glutSwapBuffers();
}


// idle
void idle() {
	glutPostRedisplay();
}

// main
int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	init();
	cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
	cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLU version: " << gluGetString(GLU_VERSION) << endl;
	glutReshapeFunc(resize);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutMainLoop();
}
