/*
Lecture 05
http://slides.com/javiergs/ser431-l05
*/

#include "glut.h"

// myDisplay
void myDisplay() {
  glClear(GL_COLOR_BUFFER_BIT); // clear the window
  glBegin(GL_POLYGON);  // fill connected polygon
  glVertex2f(0.0, 0.0);  // vertices of the square 
  glVertex2f(0.5, 0.5);
  glVertex2f(0.5, 0.0);
  glEnd();
  glBegin(GL_POLYGON);  // fill connected polygon
  glVertex2f(0.0, 0.0);  // vertices of the square 
  glVertex2f(-0.5, -0.5);
  glVertex2f(-0.5, 0.0);
  glEnd();
  glFlush();  //forces issued commands to execute
}

// myInit
void myInit() {
  glClearColor(1.0, 0.0, 0.0, 1.0); // red clear color
  glColor3f(1.0, 1.0, 1.0);  // white
  // projection transformations
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // units inside
}

// main
void main(int argc, char** argv) {
  glutInit(&argc, argv); // glut init
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(500, 500); // actual window size	
  glutInitWindowPosition(0, 0); // window location
  glutCreateWindow("simple");
  myInit();
  glutDisplayFunc(myDisplay);
  glutMainLoop(); //event loop 
}
