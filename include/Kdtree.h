#include "Kdnode.h"
#include <vector>

#ifndef KDTREE_OBJ
#define KDTREE_OBJ

class Kdtree {
public:
  Kdnode *root;
  std::vector <Kdnode*> nodes;

  Kdtree();
  void add(Vectr point);
  Kdnode* parentNodeFor(Vectr point);
  Kdnode* suckyFindNearest(Vectr point);
  Vectr findNearest(Vectr point);
  Kdnode* searchSubtreeAt(Kdnode *subroot, Vectr point, Kdnode *closest);
  float distance(Vectr v1, Vectr v2);
  void print();
  void burn();
};

#endif
