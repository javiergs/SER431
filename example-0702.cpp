#include "glut.h"

void init() {
  glClearColor(0.0, 1.0, 0.0, 0.0);
  glColor3f(1.0, 1.0, 1.0);
}

void display() {

  glViewport(0, 0, 250, 250); // the first viewport
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
  glShadeModel(GL_FLAT); // SMOOTH or FLAT
  glBegin(GL_TRIANGLES);
  glColor3f(1.0, 0.0, 0.0); glVertex2f(0.0f, 0.0f);
  glColor3f(0.0, 1.0, 0.0); glVertex2f(0.5f, 0.5f);
  glColor3f(0.0, 0.0, 1.0); glVertex2f(0.5f, 0.0f);
  glEnd();

  glViewport(250, 250, 250, 250); // the second viewport
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
  glShadeModel(GL_SMOOTH); // SMOOTH or FLAT
  glBegin(GL_TRIANGLES);
  glColor3f(1.0, 0.0, 0.0); glVertex2f(0.0f, 0.0f);
  glColor3f(0.0, 1.0, 0.0); glVertex2f(-0.5f, -0.5f);
  glColor3f(0.0, 0.0, 1.0); glVertex2f(-0.5f, 0.0f);
  glEnd();

  glFlush();    //forces previously issued commands to execute
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(0, 0);
  glutCreateWindow("simple");
  init();
  glutDisplayFunc(display);
  glutMainLoop();
  return 0;
}
