/*
Lecture 04
http://slides.com/javiergs/ser431-l04
*/

#include "Imathvec.h"
using namespace Imath;

// this makes the following declaration easier to write
typedef Vec3<float> Vec3f;

Vec3f v0; // instead of Vec3<float> v0;
Vec3f v1;
Vec3f v2;

//calculate the area of a triangle given three vertices
template<class T>
T triangle_area(const Vec3<T> &v1, const Vec3<T> &v2, const Vec3<T> &v3) {
  Vec3<T> AB = v2 - v1, AC = v3 - v1;
  return (AB.cross(AC)).length() / 2;
}

//main
void main() {
  v0.setValue(-1, 0, 0);  //Ok
  v1.setValue(1.0, 0.0, 0.0); //Ok		
                              // v1.setValue(1.0, 0, 0); <-- ERROR: type mismatch
  v2.setValue(0, 1, 0); //Ok
  printf("V0 = (%f, %f, %f)\n", v0.x, v0.y, v0.z);
  printf("v1 = (%f, %f, %f)\n", v1.x, v1.y, v1.z);
  printf("v2 = (%f, %f, %f)\n", v2.x, v2.y, v2.z);
  // dot product
  float dotproduct = v1.dot(v2);
  printf("v1 * v2 = %f\n", dotproduct);

  //cross product
  Vec3f v3 = v1.cross(v2);
  printf("v0 x v1 = (%.2f,%.2f,%.2f)\n", v3.x, v3.y, v3.z);

  // test equality
  v1.setValue(100.0, 100.0, 100.0);
  v2 = v1;
  v2 += Vec3f(1, 0, 0);
  printf("v1 = (%.2f,%.2f,%.2f)\n", v1.x, v1.y, v1.z);
  printf("v2 = (%.2f,%.2f,%.2f)\n", v2.x, v2.y, v2.z);
  printf("v1 ==  v2 is (%i)\n", (v1==v2));

  //calculate the area of the triangle with vertex v0,v1,v2
  float area = triangle_area(v0, v1, v2);
  printf("the area of the triangle is %.4f\n", area);
}
