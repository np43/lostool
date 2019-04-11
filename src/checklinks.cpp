#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <iomanip>

#include <json.h>

#include <QGuiApplication>
#include <QApplication>
#include <QSurfaceFormat>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <QPainter>

#include <Gui2D.h>
#include <MainGui.h>

#include <NetworkObject.h>

int main(int argc, char **argv) {
  std::cout.setf(std::ios::unitbuf);

  int wWidth = 600, wHeight = 190;

  QApplication app(argc, argv);

  QGraphicsScene *scene = new QGraphicsScene();

  QWidget window;
  window.setFixedSize(wWidth, wHeight);

  Json::Value config;
  std::ifstream confile("../config.json", std::ifstream::binary);
  confile >> config;

  std::vector <std::string> confKeys = config.getMemberNames();

  QPushButton polesButton("Setup Poles", &window);
  polesButton.setGeometry(QRect(QPoint(10, 10), QSize(130, 30)));
  QPushButton networkButton("Check Links", &window);
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

  // reall stuff begins here
  NetworkObject netObj(config, scene, &window);

  // Check for poles.txt initially. If present, read poles and find access points.
  if(polesFileLabel.checkFile()) {
    std::cout << "poles file found" << std::endl;
    netObj.readPoles(&progressBar, &progressLabel);
    polesFileLabel.setText(QString::number(netObj.nodes.size()) + " poles found.");
    netObj.setupNetworkInit(&progressBar, &progressLabel);
  }

  QObject::connect(&polesButton, &QPushButton::clicked, [&]() {
    netObj.findPoles(&progressBar, &progressLabel);
    polesFileLabel.checkFile(); // to update gui
    polesFileLabel.setText(QString(std::to_string(netObj.nodes.size()).c_str()) + " poles found");
    netObj.setupNetworkInit(&progressBar, &progressLabel);
  });

  QObject::connect(&networkButton, &QPushButton::clicked, [&]() {
    netObj.fullNetwork(&progressBar, &progressLabel);
    netFileLabel.checkFile(); // to update gui
  });

  window.show();

  Gui2D *view = new Gui2D();
  view->setRenderHints(QPainter::Antialiasing);
  view->setScene(scene);
  view->setWindowTitle(QApplication::translate("Planning Tool", "Network View"));
  view->resize(400, 400);
  //view->show();

  return app.exec();
}
