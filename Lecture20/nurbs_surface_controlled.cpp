#include <GL/glut.h>
#include <math.h>

// global 
int width = 1200;
int height = 600;
float ratio = 1.0;

// controling parameters
int mouse_button;
int mouse_x = 0;
int mouse_y = 0;
float scale = 0.3;
float x_angle = 0.0;
float y_angle = 0.0;

// reshape
void reshape(int w, int h) {
	width = w;
	height = h;
	if (h == 0) h = 1;
	ratio = 1.0f * w / h;
	glutPostRedisplay();
}

// mouse
void mouse(int button, int state, int x, int y) {
	mouse_x = x;
	mouse_y = y;
	mouse_button = button;
}

// motion
void motion(int x, int y) {
	if (mouse_button == GLUT_LEFT_BUTTON) {
		y_angle += (float(x - mouse_x) / width) *360.0;
		x_angle += (float(y - mouse_y) / height)*360.0;
	}
	if (mouse_button == GLUT_RIGHT_BUTTON) {
		scale += (y - mouse_y) / 100.0;
		if (scale < 0.1) scale = 0.1;
		if (scale > 7)	scale = 7;
	}
	mouse_x = x;
	mouse_y = y;
	glutPostRedisplay();
}

// Shadows
GLfloat light_position[4];
float lightAngle = 0.0, lightHeight = 75;

// init
void init() {
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	// general
	ratio = (double)width / (double)height;	
	// light
	GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	// Materials
	GLfloat mat_diffuse[] = { 0.0f, 0.5f, 0.31f, 1 };
	GLfloat mat_specular[] = { 0.1f, 0.5f, 0.5f, 1 };
	GLfloat mat_ambient[] = { 0.0f, 0.5f, 0.31f, 1 };
	GLfloat mat_shininess[] = { 100.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);
	glEnable(GL_AUTO_NORMAL);
}

// draw an arrow to visualize the source of light
void drawLightArrow() {
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 1.0, 0.0);
	// draw arrowhead. 
	glTranslatef(light_position[0], light_position[1], light_position[2]);
	glRotatef(lightAngle * -180.0 / 3.141592, 0, 1, 0);
	glRotatef(atan(lightHeight / 500) * 180.0 / 3.141592, 0, 0, 1);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0, 0, 0);
	glVertex3f(20, 10, 10);
	glVertex3f(20, -10, 10);
	glVertex3f(20, -10, -10);
	glVertex3f(20, 10, -10);
	glVertex3f(20, 10, 10);
	glEnd();
	// draw a  line from light direction
	glBegin(GL_LINES);
	glVertex3f(0, 0, 0);
	glVertex3f(50, 0, 0);
	glEnd();
	glEnable(GL_LIGHTING);
	glPopMatrix();
}


GLUnurbsObj *theNurb;

// display
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// light source position
	light_position[0] = 200 * cos(lightAngle);
	light_position[1] = lightHeight;
	light_position[2] = 200 * sin(lightAngle);
	light_position[3] = 0.0; // directional light
	lightAngle += 0.0005;
	// projection and view
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, width, height);
	gluPerspective(40.0, ratio, 1, 1000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// lookAt 
	gluLookAt(0.0f, 40.0f, 320.0, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f);
	// camera
	glScalef(scale, scale, scale);
	glRotatef(x_angle, 1.0f, 0.0f, 0.0f);
	glRotatef(y_angle, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, 0.0f);
	// draw arrow light
	glPushMatrix();
      glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	  drawLightArrow();
	glPopMatrix();

	// NURBS
	// 6 curves with 6 control points each
	// 10 knots per curve and 10 knots per inter-curve conection (controlpnt + 4)
	// 6*3 and 3 offsets
	// cubic equations (4)

	GLfloat ctlpoints[6][6][3] = {
	{ { 25, 5,  15 } ,{ 20, 5,  15 },{ 0, 0,  15 },{ -5, 0,  15 },{ -10, 5,  15 }   ,{ -15, 5,  15 } },
	{ { 25, 5,  10 } ,{ 20, 0,  10 },{ 0, 0,  10 },{ -5, 0,  10 },{ -10, 0,  10 }   ,{ -15, 5,  10 } },
	{ { 25, 0,   5 } ,{ 20, 0,   5 },{ 0, 15,   5 },{ -5, 15,   5 },{ -10, 0,   5 } ,{ -15, 0,  5 } },
	{ { 25, 0,  -5 } ,{ 20, 0,  -5 },{ 0, 10,  -5 },{ -5, 10,  -5 },{ -10, 0,  -5 } ,{ -15, 0,  -5 } },
	{ { 25, 5,  -10 } ,{ 20, 0, -10 },{ 0, 0, -10 },{ -5, 0, -10 },{ -10, 0, -10 }   ,{ -15, 5,  -10 } },
	{ { 25, 5,  -15 } ,{ 20, 5, -15 },{ 0, 0, -15 },{ -5, 0, -15 },{ -10, 5, -15 }   ,{ -15, 5,  -15 } }
	};
	GLfloat uknots[10] = { 0.0, 0.0, 0.0, 0.0, 1.0, 3.0, 5.0, 5.0, 5.0, 5.0 }; // 4 times
	GLfloat vknots[10] = { 0.0, 0.0, 0.0, 0.0, 1.0, 3.0, 5.0, 5.0, 5.0, 5.0 };
	GLUnurbsObj *theNurb;
	theNurb = gluNewNurbsRenderer();
	gluNurbsProperty(theNurb, GLU_SAMPLING_TOLERANCE, 25.0);
	gluNurbsProperty(theNurb, GLU_DISPLAY_MODE, GLU_FILL);

	glPushMatrix();
	glScalef(15, 15, 15);
	gluBeginSurface(theNurb);
	gluNurbsSurface(theNurb,
		10, uknots,
		10, vknots,
		6 * 3,
		3,
		&ctlpoints[0][0][0],
		4, 4,
		GL_MAP2_VERTEX_3);	
	gluEndSurface(theNurb);

	// control graph
	glDisable(GL_LIGHTING);
	glPointSize(1.0);
	glColor3f(0, 0, 1);
	for (int i = 0; i < 6; i++) {
		glBegin(GL_LINE_STRIP);
		for (int j = 0; j < 6; j++) {
			glVertex3f(ctlpoints[i][j][0], ctlpoints[i][j][1], ctlpoints[i][j][2]);
		}
		glEnd();
	}
	// show control points
	glPointSize(5.0);
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			glVertex3f(ctlpoints[i][j][0], ctlpoints[i][j][1], ctlpoints[i][j][2]);
		}
	}
	glEnd();
	glEnable(GL_LIGHTING);
	glPopMatrix();

	glutSwapBuffers();
}

// main
void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("NURBS playground");
	// callbacks
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	init();
	glutMainLoop();
}
