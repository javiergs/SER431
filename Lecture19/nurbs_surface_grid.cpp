#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>

GLfloat ctlpoints[4][4][3] = {
	{ { 20, 0,  10 },{ 0, 0,  10 },{ -5, 0,  10 },{ -10, 0,  10 } },
    { { 20, 0,   5 },{ 0, 15,   5 },{ -5, 15,   5 },{ -10, 0,   5 } },
    { { 20, 0,  -5 },{ 0, 10,  -5 },{ -5, 10,  -5 },{ -10, 0,  -5 } },
    { { 20, 0, -10 },{ 0, 0, -10 },{ -5, 0, -10 },{ -10, 0, -10 } }
};

GLUnurbsObj *theNurb;

// init
void init(void) {
	theNurb = gluNewNurbsRenderer();
	gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 25.0);
	gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_OUTLINE_POLYGON);
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gluLookAt(2, 2, -6.5, 0, 0, 0, 0, 1, 0);

	GLfloat knots[8] = { 0.0, 0.0, 0.0, 0.0, 3.0, 3.0, 3.0, 3.0 };
	int i, j;
	
	glPushMatrix();
	glScalef(0.125, 0.125, 0.125);
	// NURBS
	glColor3f(0, 1, 0);
	gluBeginSurface(theNurb);
	gluNurbsSurface(theNurb, 8, knots, 8, knots, 4 * 3, 3, &ctlpoints[0][0][0], 4, 4, GL_MAP2_VERTEX_3);
	gluEndSurface(theNurb);

	// show control points
	glPointSize(1.0);
	glColor3f(0, 0, 1);
	for (i = 0; i < 4; i++) {
		glBegin(GL_LINE_STRIP);
		for (j = 0; j < 4; j++) {
			glVertex3f(ctlpoints[i][j][0], ctlpoints[i][j][1], ctlpoints[i][j][2]);
		}
		glEnd();
	}

	// show control points
	glPointSize(5.0);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			glVertex3f(ctlpoints[i][j][0], ctlpoints[i][j][1], ctlpoints[i][j][2]);
		}
	}
	glEnd();

	glPopMatrix();
	glFlush();
}

void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (GLdouble)w / (GLdouble)h, 3.0, 8.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// main
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("NURBS Grid");
	init();
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutMainLoop();
	return 0;
}
