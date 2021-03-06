#include "viewer.h"
#ifdef _GRAPHICS_
Viewer::Viewer(QWidget *parent, Experiment *_exp, int argc, char** argv /*, Video *_video */)
// : QGLWidget (/*QGLFormat(QGL::SampleBuffers),*/ parent)
 : QGLViewer(QGLFormat(QGL::Rgba),parent)
//  : QGLViewer()
{
  exp = _exp;
  //video = _video;
  glutInit(&argc, argv);
  Xcam  = 0.0;
  Ycam  = 1.0;
  Zcam  = 3.0;
  theta = 0.0;
  phi   = -100.0;

  //    QSound::play("../../musica/out.wav");
}


Viewer::~Viewer()
{
  delete[] exp;
}

void Viewer::init()
{
  //  resizeGL(200, 200);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  //glClearDepth(0.0);
  glClear(GL_COLOR_BUFFER_BIT );//| GL_DEPTH_BUFFER_BIT );
  glFlush();
  //setGridIsDrawn();
  setSceneRadius( 3.0 );//300 cm
  setSceneCenter(Vec(0.0, 0.0, 0.0) );
  //showEntireScene();
  
 
  
  //camera()->setPosition(Vec(0,100,200));
  //theta rotates the Camera around its Y axis\, and then phi rotates it around its X axis.
  //float theta=M_PI, phi=(3.0*M_PI)/2.0;
  //camera()->setOrientation(theta, phi );

#ifdef _BULLET_WORLD_
  camera()->setPosition(Vec(Xcam,Ycam,Zcam));
  camera()->setOrientation(theta, phi);
#else
  camera()->setPosition(Vec(0,2,0.6));
  //theta rotates the Camera around its Y axis, and then phi rotates it around its X axis.
  float theta=M_PI;
  float phi=(3.0*M_PI)/2.0;
  camera()->setOrientation(theta, phi );
#endif

  
  //initialiser le ciel
  // mySky.LoadTexture( SKY_FRONT, "/Users/eliotuci/Simulations/epuck_aggregation/bin/sky.jpg" );
  // mySky.LoadTexture( SKY_BACK, "/Users/eliotuci/Simulations/epuck_aggregation/bin/ground.jpg" );
  // mySky.LoadTexture( SKY_RIGHT,"/Users/eliotuci/Simulations/epuck_aggregation/binbrick.jpg" );
  // mySky.LoadTexture( SKY_LEFT, "/Users/eliotuci/Simulations/epuck_aggregation/bin/sky.jpg" );
  // mySky.LoadTexture( SKY_TOP, "/Users/eliotuci/Simulations/epuck_aggregation/bin/sky.jpg" );
  // mySky.LoadTexture( SKY_BOTTOM, "/Users/eliotuci/Simulations/epuck_aggregation/bin/brick.jpg" );
  
  /* turn on default lighting */
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  GLfloat ambient[] = { 0.5f, 0.5f, 0.5f };
  GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f , 0.0f};
  GLfloat specular[] = { 0.25f, 0.25f, 0.25f , 0.0f};
  //GLfloat lightcolor[] = { 0.8, 0.8, 0.8 };
  
  glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
  
}

void Viewer::draw()
{
     GLfloat lightpos[] = { 0.0, 0.0, 7.0, 1.0 };
     glLightfv(GL_LIGHT0, GL_POSITION, lightpos);

     glColor3f(0.0, 0.0, 0.0);
     //if (mySky.wantSky())
     //{
         //mySky.Set( -18.0, -18.0, 0.0, 36.0f );
         //mySky.Set( -150.0, -150.0, 0.0, 300.0f );
         //mySky.Render( );
     //}
#ifdef _BULLET_WORLD_
     exp->getPlane()[0]->render();
#else
  //Arena
  glTranslatef(0.0, 0.0, +0.07);
  glColor3f(1.0, 1.0, 1.0);
  glDrawCircle(0, 0, 4/*75*/);
#endif
  
     for(int r = 0; r < exp->getAgents().size(); r++ ){
         //       if(!exp->param->agent[r]->is_removed()){
       exp->getAgents()[r]->render( );
       
#ifndef _BULLET_WORLD_
       exp->getIrSensor()->render_ir_sensor( exp->getAgents()[r], exp->get_ir_ray_length(r) );
       //exp->getCamera()->render( exp->getAgents()[r] ); 
#endif
       //printf("\nrobot%d rendered\n",r);
       //      }
     }
     //   glColor3f( exp->param->agent[r]->get_colour()[0],
     // 	       exp->param->agent[r]->get_colour()[1],
     // 	       exp->param->agent[r]->get_colour()[2]);
     //   //exp->param->camera->render( exp->param->agent[r] );
     // }
    
     for(int ob = 0; ob < exp->getObjects().size(); ob++){
       if( !exp->getObjects()[ob]->is_removed() )
        exp->getObjects()[ob]->render( );
     }
}


void Viewer::glDrawCircle(double x1, double y1, double radius) {
  
  double x2,y2,angle;
  
  glBegin(GL_TRIANGLE_FAN);
  glVertex2f(x1,y1);
  
  for (angle=1.0f;angle<361.0f;angle+=0.2)
    {
      x2 = x1+sin(angle)*radius;
      y2 = y1+cos(angle)*radius;
      glVertex2f(x2,y2);
    }
  
  glEnd();
}
#endif
