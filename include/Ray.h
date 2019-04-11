#include "Vectr.h"

#ifndef RAY_OBJ
#define RAY_OBJ
class Ray {
public:
  Vectr start, direct;
  Ray() {}
  Ray(Vectr s, Vectr d);
};
#endif
