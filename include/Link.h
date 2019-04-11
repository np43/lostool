#include <Vectr.h>
#include <Node.h>

#ifndef LINK_OBJ
#define LINK_OBJ
class Link {
public:
	Node nodes[2];
	float distance;
	double lmaxx, lminx, lmaxy, lminy;
	Link() {}
	Link(Node n1, Node n2);

	int nActiveSublinks;
	std::vector <Ray> sublinks;
	std::vector <bool> activeSublinks;
};
#endif
