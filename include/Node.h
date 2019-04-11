#include <vector>
#include <Vectr.h>
#include <Ray.h>

#ifndef NODE_OBJ
#define NODE_OBJ
class Node {
public:
	std::vector <Vectr> vertices;
	std::vector <Vectr> points;
	Ray poleLine;	
	Node();
	Node(std::vector <Vectr> pts);	
	Vectr centroid();

	// to be removed in THE FUTURE !
	Ray fitLine();
};
#endif
