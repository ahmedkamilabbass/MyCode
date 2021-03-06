#ifndef VIEWER_H
#define VIEWER_H

#ifdef _GRAPHICS_
//#include <OpenGL/gl.h> // use this on my mac
//#include <OpenGL/glu.h> // use this on my mac
//#include <GLUT/glut.h> // use this on my mac

#include <GL/glut.h> // use this on rooster
#include <QApplication>
//#include <qsound.h>

//#include <QtOpenGL/QGLWidget>
//#include <QWidget>
//#include <QtOpenGL/QtOpenGL>
//#include <QtOpenGL/QGLFormat>

#include <QGLViewer/qglviewer.h>
//#if QT_VERSION >= 0x040000
//#include <QKeyEvent>
//#endif

#include "../MISC/general.h"
#include "../EXP/experiment.h"
#ifdef _BULLET_WORLD_
#include "../WORLD/simple_agents.h"
#include "../WORLD/simple_objects.h"
#else
#include "../WORLD/kinetic_agent.h"
#include "../WORLD/kinetic_objects.h"
#endif

//#include "video.h"
//#include "sky.h"

using namespace qglviewer;

class Viewer : /*public QGLWidget*/ public QGLViewer
{
 protected:
  
  Q_OBJECT;
 public:
  Viewer(QWidget *parent, Experiment *_exp, int argc, char** argv  );
  virtual ~Viewer();
  /* void keyPressEvent(QKeyEvent *e); */
  /*  void glDrawCircle(double x1, double y1, double radius); */
  Experiment *exp;
  /*  //SKY mySky; */
  /*  //Video *video; */
  
 protected:
  virtual void init();
  virtual void draw();

    
 private:
  unsigned char *pRGB;
  double Xcam,Ycam,Zcam,theta,phi;
  //double viewangle;
  
  //QSound bucket3(const QString, QWidget *parent);
  
  virtual void glDrawCircle(double x1, double y1, double radius);
};
#endif
#endif
