#include "ir_sensors.h"

/* ---------------------------------------------------------------------------------------- */
IR_Sensors::IR_Sensors( void )
{
  ir_num_sensors               = 8;
  ir_dist_range                = 0.04;
  epuck_sensors_angular_pos.resize(  ir_num_sensors );
  epuck_sensors_angular_pos[7] = 0.3; // 17 degree
  epuck_sensors_angular_pos[6] = 0.8; // 45 degree
  epuck_sensors_angular_pos[5] = 1.57;// 90 degree
  epuck_sensors_angular_pos[4] = 2.64;// 151 degree
  epuck_sensors_angular_pos[3] = 3.64;// 208 degree -2.64;
  epuck_sensors_angular_pos[2] = 4.71;// 270 degree -1.57;
  epuck_sensors_angular_pos[1] = 5.48;// 314 degree -0.8;
  epuck_sensors_angular_pos[0] = 5.98;// 342 degree  -0.3;
}

IR_Sensors::IR_Sensors(const IR_Sensors& other)
{
  copy(other);
}

/* ---------------------------------------------------------------------------------------- */

IR_Sensors::~IR_Sensors()
{
  destroy();
}

/* ---------------------------------------------------------------------------------------- */

void IR_Sensors::copy(const IR_Sensors &other)
{
  ir_num_sensors            = other.ir_num_sensors;
  ir_dist_range             = other.ir_dist_range;
  epuck_sensors_angular_pos = other.epuck_sensors_angular_pos;
}

/* ---------------------------------------------------------------------------------------- */

void IR_Sensors::destroy() {}

/* ---------------------------------------------------------------------------------------- */
/*                               NEW AGENT BRICK IR READINGS                                */
/* ---------------------------------------------------------------------------------------- */

void IR_Sensors::agent_brick_readings ( World_Entity *agent, World_Entity *target, vector <double> &distance)
{
  vector <double> projection_point;
  projection_point.assign( 2, 0.0 );
  //Compute distance between the robot centre and the brick (i.e., the
  //line segment) and the coordinates of the projection of robot
  //centre onto the segment
  double target_distance = sqrt( agent->get_circle_line_sqr_distance <double> ( target, projection_point ) );
  //cerr << " p_p[0] = " << projection_point[0] << " p_p[1] = "  << projection_point[1] << endl;
 
  //If the centre of the robot is at less than ir_dist_range from the
  //brick
  if( (target_distance - agent->get_dim()[0])  < ir_dist_range ){
    
    //Compute relative orientation (Theta) of brick wrt robot in general framework of reference
    double x_offset = (projection_point[0] - agent->get_pos()[0]);
    double y_offset = (projection_point[1] - agent->get_pos()[1]);
    double theta = atan2( y_offset, x_offset);
    if( theta < 0.0 ) theta += TWO_PI;    
    //cerr << " t_dist = " << target_distance << " theta = " << theta << endl;
    
    //for each IR sensor - there are 8 IR sensors in the epuck
    for(int i = 0; i < ir_num_sensors; i++){
      //Compute the sensor i orientation (sensor_heading) relative to the
      //robot current heading
      double sensor_heading = agent->get_rot()[2] + epuck_sensors_angular_pos[i];
      if(sensor_heading < 0.0 ) sensor_heading += TWO_PI;
      else if(sensor_heading > TWO_PI ) sensor_heading -= TWO_PI;
      
      //compute angle (theta_s) between relative sensor heading and
      //perpendicular to the brick from robot centre
      double theta_s = sensor_heading - theta;
      if(theta_s < 0.0 ) theta_s += TWO_PI;
        
      //Check if relative sensor heading (sensor_heading) in +-90
      //degree wrt theta
      if( theta_s < (PI*0.5) || theta_s > (PI*3*0.5) ){
	double robot_brick_distance_through_sensor_from_centre = target_distance / cos(theta_s);
	double robot_brick_distance_through_sensor_from_robot_body = robot_brick_distance_through_sensor_from_centre-agent->get_dim()[0];
	if( robot_brick_distance_through_sensor_from_robot_body < ir_dist_range  &&
	    robot_brick_distance_through_sensor_from_robot_body < distance[i] ){
	  distance[i] = robot_brick_distance_through_sensor_from_robot_body;
	  
	  /*cerr << "rob rot = " <<  agent->get_rot()[2] 
	    << " sen["<<i<<"] pos = " << epuck_sensors_angular_pos[i]
	    << " s_heading["<< i << "] = "  << sensor_heading
	    << " sen["<<i<<"] dist = " << robot_brick_distance_through_sensor_from_robot_body
	    << "  ir_dist_range  = " <<  ir_dist_range 
	    << " theta_s = " << theta_s << endl;*/
	}
      }
    }
  }
}

/* ---------------------------------------------------------------------------------------- */
/*                               NEW AGENT-AGENT IR RAY TRACING                             */
/* ---------------------------------------------------------------------------------------- */

double IR_Sensors::agent_agent_readings ( World_Entity *agent, World_Entity *target, vector <double> &a_distance, vector <double> &t_distance )
{
  //Compute distance to target
  double x_offset            = (target->get_pos()[0] - agent->get_pos()[0]);
  double y_offset            = (target->get_pos()[1] - agent->get_pos()[1]);
  double remove              = (agent->get_dim()[0] + target->get_dim()[0]);//this is the diameter
  double target_distance     = sqrt( (x_offset*x_offset)+(y_offset*y_offset) );

  if( (target_distance-remove) < ir_dist_range ){
    
    double a_theta = atan2( y_offset, x_offset); //Angular distance of target in general framework of reference
    if( a_theta < 0.0 ) a_theta += TWO_PI;
    double a_theta_r = a_theta - agent->get_rot()[2]; //Angular distance of target from agent current heading
    if(a_theta_r < 0.0 ) a_theta_r += TWO_PI;
    
    /*cerr  << " theta="<< (a_theta*180.0)/PI
	  << " theta_r="<< (a_theta_r*180.0)/PI
	  << " rot = " << (agent->get_rot()[2]*180.0)/PI << endl;*/
    
    double t_theta = atan2( -y_offset, -x_offset); //Angular distance of agent in general framework of reference
    if( t_theta < 0.0 ) t_theta += TWO_PI;
    double t_theta_r = t_theta - target->get_rot()[2]; //Angular distance of agent from target current heading
    if(t_theta_r < 0.0 ) t_theta_r += TWO_PI;
    
    //Angle of view from robot centre
    double max_angle_facing = fabs( asin(target->get_dim()[0]/target_distance) );
    
    for(int i = 0; i < ir_num_sensors; i++){//for each IR sensor - there are 8 IR sensors in the epuck
      
      /*double a_sensor_heading = agent->get_rot()[2] + epuck_sensors_angular_pos[i];
	if(a_sensor_heading < 0.0 ) a_sensor_heading += TWO_PI;
	else if(a_sensor_heading > TWO_PI ) a_sensor_heading -= TWO_PI;*/
      
      //cerr << " s["<<i<<"] = "<< (epuck_sensors_angular_pos[i]*180.0)/PI
      //   << " sr["<<i<<"] = "<< (a_sensor_heading*180.0)/PI << endl;
      //<< " diff_a = " << (fabs(epuck_sensors_angular_pos[i] - a_theta_r) * 180.0)/PI 
      // << " diff_b = " << (((a_theta_r+max_angle_facing)-TWO_PI) * 180.0)/PI << endl; // > */
      //<< " h = " << (a_sensor_heading*180.0)/PI << endl; 
      
      /* double t_sensor_heading = target->get_rot()[2] + epuck_sensors_angular_pos[i];
	 if(t_sensor_heading < 0.0 ) t_sensor_heading += TWO_PI;
	 else if(t_sensor_heading > TWO_PI ) t_sensor_heading -= TWO_PI; */
      
      
      //Check if relative sensor heading (a_sensor_heading) in +-90
      //degree wrt a_theta and update a_distance
      //if( fabs(a_theta - a_sensor_heading)  <= max_angle_facing ){
      
      if( (fabs(epuck_sensors_angular_pos[i] - a_theta_r) < max_angle_facing) || 
	  ((a_theta_r+max_angle_facing)-TWO_PI) > epuck_sensors_angular_pos[i]  ||
	  ((TWO_PI+a_theta_r)-epuck_sensors_angular_pos[i]) < max_angle_facing ) {  
	double angle_facing = fabs(epuck_sensors_angular_pos[i] - a_theta_r);
	double tmp_dist = target_distance * cos( angle_facing ) - target->get_dim()[0] * sin( acos ( target_distance * sin( angle_facing ) / target->get_dim()[0] ) ) - agent->get_dim()[0];
	if( tmp_dist <  a_distance[i] )
	  a_distance[i] = tmp_dist;
      }
      
      //Check if relative sensor heading (t_sensor_heading) in +-90
      //degree wrt t_theta and update t_distance
      //if( fabs(t_theta - t_sensor_heading)  <= max_angle_facing ){
      if( (fabs(epuck_sensors_angular_pos[i] - t_theta_r) < max_angle_facing) || 
	  ((t_theta_r+max_angle_facing)-TWO_PI) > epuck_sensors_angular_pos[i]  ||
	  ((TWO_PI+t_theta_r)-epuck_sensors_angular_pos[i]) < max_angle_facing ) {  
	double angle_facing = fabs(epuck_sensors_angular_pos[i] - t_theta_r);
	double tmp_dist = target_distance * cos( angle_facing ) - agent->get_dim()[0] * sin( acos ( target_distance * sin( angle_facing ) / agent->get_dim()[0] ) ) - target->get_dim()[0];
	if( tmp_dist <  t_distance[i] )
	  t_distance[i] = tmp_dist;
      }
    }
  }
  /*  for(int i = 0; i < 8; i++){
      cerr << " a_d["<<i<<"]= " << a_distance[i];
      cerr << " t_d["<<i<<"]= " << t_distance[i];
      }
      cerr << endl;*/
  return target_distance;
}

/* ---------------------------------------------------------------------------------------- */

void IR_Sensors:: agent_round_obj_readings ( World_Entity *agent, World_Entity *target, vector <double> &distance)
{
  //Compute distance to target
  double x_offset            = (target->get_pos()[0] - agent->get_pos()[0]);
  double y_offset            = (target->get_pos()[1] - agent->get_pos()[1]);
  double remove              = (agent->get_dim()[0] + target->get_dim()[0]);
  double target_distance     = sqrt( (x_offset*x_offset)+(y_offset*y_offset) );
  
  if( (target_distance-remove) < ir_dist_range ){
    double theta = atan2( y_offset, x_offset); //Angular distance of target in general framework of reference
    if( theta < 0.0 ) theta += TWO_PI;
    
    //Angular distance of target from robot current heading
    double theta_r = theta - agent->get_rot()[2];
    if(theta_r < 0.0 ) theta_r += TWO_PI;
    
    //Angle of view from robot centre
    double max_angle_facing = fabs( asin(target->get_dim()[0]/target_distance) );
    
    for(int i = 0; i < ir_num_sensors; i++){//for each IR sensor - there are 8 IR sensors in the epuck
      /*double sensor_heading = agent->get_rot()[2] + epuck_sensors_angular_pos[i];
      if(sensor_heading < 0.0 ) sensor_heading += TWO_PI;
      else if(sensor_heading > TWO_PI ) sensor_heading -= TWO_PI;*/
      
      //Check if relative sensor heading (sensor_heading) in +-90
      //degree wrt theta
      //if( fabs(theta - sensor_heading)  <= max_angle_facing ){
      if( (fabs(epuck_sensors_angular_pos[i] - theta_r) < max_angle_facing) || 
	  ((theta_r+max_angle_facing)-TWO_PI) > epuck_sensors_angular_pos[i]  ||
	  ((TWO_PI+theta_r)-epuck_sensors_angular_pos[i]) < max_angle_facing ) {  
	double angle_facing = fabs(epuck_sensors_angular_pos[i] - theta_r);
	double tmp_dist = target_distance * cos( angle_facing ) - target->get_dim()[0] * sin( acos ( target_distance * sin( angle_facing ) / target->get_dim()[0] ) ) - agent->get_dim()[0];
	if( tmp_dist <  distance[i] )
	  distance[i] = tmp_dist;
      }
    }
  }
  /*  for(int i = 0; i < 8; i++)
      cerr << " d["<<i<<"]= " << distance[i];
      cerr << endl;*/
}

/* ---------------------------------------------------------------------------------------- */

void IR_Sensors::add_noise( const vector <double> ir_ray_length, vector <double> &input, int *count_inputs )
{
  //calibrating distance to IR value reading according to a line equations
  for(int i = 0; i < ir_num_sensors; i++){
    bool add_noise = false;
    if ( ir_ray_length[i] > 0.03 && ir_ray_length[i] < ir_dist_range ){
      input[i] = -20600 * ir_ray_length[i] + 924; //[306, 100]
      add_noise = true;
    }
    else if ( ir_ray_length[i] > 0.02 && ir_ray_length[i] <= 0.03){
      input[i] = -37000 * ir_ray_length[i] + 1416; //[676, 306]
      add_noise = true;
    } 
    else if ( ir_ray_length[i] > 0.01 && ir_ray_length[i] <= 0.02){
      input[i] = -153500 * ir_ray_length[i] + 3746; //[2211, 676]
      add_noise = true;
    }
    else if ( ir_ray_length[i] > 0.005 && ir_ray_length[i] <= 0.01){
      input[i] = -252600 * ir_ray_length[i] + 4737;//[3474, 2211]
      add_noise = true;
    }
    else if ( ir_ray_length[i] >= 0.0 && ir_ray_length[i] <= 0.005 ){
      input[i] = -124200 * ir_ray_length[i] + 4095; //[4095, 3474]
      add_noise = true;
    }
    else if ( ir_ray_length[i] == ir_dist_range ){
      //just background noise
      input[i] = ((double)(gsl_rng_uniform_int(GSL_randon_generator::r_rand, 150 )));
      input[i] /= (double)(max_ir_reading);
    }
    
    if( add_noise ){
      double noise = (rint)(gsl_ran_gaussian(GSL_randon_generator::r_rand, 50));
      //(double)(gsl_rng_uniform_int(GSL_randon_generator::r_rand, 50 )*2)-49;
      input[i] += noise ;
      if( input[i] > max_ir_reading ) input[i] = max_ir_reading;
      else if ( input[i] < 0.0 ) input[i] = 0.0;
      input[i] /= (double)(max_ir_reading);
    }
  }
  *count_inputs = ir_num_sensors-1;
}

/* ---------------------------------------------------------------------------------------- */

IR_Sensors& IR_Sensors::operator=(const IR_Sensors &other)
{
    if(this != &other)
    {
        destroy();
        copy(other);
    }
    return *this;
}

/* ---------------------------------------------------------------------------------------- */

#ifdef _GRAPHICS_
void IR_Sensors::render_ir_sensor( World_Entity *agent, const vector <double> ir_ray_length ){
  for(int i = 0; i < ir_num_sensors; i++){
    double sensor_heading = agent->get_rot()[2] + epuck_sensors_angular_pos[i];    
    if(sensor_heading < 0.0 ) sensor_heading += TWO_PI;
    else if(sensor_heading > TWO_PI ) sensor_heading -= TWO_PI;
    
    double dist =  ir_ray_length[i] + agent->get_dim()[0]; 
    double x_projection = agent->get_pos()[0] + (dist * cos(sensor_heading));
    double y_projection = agent->get_pos()[1] + (dist * sin(sensor_heading));

    glEnable( GL_LINE_SMOOTH );
    glLineWidth(1.25);
    glColor3f(0.0, 0.8, 0.8);
    glBegin(GL_LINES);
    glVertex3f(agent->get_pos()[0], agent->get_pos()[1], agent->get_dim()[1]*0.9);
    glVertex3f(x_projection, y_projection, agent->get_dim()[1]*0.9);
    glEnd();
  }
}
#endif

/* ---------------------------------------------------------------------------------------- */
