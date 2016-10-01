/*
Lecture 07
http://slides.com/javiergs/ser431-l07
*/

#include "glut.h"

int defaultHeight = 500;
int defaultWidth = 500;

void reshapeWindow(int width, int height) {
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  float aspectRatio = (float)width / (float)height;
  if (width >= height) {
    gluOrtho2D(-defaultWidth*aspectRatio, defaultWidth*aspectRatio, -defaultHeight, defaultHeight);
  }
  else {
    gluOrtho2D(-defaultWidth, defaultWidth, -defaultHeight / aspectRatio, defaultHeight / aspectRatio);
  }
}

void init() {
  glClearColor(0.0, 1.0, 0.0, 0.0);
  glColor3f(1.0, 1.0, 1.0);
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);
  glShadeModel(GL_FLAT); // SMOOTH or FLAT
  glBegin(GL_TRIANGLES);
  glColor3f(1.0, 0.0, 0.0); glVertex2f(0.0f, 0.0f);
  glColor3f(0.0, 1.0, 0.0); glVertex2f(50.0f, 50.0f);
  glColor3f(0.0, 0.0, 1.0); glVertex2f(50.0f, 0.0f);
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
  glutReshapeFunc(reshapeWindow);
  glutMainLoop();
  return 0;
}
