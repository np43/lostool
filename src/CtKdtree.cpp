#include <Kdnode.h>
#include <Kdtree.h>

#include <iostream>
#include <cmath>

Kdtree::Kdtree() {
  root = NULL;
}

Kdnode* Kdtree::parentNodeFor(Vectr point) {
  Kdnode *nxt = root, *prev = root;
  if(!nxt) {
    return NULL;
  }
  else {
    float dv1, dv2;
    while(nxt) {
      if(nxt->me == point) {
        return nxt;
      }
      dv1 = point.x[nxt->dim];
      dv2 = nxt->me.x[nxt->dim];
      prev = nxt;
      nxt = (dv1 > dv2)?nxt->right:nxt->left;
    }
    prev->leftOrRight = (dv1 > dv2)?1:0;
    return prev;
  }
}

void Kdtree::add(Vectr point) {
  Kdnode *parent = parentNodeFor(point);
  if(!parent) {
    // needs root
    root = new Kdnode(point, 0);
    nodes.push_back(root);
  }
  else {
    Kdnode *newNode = new Kdnode(point, (parent->dim + 1) % 3);
    newNode->parent = parent;
    nodes.push_back(newNode);
    ((parent->leftOrRight)?parent->right:parent->left) = newNode;
  }
}

Kdnode* Kdtree::suckyFindNearest(Vectr point) {
  Kdnode *closest;
  float closeDist;
  closeDist = distance(point, root->me);
  for(long int i = 0; i < nodes.size(); i++) {
    if(distance(point, nodes[i]->me) < closeDist) {
      closest = nodes[i];
      closeDist = distance(point, closest->me);
    }
  }
  return closest;
}

Vectr Kdtree::findNearest(Vectr point) {
  Kdnode *closest = parentNodeFor(point), *parent = closest->parent;
  if(closest->me == point) {
    return closest->me;
  }

  while(parent) {
    Kdnode *temp = searchSubtreeAt(parent, point, closest);
    if(distance(point, temp->me) < distance(point, closest->me)) {
      closest = temp;
    }
    parent = parent->parent;
  }
  return closest->me;
}

Kdnode* Kdtree::searchSubtreeAt(Kdnode *subroot, Vectr point, Kdnode *closest) {
  if(!subroot) {
    return closest;
  }

  if(subroot->checked) {
    return closest;
  }

  subroot->checked = true;
  Kdnode *temp;
  float dimDist, closeDist = distance(point, closest->me);

  if(closeDist > distance(point, subroot->me)) {
    closest = subroot;
    closeDist = distance(point, closest->me);
  }

  dimDist = point.x[subroot->dim] - subroot->me.x[subroot->dim];
  if(fabs(dimDist) > closeDist) { // ignore one of the subtrees
    if(dimDist > 0) {
      // ignore left subtree, traverse right subtree
      temp = searchSubtreeAt(subroot->right, point, closest);
      if(distance(point, temp->me) < closeDist) {
        closest = temp;
      }
    }
    else {
      // ignore right subtree, traverse left subtree
      temp = searchSubtreeAt(subroot->left, point, closest);
      if(distance(point, temp->me) < closeDist) {
        closest = temp;
      }
    }
  }
  else { // don't ignore the subtrees, traverse both
    temp = searchSubtreeAt(subroot->right, point, closest);
    if(distance(point, temp->me) < closeDist) {
      closest = temp;
    }
    temp = searchSubtreeAt(subroot->left, point, closest);
    if(distance(point, temp->me) < distance(point, closest->me)) {
      closest = temp;
    }
  }
  return closest;
}

float Kdtree::distance(Vectr v1, Vectr v2) {
  Vectr v = (v1 - v2);
  return v.mag();
}

void Kdtree::print() {
  printf("Printing ..\n");
  for(int i = 0; i < nodes.size(); i++) {
    printf("------------------------------------\n");
    printf("Point: (%f, %f)\n", nodes[i]->me.x[0], nodes[i]->me.x[1]);
    if(nodes[i]->left) {
      printf(" Left: (%f, %f)\n", nodes[i]->left->me.x[0], nodes[i]->left->me.x[1]);
    }
    if(nodes[i]->right) {
      printf("Right: (%f, %f)\n", nodes[i]->right->me.x[0], nodes[i]->right->me.x[1]);
    }
  }
}

void Kdtree::burn() {
  for(int i = 0; i < nodes.size(); i++) {
    delete nodes[i];
  }
}
