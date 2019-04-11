#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <vector>
#include <cfloat>
#include <iomanip>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include <json.h>
#include <liblas/liblas.hpp>

#include <QGuiApplication>
#include <QApplication>
#include <QSurfaceFormat>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <QPainter>

#include <Gui.h>
#include <Gui2D.h>
#include <MainGui.h>

#include <NetworkObject.h>

#include <Vectr.h>
#include <Ray.h>
#include <Node.h>
#include <Link.h>
#include <PclPcloud.h>

#include <Kdnode.h>
#include <Kdtree.h>

int main(int argc, char **argv) {
  std::cout.setf(std::ios::unitbuf);

  std::vector <std::vector <int>> subnets;

  int wWidth = 600, wHeight = 190;

  QApplication app(argc, argv);

  QGraphicsScene *scene = new QGraphicsScene();

  Gui gui;

  QWidget window;
  window.setFixedSize(wWidth, wHeight);


  QSurfaceFormat format;
  //format.setDepthBufferSize(24);
  format.setRenderableType(QSurfaceFormat::OpenGL);
  QSurfaceFormat::setDefaultFormat(format);
  format.setProfile(QSurfaceFormat::CoreProfile);
  format.setVersion(3, 3);

  Json::Value config;
  std::ifstream confile("../config.json", std::ifstream::binary);
  confile >> config;

  std::vector <std::string> confKeys = config.getMemberNames();

  QLineEdit nodeInput(&window);
  QPushButton polesButton("Setup Poles", &window);
  polesButton.setGeometry(QRect(QPoint(10, 10), QSize(130, 30)));
  QPushButton networkButton("Setup Network", &window);
  networkButton.setGeometry(QRect(QPoint(150, 10), QSize(130, 30)));

  std::string confLabelText = "CONFIG DATA:\n";
  for(int i = 0; i < confKeys.size(); i++) {
    if(confKeys[i].compare("ignorelas") == 0) {
      continue;
    }
    confLabelText += " " + confKeys[i] + ": " + config[confKeys[i]].asString() + "\n";
  }

  QWidget checksWindow(&window);
  QVBoxLayout checksLayout;
  DotLabel polesFileLabel(QString(config["polesFile"].asString().c_str()),
                          "Poles file found",
                          "Poles file not found ! Please setup poles.", &window);
  DotLabel netFileLabel(QString(config["netFile"].asString().c_str()),
                        "Network file found",
                        "Network file not found ! Please setup network.", &window);
  QProgressBar progressBar(&window);
  progressBar.setGeometry(progressBar.pos().x(), progressBar.pos().y(), 460, 40);
  QLabel progressLabel;
  progressLabel.setAlignment(Qt::AlignCenter);

  QLabel confLabel(QString(confLabelText.c_str()));
  confLabel.setTextInteractionFlags(Qt::TextSelectableByMouse);
  confLabel.setCursor(Qt::IBeamCursor);
  //checksLayout.addWidget(&nodeInput);
  checksLayout.addWidget(&polesFileLabel);
  checksLayout.addWidget(&netFileLabel);
  checksLayout.addWidget(&progressBar);
  checksLayout.addWidget(&progressLabel);
  //checksLayout.addWidget(&confLabel);
  checksLayout.setMargin(10);
  checksLayout.setSpacing(16);
  checksWindow.setLayout(&checksLayout);
  checksWindow.move(checksWindow.pos().x() + 5, 50);
  checksWindow.setFixedWidth(wWidth - 10);

  Vectr colr = Vectr(1.0f, 0.0f, 0.0f);
  Vectr colg = Vectr(0.0f, 1.0f, 0.0f);
  Vectr colb = Vectr(0.0f, 0.0f, 1.0f);
  Vectr colgray = Vectr(0.7, 0.7, 0.7);

  // reall stuff begins here
  NetworkObject netObj(config, scene, &window);

  // Check for poles.txt initially. If present, read poles and find access points.
  if(polesFileLabel.checkFile()) {
    std::cout << "poles file found" << std::endl;
    netObj.readPoles(&progressBar, &progressLabel);
    polesFileLabel.setText(QString::number(netObj.nodes.size()) + " poles found.");
    netObj.setupNetworkInit(&progressBar, &progressLabel);

    for(int i = 0; i < netObj.nodes.size(); i++) {
      gui.drawText(QString::number(i), netObj.nodes[i].poleLine.direct, 15);
      gui.addPoint(netObj.nodes[i].poleLine.start, colgray);
      gui.addLine(netObj.nodes[i].poleLine.start, netObj.nodes[i].poleLine.direct, colg);
    }
  }

  PclPcloud cloud1 = PclPcloud(config["lasdir"].asString() + "/SET-1/las/overpasses.las", netObj.centerVec, 32);
  cloud1.viewCloud(gui);

  PclPcloud cloud2 = PclPcloud(config["lasdir"].asString() + "/SET-1/las/misc.las", netObj.centerVec, 32);
  cloud2.viewCloud(gui);

  // Link tlink = netObj.singleLink(&progressBar, &progressLabel, 12, 32);
  // for(int i = 0; i < tlink.sublinks.size(); i++) {
  //   if(tlink.activeSublinks[i]) {
  //     gui.addPoint(tlink.sublinks[i].start, colgray);
  //     gui.addLine(tlink.sublinks[i].start, tlink.sublinks[i].direct, colb);
  //   }
  // }

  // int one = 12, two = 34;
  // std::cout << "nsublinks: " << netObj.netsublinks[one][two].size() << std::endl;
  // for(int i = 0; i < netObj.netsublinks[one][two].size(); i++) {
  //   gui.addPoint(netObj.netsublinks[one][two][i].start, colgray);
  //   gui.addLine(netObj.netsublinks[one][two][i].start, netObj.netsublinks[one][two][i].direct, colb);
  // }

  // for(int i = 0; i < netObj.netsublinks[one][two].size(); i++) {
  //   gui.addPoint(netObj.netsublinks[one][two][i].start, colgray);
  //   gui.addLine(netObj.netsublinks[one][two][i].start, netObj.netsublinks[one][two][i].direct, colb);
  // }

  // gui.addPoint(Vectr(0, 0, 0), colgray);
  // gui.addLine(Vectr(0, 0, 1), Vectr(0, 0, 0), colgray);

  QPen linePen;
  linePen.setWidth(0.5);
  linePen.setColor(QColor(0, 87, 231, 255));

  std::vector <QGraphicsLineItem*> gLines;

  // subnets.push_back(netObj.dijkstra(16, 42));
  // for(int q = 0; q < subnets.size(); q++) {
  //   std::cout << "subnet size: " << subnets[q].size() << std::endl;
  //   for(int i = 0; i < (subnets[q].size() - 1); i++) {
  //     gLines.push_back(scene->addLine(QLineF(QPointF(netObj.nodes[subnets[q][i]].poleLine.start.x[0], netObj.nodes[subnets[q][i]].poleLine.start.x[1]), QPointF(netObj.nodes[subnets[q][i + 1]].poleLine.start.x[0], netObj.nodes[subnets[q][i + 1]].poleLine.start.x[1])), linePen));
  //
  //     for(int j = 0; j < netObj.netsublinks[subnets[q][i]][subnets[q][i + 1]].size(); j++) {
  //       gui.addPoint(netObj.netsublinks[subnets[q][i]][subnets[q][i + 1]][j].start, colgray);
  //       gui.addLine(netObj.netsublinks[subnets[q][i]][subnets[q][i + 1]][j].start, netObj.netsublinks[subnets[q][i]][subnets[q][i + 1]][j].direct, colb);
  //     }
  //   }
  //   linePen.setColor(QColor(219, 50, 54, 255));
  // }

  QObject::connect(&polesButton, &QPushButton::clicked, [&]() {
    netObj.findPoles(&progressBar, &progressLabel);
    polesFileLabel.checkFile(); // to update gui
    polesFileLabel.setText(QString(std::to_string(netObj.nodes.size()).c_str()) + " poles found");
    netObj.setupNetworkInit(&progressBar, &progressLabel);
  });

  QObject::connect(&networkButton, &QPushButton::clicked, [&]() {
    netObj.reduceNetwork();
    netObj.markDisconnects();
    std::cout << "disconnects: " << netObj.theLabel << std::endl;

    // int llink = nodeInput.text().toInt();
    // for(int j = 0; j < netObj.nodes.size(); j++) {
    //   if(netObj.netsublinks[j][llink].size() > 0) {
    //     gLines.push_back(scene->addLine(QLineF(QPointF(netObj.nodes[j].poleLine.start.x[0], netObj.nodes[j].poleLine.start.x[1]), QPointF(netObj.nodes[llink].poleLine.start.x[0], netObj.nodes[llink].poleLine.start.x[1])), linePen));
    //   }
    // }

    //netObj.fullNetwork(&progressBar, &progressLabel);

    linePen.setColor(QColor(22, 200, 76, 255));
    subnets.push_back(netObj.dijkstra(netObj.reducedNetworkD, 7, 210));
    subnets.push_back(netObj.dijkstra(netObj.reducedNetworkD, 355, 419));
    subnets.push_back(netObj.dijkstra(netObj.reducedNetworkD, 355, 127));
    subnets.push_back(netObj.dijkstra(netObj.reducedNetworkD, 156, 419));
    subnets.push_back(netObj.dijkstra(netObj.reducedNetworkD, 419, 300));
    subnets.push_back(netObj.dijkstra(netObj.reducedNetworkD, 210, 300));

    for(int q = 0; q < subnets.size(); q++) {
      std::cout << "subnet size: " << subnets[q].size() << std::endl;
      for(int i = 0; i < (subnets[q].size() - 1); i++) {
        gLines.push_back(scene->addLine(QLineF(QPointF(netObj.nodes[subnets[q][i]].poleLine.start.x[0], netObj.nodes[subnets[q][i]].poleLine.start.x[1]), QPointF(netObj.nodes[subnets[q][i + 1]].poleLine.start.x[0], netObj.nodes[subnets[q][i + 1]].poleLine.start.x[1])), linePen));
      }
      //linePen.setColor(QColor(219, 50, 54, 255));
    }

  });

  window.show();

  gui.drawText("O", Vectr(0, 0, 0), 15);
  gui.setFormat(format);
  gui.resize(QSize(600, 600));
  gui.show();

  Gui2D *view = new Gui2D();
  view->setRenderHints(QPainter::Antialiasing);
  view->setScene(scene);
  view->setWindowTitle(QApplication::translate("Planning Tool", "Network View"));
  view->resize(400, 400);
  view->show();

  return app.exec();
}
