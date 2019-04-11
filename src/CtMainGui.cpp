#include <MainGui.h>
#include <iostream>
#include <fstream>

DotLabel::DotLabel(const QString &flName,
                   const QString &foString,
                   const QString &text,
                   QWidget *parent,
                   Qt::WindowFlags f):
                   fileName{flName},
                   foundString{foString},
                   QLabel(text, parent, f) {
  this->setIndent(20);
  this->setTextInteractionFlags(Qt::TextSelectableByMouse);
  this->setCursor(Qt::IBeamCursor);
  this->fileName = flName;
  this->foundString = foString;

  checkFile();
}

DotLabel::~DotLabel() {}

bool DotLabel::checkFile() {
  //std::cout << "checking file: " << this->fileName.toUtf8().constData() << std::endl;
  std::ifstream fileCheck(this->fileName.toUtf8().constData());
  if(fileCheck.good()) {
    this->isFile = true;
    this->dotColor.setRgb(22, 200, 76, 255);
    this->setText(foundString);
    this->setToolTip(fileName);
  }
  else {
    this->isFile = false;
    this->dotColor.setRgb(219, 50, 54, 255);
  }
  this->repaint();
  return this->isFile;
}

void DotLabel::paintEvent(QPaintEvent *event) {
  QPainter p(this);
  p.setRenderHint(QPainter::Antialiasing, true);
  p.setPen(Qt::NoPen);
  p.setBrush(QBrush(this->dotColor));
  p.drawEllipse(0, 0, 15, 15);
  p.end();
  QLabel::paintEvent(event);
}
