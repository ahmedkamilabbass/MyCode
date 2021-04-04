#ifdef _BULLET_WORLD_
#include "first_exp.h"

/* ----------------------------------------------------------------------------------- */

First_experiment::First_experiment( Parameters* params ) : Experiment ( params ) {

  int size = m_parameters->getNbAgentsPerGroup();
  inputs.resize                      ( size );
  outputs.resize                     ( size );
  camera_sector_readings.resize      ( size );
  optic_flow_sensor_readings.resize  ( size );
  gyro_readings.resize               ( 1 );
  //m_local_fitness.resize             ( m_parameters->getNbFitObjectives()  );
  
  for (unsigned int r = 0; r < size; r++) {
    outputs[r].assign                    ( m_networks[r]->get_num_output(),   0.0 );
    camera_sector_readings[r].assign     ( m_agents[r]->num_camera_sectors,   0.0 );
    optic_flow_sensor_readings[r].assign ( m_agents[r]->num_mouse_parameters, 0.0 );
  }
    
  //m_init_agent_dist_to_object.resize ( size );
  //m_max_obj_displacement     = 1.5;
  //m_max_agent_dist_to_object = 0.58;
  //m_evals_per_obj_shape      = m_parameters->getNbMaxEvaluations()/m_parameters->getNbFitObjectives();
}

/* ----------------------------------------------------------------------------------- */

First_experiment::~First_experiment() {}

/* ----------------------------------------------------------------------------------- */

void First_experiment::evaluate_solutions( void ) {
  init_evaluations_loop( );
  do{//Loop for evaluations for single trial
    init_single_evaluation( );
    do{//This is the loop for the iterations mean simulation steps
      adv();
    }while( stop_iterations_loop( ) );//Untill the last iteration
  }while( stop_evaluations_loop( ) );//Until the last evaluation
}

/* ----------------------------------------------------------------------------------- */

void First_experiment::init_evaluations_loop( ){
  m_evaluation = 0;
  fill( m_fitness.begin(), m_fitness.end(), 0.0 );
  //fill( m_local_fitness.begin(), m_local_fitness.end(), 0.0 ); 
  
  if( m_parameters->isModeEvolution() ){//Evolutionary mode only    
    vector < chromosome_type > geneSet;
    for(int r = 0; r < m_parameters->getNbAgentsPerGroup(); r++){
      if ( m_parameters->AreGroupsHeteroGroups() ) {
	for( int j = 0; j < m_NbGenesPerInd; j++){
	  geneSet.push_back( m_genes[(r*m_NbGenesPerInd) + j] );
	}
	m_networks[r]->init( geneSet );
	geneSet.erase(geneSet.begin(), geneSet.begin() + geneSet.size() );
	geneSet.clear();
      }
      else{
	m_networks[r]->init( m_genes );
      }
    }
  }
}

/* ----------------------------------------------------------------------------------- */

void First_experiment::init_single_evaluation( void ){ 
  m_iteration = 0;
  cout << " HERE 1 " << endl;
  set_agents_position( 0 );
  
  for(int r = 0; r < m_parameters->getNbAgentsPerGroup(); r++) {
    if( !m_agents[r]->is_removed() ){
      
      //reset the network
      m_networks[r]->reset();
      
      //reset output values to zero
      fill( outputs[r].begin(), outputs[r].end(), 0.0);
    
      //reset the camera_sector_readings to zero
      fill( camera_sector_readings[r].begin(), camera_sector_readings[r].end(), 0.0);
      
      //reset the optic_flow_sensor_readings to zero
      fill( optic_flow_sensor_readings[r].begin(), optic_flow_sensor_readings[r].end(), 0.0);
      
      //init distance to object to be transported for fitness
      //m_init_agent_dist_to_object[r] = find_distance <double > ( m_objects[0]->get_pos(), m_agents[r]->get_pos() );
    }
  }
  cout << " HERE 3 " << endl;
  // m_obj_displacement      = 0.0;
  // m_time_taken_to_succeed = (double)(m_parameters->getNbMaxIterations());
}

/* ----------------------------------------------------------------------------------- */

void First_experiment::set_agents_position( int r ){
  
  // //Elio, the following if else switch between object types
  // if(m_evaluation == 0){
  //   m_objects[0]->set_size(0.0); //select cubiod object
  //   // cout<<"cub"<<endl;
  // }else if(m_evaluation == m_evals_per_obj_shape ){
  // m_objects[0]->set_size(1.0); //select cross object
  //   // cout<<"cr"<<endl;
  // }else if(m_evaluation == (m_evals_per_obj_shape * 2 )){
  // m_objects[0]->set_size(2.0); // select H-shaped object
  //   // cout<<"h"<<endl;
  // }
  resetPhysicsState();
  
  for(int obj=0; obj<m_objects.size(); obj++){
    if( m_objects[obj]->get_mass() != 0.0){
      m_objects[obj]->reset_pos();
      m_objects[obj]->body[m_objects[obj]->object_id]->clearForces();
      m_objects[obj]->body[m_objects[obj]->object_id]->setLinearVelocity(btVector3(0.0,0.0,0.0));
      m_objects[obj]->body[m_objects[obj]->object_id]->setAngularVelocity(btVector3(0.0,0.0,0.0));
    }
    //m_objects[obj]->add_object();
  }

  cout << " nb actvive agents = " << m_nbActiveAgents << endl;
  
  init_agents_pos.erase(init_agents_pos.begin(), init_agents_pos.begin() + init_agents_pos.size() );
  init_agents_pos.clear();
  init_agents_rot.erase(init_agents_rot.begin(), init_agents_rot.begin() + init_agents_rot.size() );
  init_agents_rot.clear();
  init_agents_pos.resize( m_nbActiveAgents );
  init_agents_rot.resize( m_nbActiveAgents );
  for(int r = 0; r < m_nbActiveAgents; r++){
    init_agents_pos[r].assign(3, 0.0);
    init_agents_rot[r].assign(3, 0.0);
  }
  
//     double x_swift, y_swift;
//     double min_dist    = 0.10;
//     bool flag          = false;
//     double alfa_sector = TWO_PI/(double)(m_nbActiveAgents);
//     for(int r = 0; r < m_nbActiveAgents; r++){
//       if(!m_agents[r]->is_removed())
//         {
//    init_agents_pos[r].assign(3, 0.0);
//    init_agents_rot[r].assign(3, 0.0);
//    do{
//      flag = false;
  
//      if( r < m_nbActiveAgents * 0.25 ){
//        x_swift = 0.022;
//        y_swift = 0.022;
//      }
//      else if( r < m_nbActiveAgents * 0.5 ){
//        x_swift = -0.022;
//        y_swift = 0.022;
//      }
//      else if( r < m_nbActiveAgents * 0.75 ){
//        x_swift = 0.022;
//        y_swift = -0.022;
//      }
//      else {
//        x_swift = -0.022;
//        y_swift = -0.022;
//      }
  
//      double angle = (alfa_sector*r) + (gsl_rng_uniform_pos(GSL_randon_generator::r_rand) * alfa_sector );
//      init_agents_pos[r][0] = x_swift + (cos(angle) * (gsl_rng_uniform_pos(GSL_randon_generator::r_rand) * 1) );
//      init_agents_pos[r][2] = y_swift + (sin(angle) * (gsl_rng_uniform_pos(GSL_randon_generator::r_rand) * 1) );
//      for(int ar = 0; ar < r; ar++){
//        if ( find_distance <double > ( init_agents_pos[r], init_agents_pos[ar] ) < min_dist ) {
//          flag = true;
//          break;
//        }
//      }
  
//           } while( flag );
  
//    init_agents_rot[r][1] = gsl_rng_uniform_pos( GSL_randon_generator::r_rand )*TWO_PI;
//      //atan2(init_agents_pos[r][0], init_agents_pos[r][2]) + (PI*0.5) + (gsl_rng_uniform_pos( GSL_randon_generator::r_rand )*PI/5.0 - (PI/10.0) );
//    if( init_agents_rot[r][1] < 0.0) init_agents_rot[r][1] = ( TWO_PI + init_agents_rot[r][1]);
//    else if ( init_agents_rot[r][1] > TWO_PI) init_agents_rot[r][1] = (init_agents_rot[r][1] - TWO_PI);
  
//    m_agents[r]->set_robot_pos_rot( init_agents_pos[r], init_agents_rot[r] );
//         }
//     }

/* 
  double min_dist = 0.10;//(1.5 + (2.0 * m_agents[0]->get_radius())) * (1.5 + (2.0 * m_agents[0]->get_radius()));
  bool flag = false;
  double angle = 0.0;
  double dist = 0.0;
  
  std::vector < double > sector;
  sector.resize(m_nbActiveAgents);
  for (int i = 0; i < sector.size(); i++) sector[i] = 2.0*PI/((double)m_nbActiveAgents) - (2.0*PI/((double)m_nbActiveAgents)*i);
  
  for(int r = 0; r < m_nbActiveAgents; r++){
    if(!m_agents[r]->is_removed())
      {
	init_agents_pos[r].assign(3, 0.0);
	init_agents_rot[r].assign(3, 0.0);
	do{
	  flag = false;
	  int pos1 = gsl_rng_uniform_int (GSL_randon_generator::r_rand, sector.size() );
	  double index = sector[pos1];
	  sector.erase( sector.begin() + pos1 );
	  if( sector.empty() ) {
	    sector.resize(m_nbActiveAgents);
	    for (int i = 0; i < sector.size(); i++) sector[i] = 2.0*PI/((double)m_nbActiveAgents) - (2.0*PI/((double)m_nbActiveAgents)*i);;
	  }
	  angle = index - (gsl_rng_uniform_pos(GSL_randon_generator::r_rand) * PI/3.0);
	  //dist = ((floor_target_dim[nest_id][1] - 5.0) * gsl_rng_uniform_pos(GSL_randon_generator::r_rand));
	  //cerr << " pos = " << pos << " sector = " << sector[pos] << " index = " << index << " angle = " << ((angle*180.0)/PI) << endl;
	  //getchar();
	  
	  init_agents_pos[r][0]=   cos(angle) * 0.50;
	  init_agents_pos[r][2] =  sin(angle) * 0.50;
	  for(int ar = 0; ar < r; ar++){
	    double x_off = init_agents_pos[r][0] - init_agents_pos[ar][0];
	    double y_off = init_agents_pos[r][2] - init_agents_pos[ar][2];
	    if( ((x_off*x_off)+(y_off*y_off)) < (min_dist*min_dist) ){
	      flag = true;
	      break;
	    }
	  }
	} while( flag );
      }
  }
  
  //}while(!(find_distance(pos[0],pos[3])>0.10 && find_distance(pos[1],pos[2])>0.10));
  
  for(int i=0;i < m_nbActiveAgents;i++){
    init_agents_rot[i][1] = atan2(init_agents_pos[i][2], init_agents_pos[i][0]);
    if(init_agents_rot[i][1] < 0.0) init_agents_rot[i][1] = ( TWO_PI + init_agents_rot[i][1]);
    init_agents_rot[i][1] = (TWO_PI - init_agents_rot[i][1]) + PI + gsl_rng_uniform_pos( GSL_randon_generator::r_rand )*PI/2 - (PI/4);
  }
*/
  
  init_agents_pos[0][2] = 0.0;
  init_agents_pos[0][0] = 0.0;
  init_agents_rot[0][1] = 0.5*PI;
  
  cout << " HERE 2 " << endl;
  for(int r=0;r<m_nbActiveAgents;r++)
    // if(!param->agent[r]->is_removed())
    m_agents[r]->set_robot_pos_rot( init_agents_pos[r], init_agents_rot[r] );
  
}

/* ----------------------------------------------------------------------------------- */

void First_experiment::adv ( void ){

  if( m_parameters->isModeViewing() )
    stop_iterations_loop( );
  
  update_sensors( );
  update_controllers ();
  update_actuators();

  for(int i=0; i < NbPhysicsWorldUpdates; i++){
    update_world();
    m_world->stepSimulation( m_physicsStep );
  }
  
  // Elio, this loop update the max distance between the agents and the object otherwise you get negative fitness (see computer fitness)
  /*for(int r = 0;r<m_parameters->getNbAgentsPerGroup();r++){
    if(!this->m_agents[r]->is_removed()){
      double current_dist = find_distance(this->m_objects[0]->get_pos(), m_agents[r]->get_pos());
      //cerr << " " << current_dist << endl;
      if(current_dist > m_max_agent_dist_to_object){
	m_max_agent_dist_to_object = current_dist;
      }
    }
    }*/
  
  update_optic_flow_sensors();
  manage_collisions ();

  //Elio, fitness will be evaluated at the end of each evaluation
  compute_fitness();
  m_iteration++;
}

/* ----------------------------------------------------------------------------------- */

void First_experiment::update_sensors( void ){
  
  for(int r=0; r < m_parameters->getNbAgentsPerGroup(); r++){    
    if(!m_agents[r]->is_removed()){
      
      //Clean and update the inputs vector with the 8 IR readings
      inputs[r].erase(inputs[r].begin(), inputs[r].begin() + inputs[r].size() );
      inputs[r].clear();
      m_agents[r]->get_IR_reading( inputs[r] );
      m_agents[r]->add_noise( inputs[r] );
	      
      //Update input vector with the 3 camera readings
      fill( camera_sector_readings[r].begin(), camera_sector_readings[r].end(), 0.0 );
      m_agents[r]->get_camera_reading( camera_sector_readings[r] );
      for( int i = 0; i < camera_sector_readings[r].size(); i++){
	inputs[r].push_back( camera_sector_readings[r][i] );
      }

      //Copy (and then reset) previous 4 outputs into input vector
      /* for(int i = 0; i < outputs[r].size(); i++){
	 inputs[r].push_back( outputs[r][i] );
	 outputs[r][i] = 0.0;
	 } */

      //Copy (and then reset) the 4 optic flow sensor readings into input vector
      for(int i = 0; i < optic_flow_sensor_readings[r].size(); i++){
	inputs[r].push_back( optic_flow_sensor_readings[r][i] );
	optic_flow_sensor_readings[r][i] = 0.0;
      }

      //Update the 1 gyro reading
      fill( gyro_readings.begin(), gyro_readings.end(), 0.0 );
      m_agents[r]->get_gyroY_readings( gyro_readings );
      for( int i = 0; i < gyro_readings.size(); i++){
	inputs[r].push_back( gyro_readings[i] );
      }
      
    }
    
    //for(int i = 0; i < inputs[r].size(); i++)
    //cerr << " inputs["<<r<<"]["<<i<< "] = " << inputs[r][i] << endl;
  }
}

/* ----------------------------------------------------------------------------------- */

void First_experiment::update_controllers(void)
{
  // Update robot controllers
  // Give the inputs values to the network, and compute outputs values
  for(int r = 0 ; r < m_parameters->getNbAgentsPerGroup(); r++) {
    if( !m_agents[r]->is_removed() ) {
      m_networks[r]->step( inputs[r], outputs[r] );
    }
  }
}

/* ----------------------------------------------------------------------------------- */

void First_experiment::update_actuators( void ){
  for(int r=0; r < m_parameters->getNbAgentsPerGroup(); r++){
    outputs[r][0] = 1.0;
    outputs[r][1] = 0.0;
    outputs[r][2] = 0.0;
    outputs[r][3] = 1.0;
    /* for(int i = 0; i < outputs[r].size(); i++)
      cerr << " outputs["<<r<<"]["<<i<<"]=" << outputs[r][i];
      cerr << endl; */
    /*    cerr << " X = " << m_agents[r]->get_pos()[0]
	  << " Y = " << m_agents[r]->get_pos()[2] << endl; */
      
    //update robot wheels velocity
    if( !m_agents[r]->is_removed() ){
      m_agents[r]->set_vel( outputs[r] );
    }
  }
}

/* ----------------------------------------------------------------------------------- */

void First_experiment::update_world( void ){
  for(int r=0; r < m_parameters->getNbAgentsPerGroup(); r++){
    if(!m_agents[r]->is_removed()){
      //update robot position and rotation
      m_agents[r]->update_pos_rot( );
    }
  }
}

/* ----------------------------------------------------------------------------------- */

void First_experiment::update_optic_flow_sensors( void ){
  for(int r=0; r < m_parameters->getNbAgentsPerGroup(); r++){
    if( !m_agents[r]->is_removed() ){
      m_agents[r]->get_mouse_reading( optic_flow_sensor_readings[r] );
    }
  }
}

/* ----------------------------------------------------------------------------------- */

void First_experiment::manage_collisions (void ){ }

/* ----------------------------------------------------------------------------------- */

void First_experiment::compute_fitness( void ){
  //if ( find_distance(m_objects[0]->get_pos(), m_objects[0]->start_pos) > m_max_obj_displacement)
  //m_iteration = m_parameters->getNbMaxIterations();
}

/* ----------------------------------------------------------------------------------- */

bool First_experiment::stop_iterations_loop(void)
{
  if(m_iteration >= m_parameters->getNbMaxIterations() )
    {
      finalise_single_evaluation( );
      m_iteration = 0;
      if( m_parameters->isModeEvolution() || m_parameters->isModeEvaluation() ){
	return false;
      }
      else if ( m_parameters->isModeViewing() ) {
      	if ( stop_evaluations_loop( ) ){
	  return true;
	}
	else{
	  return false;
	}
      }
    }
  else
    return true;
  return true;
}

/* ----------------------------------------------------------------------------------- */

bool First_experiment::stop_evaluations_loop(void)
{
  m_evaluation++;
  if( m_parameters->isModeEvolution() ){
    if( m_evaluation >= m_parameters->getNbMaxEvaluations() )
      {
	m_evaluation = 0;
	finalise_evaluation_loop( );
	return false;
      }
    else
      return true;
  }
  else if( m_parameters->isModeViewing() ){
    if( m_evaluation >= m_parameters->getNbMaxEvaluations() ) {
      m_evaluation = 0;
      finalise_evaluation_loop( );
      init_evaluations_loop( );
      return false;
    }
    else{
      init_single_evaluation( );
      return true;
    }
  }
  else if(m_parameters->isModeEvaluation() ) {
    if( m_evaluation >= m_parameters->getNbMaxEvaluations() ) {
      m_evaluation = 0;
      finalise_evaluation_loop( );
      return false;
    }
    else{
      return true;
    }
  }
}

/* ----------------------------------------------------------------------------------- */

void First_experiment::finalise_single_evaluation( void ){
  /* 20/01/2021
  double agents_closeness_to_object = 0.0;
  for(int r = 0; r < m_parameters->getNbAgentsPerGroup(); r++){
    if(!m_agents[r]->is_removed()){
      double current_dist = find_distance(m_objects[0]->get_pos(), m_agents[r]->get_pos());
      if (current_dist > m_max_agent_dist_to_object )
	current_dist = m_max_agent_dist_to_object;
      else if(current_dist < 0.2)
	current_dist = 0.0;
      agents_closeness_to_object += 1.0 - (current_dist/m_max_agent_dist_to_object);
    }
  }
  agents_closeness_to_object /= (double)(m_nbActiveAgents);
  */

  
  //cerr << " agents_closeness_to_object = " << agents_closeness_to_object << endl;
  
  // printf("\n");
  // printf("\n linear = %f iter = %d", linear_vel, remaining_iter);
  // printf("\n lin_fit = %f dis_fit = %f", linear_vel/35.0, find_distance(param->object[0]->get_pos(),param->object[0]->start_pos));
  // printf("\n distance = %f  agent_clos = %f",find_distance(param->object[0]->get_pos(),param->object[0]->start_pos), agents_closeness_to_object);

  /* 20/01/2021
  double obj_displacement = find_distance(m_objects[0]->get_pos(), m_objects[0]->start_pos);
  */

  //if( obj_displacement > m_max_obj_displacement){
  //obj_displacement = m_max_obj_displacement;
  //}

  /* 20/01/2021
  if( obj_displacement > 0.1 ){
    m_local_fitness[0] +=  (obj_displacement*10.0);
  }
  else{
    m_local_fitness[0] +=  agents_closeness_to_object;
    } */
  
  // if(m_evaluation < m_evals_per_obj_shape ){  // fitness for cubiod object
  // m_local_fitness[0] +=  (obj_displacement*10.0) + agents_closeness_to_object;
  // }else if (m_evaluation < (m_evals_per_obj_shape * 2) ) {// fitness for cross object
  //   m_local_fitness[1] +=  obj_displacement + agents_closeness_to_object;
  // }else{ // fitness for H-shaped object
  //   m_local_fitness[2] +=  obj_displacement + agents_closeness_to_object;
  // }
  /*cerr << " local_f " << m_local_fitness[0]
    << " local_f " << m_local_fitness[1]
    << " local_f " << m_local_fitness[2] << endl;*/ 
}

/* ----------------------------------------------------------------------------------- */

void First_experiment::finalise_evaluation_loop( void ){
  // if( !m_parameters->AreGroupsHeteroGroups() ){ // Homogeneous groups
  // if(!m_parameters->IsSelectionIndividual()){ // Group selection
  // In this case m_pop_solutions_received[i].size = m_parameters->getNbAgentsPerGroup() but all the values are the same
  
  // int counter = 0;
  // double tmp_genes = (m_genes[0]*20.0)-10.0;
  // for(int solutions = 0; solutions <  m_parameters->get_nb_chromosome_per_group(); solutions++){
  //   for(int obj = 0; obj < m_parameters->getNbFitObjectives(); obj++){
  //     m_fitness[counter] = m_genes[obj];
  //     /*if( obj == 0 ){
  // 	m_fitness[counter] = tmp_genes*tmp_genes;
  // 	}
  // 	else{
  // 	m_fitness[counter] = (tmp_genes-2.0)*(tmp_genes-2.0);
  // 	}*/
  //     counter++;
  //   }
  // }

      
  //for(int i = 0; i < 1;/*m_parameters->getNbAgentsPerGroup();*/ i++){
  // for( int j = 0; j < m_NbGenesPerInd; j++){
  //  sum_genes += m_genes[m_pop_solutions_received[i]*(m_NbGenesPerInd) + j];
  //}
  //}
  //sum_genes /=  (m_parameters->getNbAgentsPerGroup() * m_NbGenesPerInd);
  //sum_genes = (sum_genes * 2000.0) - 1000.0;

  /* 20/01/2021
     int counter = 0;
     for(int solutions = 0; solutions <  m_parameters->get_nb_chromosome_per_group(); solutions++){
     for(int obj = 0; obj < m_parameters->getNbFitObjectives(); obj++){
     m_fitness[counter] =  m_local_fitness[obj] / (double)(m_evals_per_obj_shape);
     counter++;
     }    
     } */
  
  //gsl_rng_uniform_int(GSL_randon_generator::r_rand, 10);
  /*
    }
    else{//Individual selection
    // In this case m_pop_solutions_received[i].size = m_parameters->getNbAgentsPerGroup() and values are different
    }
    }
    else{//Heterogeneous
    if(!m_parameters->IsSelectionIndividual()){//Group selection
    }
    else{//Individual selection
    }
    } */
}

/* ----------------------------------------------------------------------------------- */
#endif

