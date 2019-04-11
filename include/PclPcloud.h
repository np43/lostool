#include <Vectr.h>
#include <liblas/liblas.hpp>
#include <json.h>
#include <Node.h>
#include <Link.h>
#include <Gui.h>

#include <CGAL/enum.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2_algorithms.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_3.h>
#include <list>

#ifndef PCLPCLOUD_OBJ
#define PCLPCLOUD_OBJ

// kdtree
typedef CGAL::Simple_cartesian<double> K;
typedef K::Point_3 Point_d;
typedef CGAL::Search_traits_3<K> TreeTraits;
typedef CGAL::Orthogonal_k_neighbor_search<TreeTraits> Neighbor_search;
typedef Neighbor_search::Tree Tree;

// intersection
typedef CGAL::Exact_predicates_inexact_constructions_kernel KK;
typedef KK::Point_2 Point;

class PclPcloud {
public:
	std::list<Point_d> pCloud;
	Tree *tree;

	long pCloudSize;
	long lasTotal;
	Json::Value config;
	std::string fileName;
	Vectr center;
	int decimate;
	double maxx, maxy, maxz, minx, miny, minz;
	bool file;
	bool treeGrown;

	PclPcloud(std::string filename, Vectr av, int decim);
	~PclPcloud();
	void buildTree();
	bool checkIntersection(Vectr v1, Vectr v2);
	bool checkIntersectionProper(Vectr v1, Vectr v2);
	bool checkLOS(Vectr v1, Vectr v2);
	void optimalLink(Link *tglink);
	void viewCloud(Gui &gui);
};
#endif
