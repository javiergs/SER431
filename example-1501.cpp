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
// Update
struct Mesh {
  // vertex
  vector<Vec3f> dot_vertex;
  vector<Vec3f> dot_normalPerFace;
  vector<Vec3f> dot_normalPerVertex;
  vector<Vec2f> dot_texture;
  // faces
  vector<int> face_index_vertex;
  vector<int> face_index_normalPerFace;
  vector<int> face_index_normalPerVertex;
  vector<int> face_index_texture;
};


// global
Mesh* mesh1;
Mesh* mesh2;
Mesh* mesh3;
Mesh* mesh4;

int width = 1200;
int height = 600;
float ratio = 1.0;
GLuint displayDiamond, displayPlane, displayMesh, displayMeshB;

// controling parameters
int mouse_button;
int mouse_x = 0;
int mouse_y = 0;
float scale = 0.3;
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
bool perV = false;

float ylight = 100.0;
float xlight = 100.0;
GLfloat light_position[] = { xlight, ylight , 0, 0 };
bool lightleft = false;

//
int PERVERTEX = 0; 
int PERFACE   = 1;
int LIGHT_TOP = 1;
int LIGHT_MOVE =0;

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
  Mesh *m = new Mesh;
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
        m->dot_normalPerFace.push_back(Vec3f(x, y, z));
        m->dot_normalPerVertex.push_back(Vec3f(x, y, z));
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
          if (vnt[j][2] != -1) m->face_index_normalPerFace.push_back(vnt[j][2]);
          if (vnt[j][2] != -1) m->face_index_normalPerVertex.push_back(vnt[j][2]);
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

// create a triangulated diamond
Mesh* createDiamond() {
  Mesh *mesh = new Mesh;
  // Vertexes
  mesh->dot_vertex.push_back(Vec3f(100, 0, 100));
  mesh->dot_vertex.push_back(Vec3f(-100, 0, 100));
  mesh->dot_vertex.push_back(Vec3f(-100, 0, -100));
  mesh->dot_vertex.push_back(Vec3f(100, 0, -100));
  mesh->dot_vertex.push_back(Vec3f(0, 100, 0));
  mesh->dot_vertex.push_back(Vec3f(0, -100, 0));
  // Faces
  mesh->face_index_vertex.push_back(0); mesh->face_index_vertex.push_back(2); mesh->face_index_vertex.push_back(1);
  mesh->face_index_vertex.push_back(0); mesh->face_index_vertex.push_back(3); mesh->face_index_vertex.push_back(2);
  mesh->face_index_vertex.push_back(3); mesh->face_index_vertex.push_back(2); mesh->face_index_vertex.push_back(4);
  mesh->face_index_vertex.push_back(0); mesh->face_index_vertex.push_back(3); mesh->face_index_vertex.push_back(4);
  mesh->face_index_vertex.push_back(1); mesh->face_index_vertex.push_back(0); mesh->face_index_vertex.push_back(4);
  mesh->face_index_vertex.push_back(4); mesh->face_index_vertex.push_back(2); mesh->face_index_vertex.push_back(1);
  mesh->face_index_vertex.push_back(0); mesh->face_index_vertex.push_back(1); mesh->face_index_vertex.push_back(5);
  mesh->face_index_vertex.push_back(0); mesh->face_index_vertex.push_back(5); mesh->face_index_vertex.push_back(3);
  mesh->face_index_vertex.push_back(3); mesh->face_index_vertex.push_back(5); mesh->face_index_vertex.push_back(2);
  mesh->face_index_vertex.push_back(2); mesh->face_index_vertex.push_back(5); mesh->face_index_vertex.push_back(1);

  //Normals (Per Face)
  return mesh;
}

// normal per face
void calculateNormalPerFace(Mesh* m) {
  Vec3<float> v1, v2, v3, v4, v5;
  for (int i = 0; i < m->face_index_vertex.size(); i += 3) {
    v1 = m->dot_vertex[m->face_index_vertex[i]];
    v2 = m->dot_vertex[m->face_index_vertex[i + 1]];
    v3 = m->dot_vertex[m->face_index_vertex[i + 2]];
    v4 = (v2 - v1);
    v5 = (v3 - v1);
    v4 = v4.cross(v5);
    v4.normalize();
    m->dot_normalPerFace.push_back(v4);
    int pos = m->dot_normalPerFace.size() - 1;
    // same normal for all vertex in this face
    m->face_index_normalPerFace.push_back(pos);
    m->face_index_normalPerFace.push_back(pos);
    m->face_index_normalPerFace.push_back(pos);
  }
}

// calculate normal per vertex
void calculateNormalPerVertex(Mesh* m) {
  m->dot_normalPerVertex.clear();
  m->face_index_normalPerVertex.clear();
  Vec3<float> suma; suma.x = 0; suma.y = 0; suma.z = 0;
  //initialize
  for (unsigned int val = 0; val < m->dot_vertex.size(); val++) {
    m->dot_normalPerVertex.push_back(suma);
  }
  // calculate sum for vertex
  for (long pos = 0; pos < m->face_index_vertex.size(); pos++) {
    m->dot_normalPerVertex[m->face_index_vertex[pos]] +=
      m->dot_normalPerFace[m->face_index_normalPerFace[pos]];
  }
  // normalize for vertex 
  for (unsigned int val = 0; val < m->dot_normalPerVertex.size(); val++) {
    m->dot_normalPerVertex[val] = m->dot_normalPerVertex[val].normalize();
  }
  //normalVertexIndex is the same that vertexIndex
  for (unsigned int pos = 0; pos < m->face_index_vertex.size(); pos++) {
    m->face_index_normalPerVertex.push_back(m->face_index_vertex[pos]);
  }
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
    if (PERVERTEX && (!m->dot_normalPerVertex.empty() && !m->face_index_normalPerVertex.empty())) {
      glNormal3fv(&m->dot_normalPerVertex[m->face_index_normalPerVertex[i]].x);
      //printf("PERVERTEX");
    } else if (PERFACE && (!m->dot_normalPerFace.empty() && !m->face_index_normalPerFace.empty())) {
      glNormal3fv(&m->dot_normalPerFace[m->face_index_normalPerFace[i]].x);
      //printf("PERFACE");
    }
    if (!m->dot_texture.empty() && !m->face_index_texture.empty()) {
      glTexCoord2fv(&m->dot_texture[m->face_index_texture[i]].x);
    }
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

// menuListener
void menuListener(int option) {
  switch (option) {
  case 1:
    PERFACE=1; PERVERTEX=0; 
    displayDiamond = meshToDisplayList(mesh1, 1);
    displayPlane = meshToDisplayList(mesh2, 2);
    displayMesh = meshToDisplayList(mesh3, 3);
    break;
  case 2:
    PERFACE=0; PERVERTEX=1; 
    displayDiamond = meshToDisplayList(mesh1, 1);
    displayPlane = meshToDisplayList(mesh2, 2);
    displayMesh = meshToDisplayList(mesh3, 3);
    break;
  case 3:
    LIGHT_TOP = 1; LIGHT_MOVE = 0; break;
  case 4:
    LIGHT_TOP = 0; LIGHT_MOVE = 1; break;
  }
  glutPostRedisplay();
}

// create menu
void createMenus() {
  //add entries to submenu Normals
  int menuA = glutCreateMenu(menuListener);
  glutAddMenuEntry("Per Face", 1);
  glutAddMenuEntry("Per Vertex", 2);
  //add entries to submenu Light
  int menuB = glutCreateMenu(menuListener);
  glutAddMenuEntry("Top", 3);
  glutAddMenuEntry("Moving", 4);
  // create main menu
  int menu = glutCreateMenu(menuListener);
  glutAddSubMenu("Normals", menuA);
  glutAddSubMenu("Light", menuB);
  // attach the menu to the right button
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// init
void init() {
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  ratio = (double)width / (double)height;
  // menu
  createMenus();
  // mesh
  mesh1 = createDiamond();
  mesh2 = createPlane(1000, 1000, 40);
  mesh3 = loadFile("../obj files/porsche.obj");
  calculateNormalPerFace(mesh1);
  calculateNormalPerFace(mesh2);
  calculateNormalPerVertex(mesh1);
  calculateNormalPerVertex(mesh2);
  displayDiamond = meshToDisplayList(mesh1, 1);
  displayPlane = meshToDisplayList(mesh2, 2);
  displayMesh = meshToDisplayList(mesh3, 3);
  // light
  GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
  GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat light_position[] = { 250.0, 125.0, 250.0, 0.0 };
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  // enable
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
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
  //if (mouse_button == GLUT_RIGHT_BUTTON) {
  //  scale += (y - mouse_y) / 100.0;
  //  if (scale < 0.1) scale = 0.1;
  //  if (scale > 7)	scale = 7;
  //}
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
  // lookAt
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
  glTranslatef(-500, 0, -500);
  glCallList(displayPlane);
  glPopMatrix();
  // diamond center
  glPushMatrix();
  glCallList(displayDiamond);
  glPopMatrix();
  // diamond moving
  glPushMatrix();
  if (angle_animated_object <= -360) angle_animated_object = 0;
  angle_animated_object -= 0.03;
  glRotatef(angle_animated_object, 0, 1, 0);
  glTranslatef(200, 20, 0);
  glCallList(displayMesh);
  glPopMatrix();
  
  // light
  glPushMatrix();
  if (LIGHT_TOP) {
    GLfloat light_position[] = { 250.0, 125.0, 250.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  } else {
    glRotatef(angle_animated_object, 0, 0, 1);
    glTranslatef(500, 20, 0);
    GLfloat light_position[] = { 0.0, 0.0, 0.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  }
  glPopMatrix();

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
  //renderBitmapString(0.0, height - 26.0f, 0.0f, "Use [Mouse Right Key] to scale");
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
