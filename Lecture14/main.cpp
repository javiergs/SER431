/**
 * Hermite Curves
 */
 
#include <stdlib.h>
#include <GL/glut.h>

float Geometry[4][3] = {
 	{ -10, -10, 0 },  //  Point 1
  { 10,  10, 0 },	  //	Point2
  {-10,  20, 0 },	  //	Tangent1
  { 10,   0, 0 }		//	Tangent2
};
unsigned int N = 20;

// reshape
void reshape(int w, int h) {
	if (h == 0) h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)w / h, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// display
void display() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0, 0, 50, 0, 0, 0,	0, 1, 0);	
	glColor3f(0, 1, 0);
	glBegin(GL_LINE_STRIP);
	// use the parametric time value 0 to 1
	for (int i = 0; i != N; ++i) {
		float t = (float)i / (N - 1);
		// calculate blending functions
		float b0 =  2 * t*t*t - 3 * t*t + 1;
		float b1 = -2 * t*t*t + 3 * t*t;
		float b2 =      t*t*t - 2 * t*t + t;
		float b3 =      t*t*t - t * t;
		// calculate the x, y and z of the curve point
		float x = b0 * Geometry[0][0] + b1 * Geometry[1][0] + b2 * Geometry[2][0] + b3 * Geometry[3][0];
		float y = b0 * Geometry[0][1] + b1 * Geometry[1][1] + b2 * Geometry[2][1] + b3 * Geometry[3][1];
		float z = b0 * Geometry[0][2] +	b1 * Geometry[1][2] + b2 * Geometry[2][2] + b3 * Geometry[3][2];
		// specify the point
		glVertex3f(x, y, z);
	}
	glEnd();
  // Points
	glColor3f(1, 0, 0);
	glPointSize(3);
	glBegin(GL_POINTS);
		glVertex3fv(Geometry[0]);
		glVertex3fv(Geometry[1]);
	glEnd();
	// Control Graph
	glColor3f(0, 0, 1);
	glPushMatrix();
	glTranslatef(Geometry[0][0], Geometry[0][1], Geometry[0][2]);
	glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3fv(Geometry[2]);
	glEnd();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(Geometry[1][0], Geometry[1][1], Geometry[1][2]);
	glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3fv(Geometry[3]);
	glEnd();
	glPopMatrix();

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
	glutCreateWindow("Hermite Curve: W and S to Change Level of Detail");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	init();
	glutMainLoop();
	return 0;
}
