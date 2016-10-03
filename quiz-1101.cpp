/*
Quiz 1 - Lectures 1- 10
http://slides.com/javiergs/ser431-l10
*/

#include <vector>
#include "imathvec.h"
#include "glut.h"

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
GLuint displayDiamond, displayPlane;

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
int angle_diamond=0;
/* ---------------------------------------- BEGIN:MESH CODE  ---------------------------------------- */


// create a triangulated diamond
Mesh* createDiamond() {
  Mesh *mesh = new Mesh;
  // Vertexes
  mesh->dot_vertex.push_back(Vec3f(1, 0, 1));
  mesh->dot_vertex.push_back(Vec3f(-1, 0, 1));
  mesh->dot_vertex.push_back(Vec3f(-1, 0, -1));
  mesh->dot_vertex.push_back(Vec3f(1, 0, -1));
  mesh->dot_vertex.push_back(Vec3f(0, 1, 0));
  mesh->dot_vertex.push_back(Vec3f(0, -1, 0));
  // Indexes
  mesh->face_index_vertex.push_back(0); mesh->face_index_vertex.push_back(2); mesh->face_index_vertex.push_back(1); // Plane 0
  mesh->face_index_vertex.push_back(0); mesh->face_index_vertex.push_back(3); mesh->face_index_vertex.push_back(2);
  mesh->face_index_vertex.push_back(3); mesh->face_index_vertex.push_back(2); mesh->face_index_vertex.push_back(4); // Upper 6
  mesh->face_index_vertex.push_back(0); mesh->face_index_vertex.push_back(3); mesh->face_index_vertex.push_back(4);
  mesh->face_index_vertex.push_back(1); mesh->face_index_vertex.push_back(0); mesh->face_index_vertex.push_back(4);
  mesh->face_index_vertex.push_back(4); mesh->face_index_vertex.push_back(2); mesh->face_index_vertex.push_back(1);
  mesh->face_index_vertex.push_back(0); mesh->face_index_vertex.push_back(1); mesh->face_index_vertex.push_back(5); // Lower 18
  mesh->face_index_vertex.push_back(0); mesh->face_index_vertex.push_back(5); mesh->face_index_vertex.push_back(3);
  mesh->face_index_vertex.push_back(3); mesh->face_index_vertex.push_back(5); mesh->face_index_vertex.push_back(2);
  mesh->face_index_vertex.push_back(2); mesh->face_index_vertex.push_back(5); mesh->face_index_vertex.push_back(1);
  return mesh;
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
  Mesh* mesh1 = createDiamond();
  displayDiamond = meshToDisplayList(mesh1,1);
  Mesh* mesh2 = createPlane(1000,  1000, 40);
  displayPlane = meshToDisplayList(mesh2, 2);
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
  //plane
  glPushMatrix();
    glTranslatef(-500,0,-500);
    glCallList(displayPlane);
  glPopMatrix();
  // diamond center
  glPushMatrix();
    glScalef(40, 40, 40);
    glCallList(displayDiamond);
  glPopMatrix();

  // diamond moving
  glPushMatrix();
    if (angle_diamond>=360) angle_diamond=0; 
    angle_diamond+=1;

    glRotatef(angle_diamond, 0,1,0);
    glTranslatef(160, 0, 0);
    glScalef(40, 40, 40);
    glCallList(displayDiamond);
  glPopMatrix();

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
  glutIdleFunc(display);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  init();
  glutMainLoop();
}
