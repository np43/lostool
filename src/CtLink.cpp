#include <cmath>
#include <iostream>
#include <Vectr.h>
#include <Node.h>
#include <Link.h>

Link::Link(Node n1, Node n2) {
	nodes[0] = n1;
	nodes[1] = n2;

	// setup sublinks
  Ray p1 = n1.poleLine;
  Ray p2 = n2.poleLine;

	lmaxx = std::max(p1.start.x[0], p2.start.x[0]);
  lminx = std::min(p1.start.x[0], p2.start.x[0]);
  lmaxy = std::max(p1.start.x[1], p2.start.x[1]);
  lminy = std::min(p1.start.x[1], p2.start.x[1]);

	Vectr dist = p1.start - p2.start;
	distance = dist.mag();
  double minz = std::max(p1.start.x[2], p2.start.x[2]) + 2.5;
  double maxz = std::min(p1.direct.x[2], p2.direct.x[2]);
	double stepz = 0.2; // 20 cm steps
	Vectr zinc = Vectr(0, 0, stepz);

  Vectr polePointer = Vectr(p1.start.x[0],
                            p1.start.x[1],
                            minz);
  Vectr polePointer2 = Vectr(p2.start.x[0],
                             p2.start.x[1],
                             minz);
	while(polePointer.x[2] < maxz) {
		sublinks.push_back(Ray(polePointer, polePointer2));
		activeSublinks.push_back(true);
		polePointer = polePointer + zinc;
		polePointer2 = polePointer2 + zinc;
  }
	nActiveSublinks = sublinks.size();
}
