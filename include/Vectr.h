#ifndef VECTR_OBJ
#define VECTR_OBJ
class Vectr {
public:
  float x[3];

  Vectr() {}
  Vectr(float a, float b, float c);

  float mag();
  Vectr normalize();
  void printit();

  Vectr operator + (const Vectr& v) { // sum of TWO VECTORS
    return Vectr(this->x[0] + v.x[0], this->x[1] + v.x[1], this->x[2] + v.x[2]);
  }

  Vectr operator - (const Vectr& v) { // diff of TWO VECTORS
    return Vectr(this->x[0] - v.x[0], this->x[1] - v.x[1], this->x[2] - v.x[2]);
  }

  bool operator == (const Vectr& v) {
    for(int i = 0; i < 3; i++) {
      if(this->x[i] != v.x[i]) {
        return 0;
      }
    }
    return 1;
  }

  float operator | (const Vectr& v) { // dot product of TWO VECTORS
    return (this->x[0] * v.x[0]) + (this->x[1] * v.x[1]) + (this->x[2] * v.x[2]);
  }

  Vectr operator ^ (const Vectr& v) { // cross product of TWO VECTORS
    Vectr vt;
    vt.x[0] = (this->x[1] * v.x[2]) - (this->x[2] * v.x[1]);
    vt.x[1] = (this->x[2] * v.x[0]) - (this->x[0] * v.x[2]);
    vt.x[2] = (this->x[0] * v.x[1]) - (this->x[1] * v.x[0]);
    return vt;
  }

  Vectr operator + (const float f) { // sum of VECTOR with one SCALAR
    return Vectr(this->x[0] + f, this->x[1] + f, this->x[2] + f);
  }

  Vectr operator * (const float f) { // product of VECTOR with one SCALAR
    return Vectr(this->x[0] * f, this->x[1] * f, this->x[2] * f);
  }

  Vectr operator / (const float f) { // division of VECTOR with one SCALAR
    return Vectr(this->x[0] / f, this->x[1] / f, this->x[2] / f);
  }
};
#endif
