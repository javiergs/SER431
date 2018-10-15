#include <stdlib.h>
#include <GL/glut.h>

float Points[7][3] = {
	{ 10, 10, 0 },
	{  5, 10, 0 },
  {  0,  0, 0 },
  { -5, -5, 0 },
	{-10,  0, 0 },
	{ -5, 10, 0 },
	{  0,  5, 0 }
};
#define NUM_POINTS 7
unsigned int N = 20;

#define NUM_SEGMENTS (NUM_POINTS+1)

// getPoint
float* GetPoint(int i) {
	if (i<0) return	Points[0];
	if (i<NUM_POINTS) return Points[i];
	return Points[NUM_POINTS - 1];
}

// keyboard
void keyboard(unsigned char key, int, int) {
	switch (key) {
	case 'W':
	case 'w': ++N; break;
	case 's':
	case 'S': --N; if (N<2) N = 2; break;
	default: break;
	}
	glutPostRedisplay();
}

//	curve segments to be draw
//
//	0 0 0 1
//    0 0 1 2
//      0 1 2 3
//        1 2 3 4
//          2 3 4 5
//            3 4 5 6
//              4 5 6 6
//                5 6 6 6
//

// display
void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0, 0, 30,	0, 0, 0, 0, 1, 0);
	glColor3f(0.5, 0.2, 0);
	glPointSize(3);

	// control graph
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i != NUM_POINTS; ++i) {
		glVertex3fv(Points[i]);
	}
	glEnd();

	// draw (NUM_POINTS+1) curves. 
	glBegin(GL_LINE_STRIP);
	float loop=0;
	for (int start_cv = -3, j = 0; j != NUM_SEGMENTS; ++j, ++start_cv) {
		// for each section of curve, draw N divisions
		loop = !loop;
		glColor3f(loop, 1, loop);
		for (int i = 0; i != N; ++i) {
			float t = (float)i / N;
			float it = 1.0f - t;
			// calculate blending functions for cubic bspline
			float b0 = it * it*it / 6.0f;
			float b1 = (3 * t*t*t - 6 * t*t + 4) / 6.0f;
			float b2 = (-3 * t*t*t + 3 * t*t + 3 * t + 1) / 6.0f;
			float b3 = t * t*t / 6.0f;
			// calculate the x,y and z of the curve point
			float x = b0 * GetPoint(start_cv + 0)[0] +
				b1 * GetPoint(start_cv + 1)[0] +
				b2 * GetPoint(start_cv + 2)[0] +
				b3 * GetPoint(start_cv + 3)[0];

			float y = b0 * GetPoint(start_cv + 0)[1] +
				b1 * GetPoint(start_cv + 1)[1] +
				b2 * GetPoint(start_cv + 2)[1] +
				b3 * GetPoint(start_cv + 3)[1];

			float z = b0 * GetPoint(start_cv + 0)[2] +
				b1 * GetPoint(start_cv + 1)[2] +
				b2 * GetPoint(start_cv + 2)[2] +
				b3 * GetPoint(start_cv + 3)[2];

			// specify the point
			glVertex3f(x, y, z);
		}
	}

	// we need to specify the last point on the curve
	glVertex3fv(Points[NUM_POINTS - 1]);
	glEnd();

	// draw control points
	glColor3f(1, 0, 0);
	glBegin(GL_POINTS);
	glPointSize(10);
	for (int i = 0; i != NUM_POINTS; ++i) {
		glVertex3fv(Points[i]);
	}
	glEnd();

	glutSwapBuffers();
}

// init
void init() {
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
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutCreateWindow("B-Spline Curves: w/s to Change Level of Detail");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	init();
	glutMainLoop();
	return 0;
}
