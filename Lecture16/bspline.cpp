#include <stdlib.h>
#include <GL/glut.h>

float Points[4][3] = {
	{  10, 10, 0 },
	{   5, 10, 0 },
	{  -5,  0, 0 },
	{ -10,  5, 0 }
};

unsigned int N = 20;

// reshape
void reshape(int w, int h) {
	if (h == 0) h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)w / h, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
}

// display
void display() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0, 0, 30, 0, 0, 0,	0, 1, 0);	
	glColor3f(0, 1, 0);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i != N; ++i) {
		float t = (float)i / (N - 1);
		// the t value inverted
		float it = 1.0f - t;
		// calculate blending functions
		float b0 = it * it*it / 6.0f;
		float b1 = (3 * t*t*t - 6 * t*t + 4) / 6.0f;
		float b2 = (-3 * t*t*t + 3 * t*t + 3 * t + 1) / 6.0f;
		float b3 = t * t*t / 6.0f;
		// sum the control points mulitplied by their respective blending functions
		float x = b0 * Points[0][0] + b1 * Points[1][0] + b2 * Points[2][0] + b3 * Points[3][0];
		float y = b0 * Points[0][1] + b1 * Points[1][1] + b2 * Points[2][1] + b3 * Points[3][1];
		float z = b0 * Points[0][2] + b1 * Points[1][2] + b2 * Points[2][2] + b3 * Points[3][2];
		// specify the point
		glVertex3f(x, y, z);
	}
	glEnd();

	// draw the control points
	glColor3f(1, 0, 0);
	glPointSize(3);
	glBegin(GL_POINTS);
	for (int i = 0; i != 4; ++i) {
		glVertex3fv(Points[i]);
	}
	glEnd();

	// draw the hull of the curve
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_STRIP);
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

// keyboard
void keyboard(unsigned char key, int, int) {
	switch (key) {
		case 'W':
		case 'w': ++N; break;
		case 'S':
		case 's': --N; if (N<3) N = 3; break;
		default: break;
	}
	glutPostRedisplay();
}

// main
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutCreateWindow("B-spline Curve: w/s to Change Level of Detail");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	init();
	glutMainLoop();
	return 0;
}
