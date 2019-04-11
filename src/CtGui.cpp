#include <iostream>
#include <Vectr.h>
#include <Gui.h>
#include <QDebug>
#include <QString>
#include <QFont>
#include <QOpenGLShader>
#include <QExposeEvent>
#include <vertex.h>
#include <cmath>

Gui::Gui() {
  alpha = 0;
  beta = 0;
  distance = 200;

  txx = 0;
  tyy = 0;
  tzz = 0;
}

Gui::~Gui() {
  pointsVao.destroy();
  pointsBuffer.destroy();

  linesVao.destroy();
  linesBuffer.destroy();

  delete m_program;
}

void Gui::addPoint(Vectr p, Vectr color) {
  points.push_back(p);
  points.push_back(color);
}

void Gui::addLine(Vectr p1, Vectr p2, Vectr color) {
  lines.push_back(p1);
  lines.push_back(color);
  lines.push_back(p2);
  lines.push_back(color);
}

void Gui::addLinePoint(Vectr p, Vectr color) {
  lineloops.push_back(p);
  lineloops.push_back(color);
}

void Gui::drawText(QString text, Vectr loc, int fsize) {
  QPainterPath path;
  QFont font("Hack", fsize);
  path.addText(QPointF(0, 0), QFont("Hack", fsize), text);
  QList<QPolygonF> poly = path.toSubpathPolygons();
  long textLen = 0;
  for(QList<QPolygonF>::iterator i = poly.begin(); i != poly.end(); i++) {
    for(QPolygonF::iterator p = (*i).begin(); p != i->end(); p++) {
      textLen += 1;
      addLinePoint(loc + Vectr(p->rx()*0.1f, (-p->ry()*0.1f), 0), Vectr(1.0, 0.0, 0.0));
    }
  }
  if(textLengths.size() == 0) {
	  textLengths.push_back(0);
	  textLengths.push_back(textLen);
  }
  else {
    textLengths.push_back(textLengths[textLengths.size() - 1] + textLen);
  }
}

void Gui::initializeGL() {
  // Initialize OpenGL Backend
  initializeOpenGLFunctions();
  connect(context(), SIGNAL(aboutToBeDestroyed()), this, SLOT(teardownGL()), Qt::DirectConnection);
  connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
  printVersionInformation();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClearColor((245.0 / 255.0), (245.0 / 255.0), (220.0 / 255.0), 1.0);

  Vectr origin = Vectr(0.0f, 0.0f, 0.0f);
  Vectr xax = Vectr(1000.0f, 0.0f, 0.0f);
  Vectr yax = Vectr(0.0f, 1000.0f, 0.0f);
  Vectr zax = Vectr(0.0f, 0.0f, 1000.0f);
  Vectr colr = Vectr(1.0f, 0.0f, 0.0f);
  Vectr colg = Vectr(0.0f, 1.0f, 0.0f);
  Vectr colb = Vectr(0.0f, 0.0f, 1.0f);

  //addLine(origin, xax, colr);
  //addLine(origin, yax, colg);
  //addLine(origin, zax, colb);

  {
    //Create Shader (Do not release until VAO is created)
    m_program = new QOpenGLShaderProgram();
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, "../src/shaders/simple.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, "../src/shaders/simple.frag");

    m_program->link();
    m_program->bind();

    // Create Buffer (Do not release until VAO is created)
    pointsBuffer.create();
    pointsBuffer.bind();
    pointsBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    std::cout << "Allocating buffer: " << ((points.size() + lines.size()) * sizeof(Vectr)) << " Bytes" << std::endl;
    pointsBuffer.allocate(points.data(), points.size() * sizeof(Vectr));

    // Create Vertex Array Object
    pointsVao.create();
    pointsVao.bind();
    m_program->enableAttributeArray(0);
    m_program->enableAttributeArray(1);
    m_program->setAttributeBuffer(0, GL_FLOAT, offsetof(Vectr, x), 3, 2 * sizeof(Vectr));
    m_program->setAttributeBuffer(1, GL_FLOAT, sizeof(Vectr) + offsetof(Vectr, x), 3, 2 * sizeof(Vectr));

    // Release (unbind) all
    pointsVao.release();
    pointsBuffer.release();

    /****************** LINES ****************/
    linesBuffer.create();
    linesBuffer.bind();
    linesBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

    std::cout << "Allocating buffer: " << ((points.size() + lines.size()) * sizeof(Vectr)) << " Bytes" << std::endl;
    linesBuffer.allocate(lines.data(), lines.size() * sizeof(Vectr));

    // Create Vertex Array Object
    linesVao.create();
    linesVao.bind();
    m_program->enableAttributeArray(0);
    m_program->enableAttributeArray(1);

    m_program->setAttributeBuffer(0, GL_FLOAT, offsetof(Vectr, x), 3, 2 * sizeof(Vectr));
    m_program->setAttributeBuffer(1, GL_FLOAT, sizeof(Vectr) + offsetof(Vectr, x), 3, 2 * sizeof(Vectr));

    // Release (unbind) all
    linesVao.release();
    linesBuffer.release();
    /****************** LINES ****************/

    /****************** LINE LOOPS ****************/
    lineLoopsBuffer.create();
    lineLoopsBuffer.bind();
    lineLoopsBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

    std::cout << "Allocating buffer: " << (lineloops.size() * sizeof(Vectr)) << " Bytes" << std::endl;
    lineLoopsBuffer.allocate(lineloops.data(), lineloops.size() * sizeof(Vectr));

    // Create Vertex Array Object
    lineLoopsVao.create();
    lineLoopsVao.bind();
    m_program->enableAttributeArray(0);
    m_program->enableAttributeArray(1);

    m_program->setAttributeBuffer(0, GL_FLOAT, offsetof(Vectr, x), 3, 2 * sizeof(Vectr));
    m_program->setAttributeBuffer(1, GL_FLOAT, sizeof(Vectr) + offsetof(Vectr, x), 3, 2 * sizeof(Vectr));

    // Release (unbind) all
    lineLoopsVao.release();
    lineLoopsBuffer.release();
    /****************** LINE LOOPS ****************/

    m_program->release();
  }
}

void Gui::resizeGL(int width, int height) {
  guiwidth = width;
  guiheight = height;

  qreal aspect = qreal(width) / qreal(height ? height : 1);
  // fov: field of vision
  fov = 60.0;
  zNear = 0.001;
  zFar = 10000.0;

  pmatrix.setToIdentity();
  pmatrix.perspective(fov, aspect, zNear, zFar);

  glViewport(0, 0, width, height);
}

void Gui::paintGL() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  QMatrix4x4 mmatrix, vmatrix;

  cameraTransformation.setToIdentity();
  cameraTransformation.rotate(alpha, 0, 0, 1);
  cameraTransformation.rotate(beta, 1, 0, 0);

  QVector3D cameraPosition = cameraTransformation * QVector3D(0, 0, distance);
  QVector3D cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);

  vmatrix.translate(QVector3D(txx, tyy, tzz));
  vmatrix.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);

  m_program->bind();
  m_program->setUniformValue("mvpMatrix", pmatrix * vmatrix * mmatrix);
  {
    pointsVao.bind();
    glDrawArrays(GL_POINTS, 0, (points.size() * sizeof(Vectr)) / sizeof(Vectr));
    pointsVao.release();
  }
  {
    linesVao.bind();
    glDrawArrays(GL_LINES, 0, (points.size() * sizeof(Vectr)) / sizeof(Vectr));
    linesVao.release();
  }

  cameraTransformation.setToIdentity();
  cameraTransformation.rotate(alpha, 0, 0, 1);
  cameraTransformation.rotate(beta, 1, 0, 0);

  QMatrix4x4 vmatrix_t;
  cameraPosition = cameraTransformation * QVector3D(0, 0, distance);
  cameraUpDirection = cameraTransformation * QVector3D(0, 1, 0);
  vmatrix_t.translate(QVector3D(txx, tyy, tzz));

  vmatrix_t.lookAt(cameraPosition, QVector3D(0, 0, 0), cameraUpDirection);
  m_program->setUniformValue("mvpMatrix", pmatrix * vmatrix_t * mmatrix);
  {
    lineLoopsVao.bind();
    //glDrawArrays(GL_LINE_STRIP, 0, lineloops.size() * sizeof(Vectr));
    for(int ijk = 0; ijk < textLengths.size() - 1; ijk++) {
      glDrawArrays(GL_LINE_STRIP, textLengths[ijk], textLengths[ijk + 1] - textLengths[ijk]);
    }
    //glDrawArrays(GL_LINE_LOOP, textLengths[0] + textLengths[1], textLengths[2]);
    //glDrawArrays(GL_LINE_LOOP, textLengths[0] + textLengths[1] + textLengths[2], textLengths[3]);

    lineLoopsVao.release();
  }

  m_program->release();
}

void Gui::teardownGL() {

}

void Gui::keyPressEvent(QKeyEvent *event) {
  switch(event->key()) {
    case Qt::Key_Right: {
      txx -= 8.0f;
      break;
    }
    case Qt::Key_Left: {
      txx += 8.0f;
      break;
    }
    case Qt::Key_PageUp: {
      tyy -= 8.0f;
      break;
    }
    case Qt::Key_PageDown: {
      tyy += 8.0f;
      break;
    }
    case Qt::Key_Up: {
      tzz += 8.0f;
      break;
    }
    case Qt::Key_Down: {
      tzz -= 8.0f;
      break;
    }
    default:
      break;
  }
}

void Gui::mouseMoveEvent(QMouseEvent *ev) {
  int deltaX = ev->x() - lastMousePosition.x();
  int deltaY = ev->y() - lastMousePosition.y();
  if(ev->buttons() & Qt::LeftButton) {
    alpha -= deltaX;
    while(alpha < 0) {
      alpha += 360;
    }
    while(alpha >= 360) {
      alpha -= 360;
    }
    beta -= deltaY;
    if(beta >= 90) {
	    beta = 90;
    }
    if(beta <= 0) {
	    beta = 0;
    }
    QOpenGLWindow::update();
  }
  lastMousePosition = ev->pos();
  ev->accept();
}

void Gui::wheelEvent(QWheelEvent *ev) {
  int delta = ev->delta();
  if(ev->orientation() == Qt::Vertical) {
    if(delta < 0) {
      distance *= 1.1;
    }
    else if(delta > 0) {
      distance *= 0.9;
    }
    if(distance > 10000.0) {
	    distance = 10000.0;
    }
    QOpenGLWindow::update();
  }
  ev->accept();
}

void Gui::mousePressEvent(QMouseEvent *ev) {
  lastMousePosition = ev->pos();
  ev->accept();
}

void Gui::update() {
  QOpenGLWindow::update();
}

void Gui::exposeEvent(QExposeEvent *ev) {
  if (ev->region() != m_cachedRegion) {
    m_cachedRegion = ev->region();
    QOpenGLWindow::exposeEvent(ev);
  }
  else {
    ev->ignore();
  }
}

void Gui::printVersionInformation() {
  QString glType;
  QString glVersion;
  QString glProfile;

  // Get Version Information
  glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
  glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

// Get Profile Information
#define CASE(c) case QSurfaceFormat::c: glProfile = #c; break
  switch (format().profile()) {
    CASE(NoProfile);
    CASE(CoreProfile);
    CASE(CompatibilityProfile);
  }
#undef CASE

  // qPrintable() will print our QString w/o quotes around it.
  qDebug() << qPrintable(glType) << qPrintable(glVersion) << "(" << qPrintable(glProfile) << ")";
}
