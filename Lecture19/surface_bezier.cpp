#include <stdlib.h>
#include <GL/glut.h>

/// a structure to hold a control point of the surface
struct Point {
	float x;
	float y;
	float z;
};

/// 4x4 grid of points that will define the surface
Point Points[4][4] = {
	{ { 20, 0,  10 }, { 0, 0,  10 }, { -5, 0,  10 }, { -10, 0,  10 } },
	{ { 20, 0,   5 }, { 0, 15,   5 }, { -5, 15,   5 }, { -10, 0,   5 } },
	{ { 20, 0,  -5 }, { 0, 10,  -5 }, { -5, 10,  -5 }, { -10, 0,  -5 } },
	{ { 20, 0, -10 }, { 0, 0, -10 }, { -5, 0, -10 }, { -10, 0, -10 }}
};

// the level of detail of the surface
unsigned int N = 20;

// calculate curves (in rows)
Point CalculateU(float t, int row) {
	// the final point
	Point p;
	// the t value inverted
	float it = 1.0f - t;
	// blending functions
	float b0 = t * t*t;
	float b1 = 3 * t*t*it;
	float b2 = 3 * t*it*it;
	float b3 = it * it*it;
	// curve
	p.x = b0 * Points[row][0].x + b1 * Points[row][1].x + b2 * Points[row][2].x + b3 * Points[row][3].x;
	p.y = b0 * Points[row][0].y + b1 * Points[row][1].y + b2 * Points[row][2].y + b3 * Points[row][3].y;
	p.z = b0 * Points[row][0].z + b1 * Points[row][1].z + b2 * Points[row][2].z + b3 * Points[row][3].z;
	return p;
}

// integrate curves as a surface (in columns)
Point CalculateV(float t, Point* pnts) {
	Point p;
	// the t value inverted
	float it = 1.0f - t;
	// calculate blending functions
	float b0 = t * t*t;
	float b1 = 3 * t*t*it;
	float b2 = 3 * t*it*it;
	float b3 = it * it*it;
	// blending functions
	p.x = b0 * pnts[0].x + b1 * pnts[1].x + b2 * pnts[2].x + b3 * pnts[3].x;
	p.y = b0 * pnts[0].y + b1 * pnts[1].y + b2 * pnts[2].y + b3 * pnts[3].y;
	p.z = b0 * pnts[0].z + b1 * pnts[1].z + b2 * pnts[2].z + b3 * pnts[3].z;
	return p;
}

// calculate rows and columns
Point calculate(float u, float v) {
	Point temp[4];
	// calculate each point on our final v curve
	temp[0] = CalculateU(u, 0);
	temp[1] = CalculateU(u, 1);
	temp[2] = CalculateU(u, 2);
	temp[3] = CalculateU(u, 3);
	return CalculateV(v, temp);
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

// display
void display() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(30, 20, -30, 0, 0, 0, 0, 1, 0);
	glColor3f(0, 1, 0);
	glPointSize(3);
	// curves
	glBegin(GL_POINTS);
		for (int i = 0; i != N; ++i) {
			float u = (float)i / (N - 1);
			for (int j = 0; j != N; ++j) {
				float v = (float)j / (N - 1);
				Point p = calculate(u, v);
				glVertex3f(p.x, p.y, p.z);
			}
		}
	glEnd();
	// control graph
	glColor3f(0, 0, 1);
	for (int i = 0; i < 4; ++i) {
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j < 4; ++j) {	
			glVertex3f(Points[i][j].x, Points[i][j].y, Points[i][j].z);
		}
		glEnd();
	}

	// control points
	glColor3f(1, 0, 0);
	glPointSize(5);
	for (int i = 0; i < 4; ++i) {
		glBegin(GL_POINTS);
		for (int j = 0; j < 4; ++j) {
			glVertex3f(Points[i][j].x, Points[i][j].y, Points[i][j].z);
		}
		glEnd();
	}

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
		case 's':
		case 'S': --N; if (N<3) N = 3; break;
	default: break;
	}
	glutPostRedisplay();
}

// main
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutCreateWindow("Surface - Use W/S to Change Level of Detail");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	init();
	glutMainLoop();
	return 0;
}
