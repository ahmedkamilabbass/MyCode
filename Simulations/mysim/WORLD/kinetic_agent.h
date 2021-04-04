#ifndef _KINETIC_AGENTS_
#define _KINETIC_AGENTS_

#include "world_entity.h"

#define EPSILON (double)(exp(-3.0))
#define OFFSET_COLLISION (double)(0.002)

class Kinetic_Agents : public World_Entity {
  
 protected:
  vector <double> pos;
  vector <double> dim;
  vector <double> previous_pos;
  vector <double> tmp_pos;
  
  vector <double> rot;
  vector <double> previous_rot;
  vector <double> vel;
  vector <double> long_term;
  vector <double> medium_term;
  
  double right_out;
  double left_out;
  double safe_dist;
  
  //double* centreDisplacement;
  vector<double> centreDisplacement;
  double displacement;
  double direction_of_displacement;
  bool removed;
  //double simulation_time_step;
  int type_id;
  
  //static const double updates_per_second;
  static const double cm_per_pulse;
  static const double wheels_distance;
  static const double wheel_radius;
  static const double robot_radius;
  static const double half_robot_radius;
  static const double robot_height;
  
  static const int robot_slices;
  
  static const double pos_noise;
  static const double rot_noise;
  static const double vel_noise;
  static const double max_vel;
  static const double min_vel;
  static const double prob_long_term_change;
  static const double prob_medium_term_change;
  
  void copy(const Kinetic_Agents &other);
  void destroy();
  
 public:
  //Kinetic_Agents() : World_Entity() {}
  Kinetic_Agents( /*double simulation_time_step*/ );
  Kinetic_Agents(const Kinetic_Agents& other);
  ~Kinetic_Agents();
  
  void   set_pos( const vector <double> &_pos );
  void   set_rot( const vector <double> &_rot );
  void   set_vel( const vector <double> &_vel );
  
  inline const vector <double> get_pos( void ){return pos;}
  inline const vector <double> get_previous_pos( void ){return previous_pos;}
  inline const vector <double> get_tmp_pos( void ){return tmp_pos;}
  inline const vector <double> get_dim( void ){ return dim;}
  inline const vector <double> get_colour  ( void ){ return colour;}
  inline void set_colour ( const vector <double> c ) {
    colour[0] = c[0];
    colour[1] = c[1];
    colour[2] = c[2];
  }
  inline const double          get_safe_dist ( void ) {return safe_dist;}
  inline const vector <double> get_rot( void ){return rot;}
  inline const vector <double> get_vel( void ){return vel;}
  inline static const double   get_max_vel( void ) {return max_vel;}
  inline static const double   get_min_vel( void ) {return min_vel;}
  inline static const double   get_radius( void ){return robot_radius;}
  inline vector<double>        get_centreDisplacement( void ){ return centreDisplacement;}
  inline void            set_inverse_centreDisplacement(vector<double> _centreDisplacement){
    centreDisplacement[0] += -_centreDisplacement[0];
    centreDisplacement[1] += -_centreDisplacement[1];
  }
  
  inline const double get_right_out    ( void ) { return right_out;}
  inline const double get_left_out     ( void ) { return left_out;}
  inline void         set_removed       ( bool flag ) { removed = flag;}
  inline const bool   is_removed        ( void ) { return removed;}
  //inline const bool   is_crashed       ( void ) { return crashed;}
  //inline void         set_crashed      ( bool val ) { crashed = val;}
  inline const int    get_type_id      ( void ){ return type_id;}
  
  void   update_pos_rot ( void );
  double pinkq          ( int gen );
  bool   check_collision_with_round_object ( World_Entity *target, double* theta, double* disp );
  void   update_displacement_after_collision_with_round( const double theta, const double disp );

  bool   check_collision_with_wall ( World_Entity  *target );
  
  void   return_to_previous_position( void );
  
  void clear_var_before_collision_check( void );
  void update_direction_of_displacement( void );
  void update_displacement_round_obj(  World_Entity  *target );
  void update_tmp_position_after_collisions( void );
  void update_position_after_collisions( void );
  
  Kinetic_Agents& operator=(const Kinetic_Agents &other);
  
#ifdef _GRAPHICS_
  void   render         ( void );
#endif
  
};
#endif
