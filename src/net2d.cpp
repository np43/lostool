// standard
#include <iostream>
#include <fstream>
#include <vector>

// jsoncpp
#include <json.h>

// qt
#include <QGuiApplication>
#include <QApplication>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <QPainter>

// custom
#include <Vectr.h>
#include <Ray.h>
#include <Node.h>
#include <Link.h>
#include <Gui2D.h>
#include <MainGui.h>
#include <NetworkObject.h>

// colours
// (0, 87, 231, 255)
// (22, 200, 76, 255)

int main(int argc, char **argv) {
  std::cout.setf(std::ios::unitbuf);

  // read config data
  Json::Value config;
  std::ifstream confile("../config.json", std::ifstream::binary);
  confile >> config;
  std::vector <std::string> confKeys = config.getMemberNames();

  int wWidth = 600, wHeight = 190;
  QApplication app(argc, argv);

  Gui2D *view = new Gui2D();
  QGraphicsScene *scene = new QGraphicsScene();

  QWidget window;
  window.setFixedSize(wWidth, wHeight);

  QTextEdit nodeInput(&window);
  nodeInput.setGeometry(QRect(QPoint(10, 10), QSize(130, 170)));

  QPushButton networkButton("Form Links", &window);
  networkButton.setGeometry(QRect(QPoint(150, 10), QSize(130, 30)));

  QPushButton saveButton("Save Network", &window);
  saveButton.setGeometry(QRect(QPoint(300, 10), QSize(130, 30)));

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

  checksLayout.addWidget(&polesFileLabel);
  checksLayout.addWidget(&netFileLabel);
  checksLayout.addWidget(&progressBar);
  checksLayout.addWidget(&progressLabel);
  checksLayout.setMargin(10);
  checksLayout.setSpacing(16);
  checksWindow.setLayout(&checksLayout);
  checksWindow.move(150, 50);
  checksWindow.setFixedWidth(wWidth - 10 - 150);


  // read links.txt
  std::ifstream linksFile(config["linksFile"].asString());
  std::string line;
  QString linksText;
  while(getline(linksFile, line)) {
    linksText += QString(line.c_str()) + "\n";
  }
  linksFile.close();
  nodeInput.setText(linksText);

  // reall stuff begins here
  NetworkObject netObj(config, scene, &window);
  std::vector <std::vector <int>> subnets;

  // Check for poles.txt initially. If present, read poles and find access points.
  if(polesFileLabel.checkFile()) {
    std::cout << "poles file found" << std::endl;
    netObj.readPoles(&progressBar, &progressLabel);
    polesFileLabel.setText(QString::number(netObj.nodes.size()) + " poles found.");
    netObj.setupNetworkInit(&progressBar, &progressLabel);
  }

  netObj.reduceNetwork();
  netObj.markDisconnects();
  std::cout << "disconnects: " << netObj.theLabel << std::endl;

  QPen linePen;
  linePen.setWidth(1);
  linePen.setColor(QColor(22, 200, 76, 255));

  std::vector <QGraphicsLineItem*> gLines;

  QObject::connect(&saveButton, &QPushButton::clicked, [&]() {
    std::ofstream linksFile;
    linksFile.open(config["linksFile"].asString());
    std::string content = nodeInput.toPlainText().toStdString();
    linksFile << content;
    linksFile.close();
    std::cout << "links saved" << std::endl;
  });

  QObject::connect(&networkButton, &QPushButton::clicked, [&]() {

    // int llink = nodeInput.text().toInt();
    // for(int j = 0; j < netObj.nodes.size(); j++) {
    //   if(netObj.netsublinks[j][llink].size() > 0) {
    //     gLines.push_back(scene->addLine(QLineF(QPointF(netObj.nodes[j].poleLine.start.x[0], netObj.nodes[j].poleLine.start.x[1]), QPointF(netObj.nodes[llink].poleLine.start.x[0], netObj.nodes[llink].poleLine.start.x[1])), linePen));
    //   }
    // }

    subnets.clear();
    QStringList links = nodeInput.toPlainText().split("\n");
    foreach(QString linkStr, links) {
      QStringList nodesInLink = linkStr.split(",");
      if(nodesInLink.size() > 1) {
        subnets.push_back(netObj.dijkstra(netObj.reducedNetworkD, nodesInLink.at(0).toInt(), nodesInLink.at(1).toInt()));
      }
    }

    for(int i = 0; i < gLines.size(); i++) {
      scene->removeItem(gLines[i]);
    }
    gLines.clear();

    for(int q = 0; q < subnets.size(); q++) {
      for(int i = 0; i < (subnets[q].size() - 1); i++) {
        gLines.push_back(scene->addLine(QLineF(QPointF(netObj.nodes[subnets[q][i]].poleLine.start.x[0], netObj.nodes[subnets[q][i]].poleLine.start.x[1]), QPointF(netObj.nodes[subnets[q][i + 1]].poleLine.start.x[0], netObj.nodes[subnets[q][i + 1]].poleLine.start.x[1])), linePen));
      }
    }

  });

  window.show();
  view->setRenderHints(QPainter::Antialiasing);
  view->setScene(scene);
  view->setWindowTitle(QApplication::translate("Planning Tool", "Network View"));
  view->show();

  return app.exec();
}
