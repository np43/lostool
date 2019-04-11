#ifndef GUI2D_H
#define GUI2D_H

#include <QtWidgets>
#include <Vectr.h>
#include <Node.h>

class Gui2D: public QGraphicsView {
  Q_OBJECT

public:
  Gui2D();
  ~Gui2D();

protected:
  int _numScheduledScalings;

public slots:
  void wheelEvent(QWheelEvent * event);
  void scalingTime(qreal x);
  void animFinished();
};

#endif
