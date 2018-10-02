#include <stdlib.h>
#include <GL/glut.h>

// 4 control points for our cubic bezier curve
float Points[4][3] = {
	{ 10, 10, 0 },
	{  5, 10, 2 },
	{ -5,  0, 0 },
	{-10,  5,-2 }
};

// the level of detail of the curve
unsigned int LOD = 20;

//key press control level of detail
void OnKeyPress(unsigned char key, int, int) {
	switch (key) {
	case 'W':
	case 'w':
		++LOD;
		break;
	case 'S':
	case 's':
		--LOD;
		if (LOD<2) LOD = 2;
		break;
	default:
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
	
	// draw the Control Vertices
	glColor3f(1, 0, 0);
	glPointSize(3);
	glBegin(GL_POINTS);
	for (int i = 0; i != 4; ++i) {
		glVertex3fv(Points[i]);
	}
	glEnd();

	// draw lines to make a curve
	glColor3f(0, 1, 0);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i != LOD; ++i) {
		// use a parametric time value 0 to 1
		float t = (float)i / (LOD - 1);
		float it = 1.0f - t;
		// calculate blending functions
		float b0 = 1	*	t*t*t;
		float b1 = 3	*	t*t		*	it;
		float b2 = 3	*	t		*	it*it;
		float b3 = 1	*	1		*	it*it*it;
		// calculate the x,y and z of the curve point by summing
		// the Control vertices weighted by their respective blending
		// functions
		float x = b3 * Points[0][0] + b2 * Points[1][0] + b1 * Points[2][0] + b0 * Points[3][0];
		float y = b3 * Points[0][1] + b2 * Points[1][1] + b1 * Points[2][1] + b0 * Points[3][1];
		float z = b3 * Points[0][2] + b2 * Points[1][2] + b1 * Points[2][2] + b0 * Points[3][2];
		// specify the point
		glVertex3f(x, y, z);
	}
	glEnd();

	// draw the control graph of the curve
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i != 4; ++i) {
		glVertex3fv(Points[i]);
	}
	glEnd();

	glutSwapBuffers();
}


//------------------------------------------------------------	OnInit()
//
void OnInit() {
	// enable depth testing
	glEnable(GL_DEPTH_TEST);
}

//------------------------------------------------------------	OnExit()
//
void OnExit() {
}

//------------------------------------------------------------	OnReshape()
//
void OnReshape(int w, int h)
{
	if (h == 0)
		h = 1;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// just use a perspective projection
	gluPerspective(45, (float)w / h, 0.1, 100);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


//------------------------------------------------------------	main()
//
int main(int argc, char** argv) {

	// initialise glut
	glutInit(&argc, argv);

	// request a depth buffer, RGBA display mode, and we want double buffering
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);

	// set the initial window size
	glutInitWindowSize(640, 480);

	// create the window
	glutCreateWindow("Bezier Curve: +/- to Change Level of Detail");

	// set the function to use to draw our scene
	glutDisplayFunc(display);

	// set the function to handle changes in screen size
	glutReshapeFunc(OnReshape);

	// set the function for the key presses
	glutKeyboardFunc(OnKeyPress);

	// run our custom initialisation
	OnInit();

	// set the function to be called when we exit
	atexit(OnExit);

	// this function runs a while loop to keep the program running.
	glutMainLoop();
	return 0;
}
