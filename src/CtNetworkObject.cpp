#include <iostream>
#include <fstream>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include <QApplication>
#include <QProgressBar>
#include <QLabel>
#include <QGraphicsTextItem>

#include <json.h>
#include <liblas/liblas.hpp>

#include <Node.h>
#include <Link.h>
#include <PclPcloud.h>
#include <NetworkObject.h>

NetworkObject::NetworkObject(Json::Value cnf, QGraphicsScene *scene, QWidget *window) {
  config = cnf;
  initialized = false;
  reduced = false;
  marked = false;
  gScene = scene;
  gWindow = window;
}

NetworkObject::~NetworkObject() {
  if(initialized) {
    for(int i = 0; i < nodes.size(); i++) {
      delete[] network[i];
    }
    delete[] network;

    for(int i = 0; i < nodes.size(); i++) {
      delete[] networkL[i];
    }
    delete[] networkL;

    for(int i = 0; i < nodes.size(); i++) {
      delete[] netsublinks[i];
    }
    delete[] netsublinks;
  }

  if(reduced) {
    for(int i = 0; i < reducedNodes.size(); i++) {
      delete[] reducedNetwork[i];
      delete[] reducedNetworkD[i];
    }
    delete[] reducedNetwork;
    delete[] reducedNetworkD;
  }

  if(marked) {
    delete[] labNodes;
  }

  nodesTree.burn();
}

float NetworkObject::checkRange(Vectr v1, Vectr v2) {
  Vectr diff = v2 - v1;
  if((diff.mag() > stof(config["max_dist"].asString())) || (diff.mag() < stof(config["min_dist"].asString()))) {
    return -1;
  }
  else {
    return diff.mag();
  }
}

// run ONLY AFTER the poles are found
void NetworkObject::setupNetworkInit(QProgressBar *pBar, QLabel *pLabel) {
  network = new float*[nodes.size()];
  networkL = new Link*[nodes.size()];
  netsublinks = new std::vector <Ray> *[nodes.size()];
  for(int i = 0; i < nodes.size(); i++) {
    network[i] = new float[nodes.size()];
    networkL[i] = new Link[nodes.size()];
    netsublinks[i] = new std::vector <Ray>[nodes.size()];
  }

  std::ifstream netFile(config["netFile"].asString());
  if(netFile.good()) {
    std::cout << "network file found" << std::endl;
    readNetworkFull(pBar, pLabel);
  }
  else {
    std::cout << "no netfile found" << std::endl;
  }

  // now find access points
  float size = 5, xdiff, ydiff;
  for(int i = 0; i < nodes.size(); i++) {
    nodesTree.add(Vectr(nodes[i].poleLine.start.x[0], nodes[i].poleLine.start.x[1], 0));
  }
  float step = stof(config["ap_range"].asString());
  std::vector <float> xticks;
  float xinc = pMin.x[0];
  float yinc = pMin.x[1];
  while(xinc < pMax.x[0]) {
    yinc = pMin.x[1];
    while(yinc < pMax.x[1]) {
      Vectr closePole = nodesTree.findNearest(Vectr(xinc, yinc, 0));
      for(int i = 0; i < nodes.size(); i++) {
        Vectr vtemp = nodes[i].poleLine.start;
        xdiff = abs(closePole.x[0] - vtemp.x[0]);
        ydiff = abs(closePole.x[1] - vtemp.x[1]);
        if((xdiff <= size) && (ydiff <= size)) {
          int j;
          for(j = 0; j < apNodesInt.size(); j++) {
            if(apNodesInt[j] == i) {
              break;
            }
          }
          if(j == apNodesInt.size()) {
            apNodesInt.push_back(i);
            apNodes.push_back(closePole);
          }
          break;
        }
      }
      yinc += step;
    }
    xinc += step;
  }
  // clustering the accesspoints
  float sizeap = 100;
  for(int i = 0; i < apNodesInt.size(); i++) {
    bool marked = false;
    for(int j = 0; j < apNodesIntReduced.size(); j++) {
      Vectr ref = nodes[apNodesIntReduced[j][0]].poleLine.start;
      ref = ref - nodes[apNodesInt[i]].poleLine.start;
      if(ref.mag() < sizeap) {
        marked = true;
        apNodesIntReduced[j].push_back(apNodesInt[i]);
      }
    }
    if(!marked) {
      std::vector <int> vt;
      vt.push_back(apNodesInt[i]);
      apNodesIntReduced.push_back(vt);
    }
  }

  // draw the points
  // double rad = 0.5, rad2 = 3, rad3 = 5;
  // QPen pointPen;
  //
  // pointPen.setColor(QColor(70, 70, 70, 128));
  // for(int i = 0; i < nodes.size(); i++) {
  //   Vectr vtr = nodes[i].poleLine.start;
  //   gScene->addEllipse(vtr.x[0] - rad, vtr.x[1] - rad, rad * 2.0, rad * 2.0, pointPen, QBrush(QColor(70, 70, 70, 128), Qt::SolidPattern));
  //   QGraphicsTextItem *text = gScene->addText(QString::number(i), QFont("Hack", 2));
  //   text->setPos(vtr.x[0] - (5 * rad), vtr.x[1] - (5 * rad));
  // }

  // pointPen.setColor(QColor(219, 50, 54, 200));
  // for(int i = 0; i < apNodes.size(); i++) {
  //   Vectr vtr = apNodes[i];
  //   gScene->addEllipse(vtr.x[0] - rad2, vtr.x[1] - rad2, rad2 * 2.0, rad2 * 2.0, pointPen, QBrush(QColor(219, 50, 54, 0), Qt::SolidPattern));
  // }
  // pointPen.setColor(QColor(0, 87, 231, 140));
  // for(int i = 0; i < apNodesInt.size(); i++) {
  //   Vectr vtr = nodes[apNodesInt[i]].poleLine.start;
  //   gScene->addEllipse(vtr.x[0] - rad3, vtr.x[1] - rad3, rad3 * 2.0, rad3 * 2.0, pointPen, QBrush(QColor(0, 87, 231, 0), Qt::SolidPattern));
  // }

  // pointPen.setColor(QColor(0, 87, 231, 140));
  // for(int i = 0; i < apNodesIntReduced.size(); i++) {
  //   Vectr vtr = nodes[apNodesIntReduced[i][0]].poleLine.start;
  //   gScene->addEllipse(vtr.x[0] - rad3, vtr.x[1] - rad3, rad3 * 2.0, rad3 * 2.0, pointPen, QBrush(QColor(0, 87, 231, 0), Qt::SolidPattern));
  // }

  initialized = true;
}

void NetworkObject::findPoles(QProgressBar *pBar, QLabel *pLabel) {
  int fStart = stoi(config["folderStart"].asString());
  int fEnd = stoi(config["folderEnd"].asString());
  int fIter;
  int nsets = fEnd - fStart;
  if(nsets == 0) {
    nsets = 1;
  }

  long totalPoints = 0, runningTotal = 0, decim = 16;

  std::ofstream polesFile;
  polesFile.open(config["polesFile"].asString());
  double minx = DBL_MAX, miny = DBL_MAX, minz = DBL_MAX;
  double maxx = DBL_MIN, maxy = DBL_MIN, maxz = DBL_MIN;
  pLabel->setText("Centering ..");
  for(fIter = fStart; fIter <= fEnd; fIter++) {
    std::ifstream ifs;
    std::string polefilename = config["lasdir"].asString() + "/" + config["folderprefix"].asString() + std::to_string(fIter) + "/" + config["polesCloudFile"].asString();
    std::cout << "reading file " << polefilename << std::endl;

    ifs.open(polefilename, std::ios::in | std::ios::binary);
    liblas::ReaderFactory f;
    liblas::Reader reader = f.CreateWithStream(ifs);
    liblas::Header const& header = reader.GetHeader();
    totalPoints += header.GetPointRecordsCount();
    minx = std::min(header.GetMinX(), minx);
    miny = std::min(header.GetMinY(), miny);
    minz = std::min(header.GetMinZ(), minz);
    maxx = std::max(header.GetMaxX(), maxx);
    maxy = std::max(header.GetMaxY(), maxy);
    maxz = std::max(header.GetMaxZ(), maxz);
  }

  totalPoints /= decim;
  runningTotal = totalPoints / 100;

  pBar->setRange(0, 100);
  pBar->setValue(0);

  double xav, yav, zav;
  xav = (minx + maxx) / 2;
  yav = (miny + maxy) / 2;
  zav = (minz + maxz) / 2;
  polesFile << xav << " " << yav << " " << zav << "\n";

  minx -= xav;
  miny -= yav;
  minz -= zav;
  maxx -= xav;
  maxy -= yav;
  maxz -= zav;

  pMin = Vectr(minx, miny, minz);
  pMax = Vectr(maxx, maxy, maxz);

  polesFile << minx << " " << miny << " " << minz << " ";
  polesFile << maxx << " " << maxy << " " << maxz << "\n";

  int fUp = 0;
  // reading poles
  for(fIter = fStart; fIter <= fEnd; fIter++) {
    std::ifstream ifs;
    std::string polefilename = config["lasdir"].asString() + "/" + config["folderprefix"].asString() + std::to_string(fIter) + "/" + config["polesCloudFile"].asString();
    ifs.open(polefilename,  std::ios::in | std::ios::binary);
    std::cout << "Reading poles from " + config["folderprefix"].asString() + std::to_string(fIter) << std::endl;
    pLabel->setText("Reading poles from " + QString(config["folderprefix"].asString().c_str()) + QString(std::to_string(fIter).c_str()));
    liblas::ReaderFactory f;
    liblas::Reader reader = f.CreateWithStream(ifs);
    liblas::Header const& header = reader.GetHeader();

    long i = 0, j, xdiff, ydiff, size = 5;
    double x, y, z;
    long total = header.GetPointRecordsCount();
    std::cout << "Total points: " << total << std::endl;
    std::vector <Vectr> vtemp;
    int marked;

    while(reader.ReadPointAt(i) && (i < (total - decim))) {
      runningTotal++;
      pBar->setValue((runningTotal * 100) / totalPoints);
      QApplication::processEvents();
      liblas::Point const& p = reader.GetPoint();
      x = p.GetX() - xav;
      y = p.GetY() - yav;
      z = p.GetZ() - zav;

      marked = 0;
      for(size_t ii = 0; ii != nodes.size(); ii++) {
        vtemp = nodes[ii].points;
        xdiff = abs(x - vtemp[0].x[0]);
        ydiff = abs(y - vtemp[0].x[1]);
        if((xdiff <= size) && (ydiff <= size)) {
          marked = 1;
          nodes[ii].points.push_back(Vectr(x, y, z));
          break;
        }
      }
      if(marked == 0) {
        Node nd;
        nd.points.push_back(Vectr(x, y, z));
        nodes.push_back(nd);
      }
      i += decim;
    }
  }

  pLabel->setText("Writing to file ..");
  polesFile << nodes.size() << "\n";
  for(int i = 0; i < nodes.size(); i++) {
    nodes[i].fitLine();
    polesFile << nodes[i].poleLine.start.x[0] << " " <<
                 nodes[i].poleLine.start.x[1] << " " <<
                 nodes[i].poleLine.start.x[2] << " " <<
                 nodes[i].poleLine.direct.x[0] << " " <<
                 nodes[i].poleLine.direct.x[1] << " " <<
                 nodes[i].poleLine.direct.x[2] << "\n";
  }
  polesFile.close();
  pLabel->setText("Done !");
}

void NetworkObject::readPoles(QProgressBar *pBar, QLabel *pLabel) {
  std::ifstream polesFile(config["polesFile"].asString());
  std::string lineString;
  int nPoles;

  getline(polesFile, lineString);
  std::stringstream lineStrm(lineString);

  lineStrm >> centerVec.x[0];
  lineStrm >> centerVec.x[1];
  lineStrm >> centerVec.x[2];

  // read extremes
  getline(polesFile, lineString);
  std::stringstream lineStrm3(lineString);
  lineStrm3 >> pMin.x[0];
  lineStrm3 >> pMin.x[1];
  lineStrm3 >> pMin.x[2];
  lineStrm3 >> pMax.x[0];
  lineStrm3 >> pMax.x[1];
  lineStrm3 >> pMax.x[2];

  // read number of poles
  getline(polesFile, lineString);
  std::stringstream lineStrm2(lineString);
  lineStrm2 >> nPoles;

  pBar->setRange(0, nPoles);
  pBar->setValue(0);
  pLabel->setText("Reading poles from file ..");

  while(getline(polesFile, lineString)) {
    std::stringstream lineStream(lineString);
    Node nd;
    Vectr v;
    for(int i = 0; i < 3; i++) {
      lineStream >> v.x[i];
    }
    nd.poleLine.start = v;
    for(int i = 0; i < 3; i++) {
      lineStream >> v.x[i];
    }
    nd.poleLine.direct = v;
    nodes.push_back(nd);
    pBar->setValue(nodes.size());
    //pBar->setFormat(QString::number(nodes.size()) + " poles read from file");
  }

  pLabel->setText(QString::number(nodes.size()) + " poles read from file");
}

std::vector <std::string> NetworkObject::split(const std::string &s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector <std::string> tokens;
    while(getline(ss, item, delim)) {
      tokens.push_back(item);
    }
    return tokens;
}

bool NetworkObject::ignore(std::string fileName) {
  Json::Value ignoreLas = config["ignorelas"];
  for(int i = 0; i < ignoreLas.size(); i++) {
    if(fileName.compare(ignoreLas[i].asString()) == 0) {
      return true;
    }
  }
  return false;
}

Link NetworkObject::singleLink(QProgressBar *pBar, QLabel *pLabel, int n1, int n2) {
  QPen linePen;
  linePen.setWidth(0.5);
  linePen.setColor(QColor(0, 87, 231, 255));

  int fStart = stoi(config["folderStart"].asString());
  int fEnd = stoi(config["folderEnd"].asString());
  int fIter;
  int nsets = fEnd - fStart;
  if(nsets == 0) {
    nsets = 1;
  }

  Link testLink(nodes[n1], nodes[n2]);
  testLink.distance = checkRange(nodes[n1].poleLine.start, nodes[n2].poleLine.start);
  std::cout << "Testing link: " << n1 << ", " << n2 << std::endl;

  QGraphicsPolygonItem *sethl;
  QPolygonF bbBox;
  for(fIter = fStart; fIter <= fEnd; fIter++) {
    std::string setpath = config["lasdir"].asString() + "/" + config["folderprefix"].asString() + std::to_string(fIter) + "/las";

    int nFiles = 0;
    for(auto &laspath:fs::directory_iterator(setpath)) {
      nFiles++;
    }
    pBar->setRange(0, nFiles);
    pBar->setValue(0);
    int prog = 0;
    for(auto &laspath:fs::directory_iterator(setpath)) {
      prog++;
      pBar->setValue(prog);

      std::cout << laspath << std::endl;
      std::string fileName;
      std::vector <std::string> parts;
      parts = split(laspath.path().string(), '/');
      fileName = parts[parts.size() - 1];
      if(ignore(fileName)) {
        std::cout << "skipping" << std::endl;
        continue;
      }
      pLabel->setText(QString(config["folderprefix"].asString().c_str()) + QString::number(fIter) + ": " + QString(fileName.c_str()));
      PclPcloud cloud(laspath.path().string(), centerVec, 8);
      QPolygonF bBox;
      bBox << QPointF(cloud.minx, cloud.miny)
            << QPointF(cloud.minx, cloud.maxy)
            << QPointF(cloud.maxx, cloud.maxy)
            << QPointF(cloud.maxx, cloud.miny);
      sethl = gScene->addPolygon(bBox, QPen(), QBrush(QColor(22, 200, 76, 60), Qt::SolidPattern));
      gWindow->repaint();
      QApplication::processEvents();

      if(cloud.checkIntersectionProper(nodes[n1].poleLine.start, nodes[n2].poleLine.start)) {
        cloud.buildTree();
        cloud.optimalLink(&testLink);
      }

      // if(cloud.checkIntersection(nodes[n1].poleLine.start, nodes[n2].poleLine.start)) {
      //   cloud.buildTree();
      //   cloud.optimalLink2(&testLink);
      // }
      // else {
      //   // out of range
      //   std::cout << n1 << ", " << n2 << ": " << n2 << " [out of range]" << std::endl;
      // }

      gScene->removeItem(sethl);
    } // for each las file in the set
  } // for each set
  return testLink;
}

void NetworkObject::doIt() {

}

void NetworkObject::checkNode(int p) {
  int connections = 0;
  for(int i = 0; i < reducedNodes.size(); i++) {
    if(reducedNetwork[p][i]) {
      connections++;
    }
  }
  if(connections == 1) {
    return;
  }
  else {
    for(int i = 0; i < reducedNodes.size(); i++) {
      if(reducedNetwork[p][i]) {
        if(labNodes[i] < 0) {
          labNodes[i] = labNodes[p];
          checkNode(i);
        }
      }
    }
  }
}

void NetworkObject::markDisconnects() {
  // do DFS
  labNodes = new int[reducedNodes.size()];

  for(int i = 0; i < reducedNodes.size(); i++) {
    labNodes[i] = -1;
  }

  double rad = 2;
  QPen pointPen;
  pointPen.setColor(QColor(22, 76, 200, 128));

  theLabel = 0;
  for(int i = 0; i < reducedNodes.size(); i++) {
    if(labNodes[i] < 0) {
      // unlabelled so label
      theLabel++;
      labNodes[i] = theLabel;
      checkNode(i);
    }
    if(labNodes[i] == 1) {
      Vectr vtr = nodes[reducedNodes[i]].poleLine.start;
      gScene->addEllipse(vtr.x[0] - rad, vtr.x[1] - rad, rad * 2.0, rad * 2.0, pointPen, QBrush(QColor(0, 0, 0, 0), Qt::SolidPattern));
    }
  }
  marked = true;
}

void NetworkObject::reduceNetwork() {
  for(int i = 0; i < nodes.size(); i++) {
    bool okay = false;
    for(int j = 0; j < nodes.size(); j++) {
      // check for any link
      if(netsublinks[i][j].size() > 0) {
        okay = true;
        break;
      }
    }
    if(okay) {
      reducedNodes.push_back(i);
    }
  }

  reducedNetwork = new bool*[reducedNodes.size()];
  reducedNetworkD = new float*[reducedNodes.size()];
  for(int i = 0; i < reducedNodes.size(); i++) {
    reducedNetwork[i] = new bool[reducedNodes.size()];
    reducedNetworkD[i] = new float[reducedNodes.size()];
  }

  for(int i = 0; i < reducedNodes.size(); i++) {
    reducedNetwork[i][i] = false;
  }

  for(int i = 0; i < reducedNodes.size(); i++) {
    for(int j = (i + 1); j < reducedNodes.size(); j++) {
      if(netsublinks[reducedNodes[i]][reducedNodes[j]].size() > 0) {
        reducedNetwork[i][j] = true;
        reducedNetworkD[i][j] = dist(nodes[reducedNodes[i]], nodes[reducedNodes[j]]);
      }
      else {
        reducedNetwork[i][j] = false;
        reducedNetworkD[i][j] = FLT_MAX;
      }
      reducedNetwork[j][i] = reducedNetwork[i][j];
      reducedNetworkD[j][i] = reducedNetworkD[i][j];
    }
  }

  // draw the points
  double rad = 0.5, rad2 = 3, rad3 = 5;
  QPen pointPen;

  pointPen.setColor(QColor(70, 70, 70, 128));
  for(int i = 0; i < reducedNodes.size(); i++) {
    Vectr vtr = nodes[reducedNodes[i]].poleLine.start;
    gScene->addEllipse(vtr.x[0] - rad, vtr.x[1] - rad, rad * 2.0, rad * 2.0, pointPen, QBrush(QColor(70, 70, 70, 128), Qt::SolidPattern));
    QGraphicsTextItem *text = gScene->addText(QString::number(reducedNodes[i]), QFont("Hack", 2));
    text->setPos(vtr.x[0] - (5 * rad), vtr.x[1] - (5 * rad));
  }

  reduced = true;
}

void NetworkObject::fullNetwork(QProgressBar *pBar, QLabel *pLabel) {
  QPen linePen;
  linePen.setWidth(0.5);
  linePen.setColor(QColor(0, 87, 231, 255));

  int fStart = stoi(config["folderStart"].asString());
  int fEnd = stoi(config["folderEnd"].asString());
  int fIter;
  int nsets = fEnd - fStart;
  if(nsets == 0) {
    nsets = 1;
  }
  std::cout << "conf data: " << config["lasdir"].asString() << std::endl;

  for(int i = 0; i < nodes.size(); i++) {
    for(int j = (i + 1); j < nodes.size(); j++) {
      network[i][j] = checkRange(nodes[i].poleLine.start, nodes[j].poleLine.start);
      network[j][i] = checkRange(nodes[i].poleLine.start, nodes[j].poleLine.start);
      networkL[i][j] = Link(nodes[i], nodes[j]);
      networkL[j][i] = Link(nodes[i], nodes[j]);
      networkL[i][j].distance = network[i][j];
      networkL[j][i].distance = network[j][i];
    }
  }

  QGraphicsPolygonItem *sethl;
  //QGraphicsLineItem *gLine;
  QPolygonF bbBox;
  for(fIter = fStart; fIter <= fEnd; fIter++) {
    std::string setpath = config["lasdir"].asString() + "/" + config["folderprefix"].asString() + std::to_string(fIter) + "/las";

    int nFiles = 0;
    for(auto &laspath:fs::directory_iterator(setpath)) {
      nFiles++;
    }
    pBar->setRange(0, nFiles);
    pBar->setValue(0);
    int prog = 0;
    for(auto &laspath:fs::directory_iterator(setpath)) {
      prog++;
      pBar->setValue(prog);
      QApplication::processEvents();

      std::cout << laspath << std::endl;
      std::string fileName;
      std::vector <std::string> parts;
      parts = split(laspath.path().string(), '/');
      fileName = parts[parts.size() - 1];
      if(ignore(fileName)) {
        std::cout << "skipping" << std::endl;
        continue;
      }
      pLabel->setText(QString(config["folderprefix"].asString().c_str()) + QString::number(fIter) + ": " + QString(fileName.c_str()));
      PclPcloud cloud(laspath.path().string(), centerVec, 8);
      QPolygonF bBox;
      bBox << QPointF(cloud.minx, cloud.miny)
            << QPointF(cloud.minx, cloud.maxy)
            << QPointF(cloud.maxx, cloud.maxy)
            << QPointF(cloud.maxx, cloud.miny);
      sethl = gScene->addPolygon(bBox, QPen(), QBrush(QColor(22, 200, 76, 60), Qt::SolidPattern));

      int tott = nodes.size() * (nodes.size() - 1) / 2, counting = 0, stepp;
      stepp = tott / 50;

      QPen linePen;
      linePen.setWidth(2);
      linePen.setColor(QColor(219, 50, 54, 255));

      for(int i = 0; i < nodes.size(); i++) {
        for(int j = (i + 1); j < nodes.size(); j++) {
          counting++;
          if((counting % stepp) == 0) {
            std::cout << "#";
          }
          if((networkL[i][j].distance > 0)) {
            if(cloud.checkIntersectionProper(nodes[i].poleLine.start, nodes[j].poleLine.start)) {
              cloud.buildTree();
              cloud.optimalLink(&networkL[i][j]);
            }
            else {
              // no intersection
              // std::cout << "no intersection" << std::endl;
            }
          }
          else {
            // too far
            // std::cout << "too far" << std::endl;
          }
        }
      }
      std::cout << std::endl;
      gScene->removeItem(sethl);
    } // for each las file in the set
  } // for each set

  writeNetworkFull(pBar, pLabel);
}

void NetworkObject::writeNetworkFull(QProgressBar *pBar, QLabel *pLabel) {
  std::cout << "Writing to network.txt" << std::endl;
  std::ofstream netFile;
  netFile.open(config["netFile"].asString());

  for(int m = 0; m < nodes.size(); m++) {
    for(int p = (m + 1); p < nodes.size(); p++) {
      float dis = networkL[m][p].distance;
      if((networkL[m][p].nActiveSublinks > 0) && (dis > 0)) {
        if((m == 12) && (p == 32)) {
          std::cout << "writing: " << networkL[m][p].nActiveSublinks << std::endl;
        }
        netFile << m << " " << p << " " << dis << " " << networkL[m][p].nActiveSublinks << "\n";
        for(int i = 0; i < networkL[m][p].sublinks.size(); i++) {
          if(networkL[m][p].activeSublinks[i]) {
            netFile << networkL[m][p].sublinks[i].start.x[0] << " " <<
                       networkL[m][p].sublinks[i].start.x[1] << " " <<
                       networkL[m][p].sublinks[i].start.x[2] << " " <<
                       networkL[m][p].sublinks[i].direct.x[0] << " " <<
                       networkL[m][p].sublinks[i].direct.x[1] << " " <<
                       networkL[m][p].sublinks[i].direct.x[2] << "\n";
          }
        }
      }
      else {
        netFile << m << " " << p << " " << -1 << "\n";
      }
    }
  }

  std::cout << "Network written to " << config["netFile"].asString() << std::endl;
  netFile.close();
}

void NetworkObject::readNetworkFull(QProgressBar *pBar, QLabel *pLabel) {
  std::cout << "Reading from network.txt" << std::endl;
      std::ifstream netFile(config["netFile"].asString());
      std::string lineString;
      while(getline(netFile, lineString)) {
        std::stringstream lineStream(lineString);
        int m, n, nlinks;
        float dist;
        lineStream >> m;
        lineStream >> n;
        lineStream >> dist;
        // std::cout << "m: " << m << ", n: " << n << ", dist: " << dist << std::endl;
        // network[m][n] = dist;
        // network[n][m] = dist;
        if(dist > 0) {
          lineStream >> nlinks;
          network[m][n] = dist;
          network[n][m] = dist;
          if(nlinks == 0) {
            network[m][n] = FLT_MAX;
            network[n][m] = FLT_MAX;
          }
          while(nlinks > 0) {
            getline(netFile, lineString);
            std::stringstream lineStream2(lineString);
            Vectr st, dt;
            for(int i = 0; i < 3; i++) {
              lineStream2 >> st.x[i];
            }
            for(int i = 0; i < 3; i++) {
              lineStream2 >> dt.x[i];
            }
            netsublinks[m][n].push_back(Ray(st, dt));
            netsublinks[n][m].push_back(Ray(st, dt));
            nlinks--;
          }
        }
        else {
          network[m][n] = FLT_MAX;
          network[n][m] = FLT_MAX;
        }
      }
      std::cout << "Full network read from " << config["netFile"].asString() << std::endl;
}

void NetworkObject::printNetwork() {
  for(int i = 0; i < nodes.size(); i++) {
    for(int j = (i + 1); j < nodes.size(); j++) {
      if(i == 71 || j == 17) {
        std::cout << network[j][i] << "\n";
      }
    }
  }
}

int NetworkObject::minDistance(float dist[], bool checked[]) {
  int nNodes = nodes.size();
  float min = FLT_MAX;
  int min_index;
  for(int v = 0; v < nNodes; v++) {
    if(checked[v] == false && dist[v] <= min) {
      min = dist[v], min_index = v;
    }
  }
  return min_index;
}

std::vector <int> NetworkObject::dijkstra(float **graph, int src, int dst) {
  int nNodes = nodes.size();
  float dist[nNodes];
  bool checked[nNodes];
  for(int i = 0; i < nNodes; i++) {
    dist[i] = FLT_MAX, checked[i] = false;
  }
  dist[src] = 0;
  int prev[nNodes];

  for(int count = 0; count < (nNodes - 1); count++) {
    int u = minDistance(dist, checked);
    checked[u] = true;
    for(int v = 0; v < nNodes; v++) {
      if(!checked[v] && (network[u][v] > 0) && dist[u] != FLT_MAX && (dist[u] + network[u][v] < dist[v])) {
        dist[v] = dist[u] + network[u][v];
        prev[v] = u;
      }
    }
  }

  int sta = dst;
  std::vector <int> longGraph;
  longGraph.push_back(dst);
  while(sta != src) {
    longGraph.push_back(prev[sta]);
    sta = prev[sta];
  }
  //longGraph.push_back(sta);
  return longGraph;
}
