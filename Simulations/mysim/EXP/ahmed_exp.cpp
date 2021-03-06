#ifndef _BULLET_WORLD_

#include "ahmed_exp.h"

/* ----------------------------------------------------------------------------------- */

Ahmed_experiment::Ahmed_experiment(Parameters *params) : Experiment(params) {
    int size = m_parameters->getNbAgentsPerGroup();
    inputs.resize(size);
    outputs.resize(size);

    ctrnn_input.resize(size);
    ctrnn_output.resize(size);

    m_robot_robot_distance.resize(size);

    ir_ray_length.resize(size);
#ifdef FLOOR_SENSORS
    floor_sensor_readings.resize(size);
#endif
    m_local_fitness.resize(size);

    int tot_num_sensors = m_irSensor->get_ir_num_sensors();
#ifdef SIMPLE_CAMERA
    tot_num_sensors += m_camera->get_num_pixel();
#endif
#ifdef FLOOR_SENSORS
    tot_num_sensors += m_floorSensor->get_num_floor_sensors();
#endif

    for (unsigned int r = 0; r < size; r++) {
        m_robot_robot_distance[r].assign(size, 0.0);
        m_pfsm.push_back(new Pfsm(r));

        if (m_pfsm[r]->get_num_input() /* m_networks[r]->get_num_input() */ != tot_num_sensors) {
            cerr
                    << "In Ahmed_experiment - There is a difference between the number of sensors and the input of the robot copntroller"
                    << endl;
            exit(EXIT_FAILURE);
        } else
            inputs[r].assign(tot_num_sensors, 0.0);

        if (m_pfsm[r]->get_num_output() /* m_networks[r]->get_num_output()*/ != 2) {
            cerr << "In Ahmed_experiment - The controller output must be 2 for the kinetic robot" << endl;
            exit(EXIT_FAILURE);
        } else
            outputs[r].assign(2, 0.0);

        ctrnn_input[r].assign(m_networks[r]->get_num_input(), 0.0);
        ctrnn_output[r].assign(m_networks[r]->get_num_output(), 0.0);

        ir_ray_length[r].assign(m_irSensor->get_ir_num_sensors(), m_irSensor->get_ir_dist_range());
#ifdef FLOOR_SENSORS
        floor_sensor_readings[r].assign(m_floorSensor->get_num_floor_sensors(), 0.0);
#endif
    }

    m_num_neighbours = 2;
}

/* ----------------------------------------------------------------------------------- */

Ahmed_experiment::~Ahmed_experiment() {
    m_pfsm.erase(m_pfsm.begin(), m_pfsm.begin() + m_pfsm.size());
    m_pfsm.clear();
}

/* ----------------------------------------------------------------------------------- */

void Ahmed_experiment::evaluate_solutions(void) {
    init_evaluations_loop();
    do {//Loop for evaluations for single trial
        init_single_evaluation();
        do {//This is the loop for the iterations mean simulation steps
            adv();
        } while (stop_iterations_loop());//Until the last iteration
        //for my work i need to compute the fitness here as average of agents fitness
    } while (stop_evaluations_loop());//Until the last evaluation
}

/* ----------------------------------------------------------------------------------- */

void Ahmed_experiment::init_evaluations_loop() {
    m_evaluation = 0;
    fill(m_fitness.begin(), m_fitness.end(), 0.0);

    if (m_parameters->isModeEvolution()) {//Evolutionary mode only
        vector<chromosome_type> geneSet;
        for (int r = 0; r < m_parameters->getNbAgentsPerGroup(); r++) {
            if (m_parameters->AreGroupsHeteroGroups()) {
                for (int j = 0; j < m_NbGenesPerInd; j++) {
                    geneSet.push_back(m_genes[(r * m_NbGenesPerInd) + j]);
                }
                m_networks[r]->init(geneSet);
                geneSet.erase(geneSet.begin(), geneSet.begin() + geneSet.size());
                geneSet.clear();
            } else {
                m_networks[r]->init(m_genes);
            }
        }
    }
}

/* ----------------------------------------------------------------------------------- */

void Ahmed_experiment::init_single_evaluation(void) {
  m_iteration = 0;
  fill(m_local_fitness.begin(), m_local_fitness.end(), 0.0);
  
  for (int r = 0; r < m_agents.size(); r++) {
    
    //Reset the position of agent r
    set_agents_position(r);
    
    //reset the network
    m_networks[r]->reset();
    
    //reset input values to zero
    fill(inputs[r].begin(), inputs[r].end(), 0.0);
    fill(ctrnn_input[r].begin(), ctrnn_input[r].end(), 0.0);

    //reset output values to zero
    fill(outputs[r].begin(), outputs[r].end(), 0.0);
    fill(ctrnn_output[r].begin(), ctrnn_output[r].end(), 0.0); 
  }
  
  //Here you define the type of environment
  if( m_evaluation % 2 ){
    floor_type = WHITE_DOMINANT;
  }
  else{
    floor_type = BLACK_DOMINANT;
  }
}

/* ----------------------------------------------------------------------------------- */

void Ahmed_experiment::set_agents_position(int r) {
  double min_dist = 0.008;
  bool flag = false;
  double x, y;
  
  vector<double> init_agents_pos;
  vector<double> init_agents_rot;
  vector<double> init_agents_vel;
  
  init_agents_pos.assign(3, 0.0);
  init_agents_rot.assign(3, 0.0);
  init_agents_vel.assign(3, 0.0);
  ///////////////////////////////////////////////////////////////////////////////////////
  do {
    flag = false;
    x = gsl_rng_uniform_pos(GSL_randon_generator::r_rand) * 1.8 + 0.1;//This is the arena length
    y = gsl_rng_uniform_pos(GSL_randon_generator::r_rand) * 1.8 + 0.1;
    int ar = 0;
    while (ar < r) {
      double x_dist = (x - m_agents[ar]->get_pos()[0]);
      double y_dist = (y - m_agents[ar]->get_pos()[1]);
      double distance = (x_dist * x_dist) + (y_dist * y_dist);
      if (distance < min_dist) {
	flag = true;
	break;
      }
      ++ar;
    }
  } while (flag);
  
  init_agents_pos[0] = x;
  init_agents_pos[1] = y;
  init_agents_rot[2] = gsl_rng_uniform_pos(GSL_randon_generator::r_rand) * TWO_PI;
  if (init_agents_rot[2] < 0.0) init_agents_rot[2] += TWO_PI;
  else if (init_agents_rot[2] > TWO_PI) init_agents_rot[2] -= TWO_PI;
  
  //  if( r == 0){
  //    init_agents_pos[0] = 0.038;
  //    init_agents_pos[1] = 0.038;
  //    init_agents_rot[2] = PI*0.5; //(gsl_rng_uniform_pos( GSL_randon_generator::r_rand )*PI/2) - PI/4;
  //  }
  /* else{
     init_agents_pos[0] = 0.038;
     init_agents_pos[1] = 0.0;
     init_agents_rot[2] = PI;//(gsl_rng_uniform_pos( GSL_randon_generator::r_rand )*PI/2) - PI/4;
     }*/
  
  //Robot init position
  m_agents[r]->set_pos(init_agents_pos);
  //Robot init orientation
  m_agents[r]->set_rot(init_agents_rot);
  //Robot init velocity
  m_agents[r]->set_vel(init_agents_vel);
}

/* ----------------------------------------------------------------------------------- */

void Ahmed_experiment::adv(void) {

    //cerr << " m_iteration = " << m_iteration << endl;
    if (m_parameters->isModeViewing())
        stop_iterations_loop();

    update_sensors();
    update_controllers();
    if (m_parameters->isModeViewing())
    print_opinion();
    update_actuators();
    update_world();

#ifdef WITH_COLLISION_CHECK
    //manage_collisions_without_repositioning ( );
    //manage_collisions_with_repositioning ( );
#endif
    compute_fitness();

    m_iteration++;
}

/* ----------------------------------------------------------------------------------- */

void Ahmed_experiment::update_sensors(void) {

    //Reset the array to store the ir and floor sensors readings
    for (int r = 0; r < m_agents.size(); r++) {
        fill(ir_ray_length[r].begin(), ir_ray_length[r].end(), m_irSensor->get_ir_dist_range());
#ifdef FLOOR_SENSORS
        fill(floor_sensor_readings[r].begin(), floor_sensor_readings[r].end(), 0.0);
#endif
    }

#ifdef SIMPLE_CAMERA
    //init and reset the array to store camera pixels value
    std::vector <double> pixel;
    pixel.assign(m_camera->get_num_pixel(), 0.0);
#endif


    for (int r = 0; r < m_agents.size(); r++) {
      
      ////////////////////////////////////////////////////////////////
      int count_inputs = 0;
      int count_ctrnn_inputs = -1;
#ifdef SIMPLE_CAMERA
      /* ############################################################ */
      // Reset camera pixels readings
      m_camera->reset_camera( );
      /* ############################################################ */
#endif
      
      /* ############################################################ */
      // Update ir ray tracing wrt other agents
      if (r < (m_agents.size() - 1)) {
	for (int t = r + 1; t < m_agents.size(); t++) {
	  m_robot_robot_distance[r][t] = m_irSensor->agent_agent_readings(m_agents[r], m_agents[t], ir_ray_length[r], ir_ray_length[t]);
	  m_robot_robot_distance[t][r] = m_robot_robot_distance[r][t];
	}
      }
      
#ifdef SIMPLE_CAMERA
      /* ############################################################ */
      //Update camera pixels wrt other agents
      for(int t=0; t < m_agents.size(); t++ )
	if( t != r ) m_camera->update_pixel_rgb_values( m_agents[r], m_agents[t] );
      /* ############################################################ */
#endif
      
      /* ############################################################ */
      // Update ir ray tracing wrt objects
      for (int obj = 0; obj < m_parameters->getNbObjects(); obj++) {
	//Update ir_readings wrt objects
	if (m_objects[obj]->get_type_id() == BRICK) {
	  m_irSensor->agent_brick_readings(m_agents[r],
					   m_objects[obj], ir_ray_length[r]);
	} else
	  m_irSensor->agent_round_obj_readings(m_agents[r],
					       m_objects[obj], ir_ray_length[r]);
	
#ifdef SIMPLE_CAMERA
	//Update camera pixels wrt objects ( Does the camera see objects? if YES, uncomment line below )
	//m_camera->update_pixel_rgb_values( m_agents[r], m_objects[obj] );
#endif
	
      }
      /* ############################################################ */
      
      /* ############################################################ */
      // COMPUTE IR READINGS, add noise and normalise in [0, 1] the readings
      m_irSensor->add_noise(ir_ray_length[r], inputs[r], &count_inputs);
      /* ############################################################ */
      
#ifdef SIMPLE_CAMERA
      /* ############################################################ */
      m_camera->merge_camera_pixels_view( ); //Merge CAMERA PIXELS * 3 (RGB)
      // For the 3 front RGB camera, we put in inputs color that we want to detect
      // 0 means no object with this color in front of you
      // 1 means an object with this color in front of you
      for(int p = 0; p < m_camera->get_num_pixel(); p++) {
	m_camera->get_pixel_rgb(pixel, p);
	//cerr << " III pix0= " << pixel[0] << " pix1= " << pixel[1] << " pix2= " << pixel[2] << endl;
	// Color that robots will see
	if( p == 0 ) inputs[r][++count_inputs]     = pixel[0];
	else if (p == 1) inputs[r][++count_inputs] = pixel[0];
	else if (p == 2) inputs[r][++count_inputs] = pixel[0];
      }
#endif
      
      /* ############################################################ */
#ifdef FLOOR_SENSORS
      m_floorSensor->agent_floor_sensor_readings(m_agents[r], m_objects[0], floor_sensor_readings[r], floor_type);
      m_floorSensor->add_noise(floor_sensor_readings[r], ctrnn_input[r], &count_ctrnn_inputs);
#endif
      /* ############################################################ */
      
      /* ############################################################ */
      // This is all about communication
      vector<pair<double, int> > dist_vect;
      for (int t = 0; t < m_agents.size(); t++) {
	if (t != r)
	  dist_vect.push_back(make_pair(m_robot_robot_distance[r][t], t));// sorted from smallest to highest value ascending order ???
      }
      sort(dist_vect.begin(), dist_vect.end());
      for (int f = 0; f < m_num_neighbours; f++) {
	////////////////////////////////////////////////////////////////
	if (ctrnn_output[dist_vect[f].second][0] > 0.5 /* && dist_vect[f].first <= 0.5 */ ) {
	  ctrnn_input[r][++count_ctrnn_inputs] = 1;
	} else {
	  ctrnn_input[r][++count_ctrnn_inputs] = 0;
	}
      }
      dist_vect.erase(dist_vect.begin(), dist_vect.begin() + dist_vect.size());
      dist_vect.clear();
      
      //This is the self signal for robot r
      if (ctrnn_output[r][0] > 0.5) {
	ctrnn_input[r][++count_ctrnn_inputs] = 1;
        } else {
	ctrnn_input[r][++count_ctrnn_inputs] = 0;
      }
      /* ############################################################ */
      
      /* ############################################################ */
      //if( r == 0 ){
      /* cerr << " Iter = " << m_iteration
	 << " x = " <<   m_agents[r]->get_pos( )[0]
	 << " y = " <<   m_agents[r]->get_pos( )[1]
	 << endl; */
      //for(int i = 0; i < 8/*inputs[r].size()*/; i++){
      //	cerr << " s["<<i<<"] = " << inputs[r][i] << " ir_length = " << ir_ray_length[r][i] << endl;
      //}
      //}
      /* ############################################################ */
    }
}

/* ----------------------------------------------------------------------------------- */

void Ahmed_experiment::update_controllers(void) {
  vector<double> colour;
  
  for (int r = 0; r < m_agents.size(); r++) {
    // Update robot controllers
    //m_networks[r]->step( inputs[r], outputs[r] );
    m_pfsm[r]->step(inputs[r], outputs[r]);
    ///////////////////////////////////////////////////////////////////////
    //This is the network for the decision making
    m_networks[r]->step( ctrnn_input[r], ctrnn_output[r] );
    colour.assign(3, round(ctrnn_output[r][0]));
    m_agents[r]->set_colour( colour );
    //////////////////////////////////////////////////////////////////////
    //    cout<<m_local_fitness.size();    
    
    //    if( r == 0 ){
    //      outputs[r][0] = 0.0; //0.0; full spedd backward // 1.0;full speed forward  //0.5; no movement
    //      outputs[r][1] = 0.0; //0.0; // 1.0; // 0.5;
    //    }
  }
}

/* ----------------------------------------------------------------------------------- */

void Ahmed_experiment::update_actuators(void) {
  for (int r = 0; r < m_agents.size(); r++) {
    //update robot wheels velocity
    m_agents[r]->set_vel(outputs[r]);
  }
}

/* ----------------------------------------------------------------------------------- */

void Ahmed_experiment::update_world(void) {
  for (int r = 0; r < m_agents.size(); r++) {
    //update robot position and rotation
    m_agents[r]->update_pos_rot();
  }
}

/* ----------------------------------------------------------------------------------- */
#ifdef WITH_COLLISION_CHECK
void Ahmed_experiment::manage_collisions_without_repositioning (void ){
  
  for(int r=0; r < m_agents.size(); r++){
    
    //Check collisions with other round-shaped agents
    if( r < (m_agents.size() - 1) ) {
      for(int t=r+1; t < m_agents.size(); t++){
    double theta = 0.0, disp = 0.0;
    if( m_agents[r]->check_collision_with_round_object( m_agents[t], &theta, &disp ) ){
      cerr << "Collision between robot r = " << r << " and robot t = " << t << endl;
      exit(EXIT_FAILURE);
    }
      }
    }

    for(int obj = 0; obj < m_parameters->getNbObjects(); obj++){
      //Check collisions with bricks for agent r
      if( m_objects[obj]->get_type_id() == BRICK) {
    if( m_agents[r]->check_collision_with_wall ( m_objects[obj] ) ){
      cerr << " Collision between a brick and robot r = " << r << endl;
      exit(EXIT_FAILURE);
    }
      }

      //Check collisions with round shape objects for agent r
      if( m_objects[obj]->get_type_id() == CYLINDER || m_objects[obj]->get_type_id() == SPHERE ) {
    double theta = 0.0, disp = 0.0;
    if( m_agents[r]->check_collision_with_round_object( m_objects[obj], &theta, &disp ) ){
      cerr << " Collision between a round shape object and robot r = " << r << endl;
      exit(EXIT_FAILURE);
    }
      }
    }
  }
}
#endif

/* ----------------------------------------------------------------------------------- */

#ifdef WITH_COLLISION_CHECK
void Ahmed_experiment::manage_collisions_with_repositioning (void ){

  //reset collision matrix
  vector < int >               collision_matrix;
  collision_matrix.assign ( m_parameters->getNbAgentsPerGroup(), 0.0 );
  //vector < int >               collision_angles;
  //collision_angles.assign( m_parameters->getNbAgentsPerGroup(), 0.0 );

  //check and resolve collisions
  bool there_are_collisions = false;

  do{
    there_are_collisions = false;
    for(int r=0; r < m_agents.size()-1; r++){
      //Check collisions with other round-shaped agents
      for(int t=r+1; t < m_agents.size(); t++){
    double theta = 0.0, disp = 0.0;
    if( m_agents[r]->check_collision_with_round_object( m_agents[t],
                                        &theta, &disp ) ){
      collision_matrix[r] = 1;
      //collision_angles[r] = theta - m_agents[r]->get_rot()[2];
      collision_matrix[t] = 1;
      m_agents[r]->update_displacement_after_collision_with_round( theta, disp );
      /* cerr <<  " collision_matrix[r] = " <<  collision_matrix[r]
         << " theta = " << theta
         << " disp = " << disp; */

      theta += PI;
      if( theta > TWO_PI ) theta -= TWO_PI;
      //collision_angles[t] = theta - m_agents[t]->get_rot()[2];
      m_agents[t]->update_displacement_after_collision_with_round( theta, disp );
      /* cerr <<  " collision_matrix[t] = " <<  collision_matrix[t]
         << " theta = " << theta
         << " disp = " << disp << endl; */
    }
      }

      //Check collisions with walls for agent r
      for(int obj = 0; obj < m_parameters->getNbObjects(); obj++){
    if( m_objects[obj]->get_type_id() == BRICK) {
      if( m_agents[r]->check_collision_with_wall ( m_objects[obj] ) ){
        collision_matrix[r] = 2;
      }
      if ( r == (m_agents.size()-2) ){
        if( m_agents[idActiveAgents[r+1]]->check_collision_with_wall ( m_objects[obj] ) ){
          collision_matrix[idActiveAgents[r+1]] = 2;
        }
      }
    }
      }
    }

    for(int r=0; r < m_agents.size(); r++){
      if( collision_matrix[r] ){
    there_are_collisions = true;
    m_agents[r]->update_direction_of_displacement( );
    m_agents[r]->update_tmp_position_after_collisions();
      }
      collision_matrix[r] = 0;
    }
  }while( there_are_collisions );

  //Update positions after collisions and reset all sensors related
  //variables
  for(int r=0; r < m_agents.size(); r++){
    m_agents[r]->update_position_after_collisions();
    //    for(int l = 0; l < ir_ray_length[r].size(); l++){
    //ir_ray_length[r][l] = m_irSensor->get_ir_dist_range();

    /*    fill( ir_ray_length[r].begin(),
      ir_ray_length[r].end(),
      m_irSensor->get_ir_dist_range() ); */
    //}
  }
}
#endif

/* ----------------------------------------------------------------------------------- */
//for my experiment i dont need to compute fitness each time step
void Ahmed_experiment::compute_fitness(void) {  
  if( m_iteration > (m_parameters->getNbMaxIterations() * 0.5) ){
    if( floor_type == WHITE_DOMINANT ){
      for(int r=0; r < m_agents.size(); r++)
	m_local_fitness[r] += ctrnn_output[r][0];
    }
    else{
      for(int r=0; r < m_agents.size(); r++)
	m_local_fitness[r] += 1.0 - ctrnn_output[r][0];
    }

    /*if (m_parameters->isModeViewing()){
      cerr << " floor_type = " << floor_type << endl;
      for(int r=0; r < m_agents.size(); r++)
      cerr << " fit["<< r<< "] = "<< m_local_fitness[r];
      cerr << endl;*/
    //cerr << " ctrnn_output["<<r<<"][0] = " << ctrnn_output[r][0];
    //cerr << endl;
    //}
  }
}

/* ----------------------------------------------------------------------------------- */
//This function is called at the end of each evaluation
void Ahmed_experiment::finalise_single_evaluation(void) {
  double tmp_var = 0.0;
  for(int r=0; r < m_agents.size(); r++){
    tmp_var += ((double)(m_local_fitness[r] / (m_parameters->getNbMaxIterations() * 0.5) ));
  }
  m_fitness[0] += tmp_var / (double)(m_agents.size());
  //if (m_parameters->isModeViewing()){
  //cerr << " m_fitness[0] = " << m_fitness[0] << " tmp= " << tmp_var / (double)(m_agents.size()) << endl;
  //}

}

/* ----------------------------------------------------------------------------------- */
//This function is called at the end of all evaluations
void Ahmed_experiment::finalise_evaluation_loop(void) {
  m_fitness[0] /= (double)(m_parameters->getNbMaxEvaluations());
}

/* ----------------------------------------------------------------------------------- */
//This function is used to print the opinion of robots
void Ahmed_experiment::print_opinion(void) {
 ofstream White_opinion , Black_opinion;
 if(floor_type == WHITE_DOMINANT)
 {
  White_opinion.open("../opinion/Opionion_White.txt" , ios::app);
  if (White_opinion.is_open() ) {
    for(int r=0; r < m_agents.size(); r++)
    White_opinion<<round(ctrnn_output[r][0])<<" ";
    White_opinion<<endl;
  }
 else
 cout<< "The Opionion_White File doesnt Exist\n";
 }

 if(floor_type == BLACK_DOMINANT)
 {
  Black_opinion.open("../opinion/Opionion_Black.txt" + m_evaluation, ios::app);
  if (Black_opinion.is_open() ) {
    for(int r=0; r < m_agents.size(); r++)
    Black_opinion<<round(ctrnn_output[r][0])<<" ";
    Black_opinion<<endl;
  }
 else
 cout<< "The Opionion_Black File doesnt Exist\n";
 }
 White_opinion.close();
 Black_opinion.close();
}

/* ----------------------------------------------------------------------------------- */

bool Ahmed_experiment::stop_iterations_loop(void) {
    if (m_iteration >= m_parameters->getNbMaxIterations()) {
        finalise_single_evaluation();
        m_iteration = 0;
        if (m_parameters->isModeEvolution() || m_parameters->isModeEvaluation()) {
            return false;
        } else if (m_parameters->isModeViewing()) {
            if (stop_evaluations_loop()) {
                return true;
            } else {
                return false;
            }
        }
    } else
        return true;
    return true;
}

/* ----------------------------------------------------------------------------------- */

bool Ahmed_experiment::stop_evaluations_loop(void) {
    m_evaluation++;
    if (m_parameters->isModeEvolution()) {
        if (m_evaluation >= m_parameters->getNbMaxEvaluations()) {
            m_evaluation = 0;
            finalise_evaluation_loop();
            return false;
        } else
            return true;
    } else if (m_parameters->isModeViewing()) {
        if (m_evaluation >= m_parameters->getNbMaxEvaluations()) {
            m_evaluation = 0;
            finalise_evaluation_loop();
            exit(0);
            init_evaluations_loop();
            return false;
        } else {
            init_single_evaluation();
            return true;
        }
    } else if (m_parameters->isModeEvaluation()) {
        if (m_evaluation >= m_parameters->getNbMaxEvaluations()) {
            m_evaluation = 0;
            finalise_evaluation_loop();
            return false;
        } else {
            return true;
        }
    }
}

/* ----------------------------------------------------------------------------------- */
#endif

