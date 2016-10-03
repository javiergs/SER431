#include <stdio.h>
#include <math.h>
#include "glut.h"

float angle = 0.0;
float x = 0.0f, y = 1.75f, z = 5.0f;
float lx = 0.0f, ly = 0.0f, lz = -1.0f;
float ratio = 1.0;
int width, height;
GLuint drawList;

// font strokes
void renderStrokeString
(float x, float y, float z, float scale, void *font, char *string) {
  char *c;
  glPushMatrix();
  glTranslatef(x, y, z);   // fonts position
  glScalef(scale, scale, scale);
  for (c = string; *c != '\0'; c++)
    glutStrokeCharacter(font, *c);
  glPopMatrix();
}

// font bitmaps
void renderBitmapString
(float x, float y, float z, void *font, char *string) {
  char *c;
  glRasterPos3f(x, y, z);   // fonts position
  for (c = string; *c != '\0'; c++)
    glutBitmapCharacter(font, *c);
}

// drawingList
void drawingList() {
  GLuint pyramidDL = glGenLists(1);
  // one pyramid
  glNewList(pyramidDL, GL_COMPILE);
  glBegin(GL_TRIANGLES);
  //face1
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(0, 1, 0);
  glVertex3f(1, 0, 1);
  glVertex3f(-1, 0, 1);
  //face2
  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(0, 1, 0);
  glVertex3f(-1, 0, 1);
  glVertex3f(-1, 0, -1);
  //face3
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(0, 1, 0);
  glVertex3f(-1, 0, -1);
  glVertex3f(1, 0, -1);
  //face4
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(0, 1, 0);
  glVertex3f(1, 0, -1);
  glVertex3f(1, 0, 1);
  glEnd();
  glEndList();
  drawList = pyramidDL;
}

// reshape
void myReshape(int w, int h) {
  width = w;
  height = h;
  if (h == 0) h = 1;
  ratio = 1.0f * w / h;
}

// init
void myInit() {
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  drawingList();
}

// display
void myDisplay(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // projection
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, width, height / 10);
  glOrtho(-10, 10, -10, 10, -10, 10);
  // view
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glColor3f(1, 1, 1);
  renderBitmapString(-10.0, 0.0, 0.0, GLUT_BITMAP_8_BY_13, "Hello with Bitmaps");
  // projection
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, height / 10, width, height);
  gluPerspective(45, ratio, 1, 1000);
  // view
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(x, y, z, x + lx, y + ly, z + lz, 0.0f, 1.0f, 0.0f);
  // Draw ground
  glColor3f(0.9f, 0.9f, 0.9f);
  glBegin(GL_QUADS);
  glVertex3f(-100.0f, 0.0f, -100.0f);
  glVertex3f(-100.0f, 0.0f, 100.0f);
  glVertex3f(100.0f, 0.0f, 100.0f);
  glVertex3f(100.0f, 0.0f, -100.0f);
  glEnd();
  // Draw Pyramid
  glCallList(drawList);
  // Draw Stoke Fonts
  glColor3f(1, 1, 1);
  glLineWidth(5);
  renderStrokeString(-10.0, 1.0, -10.0, 0.01, GLUT_STROKE_ROMAN, "Hello with Stroke Fonts");

  glutSwapBuffers();
}

void orientMe(float ang) {
  lx = sin(ang); lz = -cos(ang);
}

void moveMeFlat(int i) {
  x = x + i*(lx)*0.1;
  z = z + i*(lz)*0.1;
}

// mySpecial
void mySpecial(int key, int x, int y) {
  switch (key) {
  case GLUT_KEY_UP:
    moveMeFlat(1);
    break;
  case GLUT_KEY_DOWN:
    moveMeFlat(-1);
    break;
  case GLUT_KEY_LEFT:
    angle -= 0.01f;
    orientMe(angle);
    break;
  case GLUT_KEY_RIGHT:
    angle += 0.01f;
    orientMe(angle);
    break;
  }
  glutPostRedisplay();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(800, 800);
  glutCreateWindow("Fonts");
  myInit();
  glutSpecialFunc(mySpecial);
  glutDisplayFunc(myDisplay);
  glutReshapeFunc(myReshape);
  glutMainLoop();
  return(0);
}
