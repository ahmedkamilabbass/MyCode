#ifndef _SIMPLE_AGENTS_
#define _SIMPLE_AGENTS_

#include "world_entity.h"

#define EPSILON (double)(exp(-3.0))


class SIMPLE_Agents : public World_Entity {
  
 protected:
  vector <double> long_term;
  vector <double> medium_term;
  double right_out;
  double left_out;  
   bool removed;
  double simulation_time_step;

  vector <double> tmp_pos;
  double safe_dist;
      
  static constexpr double wheel_distance      = 0.052;
  static constexpr double half_wheel_distance = 0.026;
  static constexpr double wheel_radius        = 0.0205;
  static constexpr double wheel_width         = 0.002;
  static constexpr double robot_radius        = 0.037;
  static constexpr double robot_height        = 0.055;
  
  static constexpr int    robot_slices        = 20;
  static constexpr double IR_range            = 0.04;
  static constexpr double max_ir_reading      = 4095.0;
  static constexpr double camera_ray          = 0.50;
  static constexpr double pos_noise           = 0.01;
  static constexpr double rot_noise           = 0.0087;
  static constexpr double vel_noise           = 0.0;
  static constexpr double max_vel             = 3.5;
  static constexpr double min_vel             = -3.5;
  static constexpr double prob_long_term_change = 0.02;
  static constexpr double prob_medium_term_change = 0.1;

 public:
  double rotation;
  static constexpr int num_IR_sensors              = 8;
  static constexpr int num_camera_rays_per_sectors = 6;
  static constexpr int num_camera_sectors          = 3;
  static constexpr int num_bearing_sectors         = 12;
  static constexpr int num_mouse_parameters        = 4;
  vector <double> previous_pos;
  vector <double> previous_rot;
  vector <double> start_pos;
  vector <btVector3> to1;//[8];
  vector <btVector3> from1;//[8];
  vector <btVector3> to2;//[num_camera_sectors*num_camera_rays_per_sectors];
  btVector3 from2;
  btVector3 randb_from;
  btVector3 randb_to;
  btVector3 robot_front;
  vector <double> vel;
  btDynamicsWorld* world ;
  //btCompoundShape* body1;
  //btRigidBody* stand1;
  //btRigidBody* stand2;
  btRigidBody* body;
  btRigidBody* right_wheel;
  btRigidBody* left_wheel;
  btHingeConstraint* right_hinge;
  btHingeConstraint* left_hinge;
  void addRobotPhysics( const vector <double> &_pos, const vector <double> &_rot );
  void get_IR_reading( vector <double> &_reading);
  void get_camera_reading(vector<double> &_reading);
  void get_gyroY_readings( vector <double> &readings );
  void add_noise( vector <double> &readings );
  void get_randb_reading(vector <double> _nest_pos, vector<double> &_reading);
  void get_mouse_reading( vector <double> &_reading);
  SIMPLE_Agents();
  SIMPLE_Agents(int ind, double simulation_time_step , btDynamicsWorld *world);
  ~SIMPLE_Agents();
  void   set_pos( const vector <double> &_pos );
  void   set_robot_pos_rot( const vector <double> &_pos, const vector <double> &_rot );
  void   set_rot( const vector <double> &_rot );
  double get_rotation();
  void   set_vel( const vector <double> &_vel );

  inline const vector <double> get_tmp_pos( void ){return tmp_pos;}
  inline const double          get_safe_dist ( void ) {return safe_dist;}
  
  const vector <double> get_pos();
  const vector <double> get_rot();
  inline const vector <double> get_dim( void ){ return dim;}
  inline const vector <double> get_colour  ( void ){ return colour;}
  inline void set_colour ( const vector <double> c ) {
    colour[0] = c[0];
    colour[1] = c[1];
    colour[2] = c[2];
  }
  inline const vector <double> get_vel( void ){return vel;}
  inline static const double   get_max_vel( void ) {return max_vel;}
  inline static const double   get_min_vel( void ) {return min_vel;}
  inline static const double   get_radius( void ){return robot_radius;}

  inline const double get_right_out    ( void ) { return right_out;}
  inline const double get_left_out     ( void ) { return left_out;}
  inline void         set_removed       ( bool flag ) { removed = flag;}
  inline const bool   is_removed        ( void ) { return removed;}

  void   update_pos_rot ( void );
  double pinkq          ( int gen );
#ifdef _GRAPHICS_
  void   render         ( void );
#endif
  
};

#endif

