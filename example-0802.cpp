/*
Lecture 08
http://slides.com/javiergs/ser431-l08
*/

#include "glut.h"

float r=0, g=0, b=0;
float position_y=0;

// menuListener
void menuListener(int option) {
  switch (option) {
  case 1:
    r = 1.0f; g=0.0f; b=0.0f; break;
  case 2:
    r = 0.0f; g = 0.0f; b = 1.0f; break;
  case 3:
    r = 0.0f; g = 1.0f; b = 0.0f; break;
  case 4:
    r = 1.0f; g = 1.0f; b = 1.0f; break;
  case 5:
    position_y = 0.3f; break;
  case 6:
    position_y = 0.0f; break;
  case 7:
    position_y = -0.3f; break;
  }
  glutPostRedisplay();
}

// createMenus
void createMenus() {
  //add entries to submenu Colores
  int menuA = glutCreateMenu(menuListener);
  glutAddMenuEntry("Red", 1);
  glutAddMenuEntry("Blue", 2);
  glutAddMenuEntry("Green", 3);
  glutAddMenuEntry("White", 4);
  //add entries to submenu Position
  int menuB = glutCreateMenu(menuListener);
  glutAddMenuEntry("Top", 5);
  glutAddMenuEntry("Center", 6);
  glutAddMenuEntry("Down", 7);
  // create main menu
  int menu = glutCreateMenu(menuListener);
  glutAddSubMenu("Colores", menuA);
  glutAddSubMenu("Position", menuB);
  // attach the menu to the right button
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// myInit
void myInit() {
  // create menu
  createMenus();
  glClearColor(0.764f, 0.129f, 0.282f, 1.0);
  // projection transformations
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
}

// myDisplay
void myDisplay() {
  glClear(GL_COLOR_BUFFER_BIT); 
  glColor3f(r, g, b);
  glBegin(GL_POLYGON);  
  glVertex2f(-0.7, position_y + 0.7);  
  glVertex2f(0.6, position_y + 0.7);
  glVertex2f(-0.7, position_y + -0.6);
  glEnd();
  glBegin(GL_POLYGON);  
  glVertex2f(0.7, position_y + 0.6);
  glVertex2f(-0.6, position_y + -0.7);
  glVertex2f(0.7, position_y + -0.7);
  glEnd();
  glFlush();  
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
