/**
 * SER 431
 * https://speakerdeck.com/javiergs/ser431-lecture-04
 **/

#include <GL/glut.h>
#include <fstream>
#include "mesh.h"
#include "texture.h"
#include "render.h"
#include "controls.h"

// global
Mesh *mesh1, *mesh2, *mesh3, *mesh4, *mesh5;
GLuint display1, display2, display3, display4, display5;
GLuint textures[5];

// init
void init() {
	// mesh
	mesh1 = createPlane(2000, 2000, 200);
	mesh5 = createSkyBox(6000);
	mesh2 = createCube();
	mesh3 = createCube();
	mesh4 = createCube();
	
	// normals
	calculateNormalPerFace(mesh1);
	calculateNormalPerFace(mesh2);
	calculateNormalPerFace(mesh3);
	calculateNormalPerFace(mesh4);
	calculateNormalPerFace(mesh5);
	calculateNormalPerVertex(mesh1);
	calculateNormalPerVertex(mesh2);
	calculateNormalPerVertex(mesh3);
	calculateNormalPerVertex(mesh4);
	calculateNormalPerVertex(mesh5);
	
	// textures
	loadBMP_custom(textures, "../../BMP files/brick.bmp", 0);
	loadBMP_custom(textures, "../../BMP files/oldbox.bmp", 1);
	codedTexture(textures, 2, 0); //Sky texture - noise multiscale. Type=0
	codedTexture(textures, 3, 1); //Marble texture - noise marble. Type=1
	loadBMP_custom(textures, "../../BMP files/cubesky.bmp", 4);

	// display lists
	display1 = meshToDisplayList(mesh1, 1, textures[0]);
	display2 = meshToDisplayList(mesh2, 2, textures[1]);
	display3 = meshToDisplayList(mesh3, 3, textures[2]);
	display4 = meshToDisplayList(mesh4, 4, textures[3]);
	display5 = meshToDisplayList(mesh5, 5, textures[4]);
	
	// configuration
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	
	// light
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	GLfloat light_ambient[]  = { 0.5, 0.5, 0.5, 1.0 };
	GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

// reshape
void reshape(int w, int h) {
	window_width = w;
	window_height = h;
}

// text
void renderBitmapString(float x, float y, float z, const char *string) {
	const char *c;
	glRasterPos3f(x, y, z);   // fonts position
	for (c = string; *c != '\0'; c++)
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
}

// display
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glViewport(0, 0, window_width, window_height);
	gluPerspective(45, window_ratio, 10, 100000);
	// view
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	// lookAt
	// gluLookAt(0.0f, 40.0f, 320.0,	0.0f, 1.0f, -1.0f,		0.0f, 1.0f, 0.0f);

	gluLookAt(camera_x, camera_y, camera_z, camera_viewing_x, camera_viewing_y, camera_viewing_z,		0.0f, 1.0f, 0.0f);
	// camera
	//glScalef(scale, scale, scale);
	//glRotatef(x_angle, 1.0f, 0.0f, 0.0f);
	//glRotatef(y_angle, 0.0f, 1.0f, 0.0f);
	//glTranslatef(0.0f, 0.0f, 0.0f);

	//plane
	glPushMatrix();
	glTranslatef(-1000, 200, -1000);
	glCallList(display1);
	glPopMatrix();
	// box 1
	glPushMatrix();
	glTranslatef(0, 300, 0);
	glCallList(display2);
	glPopMatrix();
	// box 2
	glPushMatrix();
	glTranslatef(200, 300, 0);
	glCallList(display3);
	glPopMatrix();
	// box 3
	glPushMatrix();
	glTranslatef(-200, 300, 0);
	glCallList(display4);
	glPopMatrix();
	// end
	// skybox
	glPushMatrix();
	glTranslatef(-1500, -1, -1500);
	glCallList(display5);
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
	gluOrtho2D(0, window_width, 0, window_height);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glColor3f(1.0, 1.0, 1.0);
	renderBitmapString(0.0, window_height - 13.0f, 0.0f, "Use [Arrows] to move in plain");
	renderBitmapString(0.0, window_height - 26.0f, 0.0f, "Use [W and S] to look up and down");
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glutSwapBuffers();
}

// rotate what the user see
void rotate_point(float angle) {
	float s = sin(angle);
	float c = cos(angle);
	// translate point back to origin:
	camera_viewing_x -= camera_x;
	camera_viewing_z -= camera_z;
	// rotate point
	float xnew = camera_viewing_x * c - camera_viewing_z * s;
	float znew = camera_viewing_x * s + camera_viewing_z * c;
	// translate point back:
	camera_viewing_x = xnew + camera_x;
	camera_viewing_z = znew + camera_z;
}

// callback function for keyboard (alfanumeric keys)
void callbackKeyboard(unsigned char key, int x, int y) {
	switch (key) {
		case 'w': case 'W':
			camera_viewing_y += (10);
			break;
		case 's': case 'S':
			camera_viewing_y -= (10);
			break;
	}
}

// callback function for arrows
void specialkeys(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT) {
		total_moving_angle += -0.01;
		rotate_point(-0.01);
	} else if (key == GLUT_KEY_RIGHT) {
		total_moving_angle += 0.01; 
		rotate_point(0.01);
	} else if (key == GLUT_KEY_DOWN) {
		//printf("Down key is pressed\n");
		//camera_z += 10;
		camera_x += (-10) * sin(total_moving_angle);//*0.1;
		camera_z += (-10) * -cos(total_moving_angle);//*0.1;
		//camera_viewing_y -= 10;
		camera_viewing_x += (-10) * sin(total_moving_angle);//*0.1;
		camera_viewing_z += (-10) * -cos(total_moving_angle);//*0.1;
	} else if (key == GLUT_KEY_UP) {
		//printf("Up key is pressed\n");
		//camera_z -= 10;
		camera_x += (10) * sin(total_moving_angle);//*0.1;
		camera_z += (10) * -cos(total_moving_angle);//*0.1;
		camera_viewing_x += (10) * sin(total_moving_angle);//*0.1;
		camera_viewing_z += (10) * -cos(total_moving_angle);//*0.1;
		//camera_viewing_y += 10;
	}
}

// main
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("Sky Box");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	
	glutSpecialFunc(specialkeys);
	glutKeyboardFunc(callbackKeyboard);
	init();
	glutMainLoop();
	return 0;
}
