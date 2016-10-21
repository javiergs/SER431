/*
Lecture 13
http://slides.com/javiergs/ser431-l13
*/

#include "oo_unit.h"

#define PI 3.1415926
using namespace std;
using namespace Imath;

// mesh
typedef Vec3<float> Vec3f;
typedef Vec2<float> Vec2f;


// global
int width = 1200;
int height = 600;
float ratio = 1.0;
GLuint displayDiamond, displayPlane, displayCar;
GLuint boxDiamond, boxCar;

// controling parameters
int mouse_button;
int mouse_x = 0;
int mouse_y = 0;
float scale = 1.0;
float x_angle = 0.0;
float y_angle = 0.0;

// global moving
float angle = 0;
Vec3f moving_position = Vec3f(0, 0, 0);
Vec3f center_position = Vec3f(0, 0, 0);

// object rotating
float moveX = 0;
float moveZ = 0;
float tick = 1.6;
float radius = 9;
float angle_animated_object = 0;
float line_animated_object = 0;
float line_next_move =+0.5;

Unit * plane = new Unit();
Unit * car = new Unit();
Unit * diamond = new Unit();

// init
void init() {
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  // diamond
  diamond->mesh->createDiamond();
  displayDiamond = diamond->mesh->meshToDisplayList(1);
  diamond->mesh->calculateAABB();
  // floor
  plane->mesh->createPlane(1000, 1000, 40);;
  displayPlane = plane->mesh->meshToDisplayList(2);
  // car
  car->mesh->loadFile("../obj files/porsche.obj");
  displayCar = car->mesh->meshToDisplayList(3);
  car->mesh->calculateAABB();
  // radio
  ratio = (double)width / (double)height;
}

// reshape
void reshape(int w, int h) {
  width = w;
  height = h;
  if (h == 0) h = 1;
  ratio = 1.0f * w / h;
}

// mouse
void mouse(int button, int state, int x, int y) {
  mouse_x = x;
  mouse_y = y;
  mouse_button = button;
}

// motion
void motion(int x, int y) {
  if (mouse_button == GLUT_LEFT_BUTTON) {
    y_angle += (float(x - mouse_x) / width) *360.0;
    x_angle += (float(y - mouse_y) / height)*360.0;
  }
  if (mouse_button == GLUT_RIGHT_BUTTON) {
    scale += (y - mouse_y) / 100.0;
    if (scale < 0.1) scale = 0.1;
    if (scale > 7)	scale = 7;
  }
  mouse_x = x;
  mouse_y = y;
  glutPostRedisplay();
}

// moving
void move() {
  // rotate diamond
  angle = angle + 0.1;
  if (angle > 360) angle -= 360;
  moving_position.x = center_position.x + 8.0f * sin(angle * PI / 180);
  moving_position.z = center_position.z + 8.0f * cos(angle * PI / 180);
  glutPostRedisplay();
}

// text
void renderBitmapString(float x, float y, float z, char *string) {
  char *c;
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
  glViewport(0, 0, width, height);
  gluPerspective(45, ratio, 1, 1000);
  // view
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  // lookAt fijo
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
    glTranslatef(-500,0,-500);
    glCallList(displayPlane);
  glPopMatrix();
  // diamond moving
  glPushMatrix();
    if (line_animated_object <= -460) line_next_move = +0.5;
    if (line_animated_object >= 460) line_next_move = -0.5;
    line_animated_object += line_next_move;
    glTranslatef(line_animated_object, 0, 0);
    glScalef(40, 40, 40);
    glCallList(displayDiamond);
    // keep a record of actions
    diamond->scale=40;
    diamond->position.x = line_animated_object;
  glPopMatrix();
  glPushMatrix();
    diamond->mapping();
    boxDiamond = diamond->boxToDisplayList(4);
    glCallList(boxDiamond);
  glPopMatrix();
  // car moving
  if (!car->intersect(diamond)) {
    glPushMatrix();
      if (angle_animated_object<=-360) angle_animated_object=0; 
      angle_animated_object-=0.5;
      glRotatef(angle_animated_object, 0,1,0);
      glTranslatef(200, 20, 0);
      glCallList(displayCar);
    glPopMatrix();
    // keep a record of actions
    car->position.x = 200;
    car->position.y = 20;
    car->rotateAngle = angle_animated_object;
    glPushMatrix();
      car->mapping();
      boxCar = car->boxToDisplayList(4);
      glCallList(boxCar);
    glPopMatrix();
  }
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
  move();
}

// main
void main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(width, height);
  glutCreateWindow("Collision Detection");
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutIdleFunc(display);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  init();
  glutMainLoop();
}
