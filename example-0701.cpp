/*
Lecture 07
http://slides.com/javiergs/ser431-l07
*/

#include "glut.h"

int year = 0, day = 0;

// myDisplay
void myDisplay(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(1.0, 1.0, 1.0);
  glPushMatrix();
  /* draw 1 */
  glBegin(GL_POLYGON);  // fill connected polygon
  glColor3f(1.0f, 0.843f, 0.0f); 
  glVertex2f(-0.7, 0.7);  // vertices of the triangl 1
  glVertex2f(0.6, 0.7);
  glVertex2f(-0.7, -0.6);
  glEnd();
  /* draw 2 */
  glRotatef((GLfloat)year, 0.0, 0.0, 1.0);
  glTranslatef(2.0, 0.0, 0.0);
  glRotatef((GLfloat)day, 0.0, 1.0, 0.0);
  glBegin(GL_POLYGON); 
  glColor3f(1.0f, 0.843f, 0.0f); 
  glVertex2f(0.7, 0.6);  // vertices of the triangle 2 
  glVertex2f(-0.6, -0.7);
  glVertex2f(0.7, -0.7);
  glEnd();
  glPopMatrix();
  glFlush();
}

// myInit
void myInit() {
  glClearColor(0.764f, 0.129f, 0.282f, 1.0); // maroon clear color
 // projection transformations
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-5.0, 5.0, -5.0, 5.0); // units inside
}

// myKeyboard
void myKeyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'd':
      day = (day + 10) % 360;
      glutPostRedisplay();
      break;
    case 'D':
      day = (day - 10) % 360;
      glutPostRedisplay();
      break;
    case 'y':
      year = (year + 5) % 360;
      glutPostRedisplay();
      break;
    case 'Y':
      year = (year - 5) % 360;
      glutPostRedisplay();
      break;
    default:
      break;
  }
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
  glutKeyboardFunc(myKeyboard);
  glutMainLoop(); //event loop 
}
