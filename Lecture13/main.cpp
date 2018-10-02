#include <stdlib.h>
#include <GL/glut.h>

// 4 control points 
float Points[4][3] = { { 10,  0,  0 },
			 {  5, 10,  2 },
			 { -5,  0,  0 },
			 {-10,  5, -2 }};

// level of detail of the curve is 20 points
unsigned int LOD = 20;

//keyboard
void keyboard(unsigned char key, int, int) {
	switch (key) {
	case 'W':
	case 'w': ++LOD;
		break;
	case 'S':
	case 's': --LOD; if (LOD<2) LOD = 2;
		break;
	}
	glutPostRedisplay();
}

// display
void display() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(1, 10, 30, 0, 0, 0, 0, 1, 0);
	glColor3f(1, 0, 1);

	// draw the control vertices in red
	glColor3f(1, 0, 0);
	glPointSize(3);
	glBegin(GL_POINTS);
		for (int i = 0; i != 4; ++i) {
			glVertex3fv(Points[i]);
		}
	glEnd();

	// draw lines to make a curve in green
	glBegin(GL_LINE_STRIP);
	glColor3f(0, 1, 0);
	for (int i = 0; i != LOD; ++i) {
		// use a parametric time value 0 to 1
		float t = (float)i / (LOD - 1);
		float it = 1.0f - t;
		// calculate blending functions
		float b0 = 1 * t*t*t;
		float b1 = 3 * t*t		*	it;
		float b2 = 3 * t		*	it*it;
		float b3 = 1 * 1 * it*it*it;
		// calculate the x, y and z of the curve point by summing
		// the Control vertices weighted by their respective blending functions
		float x = b3 * Points[0][0] + b2 * Points[1][0] + b1 * Points[2][0] + b0 * Points[3][0];
		float y = b3 * Points[0][1] + b2 * Points[1][1] + b1 * Points[2][1] + b0 * Points[3][1];
		float z = b3 * Points[0][2] + b2 * Points[1][2] + b1 * Points[2][2] + b0 * Points[3][2];
		// specify the point
		glVertex3f(x, y, z);
	}
	glEnd();

	// control graph in blue
	glBegin(GL_LINE_STRIP);
		glColor3f(0, 0, 1);
		for (int i = 0; i != 4; ++i) {
			glVertex3fv(Points[i]);
		}
	glEnd();

	glutSwapBuffers();
}

// init
void init() {
	glEnable(GL_DEPTH_TEST);
}

// reshape
void reshape(int w, int h) {
	if (h == 0) h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)w / h, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// main
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(800, 600);
  	glutCreateWindow("Bezier Curve | Press 'w' and 's' to change the level of detail");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	init();
	glutMainLoop();
	return 0;
}
