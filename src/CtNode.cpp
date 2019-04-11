#include <cmath>
#include <iostream>
#include <Vectr.h>
#include <Node.h>
#include <cfloat>

Node::Node() {

}

Node::Node(std::vector <Vectr> pts) {
  points = pts;
}

// to be removed in THE FUTURE !
Ray Node::fitLine() {
  double zmax = DBL_MIN, zmin = DBL_MAX;
  Vectr sum = Vectr(0, 0, 0);
  for(size_t jj = 0; jj != points.size(); jj++) {
    sum = sum + points[jj];
    if(points[jj].x[2] > zmax) {
      zmax = points[jj].x[2];
    }
    if(points[jj].x[2] < zmin) {
      zmin = points[jj].x[2];
    }
  }
  Vectr center = sum / points.size();
  Vectr bottom = center, top = center;
  bottom.x[2] = zmin;
  top.x[2] = zmax;
  poleLine = Ray(bottom, top);
}

Vectr Node::centroid() {
  Vectr sum(0, 0, 0);
  for(int i = 0; i < points.size(); i++) {
    sum = sum + points[i];
  }
  return (sum / points.size());
}
