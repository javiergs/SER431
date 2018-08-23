/*
Lecture 25
http://slides.com/javiergs/ser332-l25
javiergs@asu.edu
*/

#include <vector>
#include "imathvec.h"
#include "glut.h"
#include<iostream>
#include<fstream>
#include<string>
#include "improvedNoise.h"
#include <windows.h>
#include <wingdi.h>

#define PI 3.1415926

using namespace std;
using namespace Imath;

typedef Vec3<float> Vec3f;
typedef Vec2<float> Vec2f;

// mesh data structure
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
GLuint display1, display2, display3, display4;

// controling parameters
int mouse_button;
int mouse_x = 0;
int mouse_y = 0;
float scale = 0.3;
float x_angle = 0.0;
float y_angle = 0.0;

// textures with BITMAPS
GLuint textureArray[8];

// t_scale
float t_scale(float x) {
  // assuming noise report values -2.xxx to 2.xxx
  return (x + 2) / 4;
}

// marbleMap
Vec3f marbleMap(float a) {
  Vec3f black = Vec3f(0, 0, 0);
  Vec3f color = Vec3f(1, 1, 1);
  return ((1 - a) * color + a * black);
}

// skyMap
Vec3f skyMap(float a) {
  Vec3f white = Vec3f(1, 1, 1);
  Vec3f color = Vec3f(1, 0, 0);
  return ((1 - a) * white + a * color);
}

// Load a DIB/BMP file from disk.
GLubyte *LoadDIBitmap(const char *filename, BITMAPINFO **info) {
  FILE *fp;      // open file pointer
  GLubyte * bits; // bitmap pixel bits
  int bitsize;   // Size of bitmap
  int infosize;  // Size of header information
  BITMAPFILEHEADER header; // File header
                           // try opening the file; use "rb" mode to read this *binary* file.
  if ((fp = fopen(filename, "rb")) == NULL)
    return (NULL);
  // read the file header and any following bitmap information.
  if (fread(&header, sizeof(BITMAPFILEHEADER), 1, fp) < 1) {
    // Couldn't read the file header - return NULL.
    fclose(fp);
    return (NULL);
  }
  // Check for BM reversed.
  if (header.bfType != 'MB') {
    // not a bitmap file - return NULL.
    fclose(fp);
    return (NULL);
  }
  infosize = header.bfOffBits - sizeof(BITMAPFILEHEADER);
  if ((*info = (BITMAPINFO *)malloc(infosize)) == NULL) {
    // couldn't allocate memory for bitmap info - return NULL.
    fclose(fp);
    return (NULL);
  }
  if (fread(*info, 1, infosize, fp) < infosize) {
    // Couldn't read the bitmap header - return NULL.
    free(*info);
    fclose(fp);
    return (NULL);
  }
  // Now that we have all the header info read in, allocate memory for the bitmap and read *it* in.
  if ((bitsize = (*info)->bmiHeader.biSizeImage) == 0)
    bitsize = ((*info)->bmiHeader.biWidth*(*info)->bmiHeader.biBitCount + 7) / 8 * abs((*info)->bmiHeader.biHeight);
  if ((bits = (GLubyte *)malloc(bitsize)) == NULL) {
    // Couldn't allocate memory - return NULL!
    free(*info);
    fclose(fp);
    return (NULL);
  }
  if (fread(bits, 1, bitsize, fp) < bitsize) {
    // couldn't read bitmap - free memory and return NULL!
    free(*info);
    free(bits);
    fclose(fp);
    return (NULL);
  }
  // OK, everything went fine - return the allocated bitmap.
  fclose(fp);
  return (bits);
}

// Create texture from algorithm
void codedTexture(UINT textureArray[], int n, int type) {
  const int TexHeight = 128;
  const int TexWidth =  128;
  // create texture in memory
  GLubyte textureImage[TexHeight][TexWidth][3];
  ImprovedNoise noise;
  Vec3f pixelColor;
  for (int i = 0; i < TexHeight; i++)
    for (int j = 0; j < TexWidth; j++) {
      if (type==0) pixelColor = skyMap(t_scale(noise.perlinMultiscale(i * 5, j * 5)));
      else pixelColor = marbleMap(t_scale(noise.perlinMarble(i * 5, j * 5)));          
      textureImage[i][j][0] = pixelColor[0] * 255;
      textureImage[i][j][1] = pixelColor[1] * 255;
      textureImage[i][j][2] = pixelColor[2] * 255;
    }
  // setup texture
  glGenTextures(1, &textureArray[n]);
  glBindTexture(GL_TEXTURE_2D, textureArray[n]);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // must set to 1 for compact data
                                         // glTexImage2D Whith size and minification
  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, TexWidth, TexHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, textureImage); // BGRA to include alpha
}

// Create texture from BMP file
void bmpTexture(UINT textureArray[], LPSTR file, int n) {
  BITMAPINFO *bitmapInfo; // Bitmap information
  GLubyte    *bitmapBits; // Bitmap data
  if (!file) {
    cout << "texture file not found!" << endl;
    return;
  }
  bitmapBits = LoadDIBitmap(file, &bitmapInfo);
  glGenTextures(1, &textureArray[n]);
  glBindTexture(GL_TEXTURE_2D, textureArray[n]);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // must set to 1 for compact data
                                         // glTexImage2D Whith size and minification
  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bitmapInfo->bmiHeader.biWidth, bitmapInfo->bmiHeader.biHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bitmapBits);
}

// OBJ file - str to int
int StrToInt(const string &str) {
  int i;
  if (sscanf_s(str.c_str(), "%i", &i) == 1) return i;
  else return 0;
}

// OBJ file - split string
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

// OBJ file - load file
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
Mesh* createCube() {
  Mesh *mesh = new Mesh;
  // Vertexes
  int BOX_WIDTH = 100;
  int BOX_HEIGHT = 100;
  int BOX_DEPTH = 100;

  mesh->dot_vertex.push_back(Vec3<GLfloat>(0.0, BOX_HEIGHT, BOX_DEPTH));
  mesh->dot_vertex.push_back(Vec3<GLfloat>(BOX_WIDTH, BOX_HEIGHT, BOX_DEPTH));
  mesh->dot_vertex.push_back(Vec3<GLfloat>(BOX_WIDTH, 0.0, BOX_DEPTH));
  mesh->dot_vertex.push_back(Vec3<GLfloat>(0.0, 0.0, BOX_DEPTH));

  mesh->dot_vertex.push_back(Vec3<GLfloat>(0.0, BOX_HEIGHT, 0.0));
  mesh->dot_vertex.push_back(Vec3<GLfloat>(BOX_WIDTH, BOX_HEIGHT, 0.0));
  mesh->dot_vertex.push_back(Vec3<GLfloat>(BOX_WIDTH, 0.0, 0.0));
  mesh->dot_vertex.push_back(Vec3<GLfloat>(0.0, 0.0, 0.0));

  // face 1
  mesh->face_index_vertex.push_back(0);//0
  mesh->face_index_vertex.push_back(2);//1
  mesh->face_index_vertex.push_back(1);//2
  mesh->face_index_vertex.push_back(0);//0
  mesh->face_index_vertex.push_back(3);//2
  mesh->face_index_vertex.push_back(2);//3
                                       // face 2
  mesh->face_index_vertex.push_back(0);//0
  mesh->face_index_vertex.push_back(5);//1
  mesh->face_index_vertex.push_back(4);//5
  mesh->face_index_vertex.push_back(0);//0
  mesh->face_index_vertex.push_back(1);//5
  mesh->face_index_vertex.push_back(5);//4
                                       // face 3
  mesh->face_index_vertex.push_back(1);//1
  mesh->face_index_vertex.push_back(6);//2
  mesh->face_index_vertex.push_back(5);//6
  mesh->face_index_vertex.push_back(1);//1
  mesh->face_index_vertex.push_back(2);//6
  mesh->face_index_vertex.push_back(6);//5
                                       // face 4
  mesh->face_index_vertex.push_back(2);//2
  mesh->face_index_vertex.push_back(7);//3
  mesh->face_index_vertex.push_back(6);//7
  mesh->face_index_vertex.push_back(2);//2
  mesh->face_index_vertex.push_back(3);//7
  mesh->face_index_vertex.push_back(7);//6
                                       // face 5
  mesh->face_index_vertex.push_back(4);//0
  mesh->face_index_vertex.push_back(3);//3
  mesh->face_index_vertex.push_back(0);//7
  mesh->face_index_vertex.push_back(4);//0
  mesh->face_index_vertex.push_back(7);//7
  mesh->face_index_vertex.push_back(3);//4
                                       // face 6
  mesh->face_index_vertex.push_back(5);//4 
  mesh->face_index_vertex.push_back(7);//5
  mesh->face_index_vertex.push_back(4);//6
  mesh->face_index_vertex.push_back(5);//4
  mesh->face_index_vertex.push_back(6);//6
  mesh->face_index_vertex.push_back(7);//7

                                       // texture
  mesh->dot_texture.push_back(Vec2<GLfloat>(0.0, 1.0));
  mesh->dot_texture.push_back(Vec2<GLfloat>(1.0, 1.0));
  mesh->dot_texture.push_back(Vec2<GLfloat>(1.0, 0.0));
  mesh->dot_texture.push_back(Vec2<GLfloat>(0.0, 0.0));
  for (int t = 0; t<6; t++) {
    mesh->face_index_texture.push_back(0);//0
    mesh->face_index_texture.push_back(2);//1
    mesh->face_index_texture.push_back(1);//2
    mesh->face_index_texture.push_back(0);//0
    mesh->face_index_texture.push_back(3);//2
    mesh->face_index_texture.push_back(2);//3
  }

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
  //texture
  me->dot_texture.push_back(Vec2<GLfloat>(0.0, 0.0));
  me->dot_texture.push_back(Vec2<GLfloat>(0.0, 1.0));
  me->dot_texture.push_back(Vec2<GLfloat>(1.0, 0.0));
  me->dot_texture.push_back(Vec2<GLfloat>(1.0, 1.0));
  // faces
  for (int i = 0; i<(n*m) - m; i++) {
    if ((i + 1) % n == 0) continue;
    me->face_index_vertex.push_back(i); me->face_index_vertex.push_back(i + 1);
    me->face_index_vertex.push_back(i + n);
    me->face_index_vertex.push_back(i + 1); me->face_index_vertex.push_back(i + n + 1);
    me->face_index_vertex.push_back(i + n);
    // index for texture
    for (int t = 0; t<6; t++) {
      me->face_index_texture.push_back(3);//0
      me->face_index_texture.push_back(2);//2
      me->face_index_texture.push_back(1);//1
      me->face_index_texture.push_back(2);//0
      me->face_index_texture.push_back(0);//3
      me->face_index_texture.push_back(1);//2
    }
  }
  return me;
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

// draw
GLuint meshToDisplayList(Mesh* m, int id, int textureId) {
  GLuint listID = glGenLists(id);
  glNewList(listID, GL_COMPILE);
  //if (id != 3) {
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, textureArray[textureId]);
  //}
  glBegin(GL_TRIANGLES);
  for (unsigned int i = 0; i < m->face_index_vertex.size(); i++) {
    // PER VERTEX NORMALS
    if ((!m->dot_normalPerVertex.empty() && !m->face_index_normalPerVertex.empty())) {
      glNormal3fv(&m->dot_normalPerVertex[m->face_index_normalPerVertex[i]].x);
    }
    // TEXTURES
    if (!m->dot_texture.empty() && !m->face_index_texture.empty()) {
      glTexCoord2fv(&m->dot_texture[m->face_index_texture[i]].x);
    }
    // COLOR
    Vec3f offset = (m->dot_vertex[m->face_index_vertex[i]]);
    // VERTEX
    glColor3f(fabs(sin(offset.x)), fabs(cos(offset.y)), fabs(offset.z));
    glVertex3fv(&m->dot_vertex[m->face_index_vertex[i]].x);
  }
  glEnd();
  if (id != 3) {
    glDisable(GL_TEXTURE_2D);
  }
  glEndList();
  return listID;
}

// init
void init() {
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);
  ratio = (double)width / (double)height;
  // mesh
  mesh1 = createPlane(2000, 2000, 200);
  mesh2 = createCube();
  mesh3 = createCube(); 
  mesh4 = createCube();
  calculateNormalPerFace(mesh1);
  calculateNormalPerFace(mesh2);
  calculateNormalPerFace(mesh3);
  calculateNormalPerFace(mesh4);
  calculateNormalPerVertex(mesh1);
  calculateNormalPerVertex(mesh2);
  calculateNormalPerVertex(mesh3);
  calculateNormalPerVertex(mesh4);
  // textures
  bmpTexture(textureArray, "../bmp files/brick.bmp",  0);
  bmpTexture(textureArray, "../bmp files/oldbox.bmp", 1);
  codedTexture(textureArray, 2, 0); //Sky texture - noise multiscale. Type=0
  codedTexture(textureArray, 3, 1); //Marble texture - noise marble. Type=1
  // display lists
  display1 = meshToDisplayList(mesh1, 1, 0);
  display2 = meshToDisplayList(mesh2, 2, 1);
  display3 = meshToDisplayList(mesh3, 3, 2);
  display4 = meshToDisplayList(mesh4, 4, 3);
  // light
  GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
  GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  GLfloat light_position[] = { 0.0, 0.0, 1.0, 0.0 };
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
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
  if (mouse_button == GLUT_RIGHT_BUTTON) {
    scale += (y - mouse_y) / 100.0;
    if (scale < 0.1) scale = 0.1;
    if (scale > 7)	scale = 7;
  }
  mouse_x = x;
  mouse_y = y;
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
    glTranslatef(-900, 0, -900);
    glCallList(display1);
  glPopMatrix();
  // box 1
  glPushMatrix();
    glCallList(display2);
  glPopMatrix();
  // box 2
  glPushMatrix();
    glTranslatef(200, 0, 0);
    glCallList(display3);
  glPopMatrix();
  // box 3
  glPushMatrix();
  glTranslatef(-200, 0, 0);
  glCallList(display4);
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
  renderBitmapString(0.0, height - 26.0f, 0.0f, "Use [Mouse Right Key] to scale");
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glutSwapBuffers();
}

// main
void main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(0, 0);
  glutInitWindowSize(width, height);
  glutCreateWindow("Textures");
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutIdleFunc(display);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);

  init();

  glutMainLoop();
}
