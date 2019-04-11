#include <json.h>
#include <Node.h>
#include <Link.h>
#include <Kdtree.h>

#include <QGraphicsScene>

#define MAX_POLES 1000

#ifndef NETW_OBJ
#define NETW_OBJ

class NetworkObject {
public:
  NetworkObject(Json::Value config, QGraphicsScene *scene, QWidget *window);
  ~NetworkObject();

  QGraphicsScene *gScene;
  QWidget *gWindow;
  Kdtree nodesTree;
  std::vector <Vectr> apNodes;
  std::vector <int> apNodesInt;
  std::vector <std::vector <int>> apNodesIntReduced;
  Json::Value config;
  float **network;
  bool **reducedNetwork;
  float **reducedNetworkD;
  int *labNodes;
  int theLabel;
  Link **networkL;
  std::vector <Ray> **netsublinks;
  std::vector <Node> nodes;
  std::vector <int> reducedNodes;
  Vectr centerVec;
  Vectr pMin, pMax;
  bool initialized;
  bool reduced;
  bool marked;

  float checkRange(Vectr v1, Vectr v2);

  float dist(Node n1, Node n2) {
    Vectr v1 = n1.poleLine.start;
    Vectr v2 = n2.poleLine.start;
    Vectr diff = v2 - v1;
    return (diff.mag());
  }

  std::vector <std::string> split(const std::string &s, char delim);

  bool ignore(std::string fileName);

  void setupNetworkInit(QProgressBar *pBar, QLabel *pLabel);

  void readPoles(QProgressBar *pBar, QLabel *pLabel);

  void findPoles(QProgressBar *pBar, QLabel *pLabel);

  Link singleLink(QProgressBar *pBar, QLabel *pLabel, int n1, int n2);

  void reduceNetwork();

  void doIt();

  void checkNode(int p);

  void markDisconnects();

  void fullNetwork(QProgressBar *pBar, QLabel *pLabel);

  void writeNetworkFull(QProgressBar *pBar, QLabel *pLabel);

  void readNetworkFull(QProgressBar *pBar, QLabel *pLabel);

  void printNetwork();

  int minDistance(float dist[], bool checked[]);

  std::vector <int> dijkstra(float **graph, int src, int dst);
};
#endif
