
/*
Lecture 13
http://slides.com/javiergs/ser431-l13
*/

#include <vector>
#include "imathvec.h"
#include "glut.h"
#include<iostream>
#include<fstream>
#include<string>

#define PI 3.1415926
#define MIN2(a,b) (((a) < (b))?(a):(b))
#define MAX2(a,b) (((a) > (b))?(a):(b))

using namespace std;
using namespace Imath;

typedef Vec3<float> Vec3f;
typedef Vec2<float> Vec2f;

class Mesh {
  private:
    int StrToInt(const string &str);
    vector<string> split_string(const string& str, const string& split_str);
  public:
    vector<Vec3f> dot_vertex;
    vector<Vec3f> dot_normal;
    vector<Vec2f> dot_texture;
    vector<int>   face_index_vertex;
    vector<int>   face_index_normal;
    vector<int>   face_index_texture;
    // methods
    Mesh();
    void createDiamond();
    void createPlane(int total_width, int total_depth, int cell_size);
    void loadFile(const char* file);
    GLuint meshToDisplayList(int id);
    // local bounding volume
    Vec3f localBoundingMinPoint;
    Vec3f localBoundingMaxPoint;
    void calculateAABB();
};

//constructor
Mesh::Mesh() {
}

// str to int
int Mesh::StrToInt(const string &str) {
  int i;
  if (sscanf_s(str.c_str(), "%i", &i) == 1) return i;
  else return 0;
}

// split string
vector<string> Mesh::split_string(const string& str, const string& split_str) {
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
void Mesh::loadFile(const char* file) {
  this->dot_vertex.clear();
  this->face_index_vertex.clear();
  ifstream infile(file);
  if (infile.fail()) {
    cout << "Error opening file " << file;
    return;
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
        this->dot_normal.push_back(Vec3f(x, y, z));
        break;
      case 't':
        sscanf_s(current_line, "vt %f %f", &x, &y);
        this->dot_texture.push_back(Vec2f(x, y));
        break;
      default:
        sscanf_s(current_line, "v %f %f %f", &x, &y, &z);
        this->dot_vertex.push_back(Vec3f(x, y, z));
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
          this->face_index_vertex.push_back(vnt[j][0]);
          if (vnt[j][1] != -1) this->face_index_texture.push_back(vnt[j][1]);
          if (vnt[j][2] != -1) this->face_index_normal.push_back(vnt[j][2]);
        }
        memcpy(&vnt[1], &vnt[2], sizeof(int) * 3);
      }
    }
              break;
    default:
      break;
    }
  }
}

// create a triangulated diamond
void Mesh::createDiamond() {
  // Vertexes
  this->dot_vertex.push_back(Vec3f(1, 0, 1));
  this->dot_vertex.push_back(Vec3f(-1, 0, 1));
  this->dot_vertex.push_back(Vec3f(-1, 0, -1));
  this->dot_vertex.push_back(Vec3f(1, 0, -1));
  this->dot_vertex.push_back(Vec3f(0, 1, 0));
  this->dot_vertex.push_back(Vec3f(0, -1, 0));
  // Indexes
  this->face_index_vertex.push_back(0); this->face_index_vertex.push_back(2); this->face_index_vertex.push_back(1);
  this->face_index_vertex.push_back(0); this->face_index_vertex.push_back(3); this->face_index_vertex.push_back(2);
  this->face_index_vertex.push_back(3); this->face_index_vertex.push_back(2); this->face_index_vertex.push_back(4);
  this->face_index_vertex.push_back(0); this->face_index_vertex.push_back(3); this->face_index_vertex.push_back(4);
  this->face_index_vertex.push_back(1); this->face_index_vertex.push_back(0); this->face_index_vertex.push_back(4);
  this->face_index_vertex.push_back(4); this->face_index_vertex.push_back(2); this->face_index_vertex.push_back(1);
  this->face_index_vertex.push_back(0); this->face_index_vertex.push_back(1); this->face_index_vertex.push_back(5);
  this->face_index_vertex.push_back(0); this->face_index_vertex.push_back(5); this->face_index_vertex.push_back(3);
  this->face_index_vertex.push_back(3); this->face_index_vertex.push_back(5); this->face_index_vertex.push_back(2);
  this->face_index_vertex.push_back(2); this->face_index_vertex.push_back(5); this->face_index_vertex.push_back(1);
}

// creating a triangulated plane
void Mesh::createPlane(int arena_width, int arena_depth, int arena_cell) {
  int n = arena_width / arena_cell;
  int m = arena_depth / arena_cell;
  // vertices
  for (int i = 0; i<n; i++) {
    for (int j = 0; j < m; j++) {
      this->dot_vertex.push_back(Vec3<GLfloat>(i*arena_cell, 0.0, j*arena_cell));
    }
  }
  // faces
  for (int i = 0; i<(n*m) - m; i++) {
    if ((i + 1) % n == 0) continue;
    this->face_index_vertex.push_back(i); this->face_index_vertex.push_back(i + 1);
    this->face_index_vertex.push_back(i + n);
    this->face_index_vertex.push_back(i + 1); this->face_index_vertex.push_back(i + n + 1);
    this->face_index_vertex.push_back(i + n);
  }
}

// create a display list from the data in the mesh
GLuint Mesh::meshToDisplayList(int id) {
  GLuint listID = glGenLists(id);
  glNewList(listID, GL_COMPILE);
  // mesh
  glBegin(GL_TRIANGLES);
  for (unsigned int i = 0; i < face_index_vertex.size(); i++) {
    // drawing
    if (!dot_normal.empty() && !face_index_normal.empty())
      glNormal3fv(&dot_normal[face_index_normal[i]].x);
    if (!dot_texture.empty() && !face_index_texture.empty())
      glTexCoord2fv(&dot_texture[face_index_texture[i]].x);
    Vec3f offset = (dot_vertex[face_index_vertex[i]]);
    glColor3f(fabs(sin(offset.x)), fabs(cos(offset.y)), fabs(offset.z));
    glVertex3fv(&dot_vertex[face_index_vertex[i]].x);
  }
  glEnd();
  glEndList();
  return listID;
}

// calculate bounding box (AABB)
void Mesh::calculateAABB() {
  vector<Vec3<float>>& v = this->dot_vertex;
  if (v.empty()) return;
  this->localBoundingMaxPoint = this->localBoundingMinPoint = v[0];
  for (unsigned int i = 1; i < v.size(); i++) {
    this->localBoundingMinPoint.x = MIN2(this->localBoundingMinPoint.x, v[i].x);
    this->localBoundingMinPoint.y = MIN2(this->localBoundingMinPoint.y, v[i].y);
    this->localBoundingMinPoint.z = MIN2(this->localBoundingMinPoint.z, v[i].z);
    this->localBoundingMaxPoint.x = MAX2(this->localBoundingMaxPoint.x, v[i].x);
    this->localBoundingMaxPoint.y = MAX2(this->localBoundingMaxPoint.y, v[i].y);
    this->localBoundingMaxPoint.z = MAX2(this->localBoundingMaxPoint.z, v[i].z);
  }
}
