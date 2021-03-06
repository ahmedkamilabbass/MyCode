#include "pfsm.h"

/* -------------------------------------------------------------------------------------- */

void  Pfsm::read_from_file ( void ) {
  std::string absolute_path = (home + local_path).c_str();
  ifstream I ( (absolute_path + "/CONTROLLERS/pfsm").c_str() );
  if(!I) {
    cerr << "File with PFSM init parameters not found" <<endl;
    exit(0);
  }
  
  // read parameters here
  num_input                            = getInt    ('=',I);
  num_output                           = getInt    ('=',I);
  rho                                  = getDouble ('=', I);
  wheel_speed_during_forward_movement  = getDouble ('=', I);
  wheel_speed_during_rotation          = getDouble ('=', I);
  forward_movement_lenght              = getInt    ('=',I);
  
  m_turning_lenght                     = 0;
  m_count_step_turn                    = 0;
  m_count_step_forward                 = 0;
  genotype_length                      = 0;
  
  //Set angular velocity based on epuck characteristics
  double right_out = (2.0*wheel_speed_during_rotation) - 1.0;
  double left_out  = (2.0*(1.0 - wheel_speed_during_rotation)) - 1.0;
  right_out = (right_out * 3.5 );// 3.5 = max_vel
  left_out  = (left_out * 3.5 );// 3.5 = max_vel
  double linearVelocity[2];
  linearVelocity[0] = right_out * 0.0205;
  linearVelocity[1] = left_out * 0.0205;   // epuck 0.0205 = wheel_radius;
  double omega = (linearVelocity[1]-linearVelocity[0])/0.052; // epuck 0.052 = wheels_distance;
  angular_velocity = omega * m_simulationTimeStep; //0.13 =  simulation_time_step
  
  int check = getInt('=',I);
  I.close();
  if( check != 999 ){
    cerr << " In ../CONTROLLERS/pfsm specifications' file --- End check was not 999" << endl;
    exit(0);
  }
}

/* -------------------------------------------------------------------------------------- */

void Pfsm::compute_genotype_length ( void ){
  genotype_length = 0;
}

/* -------------------------------------------------------------------------------------- */

Pfsm::Pfsm( int _id )
  : Controller()
{
  // read from file net structure
  read_from_file ( );
  
  //Set genotyope length given net structure
  //compute_genotype_length();
  
  //Init. the input layers with connections
  allocate( );
  
  id = _id;
}

/* -------------------------------------------------------------------------------------- */

Pfsm::Pfsm( const Pfsm& other)
{
    allocate( );
    copy(other);
}

/* -------------------------------------------------------------------------------------- */

void Pfsm::copy(const Pfsm &other)
{
    Controller::copy(other);
}

/* -------------------------------------------------------------------------------------- */

void Pfsm::allocate( ){}

/* -------------------------------------------------------------------------------------- */

void Pfsm::destroy()
{
  Controller::destroy();
}

/* -------------------------------------------------------------------------------------- */

Pfsm::~Pfsm()
{
    destroy();
}

/* -------------------------------------------------------------------------------------- */

void Pfsm::init ( const vector <chromosome_type> &genes ){ }

/* -------------------------------------------------------------------------------------- */

bool Pfsm::is_robot_in_turn_on_spot_state ( const vector <double> &input, vector <double> &output){
  if( m_count_step_forward >= forward_movement_lenght )
    return true;
  else
    return false;
}

/* -------------------------------------------------------------------------------------- */

void Pfsm::do_random_walk( const vector <double> &input, vector <double> &output){
  if( is_robot_in_turn_on_spot_state ( input, output ) ) {
    if( !m_count_step_turn ){
      m_turning_lenght = random_turn_lenght( );//Compute the num steps while it keeps on turning on the spot
    }
    if( m_turning_lenght < 0 ) turn_right_on_spot( output );
    else turn_left_on_spot( output );
    
    m_count_step_turn++;
    if ( m_count_step_turn >= abs(m_turning_lenght) ) {
      m_count_step_turn    = 0;
      m_count_step_forward = 0;
    }
  }
  else{
    m_count_step_forward++;
    bool no_obstacles = true;
    double activation_limit = 0.4;
    
    if( input[0] > activation_limit || input[1] > activation_limit || input[2] > activation_limit ){
      turn_right_on_spot(output);
      no_obstacles = false;
    }
    else if( input[5] > activation_limit || input[6] > activation_limit || input[7] > activation_limit ){
      turn_left_on_spot(output);
      no_obstacles = false;
    }
    else if( input[7] > activation_limit && input[0] > activation_limit &&
	     input[3] > activation_limit && input[4] > activation_limit ){
      stay_still( output );
      no_obstacles = false;
    }
    
    if ( no_obstacles )
      move_forward( output );
  }
}

/* -------------------------------------------------------------------------------------- */

void Pfsm::step ( const vector <double> &input, vector <double> &output){
  do_random_walk( input, output );
}

/* -------------------------------------------------------------------------------------- */

void Pfsm::reset ( void ){

}

/* -------------------------------------------------------------------------------------- */

Pfsm& Pfsm::operator=(const Pfsm &other)
{
  if(this != &other)
    {
      destroy();
      allocate( );
      copy(other);
    }
  return *this;
}

/* -------------------------------------------------------------------------------------- */

int Pfsm::random_turn_lenght( void ){
  double rn    = gsl_rng_uniform_pos( GSL_randon_generator::r_rand ) - 0.5;
  double temp  = ((1-rho) * tan (PI * rn))/(1+rho);
  double sigma = 2.0 * atan( temp); //this is an angle in [-PI, PI]
  //cerr << " sigma = " << sigma << " angular_velocity = " << angular_velocity << endl;
  return (int)(rint)(sigma/angular_velocity);
}

/* -------------------------------------------------------------------------------------- */

void Pfsm::move_forward( vector <double> &output ){
  output[0] = wheel_speed_during_forward_movement;
  output[1] = wheel_speed_during_forward_movement;
}

/* -------------------------------------------------------------------------------------- */

void Pfsm::turn_right_on_spot( vector <double> &output ){
  output[0] = wheel_speed_during_rotation;
  output[1] = 1.0-wheel_speed_during_rotation;
}

/* -------------------------------------------------------------------------------------- */

void Pfsm::turn_left_on_spot( vector <double> &output ){
  output[1] = wheel_speed_during_rotation;
  output[0] = 1.0-wheel_speed_during_rotation;
}

/* -------------------------------------------------------------------------------------- */

void Pfsm::stay_still( vector <double> &output ){
  output[0] = 0.5;
  output[1] = 0.5;
}

/* -------------------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------------------- */

