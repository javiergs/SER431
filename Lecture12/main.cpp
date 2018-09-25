#include <stdlib.h>
#include <GL/glut.h>
#include "timer.h"
#include "core.h"
#include "particles.h"

// draw particles
void drawParticles() {
	Particle* curr = ps.particle_head;
	// glPointSize(2);
	// glBegin(GL_POINTS);
	// while (curr) {
	//   glVertex3fv(curr->position);
	//	 curr = curr->next;
	// }
	// glEnd();
	while (curr) {
	  glPushMatrix();
	  glScalef(100.0, 100.0, 100.0);
	  glTranslatef(curr->position[0], curr->position[1], curr->position[2]);
	  glScalef(0.001, 0.001, 0.001);
  	    glCallList(display3);
	  glPopMatrix();
      curr = curr->next;
	}

}

// display
void display() {
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
	gluLookAt(0.0f, 40.0f, 320.0,
		0.0f, 1.0f, -1.0f,
		0.0f, 1.0f, 0.0f);
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
	// Particles with box 1
	ps.add();
	float time = calculate_frame_time();
	ps.update(time);
	ps.remove();
	glPushMatrix();
	drawParticles();
	glPopMatrix();
	
	// box 2
	glPushMatrix();
	glTranslatef(200, 0, 0);
	glCallList(display3);
	glPopMatrix();
	// box 3
	glPushMatrix();
	glTranslatef(-200, 0, 0);
	glCallList(display4);
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

// reshape
void reshape(int w, int h) {
	width = w;
	height = h;
	if (h == 0) h = 1;
	ratio = 1.0f * w / h;
}

// init
void init() {
  // particles
  init_frame_timer();

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
	display3 = meshToDisplayList(mesh3, 3, 1);
	display4 = meshToDisplayList(mesh4, 4, 1);
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

void main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(width, height);
	glutCreateWindow("Particles");
	init();
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutMainLoop();
}
