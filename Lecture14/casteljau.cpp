#include <stdlib.h>
#include <GL/glut.h>

// 4 control points 
float Points[6][3] = { 
	{ 10,  5,  0 },
  {  8,  -10,  0  },
  {  6,  -10,  0 },
  {  4,   5,  0  },
  {  0, -5,  0  }, 
  { -10,  5,  0  }
};

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

// -Bezier N -----------------------------------------
float BezierLinear(float a, float b, float t) {
	// degree 1
	return a * (1.0f - t) + b * t;
}

float BezierQuadratic(float A, float B, float C, float t)
{
	// degree 2
	float AB = BezierLinear(A, B, t);
	float BC = BezierLinear(B, C, t);
	return BezierLinear(AB, BC, t);
}

float BezierCubic(float A, float B, float C, float D, float t)
{
	// degree 3
	float ABC = BezierQuadratic(A, B, C, t);
	float BCD = BezierQuadratic(B, C, D, t);
	return BezierLinear(ABC, BCD, t);
}

float BezierQuartic(float A, float B, float C, float D, float E, float t)
{
	// degree 4
	float ABCD = BezierCubic(A, B, C, D, t);
	float BCDE = BezierCubic(B, C, D, E, t);
	return BezierLinear(ABCD, BCDE, t);
}

float BezierQuintic(float A, float B, float C, float D, float E, float F, float t)
{
	// degree 5
	float ABCDE = BezierQuartic(A, B, C, D, E, t);
	float BCDEF = BezierQuartic(B, C, D, E, F, t);
	return BezierLinear(ABCDE, BCDEF, t);
}

float BezierSextic(float A, float B, float C, float D, float E, float F, float G, float t)
{
	// degree 6
	float ABCDEF = BezierQuintic(A, B, C, D, E, F, t);
	float BCDEFG = BezierQuintic(B, C, D, E, F, G, t);
	return BezierLinear(ABCDEF, BCDEFG, t);
}

// ---------------------------------------------------

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
	for (int i = 0; i != 6; ++i) {
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
		// float b0 = 1 * t*t*t;
		// float b1 = 3 * t*t		*	it;
		// float b2 = 3 * t		*	it*it;
		// float b3 = 1 * 1 * it*it*it;
		// calculate the x, y and z of the curve point by summing
		// the Control vertices weighted by their respective blending functions
		float x = BezierQuintic(Points[0][0], Points[1][0], Points[2][0], Points[3][0], Points[4][0], Points[5][0], t);
		float y = BezierQuintic(Points[0][1], Points[1][1], Points[2][1], Points[3][1], Points[4][1], Points[5][1], t);
		float z = BezierQuintic(Points[0][2], Points[1][2], Points[2][2], Points[3][2], Points[4][2], Points[5][2], t);
		// specify the point
		glVertex3f(x, y, z);
	}
	glEnd();

	// control graph in blue
	glBegin(GL_LINE_STRIP);
	glColor3f(0, 0, 1);
	for (int i = 0; i != 6; ++i) {
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
