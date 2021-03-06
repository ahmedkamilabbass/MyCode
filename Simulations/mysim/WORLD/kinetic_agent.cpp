#include "kinetic_agent.h"

const double Kinetic_Agents::cm_per_pulse            = 0.008;
const double Kinetic_Agents::wheels_distance         = 0.052;
const double Kinetic_Agents::wheel_radius            = 0.0205;
const double Kinetic_Agents::robot_radius            = 0.037;
const double Kinetic_Agents::half_robot_radius       = 0.0185;
const double Kinetic_Agents::robot_height            = 0.055;
const int    Kinetic_Agents::robot_slices            = 20;
const double Kinetic_Agents::pos_noise               = 0.0; //0.01;
const double Kinetic_Agents::rot_noise               = 0.0; //0.0087;
const double Kinetic_Agents::vel_noise               = 0.0;
const double Kinetic_Agents::max_vel                 = 3.5;
const double Kinetic_Agents::min_vel                 = -3.5;
const double Kinetic_Agents::prob_long_term_change   = 0.02;
const double Kinetic_Agents::prob_medium_term_change = 0.1;

/* ----------------------------------------------------------------------- */
/*                              CONSTRUCTOR                                */
/* ----------------------------------------------------------------------- */
Kinetic_Agents::Kinetic_Agents( /*double _simulation_time_step*/  ) : World_Entity ( ) {
  pos.assign(3, 0.0);
  dim.assign(3, 0.0);
  previous_pos.assign(3, 0.0);
  tmp_pos.assign(3, 0.0);
  rot.assign(3, 0.0);
  previous_rot.assign(3, 0.0);
  vel.assign(2, 0.0);
  long_term.assign(3, 0.0);
  medium_term.assign(3, 0.0);
  colour.assign(3, 0.0);
  //simulation_time_step   = _simulation_time_step;
  removed = false;
  
  dim[0] = robot_radius;
  dim[1] = robot_height;
  dim[2] = 0.0;
  
  safe_dist = dim[0] + DIST_OFFSET_SECURITY;
  
  type_id = ROBOT;
  centreDisplacement.assign(2, 0.0);
}

Kinetic_Agents::Kinetic_Agents(const Kinetic_Agents& other)
  : World_Entity(other)
{
  copy(other);
}

/* ----------------------------------------------------------------------- */

void Kinetic_Agents::copy(const Kinetic_Agents &other)
{
  pos = other.pos;
  dim = other.dim;
  previous_pos = other.previous_pos;
  tmp_pos = other.tmp_pos;
  
  rot = other.rot;
  previous_rot = other.previous_rot;
  vel = other.vel;
  long_term = other.long_term;
  medium_term = other.medium_term;
  
  right_out = other.right_out;
  left_out = other.left_out;
  
  centreDisplacement.assign(2, 0.0);
  displacement = other.displacement;
  direction_of_displacement = other.direction_of_displacement;
  removed = other.removed;
  //simulation_time_step = other.simulation_time_step;
  type_id = other.type_id;
  safe_dist = other.safe_dist;
}

/* ----------------------------------------------------------------------- */

void Kinetic_Agents::destroy()
{
  centreDisplacement.clear();
  pos.clear();
  dim.clear();
  previous_pos.clear();
  tmp_pos.clear();
  rot.clear();
  previous_rot.clear();
  vel.clear();
  long_term.clear();
  medium_term.clear();
}

/* ----------------------------------------------------------------------- */
/*                             END CONSTRUCTOR                             */
/* ----------------------------------------------------------------------- */
Kinetic_Agents::~Kinetic_Agents( )
{
  destroy();
}


/* ----------------------------------------------------------------------- */
/*                             SET AGENT POSITION                          */
/* ----------------------------------------------------------------------- */
void Kinetic_Agents::set_pos( const vector <double> &_pos ){
  for (int i = 0; i < _pos.size(); i++) {
    previous_pos[i] = _pos[i];
    pos[i]          = _pos[i];
  }
}

/* ----------------------------------------------------------------------- */
/*                             SET AGENT ROTATION                          */
/* ----------------------------------------------------------------------- */
void Kinetic_Agents::set_rot( const vector <double> &_rot ){
  for (int i = 0; i < _rot.size(); i++) {
    rot[i]          = _rot[i];
    if(rot[i] > (TWO_PI) ) rot[i] -= TWO_PI;
    else if( rot[i] < 0.0 ) rot[i] += TWO_PI;
    previous_rot[i] = rot[i];
  }
}

/* ----------------------------------------------------------------------- */
/*                            SET AGENT VELOCITY                           */
/* ----------------------------------------------------------------------- */
void Kinetic_Agents::set_vel( const vector <double> &outputs ){
  //right_out = (outputs[0]-outputs[1]);
  //left_out  = (outputs[2]-outputs[3]);
    
  right_out = (2.0*outputs[0]) - 1.0;
  left_out  = (2.0*outputs[1]) - 1.0;
  
  vel[0] = (right_out*max_vel) + ((2.0*gsl_rng_uniform_pos (GSL_randon_generator::r_rand)*vel_noise) - vel_noise);
  if( vel[0] > max_vel ) vel[0] = max_vel;
  else if( vel[0] < min_vel ) vel[0] = min_vel;
  
  vel[1] = (left_out*max_vel) + ((2.0*gsl_rng_uniform_pos (GSL_randon_generator::r_rand)*vel_noise) - vel_noise);
  if( vel[1] > max_vel ) vel[1] = max_vel;
  else if( vel[1] < min_vel ) vel[1] = min_vel;

  /*  cerr << " In set vel outputs[0] = " << outputs[0]
       << " outputs[1] = " << outputs[1]
       << " vel[0] = " << vel[0]
       << " vel[1] = " << vel[1]
       << endl;*/

    
}

/* ------------------------------------------------------------------------------------- */
/* DIFFERENTIAL DRIVE ROBOTS - DUDEK, JENKIN Computational Principles of Mobile Robotics */
/*                               UPDATE POSITION ROTATION                                */
/* ------------------------------------------------------------------------------------- */
void Kinetic_Agents::update_pos_rot( void ){
  
  //double updates_per_second = 1.0/simulation_time_step;
  
  //store previous pos and rot
  for (int i = 0; i < pos.size(); i++) previous_pos[i] = pos[i];
  for (int i = 0; i < rot.size(); i++) previous_rot[i] = rot[i];
  
  // //update rotation considering noise
  // rot[2] += (vel[0] - vel[1])*cm_per_pulse/(updates_per_second*wheels_distance);
  // rot[2] += (pinkq(2)*2.0*rot_noise-rot_noise);
  // if(rot[2] > (TWO_PI) ) rot[2] -= TWO_PI;
  // else if( rot[2] < 0.0 ) rot[2] += TWO_PI;
  
  // //update position considering noise
  // double av_speed = (vel[0] + vel[1])*0.5;
  // int look_up_angle = (int)(rot[2]/TWO_PI*35.0+0.5);
  // pos[0] += cos(rot[2])*av_speed*cm_per_pulse/updates_per_second;
  // pos[0] += (pinkq(0)*2.0*pos_noise - pos_noise);
  // pos[1] += sin(rot[2])*av_speed*cm_per_pulse/updates_per_second;
  // pos[1] += (pinkq(1)*2.0*pos_noise - pos_noise);

  vector <double> linearVelocity;
  linearVelocity.assign(2, 0.0);
  
  //Set linear velocity
  linearVelocity[0] = vel[0]*wheel_radius;
  linearVelocity[1] = vel[1]*wheel_radius;
  
  double R           = 0.0;
  double omega       = 0.0;
  
  if ( linearVelocity[0] != linearVelocity[1] ){
    R=(wheels_distance*(linearVelocity[1]+linearVelocity[0]))/
      ((linearVelocity[1]-linearVelocity[0])*2.0);
    omega = (linearVelocity[1]-linearVelocity[0])/wheels_distance;
    double ICC_x = pos[0] - R * sin(rot[2]);
    double ICC_y = pos[1] + R * cos(rot[2]);
    
    //update position considering noise
    pos[0]=((pos[0]-ICC_x)*cos(omega * m_simulationTimeStep /*simulation_time_step*/ )
  	    -(pos[1]-ICC_y)*sin(omega * m_simulationTimeStep /*simulation_time_step*/)+ICC_x);
    pos[1]=((pos[0]-ICC_x)*sin(omega * m_simulationTimeStep /*simulation_time_step*/)
  	    +(pos[1]-ICC_y)*cos(omega * m_simulationTimeStep /*simulation_time_step*/ )+ICC_y);
    pos[0] += (pinkq(0)*2.0*pos_noise-pos_noise);
    pos[1] += (pinkq(1)*2.0*pos_noise-pos_noise);
    
    //update rotation considering noise
    rot[2] += (omega * m_simulationTimeStep /*simulation_time_step*/ );
    /* cerr << " In agent rot = " <<  (omega * simulation_time_step)
	 << " vel[0] = " << vel[0]
	 << " vel[1] = " << vel[1]
	 << " l_vel[0] = " << linearVelocity[0]
	 << " l_vel[1] = " << linearVelocity[1]
	 << endl; */
    rot[2] += (pinkq(2)*2.0*rot_noise-rot_noise);
    if(rot[2] > (TWO_PI) ) rot[2] -= TWO_PI;
    else if( rot[2] < 0.0 ) rot[2] += TWO_PI;
  }
  else{
    //update position considering noise
    pos[0]  += linearVelocity[1] * m_simulationTimeStep /*simulation_time_step*/ * cos(rot[2]);
    pos[1]  += linearVelocity[0] * m_simulationTimeStep /*simulation_time_step*/ * sin(rot[2]);
    pos[0] += (pinkq(0)*2.0*pos_noise-pos_noise);
    pos[1] += (pinkq(1)*2.0*pos_noise-pos_noise);
  }
  
  clear_var_before_collision_check( );
}

/* ----------------------------------------------------------------------- */
/*                              CHECK COLLISIONS                           */
/* ----------------------------------------------------------------------- */

void Kinetic_Agents::clear_var_before_collision_check( void ){
  //This is for collision detection and repositioning
  for (int i = 0; i < pos.size(); i++) tmp_pos[i] = pos[i];
  centreDisplacement[0]     = 0.0;
  centreDisplacement[1]     = 0.0;
  displacement              = 0.0;
  direction_of_displacement = 0.0;
}

/* ----------------------------------------------------------------------- */

bool Kinetic_Agents::check_collision_with_round_object ( World_Entity  *target, double *theta, double *disp ){
  double offsets[2];
  offsets[0]       = (target->get_tmp_pos()[0] - tmp_pos[0]);
  offsets[1]       = (target->get_tmp_pos()[1] - tmp_pos[1]);
  double sumRadii  = robot_radius + target->get_dim()[0];
  
  if ( ( (offsets[0] * offsets[0]) + (offsets[1] * offsets[1]) ) < ( sumRadii * sumRadii) ){
    *theta = atan2(offsets[1], offsets[0]);
    if( *theta < -EPSILON ) *theta += TWO_PI;
    else if ( *theta < EPSILON ) *theta = 0.0;
    double dist = sqrt(( (offsets[0] * offsets[0]) + (offsets[1] * offsets[1]) ) );
    *disp = (robot_radius - (dist - target->get_dim()[0] ) + OFFSET_COLLISION ) * 0.5;
    return true;
  }
  else
    return false;
}

/* ----------------------------------------------------------------------- */

void Kinetic_Agents::update_displacement_after_collision_with_round( const double theta, const double disp ){
  centreDisplacement[0] += disp * cos( theta );
  centreDisplacement[1] += disp * sin( theta );
}

/* ----------------------------------------------------------------------- */

bool Kinetic_Agents::check_collision_with_wall ( World_Entity  *target ){
  vector <double> projection_point;
  projection_point.assign(2, 0.0);
  if( get_circle_line_sqr_distance <double> ( target, projection_point ) < (dim[0]*dim[0]) ) {
    double b1 = target->get_pos()[7] - target->get_pos()[4];
    double b0 = target->get_pos()[6] - target->get_pos()[3];
    double c1 = b1 * target->get_pos()[3] + b0 * target->get_pos()[4];
    double c2 = -b0*tmp_pos[0] + b1*tmp_pos[1];
    double determ = ((b1*b1) + (b0*b0) );
    double cx  = ((b1 * c1) - (b0 * c2))/determ;
    double cy  = ((b1 *c2) + (b0 *c1))/determ;
    double rlDist = sqrt((cx-tmp_pos[0])*(cx-tmp_pos[0])+(cy-tmp_pos[1])*(cy-tmp_pos[1]));
    centreDisplacement[0] += -(tmp_pos[0] - cx);
    centreDisplacement[1] += -(tmp_pos[1] - cy);
    return true;
  }
  else
    return false;
}

/* ----------------------------------------------------------------------- */

void Kinetic_Agents::update_direction_of_displacement( void ){
  direction_of_displacement = atan2(centreDisplacement[1], centreDisplacement[0]);
  direction_of_displacement -= PI;
  if( direction_of_displacement < -EPSILON ) direction_of_displacement += TWO_PI;
  else if ( direction_of_displacement < EPSILON ) direction_of_displacement = 0.0;
  displacement = sqrt( (centreDisplacement[1]*centreDisplacement[1]) + (centreDisplacement[0]*centreDisplacement[0]));
  if( displacement > half_robot_radius ) displacement = half_robot_radius;

/*
    direction_of_displacement = atan2(centreDisplacement[1], centreDisplacement[0]);
    direction_of_displacement -= PI;
    if( direction_of_displacement < -EPSILON ) direction_of_displacement += TWO_PI;
    else if ( direction_of_displacement < EPSILON ) direction_of_displacement = 0.0;
    displacement = sqrt( (centreDisplacement[1]*centreDisplacement[1]) + (centreDisplacement[0]*centreDisplacement[0]) );
  */    
}

/* ----------------------------------------------------------------------- */

void Kinetic_Agents::update_displacement_round_obj( World_Entity  *target ){    
  double x_offset        = (target->get_tmp_pos()[0] - tmp_pos[0]);
  double x_offset_square = (x_offset*x_offset); 
  double y_offset        = (target->get_tmp_pos()[1] - tmp_pos[1]);
  double y_offset_square = (y_offset*y_offset); 
  double sumRadii        = robot_radius + target->get_dim()[0];
  
  double dist_to_robot = sqrt(x_offset_square + y_offset_square) - sumRadii;
  if( dist_to_robot < displacement && dist_to_robot > 0.0 ) {
    double theta = atan2(y_offset, x_offset);
    if( theta < 0.0 ) theta += TWO_PI;
    if( theta < (direction_of_displacement + (PI*0.5)) && theta > (direction_of_displacement - (PI*0.5)) ) {
      if( dist_to_robot < displacement ) displacement = dist_to_robot;
    }
  }
  /*
    double x_offset        = (target->get_tmp_pos()[0] - tmp_pos[0]);
    double x_offset_square = (x_offset*x_offset);
    double y_offset        = (target->get_tmp_pos()[1] - tmp_pos[1]);
    double y_offset_square = (y_offset*y_offset);
    double sumRadii        = dim[0] + target->get_dim()[0];
    
    double dist_to_robot = sqrt(x_offset_square + y_offset_square) - sumRadii;
    if( dist_to_robot < displacement && dist_to_robot > 0.0 ) {
    double theta = atan2(y_offset, x_offset);
    if( theta < 0.0 ) theta += TWO_PI;
    if( theta < (direction_of_displacement + (PI*0.5)) && theta > (direction_of_displacement - (PI*0.5)) ) {
    displacement -= dist_to_robot;
    }
    }
  */
}

/* ----------------------------------------------------------------------- */

void Kinetic_Agents::update_tmp_position_after_collisions(){
  centreDisplacement[0] = 0.0;
  centreDisplacement[1] = 0.0;
  tmp_pos[0] += displacement * cos (direction_of_displacement );
  tmp_pos[1] += displacement * sin (direction_of_displacement );
}

/* ----------------------------------------------------------------------- */

void Kinetic_Agents::update_position_after_collisions(){
  for (int i = 0; i < pos.size(); i++) pos[i] = tmp_pos[i];
}

/* ----------------------------------------------------------------------- */

void Kinetic_Agents::return_to_previous_position( void ){
    for (int i = 0; i < pos.size(); i++)
        pos[i] = previous_pos[i];
    for (int i = 0; i < rot.size(); i++)
        rot[i] = previous_rot[i];
}

/* ----------------------------------------------------------------------- */

Kinetic_Agents& Kinetic_Agents::operator=(const Kinetic_Agents &other)
{
  if(this != &other)
    {
      destroy();
      copy(other);
    }
  return *this;
}

/* ----------------------------------------------------------------------- */
/*                                PINK NOISE                               */
/* ----------------------------------------------------------------------- */
double Kinetic_Agents::pinkq( int index ){
  if(medium_term[index] == 0.0 )
    medium_term[index] = gsl_rng_uniform_pos(GSL_randon_generator::r_rand);
  else if( gsl_rng_uniform_pos(GSL_randon_generator::r_rand) < prob_medium_term_change )
    medium_term[index] = gsl_rng_uniform_pos(GSL_randon_generator::r_rand);
  
  if(long_term[index] == 0.0 )
    long_term[index] = gsl_rng_uniform_pos(GSL_randon_generator::r_rand);
  else if( gsl_rng_uniform_pos(GSL_randon_generator::r_rand) < prob_long_term_change )
    long_term[index] = gsl_rng_uniform_pos(GSL_randon_generator::r_rand);
  
  return (long_term[index] + medium_term[index] + gsl_rng_uniform_pos(GSL_randon_generator::r_rand) )/3.0;
}

#ifdef _GRAPHICS_
/* ----------------------------------------------------------------------- */
/*                    openGL RENDERING FUNCTIONS                           */
/* ----------------------------------------------------------------------- */
void Kinetic_Agents::render( void ){
  glPushMatrix();
  glTranslated(pos[0], pos[1], pos[2]);
  glRotatef((rot[2]*360.0)/6.28, 0, 0, 1);
  GLUquadric* params_quadric = gluNewQuadric();
  glColor3f(colour[0], colour[1], colour[2] );
  gluCylinder(params_quadric, dim[0], dim[0], robot_height, robot_slices, 1);
  gluDisk(params_quadric, 0, dim[0], robot_slices, 1);
  glTranslated(0,0,robot_height);
  gluDisk(params_quadric, 0, dim[0], robot_slices, 1);
  gluDeleteQuadric(params_quadric);
  
  glTranslated(dim[0]*0.5,0,0);
  vector <double> lengths;
  lengths.assign(3, 0.0);
  lengths[0] = dim[0]*0.95;
  lengths[1] = 0.001;
  lengths[2] = dim[1]*0.3;
  glColor3f(0.0, 0.0, 0.0);
  glScalef(lengths[0], lengths[1], lengths[2]);
  glutSolidCube(1.0f);
  glPopMatrix();
}
#endif
