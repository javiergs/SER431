/*
Lecture 08
http://slides.com/javiergs/ser431-l08
*/

#include "glut.h"

GLuint drawDL;

//createDisplayList
void createDisplayList() {
  drawDL = glGenLists(1);
  glNewList(drawDL, GL_COMPILE);
  glBegin(GL_POLYGON);  
    glVertex2f(-0.7, 0.7); 
    glVertex2f(0.6, 0.7);
    glVertex2f(-0.7, -0.6);
  glEnd();
  glEndList();
}

// myInit
void myInit() {
  createDisplayList();
  glClearColor(0.764f, 0.129f, 0.282f, 1.0); // maroon clear color
  glColor3f(1.0f, 0.843f, 0.0f);  // gold
  // projection transformations
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // units inside
}

// myDisplay
void myDisplay() {
  glClear(GL_COLOR_BUFFER_BIT); 
  // compare this method with the one in example 01 from Lecture 05.
  glCallList(drawDL);	 // call the list
  glTranslatef(0.05f, 0.05f, 0.0f);
  glRotatef(180, 0.0, 0.0, 1.0);
  glCallList(drawDL);	 // call the list
  glFlush();  //forces issued commands to execute
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
