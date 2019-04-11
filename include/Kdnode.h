#include "Vectr.h"

#ifndef KDNODE_OBJ
#define KDNODE_OBJ

class Kdnode {
public:
  Vectr me;
  Kdnode *left, *right, *parent;
  int dim;
  int leftOrRight;
  bool checked;

  Kdnode(Vectr point, int dimension);
};

#endif
