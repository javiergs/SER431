/**
 * Chaikin Curves
 */

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <vector>

class Point {
public:
	float x, y, z;
	Point() { x = 0; y = 0; z = 0; }
	Point(const float a, const float b, const float c) { x = a; y = b; z = c; }
	Point(const Point& p) { x = p.x; y = p.y; z = p.z; }
};

std::vector<Point> Points;

//increase points
void increasePoints() {
	if (Points.size() >= 50)
		return;
	std::vector<Point> NewPoints;
	// keep the first point
	NewPoints.push_back(Points[0]);
	for (unsigned int i = 0; i<(Points.size() - 1); ++i) {
		// get 2 original points
		const Point& p0 = Points[i];
		const Point& p1 = Points[i + 1];
		Point Q;
		Point R;
		// average the 2 original points to create 2 new points. For each CV, another 2 verts are created.
		Q.x = 0.75f*p0.x + 0.25f*p1.x;
		Q.y = 0.75f*p0.y + 0.25f*p1.y;
		Q.z = 0.75f*p0.z + 0.25f*p1.z;
		R.x = 0.25f*p0.x + 0.75f*p1.x;
		R.y = 0.25f*p0.y + 0.75f*p1.y;
		R.z = 0.25f*p0.z + 0.75f*p1.z;
		NewPoints.push_back(Q);
		NewPoints.push_back(R);
	}
  // keep the last point
  NewPoints.push_back(Points[Points.size() - 1]);
	// update the points array
	Points = NewPoints;
}

// decrease points
void decreasePoints() {
	// original shape
	if (Points.size() <= 5)
		return;
	std::vector<Point> NewPoints;
	// keep the first point
	NewPoints.push_back(Points[0]);
	// step over every 2 points
	for (unsigned int i = 1; i<(Points.size() - 1); i += 2) {
		// get last point found in reduced array
		const Point& pLast = NewPoints[NewPoints.size() - 1];
		// get 2 original point
		const Point& p0 = Points[i];
		Point Q;
		// calculate the original point
		Q.x = p0.x - 0.75f*pLast.x;
		Q.y = p0.y - 0.75f*pLast.y;
		Q.z = p0.z - 0.75f*pLast.z;
		Q.x *= 4.0f;
		Q.y *= 4.0f;
		Q.z *= 4.0f;
		// add to new curve
		NewPoints.push_back(Q);
	}
	// copy over points
	Points = NewPoints;
}

// keyboard
void keyboard(unsigned char key, int, int) {
	switch (key) {
	  case 'w':
	  case 'W': increasePoints(); break;
	  case 's':
		case 'S': decreasePoints(); break;
		default: break;
	}
	glutPostRedisplay();
}

// display
void display() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0, 0, 30, 0, 0, 0,	0, 1, 0);	
	glColor3f(1, 1, 0);
	glBegin(GL_LINE_STRIP);
	std::vector<Point>::iterator it = Points.begin();
	for (; it != Points.end(); ++it) {
		glVertex3f(it->x, it->y, it->z);
	}
	glEnd();
	glutSwapBuffers();
}

// init
void init() {
	glEnable(GL_DEPTH_TEST);
	Points.push_back(Point(   0,  10, 0));
	Points.push_back(Point( -10, 10, 0));
	Points.push_back(Point(- 10,  -10, 0));
	Points.push_back(Point(  10, -10, 0));
	Points.push_back(Point(  10, 10, 0));
}

// reshape
void reshape(int w, int h) {
	if (h == 0) h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (float)w / h, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
}

// main
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Chaikin Curve: w/s to Change Level of Detail");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	init();
	glutMainLoop();
	return 0;
}
