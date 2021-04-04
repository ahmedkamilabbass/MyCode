#ifndef _PFSM_
#define _PFSM_

#include "controller.h"

class Pfsm : public Controller
{
  
 private:
  double rho; //rho = 1 zero turning angle - rho = 0 angle drawn from uniform distribution in [0, PI]
  int    forward_movement_lenght;  
  int    m_turning_lenght;
  double angular_velocity;
  double wheel_speed_during_forward_movement;
  double wheel_speed_during_rotation;

  int    m_count_step_turn;
  int    m_count_step_forward;
  int    id;
  
  void copy(const Pfsm &other);
  void allocate( );
  void destroy();

 public:
  Pfsm(  int id );
  Pfsm(const Pfsm& other);
  virtual ~Pfsm();
  
  
  /* -------------------------------------------------------------------------------------------------- */
  /*                                             VIRTUAL FUNCTIONS                                      */
  /* -------------------------------------------------------------------------------------------------- */
  void init                          ( const vector <chromosome_type> &genes );
  void step                          ( const vector <double> &input_array, vector <double> &output_array);
  void reset                         ( void );
  void read_from_file                ( void );
  void compute_genotype_length       ( void );
  /* -------------------------------------------------------------------------------------------------- */
  /* -------------------------------------------------------------------------------------------------- */

  int  random_turn_lenght            ( void );
  void move_forward                  ( vector <double> &output );
  void stay_still                    ( vector <double> &output );
  void turn_right_on_spot            ( vector <double> &output );
  void turn_left_on_spot             ( vector <double> &output );
  
  bool is_robot_in_turn_on_spot_state ( const vector <double> &input, vector <double> &output);
  void do_random_walk                 ( const vector <double> &input, vector <double> &output);
  
  Pfsm& operator=(const Pfsm &other);
};

#endif
