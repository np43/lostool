#ifndef MAINGUI_H
#define MAINGUI_H

#include <QtWidgets>
#include <QLabel>
#include <Vectr.h>
#include <Node.h>

class DotLabel: public QLabel {
  Q_OBJECT
public:
  DotLabel(const QString &fileName,
           const QString &foundString,
           const QString &text,
           QWidget *parent = 0,
           Qt::WindowFlags f = 0);
  ~DotLabel();
  bool checkFile();
  void paintEvent(QPaintEvent *event);
  bool isFile = false;
  QString fileName;
  QString foundString;
  QColor dotColor;
};

#endif
