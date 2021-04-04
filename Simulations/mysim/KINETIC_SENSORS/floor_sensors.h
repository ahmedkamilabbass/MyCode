#ifndef _FLOOR_SENSORS_
#define _FLOOR_SENSORS_

#include "../WORLD/world_entity.h"
#include "../WORLD/kinetic_agent.h"
#include "../WORLD/kinetic_objects.h"

#include <istream>

using namespace std;

class FLOOR_Sensors
{
  
 private:
  int num_floor_sensors;
  int num_tiles;
  double **black_floor_tiles;
  double **white_floor_tiles;
  void copy(const FLOOR_Sensors &other);
  void destroy();
  
 public:
  FLOOR_Sensors();
  FLOOR_Sensors(const FLOOR_Sensors& other);
  ~FLOOR_Sensors();
  FLOOR_Sensors& operator=(const FLOOR_Sensors &other);
  
  void agent_floor_sensor_readings ( World_Entity *agent, World_Entity *object, vector <double> &readings, int floor_type );
  void add_noise( const vector <double> floor_sensor_readings, vector <double> &inputs, int *count_inputs );
  void upload_floor_type();
  
  inline int get_num_floor_sensors( void ) { return num_floor_sensors;}
};

#endif

