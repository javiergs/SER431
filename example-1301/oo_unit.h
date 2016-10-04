/*
Lecture 13
http://slides.com/javiergs/ser431-l13
*/

#include "oo_mesh.h"
#include "imathmatrix.h"

class Unit {
  public:
    Mesh *mesh;
    Vec3<float> worldBoundingMinPoint, worldBoundingMaxPoint, worldBoundingCenterPoint;
    Vec3<float> position;
    float scale;
    int rotateAngle; 
    // methods   
    Unit();
    void mapping();
    GLuint boxToDisplayList(int id);
    void mappingDot(GLfloat* m, Vec3<float>& v, Vec3<float>& result);
    bool intersect(Unit* venus);
};

// constructor
Unit::Unit() {
  mesh = new Mesh();
  position.x = 0;
  position.y = 0;
  position.z = 0;
  scale=1.0;
  rotateAngle=0;
}

// map AABB from local to world coordinates
void Unit::mapping() {
  Matrix44<float> T(
    1, 0, 0, position.x,
    0, 1, 0, position.y,
    0, 0, 1, position.z,
    0, 0, 0, 1
  );
  Matrix44<float> S(
    scale, 0, 0, 0,
    0, scale, 0, 0,
    0, 0, scale, 0,
    0, 0, 0, 1
  );
  double angle = -(M_PI / 180)*rotateAngle;
  Matrix44<float> R(
    cos(angle), 0, -sin(angle), 0,
    0,          1,           0, 0,
    sin(angle), 0,  cos(angle), 0,
    0,          0,           0, 1);

  // Rotation * Translation * Scale
  Matrix44<float> MODEL = R * T * S;
 
  // Model matrix
  GLfloat m[16] = {
    MODEL[0][0], MODEL[1][0], MODEL[2][0], MODEL[3][0],
    MODEL[0][1], MODEL[1][1], MODEL[2][1], MODEL[3][1],
    MODEL[0][2], MODEL[1][2], MODEL[2][2], MODEL[3][2],
    MODEL[0][3], MODEL[1][3], MODEL[2][3], MODEL[3][3]
  };

  // There are 8 points of interest in the local bounding box
  Vec3<float> max = mesh->localBoundingMaxPoint;
  Vec3<float> min = mesh->localBoundingMinPoint;
  Vec3<float> max_out_x = Vec3<float>(min.x, max.y, max.z);
  Vec3<float> max_out_y = Vec3<float>(max.x, min.y, max.z);
  Vec3<float> max_out_xy = Vec3<float>(min.x, min.y, max.z);
  Vec3<float> min_out_x = Vec3<float>(max.x, min.y, min.z);
  Vec3<float> min_out_y = Vec3<float>(min.x, max.y, min.z);
  Vec3<float> min_out_xy = Vec3<float>(max.x, max.y, min.z);

  // map these to the real world
  Vec3<float> new_max_mins[8]; 
  mappingDot(m, max, new_max_mins[0]);
  mappingDot(m, max_out_x, new_max_mins[1]);
  mappingDot(m, max_out_y, new_max_mins[2]);
  mappingDot(m, max_out_xy, new_max_mins[3]);
  mappingDot(m, min, new_max_mins[4]);
  mappingDot(m, min_out_x, new_max_mins[5]);
  mappingDot(m, min_out_y, new_max_mins[6]);
  mappingDot(m, min_out_xy, new_max_mins[7]);
  
  // define world max and min
  worldBoundingMaxPoint = worldBoundingMinPoint = new_max_mins[0];
  for (unsigned int i = 1; i < 8; i++) {
    worldBoundingMinPoint.x = MIN2(worldBoundingMinPoint.x, new_max_mins[i].x);
    worldBoundingMinPoint.y = MIN2(worldBoundingMinPoint.y, new_max_mins[i].y);
    worldBoundingMinPoint.z = MIN2(worldBoundingMinPoint.z, new_max_mins[i].z);
    worldBoundingMaxPoint.x = MAX2(worldBoundingMaxPoint.x, new_max_mins[i].x);
    worldBoundingMaxPoint.y = MAX2(worldBoundingMaxPoint.y, new_max_mins[i].y);
    worldBoundingMaxPoint.z = MAX2(worldBoundingMaxPoint.z, new_max_mins[i].z);
  }

  //ajuste por rotacion... maybe
  if (worldBoundingMinPoint.x > worldBoundingMaxPoint.x) {
    float tmp = worldBoundingMinPoint.x;
    worldBoundingMinPoint.x = worldBoundingMaxPoint.x;
    worldBoundingMaxPoint.x = tmp;
  }
  if (worldBoundingMinPoint.y > worldBoundingMaxPoint.y) {
    float tmp = worldBoundingMinPoint.y;
    worldBoundingMinPoint.y = worldBoundingMaxPoint.y;
    worldBoundingMaxPoint.y = tmp;
  }
  if (worldBoundingMinPoint.z > worldBoundingMaxPoint.z) {
    float tmp = worldBoundingMinPoint.z;
    worldBoundingMinPoint.z = worldBoundingMaxPoint.z;
    worldBoundingMaxPoint.z = tmp;
  }
}

void Unit::mappingDot(GLfloat* m, Vec3<float>& v, Vec3<float>& result) {
  float vn[4];
  for (int i = 0; i<4; i++) {
    vn[i] = (m[i] * v.x) + (m[i + 4] * v.y) + (m[i + 8] * v.z) + (m[i + 12] * 1);
  }
  result.x = vn[0];
  result.y = vn[1];
  result.z = vn[2];
}

// draw bounding box
GLuint Unit::boxToDisplayList(int id) {
  GLuint listID = glGenLists(id);
  glNewList(listID, GL_COMPILE);
  glBegin(GL_LINE_STRIP);
  glColor3f(1.0, 0.0, 0.0);
  // face 1
  glVertex3f(worldBoundingMinPoint.x, worldBoundingMinPoint.y, worldBoundingMinPoint.z);
  glVertex3f(worldBoundingMinPoint.x, worldBoundingMaxPoint.y, worldBoundingMinPoint.z);
  glVertex3f(worldBoundingMaxPoint.x, worldBoundingMaxPoint.y, worldBoundingMinPoint.z);
  glVertex3f(worldBoundingMaxPoint.x, worldBoundingMinPoint.y, worldBoundingMinPoint.z);
  // face 2
  glVertex3f(worldBoundingMinPoint.x, worldBoundingMinPoint.y, worldBoundingMinPoint.z);
  glVertex3f(worldBoundingMinPoint.x, worldBoundingMaxPoint.y, worldBoundingMinPoint.z);
  glVertex3f(worldBoundingMinPoint.x, worldBoundingMaxPoint.y, worldBoundingMaxPoint.z);
  glVertex3f(worldBoundingMinPoint.x, worldBoundingMinPoint.y, worldBoundingMaxPoint.z);
  // face 3
  glVertex3f(worldBoundingMaxPoint.x, worldBoundingMinPoint.y, worldBoundingMaxPoint.z);
  glVertex3f(worldBoundingMaxPoint.x, worldBoundingMaxPoint.y, worldBoundingMaxPoint.z);
  glVertex3f(worldBoundingMinPoint.x, worldBoundingMaxPoint.y, worldBoundingMaxPoint.z);
  // face 4
  glVertex3f(worldBoundingMaxPoint.x, worldBoundingMaxPoint.y, worldBoundingMaxPoint.z);
  glVertex3f(worldBoundingMaxPoint.x, worldBoundingMaxPoint.y, worldBoundingMinPoint.z);
  glVertex3f(worldBoundingMaxPoint.x, worldBoundingMinPoint.y, worldBoundingMinPoint.z);
  glVertex3f(worldBoundingMaxPoint.x, worldBoundingMinPoint.y, worldBoundingMaxPoint.z);
  // end
  glVertex3f(worldBoundingMinPoint.x, worldBoundingMinPoint.y, worldBoundingMaxPoint.z);
  glVertex3f(worldBoundingMinPoint.x, worldBoundingMinPoint.y, worldBoundingMinPoint.z);
  glEnd();
  glEndList();
  return listID;
}

// use AABB bounding box to detect intersection
bool Unit::intersect(Unit* venus) {
  Vec3<float> a1 = this->worldBoundingMinPoint;
  Vec3<float> b1 = this->worldBoundingMaxPoint;
  Vec3<float> a2 = venus->worldBoundingMinPoint;
  Vec3<float> b2 = venus->worldBoundingMaxPoint;
  int x = 0, y = 0, z = 0;
  if ((a1.x >= a2.x && a1.x <= b2.x) ||
    (b1.x >= a2.x && a1.x <= b2.x) ||
    (a2.x >= a1.x && a2.x <= b1.x) ||
    (b2.x >= a1.x && b2.x <= b1.x))
    x = 1;
  if ((a1.y >= a2.y && a1.y <= b2.y) ||
    (b1.y >= a2.y && a1.y <= b2.y) ||
    (a2.y >= a1.y && a2.y <= b1.y) ||
    (b2.y >= a1.y && b2.y <= b1.y))
    y = 1;
  if ((a1.z >= a2.z && a1.z <= b2.z) ||
    (b1.z >= a2.z && a1.z <= b2.z) ||
    (a2.z >= a1.z && a2.z <= b1.z) ||
    (b2.z >= a1.z && b2.z <= b1.z))
    z = 1;
  if (x == 1 && y == 1 && z == 1) {
    return true;
  }
  else
    return false;
}
