#include <Gui2D.h>
#include <iostream>

Gui2D::Gui2D() {}

Gui2D::~Gui2D() {}

void Gui2D::wheelEvent(QWheelEvent * event) {
  if(event->delta() > 0) {
    scale(1.1, 1.1);
  }
  else {
    scale(0.9, 0.9);
  }

 // int numDegrees = event->delta() / 8;
 // int numSteps = numDegrees / 30; // see QWheelEvent documentation
 // _numScheduledScalings += numSteps;
 // if(_numScheduledScalings * numSteps < 0) // if user moved the wheel in another direction, we reset previously scheduled scalings
 // _numScheduledScalings = numSteps;
 //
 // QTimeLine *anim = new QTimeLine(350, this);
 // anim->setUpdateInterval(20);
 //
 // connect(anim, SIGNAL (valueChanged(qreal)), SLOT (scalingTime(qreal)));
 // connect(anim, SIGNAL (finished()), SLOT (animFinished()));
 // anim->start();
}

void Gui2D::scalingTime(qreal x) {
 qreal factor = 1.0 + qreal(_numScheduledScalings) / 300.0;
 scale(factor, factor);
}

void Gui2D::animFinished() {
 if (_numScheduledScalings > 0) {
   _numScheduledScalings--;
 }
 else {
   _numScheduledScalings++;
 }
 sender()->~QObject();
}
