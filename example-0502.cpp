
/*
Lecture 05
http://slides.com/javiergs/ser431-l05
*/

#include "glut.h"

float triangle_color_R = 1.0f;
float triangle_color_G = 0.843f;
float triangle_color_B = 0.0f;
float corner_x = -0.7f;
float corner_y =  0.7f;
int   mouse_button;
int   mouse_button_state;
int   shake =10;

// myInit
void myInit() {
  glClearColor(0.764f, 0.129f, 0.282f, 1.0); // maroon clear color
  // projection transformations
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // units inside
}

// myDisplay
void myDisplay() {
  glClear(GL_COLOR_BUFFER_BIT); // clear the window
  glColor3f(triangle_color_R, triangle_color_G, triangle_color_B);  // gold
  glBegin(GL_POLYGON);  // fill connected polygon
  glVertex2f(corner_x,  corner_y);  // vertices of the triangle 2 
  glVertex2f(corner_x+1.30,  corner_y);
  glVertex2f(corner_x, corner_y-1.30);
  glEnd();
  glBegin(GL_POLYGON);  // fill connected polygon
  glVertex2f(corner_x+1.40, corner_y-0.1);  // vertices of the triangle 2
  glVertex2f(corner_x+0.1, corner_y-1.40);
  glVertex2f(corner_x+1.40, corner_y-1.40);
  glEnd();
  glutSwapBuffers();  //forces issued commands to execute

}

// myKeyboard
void myKeyboard(unsigned char key, int x, int y) {
  switch (key) {
  case 'q': 
  case 'Q':
    exit(0); // exit the program
    break;
  case 'i': 
  case 'I':
    int mod = glutGetModifiers();
    if (mod == GLUT_ACTIVE_ALT) { // if ALT key
      triangle_color_R = 1.0f;
      triangle_color_G = 1.0f;
      triangle_color_B = 1.0f;
    } else {
      triangle_color_R = 1.0f;
      triangle_color_G = 0.843f;
      triangle_color_B = 0.0f;
    }
    glutPostRedisplay(); // update the display
    break;
  }
}

// mySpecialKeys
void mySpecialKeys(int key, int x, int y) {
  switch (key) {
    case GLUT_KEY_UP: 
      triangle_color_R = triangle_color_R + 0.1;
      break;
    case GLUT_KEY_DOWN:
      triangle_color_R = triangle_color_G - 0.1;
      break;
    case GLUT_KEY_LEFT:
      triangle_color_B = triangle_color_B + 0.1;
      break;
    case GLUT_KEY_RIGHT:
      triangle_color_B = triangle_color_B - 0.1;
      break;
  }
  glutPostRedisplay();
}

// myMouseClick
void myMouseClick(int button, int state, int x, int y) {
  // button: GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON
  // state:  GLUT_DOWN, GLUT_UP
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    corner_x =  (x/250.0) -1; // from 0 : 500 to -1 : 1
    corner_y =- ((y/250.0)-1);
  }
  mouse_button = button;
  mouse_button_state = state;
  glutPostRedisplay();
}

// myMouseMotion
void myMouseMotion(int x, int y) {
  if (mouse_button == GLUT_LEFT_BUTTON && mouse_button_state== GLUT_DOWN) {
    corner_x = (x / 250.0) - 1; // from 0 : 500 to -1 : 1
    corner_y = -((y / 250.0) - 1);
  } else {
    shake=-shake; // shaking triangles
    corner_x = ((x-shake) / 250.0) - 1; // from 0 : 500 to -1 : 1
    corner_y = -(((y-shake) / 250.0) - 1);
  }
  glutPostRedisplay();
}

// main
void main(int argc, char** argv) {
  glutInit(&argc, argv); // glut init
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(500, 500); // actual window size	
  glutInitWindowPosition(0, 0); // window location
  glutCreateWindow("simple");
  myInit();
  glutDisplayFunc(myDisplay);
  glutKeyboardFunc(myKeyboard);
  glutSpecialFunc(mySpecialKeys);
  glutMouseFunc(myMouseClick);
  glutMotionFunc(myMouseMotion);
  glutMainLoop(); //event loop 
}
