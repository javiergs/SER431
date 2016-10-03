/*
Lecture 09
http://slides.com/javiergs/ser431-l09
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
GLuint displayList;

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

// str to int
int StrToInt(const string &str) {
  int i;
  if (sscanf_s(str.c_str(), "%i", &i) == 1) return i;
  else return 0;
}

// split string
vector<string> split_string(const string& str, const string& split_str) {
  vector<string> stlv_string;
  string part_string("");
  string::size_type i;
  i = 0;
  while (i < str.size()) {
    if (split_str.find(str[i]) != string::npos) {
      stlv_string.push_back(part_string);
      part_string = "";
      while (split_str.find(str[i]) != string::npos) {
        ++i;
      }
    }
    else {
      part_string += str[i];
      ++i;
    }
  }
  if (!part_string.empty())
    stlv_string.push_back(part_string);
  return stlv_string;
}

// load file
Mesh* loadFile(const char* file) {
  Mesh *m =  new Mesh;
  m->dot_vertex.clear();
  m->face_index_vertex.clear();
  ifstream infile(file);
  if (infile.fail()) {
    cout << "Error opening file " << file;
    return NULL;
  }
  char current_line[1024];
  while (!infile.eof()) {
    infile.getline(current_line, 1024);
    switch (current_line[0]) {
    case'v':
      float x, y, z;
      switch (current_line[1]) {
      case 'n':
        sscanf_s(current_line, "vn %f %f %f", &x, &y, &z);
        m->dot_normal.push_back(Vec3f(x, y, z));
        break;
      case 't':
        sscanf_s(current_line, "vt %f %f", &x, &y);
        m->dot_texture.push_back(Vec2f(x, y));
        break;
      default:
        sscanf_s(current_line, "v %f %f %f", &x, &y, &z);
        m->dot_vertex.push_back(Vec3f(x, y, z));
        break;
      }
      break;
    case 'f': {
      vector<string> faces = split_string(current_line, " \t.\r\n");
      int vnt[3][3] = { { -1, -1, -1 },{ -1, -1, -1 },{ -1, -1, -1 } };
      string::size_type begin, end;
      for (int i = 0; i < 2; i++) {
        begin = 0;
        int j = 0;
        do {
          end = faces[i + 1].find_first_of("/", begin);
          if (begin < end) {
            vnt[i][j] = StrToInt(faces[i + 1].substr(begin, end - begin)) - 1;
          }
          else {
            vnt[i][j] = -1;
          }
          begin = end + 1;
          j++;
        } while (end != string::npos);
      }
      for (unsigned int i = 3; i < faces.size(); i++) {
        begin = 0;
        int j = 0;
        do {
          end = faces[i].find_first_of("/", begin);
          if (begin < end) {
            vnt[2][j] = StrToInt(faces[i].substr(begin, end - begin)) - 1;
          }
          else {
            vnt[2][j] = -1;
          }
          begin = end + 1;
          j++;
        } while (end != string::npos);
        for (int j = 0; j < 3; j++) {
          m->face_index_vertex.push_back(vnt[j][0]);
          if (vnt[j][1] != -1) m->face_index_texture.push_back(vnt[j][1]);
          if (vnt[j][2] != -1) m->face_index_normal.push_back(vnt[j][2]);
        }
        memcpy(&vnt[1], &vnt[2], sizeof(int) * 3);
      }
    }
    break;
    default:
      break;
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
  Mesh* mesh = loadFile("../obj files/f-16.obj");
  displayList = meshToDisplayList(mesh,1);
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
  gluLookAt(0.0f, 0.0f, 100.0, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  // camera
  glScalef(scale, scale, scale);
  glRotatef(x_angle, 1.0f, 0.0f, 0.0f);
  glRotatef(y_angle, 0.0f, 1.0f, 0.0f);
  glTranslatef(0.0f, 0.0f, 0.0f);
  // draw ground
  glColor3f(0.9f, 0.9f, 0.9f);
  glBegin(GL_QUADS);
  glVertex3f(-100.0f, 0.0f, -100.0f);
  glVertex3f(-100.0f, 0.0f, 100.0f);
  glVertex3f(100.0f, 0.0f, 100.0f);
  glVertex3f(100.0f, 0.0f, -100.0f);
  glEnd();
  // draw mesh
  glCallList(displayList);
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
