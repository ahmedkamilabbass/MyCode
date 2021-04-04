#ifndef _IR_SENSORS_
#define _IR_SENSORS_

#include "../MISC/general.h"
#include "../WORLD/world_entity.h"
#include "../WORLD/kinetic_agent.h"
#include "../WORLD/kinetic_objects.h"

#define OFFSET_ANGLE (double)(0.005)
#define OFFSET_DIST (double)(0.0000)

using namespace std;

class IR_Sensors {
  
 private:
  int ir_num_sensors;
  double ir_dist_range;
  static constexpr double max_ir_reading      = 4095.0;
  vector < double > epuck_sensors_angular_pos;
  
  void copy(const IR_Sensors &other);
  void destroy();
  
 public:
  IR_Sensors();
  IR_Sensors(const IR_Sensors& other);
  ~IR_Sensors();
  
  void agent_brick_readings       ( World_Entity *agent, World_Entity *target, vector <double> &readings);
  void agent_round_obj_readings   ( World_Entity *agent, World_Entity *target, vector <double> &readings);
  double agent_agent_readings     ( World_Entity *agent, World_Entity *target,
				    vector <double> &distance_a, vector <double> &distance_t);
  void add_noise                  ( const vector <double> ir_ray_length, vector <double> &inputs, int *count_inputs );
  void render_ir_sensor           ( World_Entity *agent, const vector <double> ir_ray_length );
    
  inline int get_ir_num_sensors   ( void ) { return ir_num_sensors;}
  inline double get_ir_dist_range ( void ) { return ir_dist_range;}
  
  IR_Sensors& operator=(const IR_Sensors &other);
};

#endif

