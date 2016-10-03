/*
Lecture 10
http://slides.com/javiergs/ser431-l10
*/

#include <vector>
#include "imathvec.h"
#include "glut.h"
#include<iostream>
#include<fstream>
#include<string>

#define PI 3.1415926

using namespace std;
using namespace Imath;

// mesh
typedef Vec3<float> Vec3f;
typedef Vec2<float> Vec2f;

struct Mesh {
  // vertex
  vector<Vec3f> dot_vertex;
  vector<Vec3f> dot_normal;
  vector<Vec2f> dot_texture;
  // faces
  vector<int> face_index_vertex;
  vector<int> face_index_normal;
  vector<int> face_index_texture;
};

// global
int width = 1200;
int height = 600;
float ratio = 1.0;
GLuint displayCylinder, displayPlane, displayCube;

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

/* ---------------------------------------- BEGIN:MESH CODE  ---------------------------------------- */


// create a triangulated box
Mesh* createBox(float w, float h, float d) {
  Mesh *me = new Mesh;
  me->dot_vertex.push_back(Vec3<GLfloat>(0.0, h, d));
  me->dot_vertex.push_back(Vec3<GLfloat>(w, h, d));
  me->dot_vertex.push_back(Vec3<GLfloat>(w, 0.0, d));
  me->dot_vertex.push_back(Vec3<GLfloat>(0.0, 0.0, d));
  me->dot_vertex.push_back(Vec3<GLfloat>(0.0, h, 0.0));
  me->dot_vertex.push_back(Vec3<GLfloat>(w, h, 0.0));
  me->dot_vertex.push_back(Vec3<GLfloat>(w, 0.0, 0.0));
  me->dot_vertex.push_back(Vec3<GLfloat>(0.0, 0.0, 0.0));
  // face 1
  me->face_index_vertex.push_back(0);
  me->face_index_vertex.push_back(1);
  me->face_index_vertex.push_back(2);
  me->face_index_vertex.push_back(0);
  me->face_index_vertex.push_back(2);
  me->face_index_vertex.push_back(3);
  // face 2
  me->face_index_vertex.push_back(0);
  me->face_index_vertex.push_back(1);
  me->face_index_vertex.push_back(5);
  me->face_index_vertex.push_back(0);
  me->face_index_vertex.push_back(5);
  me->face_index_vertex.push_back(4);
  // face 3
  me->face_index_vertex.push_back(1);
  me->face_index_vertex.push_back(2);
  me->face_index_vertex.push_back(6);
  me->face_index_vertex.push_back(1);
  me->face_index_vertex.push_back(6);
  me->face_index_vertex.push_back(5);
  // face 4
  me->face_index_vertex.push_back(2);
  me->face_index_vertex.push_back(3);
  me->face_index_vertex.push_back(7);
  me->face_index_vertex.push_back(2);
  me->face_index_vertex.push_back(7);
  me->face_index_vertex.push_back(6);
  // face 5
  me->face_index_vertex.push_back(0);
  me->face_index_vertex.push_back(3);
  me->face_index_vertex.push_back(7);
  me->face_index_vertex.push_back(0);
  me->face_index_vertex.push_back(7);
  me->face_index_vertex.push_back(4);
  // face 6
  me->face_index_vertex.push_back(4);
  me->face_index_vertex.push_back(5);
  me->face_index_vertex.push_back(6);
  me->face_index_vertex.push_back(4);
  me->face_index_vertex.push_back(6);
  me->face_index_vertex.push_back(7);
  return me;
}

// creating a triangulated plane
Mesh* createPlane(int arena_width, int arena_depth, int arena_cell) {
  Mesh *me = new Mesh;
  int n = arena_width / arena_cell;
  int m = arena_depth / arena_cell;
  // vertices
  for (int i = 0; i<n; i++) {
    for (int j = 0; j < m; j++) {
      me->dot_vertex.push_back(Vec3<GLfloat>(i*arena_cell, 0.0, j*arena_cell));
    }
  }
  // faces
  for (int i = 0; i<(n*m) - m; i++) {
    if ((i + 1) % n == 0) continue;
    me->face_index_vertex.push_back(i); me->face_index_vertex.push_back(i + 1);
    me->face_index_vertex.push_back(i + n);
    me->face_index_vertex.push_back(i + 1); me->face_index_vertex.push_back(i + n + 1);
    me->face_index_vertex.push_back(i + n);
  }
  return me;
}

// create a triangulated cylinder
Mesh* createCylinder(int h, int radio) {
  int n = 100;
  double angle;
  Mesh *m = new Mesh;

  // Generate vertices on top
  m->dot_vertex.push_back(Vec3<GLfloat>(0.0, h, 0.0));
  for (int i = 0; i<n; i++) {
    angle = (M_PI / 180)*(1.0*i * 360) / n;
    m->dot_vertex.push_back(Vec3<GLfloat>(cos(angle)* radio, h, sin(angle)* radio));
  }

  // Generate vertices on bottom
  for (int i = 0; i<n; i++) {
    angle = (M_PI / 180)*(i * 360) / n;
    m->dot_vertex.push_back(Vec3<GLfloat>(cos(angle)* radio, 0.0, sin(angle)* radio));
  }
  // Generate triangles on top
  for (int i = 1; i <= n; i++) {
    m->face_index_vertex.push_back(0); m->face_index_vertex.push_back(i);
    if (i == n) {
      m->face_index_vertex.push_back(1);
    }
    else {
      m->face_index_vertex.push_back(i + 1);
    }
  }
  // There is no bottom. Use the floor.

  // Generate triangles on the side
  for (int i = 1; i <= n; i++) {
    m->face_index_vertex.push_back(i); m->face_index_vertex.push_back(n + i);
    if (i != n) {
      m->face_index_vertex.push_back(n + i + 1);
    }
    else { m->face_index_vertex.push_back(n + 1); }
    m->face_index_vertex.push_back(i);
    if (i != n) {
      m->face_index_vertex.push_back(i + 1); m->face_index_vertex.push_back(n + i + 1);
    }
    else {
      m->face_index_vertex.push_back(1); m->face_index_vertex.push_back(n + 1);
    }
  }
  return m;
}


// draw
GLuint meshToDisplayList(Mesh* m, int id) {
  GLuint listID = glGenLists(id);
  glNewList(listID, GL_COMPILE);
  glBegin(GL_TRIANGLES);
  for (unsigned int i = 0; i < m->face_index_vertex.size(); i++) {
    if (!m->dot_normal.empty() && !m->face_index_normal.empty())
      glNormal3fv(&m->dot_normal[m->face_index_normal[i]].x);
    if (!m->dot_texture.empty() && !m->face_index_texture.empty())
      glTexCoord2fv(&m->dot_texture[m->face_index_texture[i]].x);
    // color
    Vec3f offset = (m->dot_vertex[m->face_index_vertex[i]]);
    //
    glColor3f(fabs(sin(offset.x)), fabs(cos(offset.y)), fabs(offset.z));
    glVertex3fv(&m->dot_vertex[m->face_index_vertex[i]].x);
  }
  glEnd();
  glEndList();
  return listID;
}
/* ----------------------------------------- END:MESH CODE  ----------------------------------------- */

// init
void init() {
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  Mesh* mesh1 = createCylinder(40,10);
  displayCylinder = meshToDisplayList(mesh1,1);
  Mesh* mesh2 = createPlane(1000,  1000, 40);
  displayPlane = meshToDisplayList(mesh2, 2);
  Mesh* mesh3 = createBox(40, 40, 40);
  displayCube = meshToDisplayList(mesh3, 3);
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
  gluLookAt(0.0f, 40.0f, 120.0, 
            0.0f, 1.0f, -1.0f, 
            0.0f, 1.0f, 0.0f);
  // camera
  glScalef(scale, scale, scale);
  glRotatef(x_angle, 1.0f, 0.0f, 0.0f);
  glRotatef(y_angle, 0.0f, 1.0f, 0.0f);
  glTranslatef(0.0f, 0.0f, 0.0f);
  // draw ground
  //glColor3f(0.9f, 0.9f, 0.9f);
  //glBegin(GL_QUADS);
  //glVertex3f(-100.0f, 0.0f, -100.0f);
  //glVertex3f(-100.0f, 0.0f, 100.0f);
  //glVertex3f(100.0f, 0.0f, 100.0f);
  //glVertex3f(100.0f, 0.0f, -100.0f);
  //glEnd();
  // draw the mesh: plane, cube, cylinder
  glPushMatrix();
    glTranslatef(-500,0,-500);
    glCallList(displayPlane);
  glPopMatrix();
  glPushMatrix();
    glTranslatef(40, 0, -40);
    glCallList(displayCube);
  glPopMatrix();
  glCallList(displayCylinder);

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
  glutCreateWindow("Mesh Data Structure");
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  init();
  glutMainLoop();
}
