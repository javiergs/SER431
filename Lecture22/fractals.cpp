#include <vector>
#include "Imath/imathvec.h"
#include "GL/glut.h"
#include<string>
#include<fstream>
#include <math.h>
#include <limits.h>           
#include <float.h>          
#include <time.h>
#include "common.h"

#define random_number() (((float) rand())/((float) RAND_MAX))
#define MAXLEVEL 8
int Level = 4;
long TreeSeed;
int TREE = 0, STEM = 1, LEAF = 2, LEAF_MAT = 3, TREE_MAT = 4, STEMANDLEAVES = 5, FULLTREE = 10;

// Use in thunderbolt
class Line {
	public:
		float x1, y1, x2, y2, z1,z2;
		Line(float a, float b, float c, float d, float e, float f) {
			x1 = a; y1 = b; z1 = c;  x2 = d; y2 = e; z2 = f;
		}
};
vector<Line> thunderbolt;

// calculate middle points with a displacement for the thunderbolt
Vec3f calculateMiddle(Vec3f p1, Vec3f p2, int level) {
	Vec3f O = (p2+p1)/2;	
	float m1 = (p2.y - p1.y) / (p2.x - p1.x);
	int signo = (random_number()>0.5) ? 1 : -1;
	float x = O.x + signo*((cos(PI/2 + atan(m1)))/ pow(2, level)); // higher level, small displacement
	float y = O.y + signo*((sin(PI/2 + atan(m1)))/ pow(2, level));
	return Vec3f(x, y, signo*random_number() / pow(2, level));
}

// create thunderbolt as fractal
void createBolt(Vec3f p1, Vec3f p2, int level) {
	thunderbolt.push_back(Line(p1.x,p1.y,p1.z, p2.x,p2.y, p2.z));
	// printf("* \t INSERT-A (%f, %f, %f) -- (%f, %f, %f)\n\n", p1.x, p1.y, p1.z, p2.x, p2.y, p2.z);
	for (int t = 0; t < level; t++) {
		int tam = thunderbolt.size();
		Vec3f middle; int i;
		for (i = 0; i < tam; i++) {
			p1.x = thunderbolt[0].x1; p1.y = thunderbolt[0].y1; p1.z = thunderbolt[0].z1; 
			p2.x = thunderbolt[0].x2; p2.y = thunderbolt[0].y2; p2.z = thunderbolt[0].z2;
			thunderbolt.erase(thunderbolt.begin()); 
			// printf("%i \t DELETE-- (%f, %f) -- (%f, %f)\n", i,p1.x, p1.y, p2.x, p2.y);
			middle = calculateMiddle(p1, p2, t);
			thunderbolt.push_back(Line(p1.x, p1.y, p1.z, middle.x,middle.y, middle.z));
			thunderbolt.push_back(Line(middle.x, middle.y, middle.z, p2.x,p2.y, p2.z));
			// printf("%i \t Middle level %d)\n", i, t);
			// printf("%i \t INSERT-A (%f, %f, %f) -- (%f, %f, %f)\n", i, p1.x, p1.y, p1.z, middle.x, middle.y, middle.z);
			// printf("%i \t INSERT-B (%f, %f, %f) -- (%f, %f, %f)\n", i, middle.x, middle.y, middle.z, p2.x, p2.y, p2.z);
		}
		// extension line
		Vec3f direction = middle - p1;
		Vec3f pin = middle + direction * 0.7;
		thunderbolt.push_back(Line(middle.x, middle.y, middle.z, pin.x, pin.y, pin.z));
		//printf("%i \t INSERT-C (%f, %f, %f) -- (%f, %f, %f)\n\n", i, middle.x, middle.y, middle.z, pin.x, pin.y, pin.z);	
	}

}

// create a tree as fractal
void FractalTree(int level) {
	long savedseed;
	if (level == Level) {
		glPushMatrix();
		glRotatef(random_number() * 180, 0, 1, 0);
		glCallList(STEMANDLEAVES);
		glPopMatrix();
	} else {
		glCallList(STEM);
		glPushMatrix();
		glRotatef(random_number() * 180, 0, 1, 0);
		glTranslatef(0, 1, 0);
		glScalef(0.7, 0.7, 0.7);
		glPushMatrix();
		glRotatef(110 + random_number() * 40, 0, 1, 0);
		glRotatef(30 + random_number() * 20, 0, 0, 1);
		FractalTree(level + 1);
		glPopMatrix();

		glPushMatrix();
		glRotatef(-130 + random_number() * 40, 0, 1, 0);
		glRotatef(30 + random_number() * 20, 0, 0, 1);
		FractalTree(level + 1);
		glPopMatrix();

		glPushMatrix();
		glRotatef(-20 + random_number() * 40, 0, 1, 0);
		glRotatef(30 + random_number() * 20, 0, 0, 1);
		FractalTree(level + 1);
		glPopMatrix();
		glPopMatrix();
	}
}

// Create display lists for a leaf, a set of leaves, and a stem
void CreateTreeLists(void) {
	GLUquadricObj *cylquad = gluNewQuadric();
	int i;
	// materials
	GLfloat tree_ambuse[] = { 0.4, 0.25, 0.1, 1.0 };
	GLfloat tree_specular[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat tree_shininess[] = { 0 };
	GLfloat leaf_ambuse[] = { 0.0, 0.8, 0.0, 1.0 };
	GLfloat leaf_specular[] = { 0.0, 0.8, 0.0, 1.0 };
	GLfloat leaf_shininess[] = { 10 };
	// tree material
	glNewList(TREE_MAT, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tree_ambuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, tree_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, tree_shininess);
	glEndList();
	// leaf material
	glNewList(LEAF_MAT, GL_COMPILE);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, leaf_ambuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, leaf_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, leaf_shininess);
	glEndList();
	// steam
	glNewList(STEM, GL_COMPILE);
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	gluCylinder(cylquad, 0.1, 0.08, 1, 10, 2);
	glPopMatrix();
	glEndList();
	// leaf
	glNewList(LEAF, GL_COMPILE);
	glBegin(GL_TRIANGLES);
	glNormal3f(-0.1, 0, 0.25);
	glVertex3f(0, 0, 0);
	glVertex3f(0.25, 0.25, 0.1);
	glVertex3f(0, 0.5, 0);
	glNormal3f(0.1, 0, 0.25);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0.5, 0);
	glVertex3f(-0.25, 0.25, 0.1);
	glEnd();
	glEndList();
	// steam&leaf
	glNewList(STEMANDLEAVES, GL_COMPILE);
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glCallList(STEM);
	glCallList(LEAF_MAT);
	for (i = 0; i < 3; i++) {
		glTranslatef(0, 0.333, 0);
		glRotatef(90, 0, 1, 0);
		glPushMatrix();
		glRotatef(0, 0, 1, 0);
		glRotatef(50, 1, 0, 0);
		glCallList(LEAF);
		glPopMatrix();
		glPushMatrix();
		glRotatef(180, 0, 1, 0);
		glRotatef(60, 1, 0, 0);
		glCallList(LEAF);
		glPopMatrix();
	}
	glPopAttrib();
	glPopMatrix();
	glEndList();
	//
	glNewList(FULLTREE, GL_COMPILE);
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glCallList(TREE_MAT);
	glTranslatef(0, -1, 0);
	FractalTree(0);
	glPopAttrib();
	glPopMatrix();
	glEndList();
}

// create a pile of boxes as fractal
void stackBoxes(float x, float y, float z) {
		if (y >= 0) {
			glPushMatrix();
		  	glTranslatef(x,y*100, z*100/2);
			  glCallList(display2); 
			glPopMatrix();		
			stackBoxes(x-50, y-1, z-1);
			stackBoxes(x-50, y-1, z+1);
			stackBoxes(x+50, y-1, z-1);
			//stackBoxes(x+50, y-1, z+1);
		}
}

// reshape
void reshape(int w, int h) {
	width = w;
	height = h;
	if (h == 0) h = 1;
	ratio = 1.0f * w / h;
}

// init
void init() {
	// init thunderbolt
	srand(time(0));
	createBolt(Vec3f(5, 5, 0), Vec3f(0, 0, 0), 4);
	// init tree
	CreateTreeLists();
	glEnable(GL_NORMALIZE);
	glEnable(GL_AUTO_NORMAL);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	ratio = (double)width / (double)height;
	// mesh
	mesh1 = createPlane(2000, 2000, 200);
	mesh2 = createCube();
	mesh3 = createCube();
	mesh4 = createCube();
	calculateNormalPerFace(mesh1);
	calculateNormalPerFace(mesh2);
	calculateNormalPerFace(mesh3);
	calculateNormalPerFace(mesh4);
	calculateNormalPerVertex(mesh1);
	calculateNormalPerVertex(mesh2);
	calculateNormalPerVertex(mesh3);
	calculateNormalPerVertex(mesh4);
	// textures
	bmpTexture(textureArray, "../../BMP files/brick.bmp", 0);
	bmpTexture(textureArray, "../../BMP files/oldbox.bmp", 1);	
	display1 = meshToDisplayList(mesh1, 1, 0);
	display2 = meshToDisplayList(mesh2, 2, 1);
	// light
	GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
}

// display
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glViewport(0, 0, width, height);
	gluPerspective(45, ratio, 1, 1000);
	// view
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	// lookAt
	gluLookAt(0.0f, 70.0f, 320.0, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f);
	// camera
	glScalef(scale, scale, scale);
	glRotatef(x_angle, 1.0f, 0.0f, 0.0f);
	glRotatef(y_angle, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.0f, 0.0f, 0.0f);
	//plane
	glPushMatrix();
	glTranslatef(-900, 0, -900);
	glCallList(display1);
	glPopMatrix();
	// box 1
	glPushMatrix();
	glCallList(display2);
	glPopMatrix();
	// fractal
	glPushMatrix();
	glTranslatef(-300, 0, -300);
	glScalef (0.5, 0.5, 0.5);
	stackBoxes(0, 5, 0);
	glPopMatrix();
	// tree fractal
	glPushMatrix();
	glTranslatef(300, 100, 300);
	glScalef(100, 100, 100);
	glCallList(FULLTREE);
	glPopMatrix();
	// bolt ----------------------------------------
	glPushMatrix();
	glTranslatef(-300,0,300);
	glScalef(100, 100, 100);
	int size = thunderbolt.size();
	int in_gray = (size / 2);
	int counter = 0;
	for (Line line : thunderbolt) {
		glBegin(GL_QUADS);
			glVertex3f(line.x1, line.y1, line.z1);
			glVertex3f(line.x1+0.1, line.y1, line.z1 + 0.1);
			glVertex3f(line.x2, line.y2, line.z2);
			glVertex3f(line.x2 +0.1, line.y2, line.z2 + 0.1);
		glEnd();
		counter++;
	}
	glPopMatrix();
	// end
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	// texto
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	renderBitmapString(0.0, height - 13.0f, 0.0f, "Use [Mouse Left Key] to rotate");
	renderBitmapString(0.0, height - 26.0f, 0.0f, "Use [Mouse Right Key] to scale");
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glutSwapBuffers();
}

// main
void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("Fractals");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	init();
	glutMainLoop();
}
