#include "al_sensors.h"

/* ---------------------------------------------------------------------------------------- */
AL_Sensors::AL_Sensors( void ){
    // For AmbientLight Sensors
    al_num_sensors          = 0;
    al_num_turns            = 0;
    al_num_horizontal_steps = 0;
    al_step_length          = 0;
    al_min_distance         = 0;
    max_al_reading          = 0;
    load_al_readings( );
}

/* ---------------------------------------------------------------------------------------- */

AL_Sensors::AL_Sensors(const AL_Sensors& other)
{
    copy(other);
}

/* ---------------------------------------------------------------------------------------- */

AL_Sensors::~AL_Sensors( void )
{
   destroy();
}

/* ---------------------------------------------------------------------------------------- */

void AL_Sensors::copy(const AL_Sensors &other)
{
    al_num_sensors = other.al_num_sensors;
    al_num_turns = other.al_num_turns;
    al_num_horizontal_steps = other.al_num_horizontal_steps;
    al_step_length = other.al_step_length;
    al_min_distance = other.al_min_distance;
    al_readings = other.al_readings;
    max_al_reading = other.max_al_reading;
}

/* ---------------------------------------------------------------------------------------- */

void AL_Sensors::destroy()
{
    al_readings.clear();
}

/* ---------------------------------------------------------------------------------------- */

void AL_Sensors::load_al_readings ( void )
{

  std::string absolute_path = (home + local_path).c_str();
  ifstream I ( (absolute_path + "/MISC/light.sam").c_str() );  
  if(!I)
    {
      cerr << "File light.sam for al sensors not found" <<endl;
      exit(EXIT_FAILURE);
    }
  //if( !al_readings_file_name.empty()){
  //    ifstream inFile ( al_readings_file_name );
  string s;
  int count_lines = 0, count_turns = 0, count_horizontal_steps = 0;
  
  while(getline(I, s)){
    istringstream ss(s);
    if(count_lines == 0){
      ss >> al_num_sensors;
      ss >> al_num_turns;
      ss >> al_num_horizontal_steps;
      ss >> al_min_distance;
      ss >> al_step_length;
      
      al_readings.resize(  al_num_horizontal_steps );
      for(int i = 0; i <  al_num_horizontal_steps; i++){
	al_readings[i].resize( al_num_turns );
	for(int j = 0; j < al_num_turns; j++)
	  al_readings[i][j].resize(  al_num_sensors );
      }
      
      count_turns = 0;
      count_horizontal_steps = 0;
    }
    else{
      for(int n = 0; n < al_num_sensors; n++){
	ss >> al_readings[count_horizontal_steps][count_turns][n];
	if( al_readings[count_horizontal_steps][count_turns][n] > max_al_reading )
	  max_al_reading = al_readings[count_horizontal_steps][count_turns][n];
      }
      count_turns++;
      if( count_turns  == al_num_turns ){
	count_turns = 0;
	count_horizontal_steps++;
      }
    }
    count_lines++;
  }
  I.close();
}

/* ---------------------------------------------------------------------------------------- */

void AL_Sensors::agent_light_readings( World_Entity *agent, World_Entity  *light,
                                       vector <double> &readings )
{
  //Compute distance to light
  const vector <double> agent_pos  = agent->get_pos();
  const vector <double> light_pos  = light->get_pos();
  double x_offset = (light_pos[0] - agent_pos[0]);
  double y_offset = (light_pos[1] - agent_pos[1]);
  double light_distance = 10.0 * (sqrt((x_offset*x_offset)+(y_offset*y_offset))-agent->get_dim()[0]);
  int dist_entry;
  
  /*light_distance *= 0.25;*/
  
  //cerr << " IN LIGHT - Dist to target = " << sqrt((x_offset*x_offset) + (y_offset*y_offset)) << endl;
  
  if(light_distance < al_min_distance ) dist_entry = 0;
  else dist_entry = int((light_distance-al_min_distance)/al_step_length);
  
  //cerr << " Light Distance = " << light_distance << " dist_entry = " << dist_entry << endl;
  
  if(dist_entry < al_num_horizontal_steps){
    //Compute relative orientation Theta
    double theta = atan2( y_offset, x_offset);
    if( theta < 0.0 ) theta += TWO_PI;
    
    theta = agent->get_rot()[2] - theta;
    if(theta < 0.0 ) theta += TWO_PI;
    else if(theta > TWO_PI ) theta -= TWO_PI;
    
    int turn_entry = int(  (((theta*180.0)/PI)*0.5) );
    
    //Copy readings
    for(int i = 0; i < al_num_sensors; i++){
      if(al_readings[dist_entry][turn_entry][i] > readings[i] )
	readings[i] = al_readings[dist_entry][turn_entry][i];
    }
  }
}


/* ---------------------------------------------------------------------------------------- */

void AL_Sensors::add_noise( vector <double> &readings )
{
  for(int i = 0; i < al_num_sensors; i++){
    if( readings[i] ){
      readings[i] += ((rint)(gsl_ran_gaussian(GSL_randon_generator::r_rand, 50)));
      if( readings[i] > max_al_reading ) readings[i] = max_al_reading;
      else if ( readings[i] < 0.0 ) readings[i] = 0.0;
      readings[i] /= (double)(max_al_reading);
    }
    else{
      //just background noise
      readings[i] = ((double)(gsl_rng_uniform_int(GSL_randon_generator::r_rand, 21)));
      readings[i] /= (double)(max_al_reading);
    }
  }
}

/* ---------------------------------------------------------------------------------------- */

AL_Sensors& AL_Sensors::operator=(const AL_Sensors &other)
{
  if(this != &other)
    {
      destroy();
      copy(other);
    }
  return *this;
}

/* ---------------------------------------------------------------------------------------- */
