#include <stdlib.h>
#include <GL/glut.h>

#include <stdio.h>
#include <vector>

float Points[7][3] = {
	{  5,  5, 0 },
	{  5, 10, 0 },
	{ 10, 10, 0 },
	{ 10,  0, 0 },
	{  0,  0, 0 },
	{-10,  5, 0 },
	{-10, -5, 0 }
};
#define NUM_POINTS 7
unsigned int N = 20;

#define NUM_SEGMENTS (NUM_POINTS+1)

// ----------------------------------------------------------------------------
float BezierLinear(float a, float b, float t) {
	// degree 1
	return a * (1.0f - t) + b * t;
}

float BezierQuadratic(float A, float B, float C, float t) {
	// degree 2
	float AB = BezierLinear(A, B, t);
	float BC = BezierLinear(B, C, t);
	return BezierLinear(AB, BC, t);
}

float BezierCubic(float A, float B, float C, float D, float t) {
	// degree 3
	float ABC = BezierQuadratic(A, B, C, t);
	float BCD = BezierQuadratic(B, C, D, t);
	return BezierLinear(ABC, BCD, t);
}

float BezierQuartic(float A, float B, float C, float D, float E, float t) {
	// degree 4
	float ABCD = BezierCubic(A, B, C, D, t);
	float BCDE = BezierCubic(B, C, D, E, t);
	return BezierLinear(ABCD, BCDE, t);
}

float BezierQuintic(float A, float B, float C, float D, float E, float F, float t) {
	// degree 5
	float ABCDE = BezierQuartic(A, B, C, D, E, t);
	float BCDEF = BezierQuartic(B, C, D, E, F, t);
	return BezierLinear(ABCDE, BCDEF, t);
}

float BezierSextic(float A, float B, float C, float D, float E, float F, float G, float t) {
	// degree 6
	float ABCDEF = BezierQuintic(A, B, C, D, E, F, t);
	float BCDEFG = BezierQuintic(B, C, D, E, F, G, t);
	return BezierLinear(ABCDEF, BCDEFG, t);
}

// ----------------------------------------------------------------------------

class Point {
public:
	float x, y, z;
	Point() { x = 0; y = 0; z = 0; }
	Point(const float a, const float b, const float c) { x = a; y = b; z = c; }
	Point(const Point& p) { x = p.x; y = p.y; z = p.z; }
};
std::vector<Point> points;

//increase points
void increasePoints() {
	if (points.size() >= 50)
		return;
	std::vector<Point> NewPoints;
	NewPoints.push_back(points[0]);
	for (unsigned int i = 0; i<(points.size() - 1); ++i) {
		// get 2 original points
		const Point& p0 = points[i];
		const Point& p1 = points[i + 1];
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
	NewPoints.push_back(points[points.size() - 1]);
	// update the points array
	points = NewPoints;
}

// decrease points
void decreasePoints() {
	// original shape
	if (points.size() <= NUM_POINTS)
		return;
	std::vector<Point> NewPoints;
	// keep the first point
	NewPoints.push_back(points[0]);
	// step over every 2 points
	for (unsigned int i = 1; i<(points.size() - 1); i += 2) {
		// get last point found in reduced array
		const Point& pLast = NewPoints[NewPoints.size() - 1];
		// get 2 original point
		const Point& p0 = points[i];
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
	points = NewPoints;
}


// get_point
float* get_point(int i) {
	if (i<0) return	Points[0];
	if (i<NUM_POINTS) return Points[i];
	return Points[NUM_POINTS - 1];
}

// keyboard
void keyboard(unsigned char key, int, int) {
	switch (key) {
	case 'W':
	case 'w': ++N; increasePoints();  break;
	case 's':
	case 'S': --N; if (N<2) N = 2; decreasePoints(); break;
	default: break;
	}
	glutPostRedisplay();
}

// control graph
void draw_control_graph() {
	glColor3f(0, 0, 1);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i != NUM_POINTS; ++i) {
		glVertex3fv(get_point(i));
	}
	glEnd();
}

// control points
void draw_control_points() {
	glPointSize(10);
	glColor3f(1, 0, 0);
	glBegin(GL_POINTS);
	glPointSize(10);
	for (int i = 0; i != NUM_POINTS; ++i) {
		glVertex3fv(Points[i]);
	}
	glEnd();
}

// bspline
void draw_bspline() {
	glBegin(GL_LINE_STRIP);
	glColor3f(0, 1, 0);
	for (int segment = -3, j = 0; j != NUM_SEGMENTS; ++j, ++segment) {
		for (int i = 0; i != N; ++i) {
			float t = (float)i / N;
			float it = 1.0f - t;
			float b0 = it * it*it / 6.0f;
			float b1 = (3 * t*t*t - 6 * t*t + 4) / 6.0f;
			float b2 = (-3 * t*t*t + 3 * t*t + 3 * t + 1) / 6.0f;
			float b3 = t * t*t / 6.0f;
			float x = b0 * get_point(segment + 0)[0] + b1 * get_point(segment + 1)[0] + b2 * get_point(segment + 2)[0] + b3 * get_point(segment + 3)[0];
			float y = b0 * get_point(segment + 0)[1] + b1 * get_point(segment + 1)[1] + b2 * get_point(segment + 2)[1] + b3 * get_point(segment + 3)[1];
			float z = b0 * get_point(segment + 0)[2] + b1 * get_point(segment + 1)[2] + b2 * get_point(segment + 2)[2] + b3 * get_point(segment + 3)[2];

			glVertex3f(x, y, z);
		}
	}
	glVertex3fv(Points[NUM_POINTS - 1]);
	glEnd();
	// show S points
	glColor3f(1, 1, 0);
	glBegin(GL_POINTS);
	for (int segment = -3, j = 0; j != NUM_SEGMENTS; ++j, ++segment) {
		for (int i = 0; i != N; ++i) {
			float t = (float)i / N;
			float it = 1.0f - t;
			float b0 = it * it*it / 6.0f;
			float b1 = (3 * t*t*t - 6 * t*t + 4) / 6.0f;
			float b2 = (-3 * t*t*t + 3 * t*t + 3 * t + 1) / 6.0f;
			float b3 = t * t*t / 6.0f;
			float x = b0 * get_point(segment + 0)[0] + b1 * get_point(segment + 1)[0] + b2 * get_point(segment + 2)[0] + b3 * get_point(segment + 3)[0];
			float y = b0 * get_point(segment + 0)[1] + b1 * get_point(segment + 1)[1] + b2 * get_point(segment + 2)[1] + b3 * get_point(segment + 3)[1];
			float z = b0 * get_point(segment + 0)[2] + b1 * get_point(segment + 1)[2] + b2 * get_point(segment + 2)[2] + b3 * get_point(segment + 3)[2];

			glVertex3f(x, y, z);
			break;
		}
	}
	glEnd();
}

// bezier with casteljau
void draw_casteljau() {
	glColor3f(1, 0.6, 0);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i != N; ++i) {
		float t = (float)i / (N - 1);
		float it = 1.0f - t;
		float x = BezierSextic(Points[0][0], Points[1][0], Points[2][0], Points[3][0], Points[4][0], Points[5][0], Points[6][0], t);
		float y = BezierSextic(Points[0][1], Points[1][1], Points[2][1], Points[3][1], Points[4][1], Points[5][1], Points[6][1], t);
		float z = BezierSextic(Points[0][2], Points[1][2], Points[2][2], Points[3][2], Points[4][2], Points[5][2], Points[6][2], t);
		glVertex3f(x, y, z);
	}
	glEnd();
}

// simple bezier
void draw_simple_bezier(float *A, float *B, float *C, float *D) {
	glColor3f(1, 0, 0);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i != N; ++i) {
		float t = (float)i / (N - 1);
		float it = 1.0f - t;
		// calculate blending functions
		float b0 = 1 * t*t*t;
		float b1 = 3 * t*t		*	it;
		float b2 = 3 * t		*	it*it;
		float b3 = 1 * 1 * it*it*it;
		float x = b3 * A[0] + b2 * B[0] + b1 * C[0] + b0 * D[0];
		float y = b3 * A[1] + b2 * B[1] + b1 * C[1] + b0 * D[1];
		float z = b3 * A[2] + b2 * B[2] + b1 * C[2] + b0 * D[2];
		// specify the point
		glVertex3f(x, y, z);
	}
	glEnd();
}


// simple bezier
void draw_simple_hermite(float *A, float *B, float *C, float *D) {
	glColor3f(1, 0, 0);
	glBegin(GL_LINE_STRIP);
	// use the parametric time value 0 to 1
	for (int i = 0; i != N; ++i) {
		float t = (float)i / (N - 1);
		// calculate blending functions
		float b0 = 2 * t*t*t - 3 * t*t + 1;
		float b1 = -2 * t*t*t + 3 * t*t;
		float b2 = t * t*t - 2 * t*t + t;
		float b3 = t * t*t - t * t;
		// calculate the x, y and z of the curve point
		float x = b0 * A[0] + b1 * B[0] + b2 * C[0] + b3 * D[0];
		float y = b0 * A[1] + b1 * B[1] + b2 * C[1] + b3 * D[1];
		float z = b0 * A[2] + b1 * B[2] + b2 * C[2] + b3 * D[2];
		// specify the point
		glVertex3f(x, y, z);
	}
	glEnd();
}

// chaikin
void draw_chaikin() {
	glColor3f(0.7, 0.8, 1);
	glBegin(GL_LINE_STRIP);
	std::vector<Point>::iterator it = points.begin();
	for (; it != points.end(); ++it) {
		glVertex3f(it->x, it->y, it->z);
	}
	glEnd();
}

// display
void display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0, 0, 30, 0, 0, 0, 0, 1, 0);

	draw_control_graph();
	draw_control_points();

	draw_bspline(); // 7 points
	draw_casteljau(); // 7 points bezier

	draw_simple_bezier(Points[0], Points[1], Points[2], Points[3]);
	float t1[] = {   0,-10, 0};
	float t2[] = { -10, 10, 0};
	draw_simple_hermite (Points[3], Points[4], t1, t2);
	draw_simple_bezier(Points[4], Points[5], Points[6], Points[6]);


	draw_chaikin();

	glutSwapBuffers();
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

// init
void init() {
	// init chaikin
	points.push_back(Point(5, 5, 0));
	points.push_back(Point(5, 10, 0));
	points.push_back(Point(10, 10, 0));
	points.push_back(Point(10, 0, 0));
	points.push_back(Point(0, 0, 0));
	points.push_back(Point(-10, 5, 0));
	points.push_back(Point(-10, -5, 0));
	for (int i = 0; i < 10; i++)
		increasePoints();
}

// main
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutCreateWindow("All Curves: w/s to Change Level of Detail");
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}
