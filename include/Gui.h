#ifndef GUI_H
#define GUI_H

#include <vector>

#include <QOpenGLWindow>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QOpenGLTexture>

#include <Vectr.h>
#include <Ray.h>

class QExposeEvent;
class QOpenGLShaderProgram;

class Gui: public QOpenGLWindow, protected QOpenGLFunctions {
	Q_OBJECT

		//OpenGL Events
public:
	Gui();
	~Gui();
	std::vector <Vectr> points;
	std::vector <Vectr> lines;
	std::vector <Vectr> lineloops;
	std::vector <int> textLengths;
	Vectr center;
	void addPoint(Vectr p, Vectr color);
	void addLine(Vectr p1, Vectr p2, Vectr color);
	void addLinePoint(Vectr p, Vectr color);
	void drawText(QString text, Vectr loc, int fsize);

	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	void teardownGL();
	protected slots:
	void update();

protected:
	void exposeEvent(QExposeEvent *ev);
	void mouseMoveEvent(QMouseEvent *ev);
	void mousePressEvent(QMouseEvent *ev);
	void wheelEvent(QWheelEvent *ev);
	void keyPressEvent(QKeyEvent *event);

private:
	qreal zNear, zFar, fov;

	QMatrix4x4 pmatrix, cameraTransformation, cameraTransformation2;

	double alpha;
	double beta;
	double distance;
	double txx, tyy, tzz;
	QPoint lastMousePosition;

	int guiwidth, guiheight;

	// OpenGL State Information
	QOpenGLBuffer pointsBuffer;
	QOpenGLVertexArrayObject pointsVao;

	QOpenGLBuffer linesBuffer;
	QOpenGLVertexArrayObject linesVao;

	QOpenGLBuffer lineLoopsBuffer;
	QOpenGLVertexArrayObject lineLoopsVao;

	QOpenGLShaderProgram *m_program;

	// Fix for Windows
	QRegion m_cachedRegion;

	// Private Helpers
	void printVersionInformation();
};

#endif   //WINDOW_H
