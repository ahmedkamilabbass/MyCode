#ifndef _AL_SENSORS_
#define _AL_SENSORS_

#include <iomanip>
#include <fstream>
#include <sstream>

#include "../WORLD/world_entity.h"
#include "../WORLD/kinetic_agent.h"
#include "../WORLD/kinetic_objects.h"

using namespace std;

class AL_Sensors
{

private:
    int al_num_sensors;
    int al_num_turns;
    int al_num_horizontal_steps;
    int al_step_length;
    int al_min_distance;
    vector <vector <vector <double> > > al_readings;
    int max_al_reading;

    void copy(const AL_Sensors &other);
    void destroy();

public:
    AL_Sensors();
    AL_Sensors(const AL_Sensors& other);
    ~AL_Sensors();

    void load_al_readings     ( void );
    void agent_light_readings ( World_Entity *agent, World_Entity *light_pos, vector <double> &readings );
    void add_noise( vector <double> &readings );
    inline int    get_al_num_sensors( void ) { return al_num_sensors;}

    AL_Sensors& operator=(const AL_Sensors &other);
};

#endif

