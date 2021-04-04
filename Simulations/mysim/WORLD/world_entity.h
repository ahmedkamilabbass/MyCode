#ifndef _WORLD_ENTITY_
#define _WORLD_ENTITY_

#include <map>
#include <cmath>

#ifdef _BULLET_WORLD_
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#endif

#ifdef _GRAPHICS_
//#include <OpenGL/gl.h> // use this on my mac
//#include <OpenGL/glu.h> // use this on my mac
//#include <GLUT/glut.h> // use this on my mac
#include <GL/glut.h>
#endif
#include "../MISC/general.h"

enum {WALL = 0, CYLINDER=1, SPHERE=2, LIGHT=3, BRICK = 4, ROBOT=5, PLANE = 6};

#define DIST_OFFSET_SECURITY 0.1

using namespace std;

class World_Entity {
  
 protected:
  vector <double> pos;
  vector <double> rot;
  vector <double> dim;
  vector <double> colour;
  
  int    index;
  int    type_id;
  //bool   crashed;
  
  
 public:
  template<class T>
  struct Collisions
  {
    T index;
    T id;
  };
  template<class T> Collisions<T>
  MakeIndexWithId(const T _index, const T _id )
    {
      Collisions<T> ret;
      ret.index = _index;
      ret.id = _id;
      return ret; 
    }
  vector <Collisions <int> > collision_memory;
  
  double mass;
  static double noise_level;
  
  World_Entity(){}
  virtual ~World_Entity(){}
  
  inline const int     get_index    ( void ) { return index;}
  //inline const bool    is_crashed   ( void ) { return crashed;}
  inline const double  get_mass     ( void ) { return mass;}
  inline const int     get_type_id  ( void ) { return type_id;}

  //inline void          set_crashed  ( bool val ) { crashed = val;}
  
  
  virtual void  set_pos                     ( const vector <double> &_pos ) = 0;
  virtual void  set_rot                     ( const vector <double> &_rot ) = 0;
  //virtual void  set_dim                     ( const vector <double> &_dim ) = 0;
  
  virtual const vector <double> get_pos     ( void ) = 0;
  virtual const vector <double> get_rot     ( void ) = 0;
  virtual const vector <double> get_dim     ( void ) = 0;
  virtual const vector <double> get_colour  ( void ) = 0;
  
  virtual const vector <double> get_tmp_pos ( void ) = 0;
  virtual const double          get_safe_dist ( void ) = 0;


  /* ------------------------------------------------------------ */
  inline void set_colour( const vector <double> &c ) { 
    colour[0] = c[0];
    colour[1] = c[1];
    colour[2] = c[2];
  }
  /* ------------------------------------------------------------ */



  /*
   * Originally solution was posted on the following website: http://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
   */
  template <typename T>
    T get_circle_line_sqr_distance ( World_Entity  *target, vector <T> & nearest_p ) {
    
    //T angle = ((target->get_rot()[2])*PI)/180.0;
    T sqr_length;
    T t;
    T x_offset = target->get_pos()[3]-target->get_pos()[6];
    T y_offset = target->get_pos()[4]-target->get_pos()[7];	
    sqr_length = (x_offset * x_offset) + (y_offset * y_offset);
    
    /*if (sqr_length == 0.0)//This is the case in which the brick is a point
      return (target->get_pos()[3]-this->get_tmp_pos()[0])*(target->get_pos()[3]-this->get_tmp_pos()[0])+
      (target->get_pos()[4]-this->get_tmp_pos()[1])*(target->get_pos()[4]-this->get_tmp_pos()[1]);
    */
    
    // Searching for a projection of point this->get_tmp_pos() onto an infinite line of a wall using a dot product
    t = ((this->get_tmp_pos()[0] - target->get_pos()[3]) * (target->get_pos()[6] - target->get_pos()[3]) + (this->get_tmp_pos()[1] - target->get_pos()[4]) * (target->get_pos()[7] - target->get_pos()[4])) / sqr_length;

    if (t < 0.0){
      // if nearest point is beyond point 1
      nearest_p[0] = target->get_pos()[3];
      nearest_p[1] = target->get_pos()[4];
      return 999999.0;
      //(target->get_pos()[3]-this->get_tmp_pos()[0])*(target->get_pos()[3]-this->get_tmp_pos()[0])+
      //(target->get_pos()[4]-this->get_tmp_pos()[1])*(target->get_pos()[4]-this->get_tmp_pos()[1]);
    }
    else if (t > 1.0){
      // if nearest point is beyond point 2
      nearest_p[0] = target->get_pos()[6];
      nearest_p[1] = target->get_pos()[7];
      return 999999.0;
      //(target->get_pos()[6]-this->get_tmp_pos()[0])*(target->get_pos()[6]-this->get_tmp_pos()[0])+
      //(target->get_pos()[7]-this->get_tmp_pos()[1])*(target->get_pos()[7]-this->get_tmp_pos()[1]);
    }
    else{
      // if projected point (nearest point) is between desired two points of a wall
      nearest_p[0] = target->get_pos()[3] + t * (target->get_pos()[6] - target->get_pos()[3]);
      nearest_p[1] = target->get_pos()[4] + t * (target->get_pos()[7] - target->get_pos()[4]);
      /*cerr << " t = " << t << " t->pos()[3] = " << target->get_pos()[3]
	<< " t->pos()[4] = " << target->get_pos()[4]
	<< " t->pos()[6] = " << target->get_pos()[6]
	<< " t->pos()[7] = " << target->get_pos()[7] << endl;
	cerr << " nearest_p[0] = " << nearest_p[0] << " nearest_p[1] = "  << nearest_p[1] << endl;*/
	
      return (nearest_p[0]-this->get_tmp_pos()[0])*(nearest_p[0]-this->get_tmp_pos()[0])+(nearest_p[1]-this->get_tmp_pos()[1])*(nearest_p[1]-this->get_tmp_pos()[1]) ;
    }
  }
};

#endif


