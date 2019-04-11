#include <Kdnode.h>
#include <iostream>

Kdnode::Kdnode(Vectr point, int dimension) {
  me = point;
  left = NULL;
  right = NULL;
  parent = NULL;
  dim = dimension;
  checked = false;
}
