#include <cmath>
#include <iostream>
#include <Vectr.h>
#include <PclPcloud.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2_algorithms.h>

#include <CGAL/enum.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_3.h>
#include <list>

#define OBSTRUCT_RADIUS 0.2

PclPcloud::PclPcloud(std::string filename, Vectr av, int decim) {
  Vectr a;
  fileName = filename;
  center = av;
  decimate = decim;
  std::ifstream ifs;
  ifs.open(filename, std::ios::in | std::ios::binary);
  if(ifs) {
    file = true;
    // std::cout << "Reading points from " + filename << std::endl;
    liblas::ReaderFactory f;
    liblas::Reader reader = f.CreateWithStream(ifs);
    liblas::Header const& header = reader.GetHeader();

    lasTotal = header.GetPointRecordsCount();
    pCloudSize = (lasTotal / decimate);

    maxx = header.GetMaxX() - av.x[0];
    maxy = header.GetMaxY() - av.x[1];
    maxz = header.GetMaxZ() - av.x[2];

    minx = header.GetMinX() - av.x[0];
    miny = header.GetMinY() - av.x[1];
    minz = header.GetMinZ() - av.x[2];

    treeGrown = false;
  }
  else {
    file = false;
  }
}

void PclPcloud::buildTree() {
  if(!treeGrown) {
    std::ifstream ifs;
    ifs.open(fileName, std::ios::in | std::ios::binary);
    if(ifs) {
      file = true;
      liblas::ReaderFactory f;
      liblas::Reader reader = f.CreateWithStream(ifs);

      long i = 0, j = 0;
      while((i < lasTotal) && reader.ReadPointAt(i) && (j < pCloudSize)) {
        liblas::Point const& p = reader.GetPoint();
        pCloud.push_back(Point_d(p.GetX() - center.x[0], p.GetY() - center.x[1], p.GetZ() - center.x[2]));
        i += decimate;
        j++;
      }
      tree = new Tree(pCloud.begin(), pCloud.end());
      treeGrown = true;
      std::cout << "*";
      // need to burn the tree
    }
  }
}

bool PclPcloud::checkLOS(Vectr v1, Vectr v2) {
  float radius = OBSTRUCT_RADIUS;
  if((std::min(v1.x[2], v2.x[2]) - maxz) > radius) {
    // min z of line above highest point
    return false;
  }
  Vectr diff = v2 - v1, intpoint;
  int length, pathResolution = (int)(diff.mag() / radius);
  //float step = 0.25 / pathResolution;
  float step = radius / diff.mag();
  float lambda = 3 * step;
  Vectr diffV = v2 - v1;
  bool obs = false;

  intpoint = v1 + (diffV * lambda);
  do {
    Point_d query(intpoint.x[0], intpoint.x[1], intpoint.x[2]);
    Neighbor_search search(*tree, query, 1);

    Neighbor_search::iterator it = search.begin();
    if(std::sqrt(it->second) <= radius) {
      obs = true;
    }
    lambda += step;
    intpoint = v1 + (diffV * lambda);
  }
  while((lambda < (1 - (2 * step))) && (diff.mag() > radius));
  return obs;
}

bool PclPcloud::checkIntersectionProper(Vectr v1, Vectr v2) {
  Point points[] =  {
    Point(minx, miny),
    Point(minx, maxy),
    Point(maxx, maxy),
    Point(maxx, miny)
  };

  Vectr diff = v2 - v1, pointer;
  float step = OBSTRUCT_RADIUS / diff.mag(), lambda;
  lambda = 3 * step;
  pointer = v1 + (diff * lambda);
  while(lambda < (1 - (2 * step))) {
    Point tPoint = Point(pointer.x[0], pointer.x[1]);
    for(int i = 0; i < 4; i++) {
      if((CGAL::bounded_side_2(points, points + 4, tPoint, KK()) == CGAL::ON_BOUNDED_SIDE)
          || (CGAL::bounded_side_2(points, points + 4, tPoint, KK()) == CGAL::ON_BOUNDARY)) {
          return true;
      }
    }
    lambda += step;
    pointer = v1 + (diff * lambda);
  }
  return false;
}

void PclPcloud::optimalLink(Link *tglink) {
  int contig = 0; // check for 5 contig
  for(int i = 0; i < tglink->sublinks.size(); i++) {
    if(tglink->activeSublinks[i]) {
      if(checkLOS(tglink->sublinks[i].start, tglink->sublinks[i].direct)) {
        // obstruction present
        tglink->activeSublinks[i] = false;
        tglink->nActiveSublinks--;
        if(tglink->nActiveSublinks == 0) {
          tglink->distance = -1;
        }
      }
      if(tglink->activeSublinks[i]) {
        // link still active
        contig++;
        if(contig == 5) { // 0.2 * 5 = 1m
          //break; // stop here because links are possible
        }
      }
      else {
        // good link gone bad
        contig = 0;
      }
    }
    else {
      contig = 0;
    }
  }
}

void PclPcloud::viewCloud(Gui &gui) {
  std::ifstream ifs;
  ifs.open(fileName, std::ios::in | std::ios::binary);
  if(ifs) {
    file = true;
    liblas::ReaderFactory f;
    liblas::Reader reader = f.CreateWithStream(ifs);
    liblas::Header const& header = reader.GetHeader();

    long i = 0, j, xdiff, ydiff, size = 5;
    double x, y, z;
    long total = header.GetPointRecordsCount();

    while(reader.ReadPointAt(i) && (i < (total - decimate))) {
      liblas::Point const& p = reader.GetPoint();
      x = p.GetX() - center.x[0];
      y = p.GetY() - center.x[1];
      z = p.GetZ() - center.x[2];
      gui.addPoint(Vectr(x, y, z), Vectr(0.7, 0.7, 0.7));
      i += decimate;
    }
  }
}

PclPcloud::~PclPcloud() {
  if(treeGrown) {
    delete tree;
  }
}
