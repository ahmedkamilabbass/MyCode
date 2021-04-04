#include "floor_sensors.h"


/* ---------------------------------------------------------------------------------------- */

FLOOR_Sensors::FLOOR_Sensors(void) {
  num_floor_sensors = 1;
  num_tiles         = 20;
  black_floor_tiles       = new double *[num_tiles];
  white_floor_tiles       = new double *[num_tiles];
  for (int i = 0; i < num_tiles; i++) {
    black_floor_tiles[i] = new double[num_tiles];
    white_floor_tiles[i] = new double[num_tiles];
  }
  upload_floor_type( );
}

/* ---------------------------------------------------------------------------------------- */

void FLOOR_Sensors::upload_floor_type( ){
  
  ifstream white_arena("../Arena/60White.txt");
  if (white_arena.is_open() ) {
    for (int i = 0; i < num_tiles; i++){
      for (int j = 0; j < num_tiles; j++) {
        white_arena >> white_floor_tiles[i][j];
      }
    }
  }
  else
    cout << "ERROR......the white arena image file isn not found!!\n";
  
  ifstream black_arena("../Arena/60Black.txt");
  if (black_arena.is_open() ) {
    for (int i = 0; i < num_tiles; i++){
      for (int j = 0; j < num_tiles; j++) {
        black_arena >> black_floor_tiles[i][j];
      }
    }
  }
  else
    cout << "ERROR......the black arena image file isn not found!!\n";
}

/* ---------------------------------------------------------------------------------------- */


/* ---------------------------------------------------------------------------------------- */

FLOOR_Sensors::FLOOR_Sensors(const FLOOR_Sensors &other) {
    copy(other);
}

/* ---------------------------------------------------------------------------------------- */

FLOOR_Sensors::~FLOOR_Sensors(void) {
    destroy();
}

/* ---------------------------------------------------------------------------------------- */

void FLOOR_Sensors::copy(const FLOOR_Sensors &other) {
    num_floor_sensors = other.num_floor_sensors;
}

/* ---------------------------------------------------------------------------------------- */

void FLOOR_Sensors::destroy() {
    for (int i = 0; i < num_tiles; i++) {
        delete white_floor_tiles[i];
    }
    delete[] white_floor_tiles;

    for (int i = 0; i < num_tiles; i++) {
        delete black_floor_tiles[i];
    }
    delete[] black_floor_tiles;
}

/* ---------------------------------------------------------------------------------------- */

void FLOOR_Sensors::agent_floor_sensor_readings(World_Entity *agent, World_Entity *object, vector<double> &readings, int floor_type) {
  const vector<double> agent_pos = agent->get_pos();
  int x_entry = (int) (floor(agent_pos[0] / object->get_dim()[0] * num_tiles));
  int y_entry = (int) (floor(agent_pos[1] / object->get_dim()[0] * num_tiles));
  
  if(floor_type == WHITE_DOMINANT )
    readings[0] = white_floor_tiles[x_entry][y_entry];
  else
    readings[0] = black_floor_tiles[x_entry][y_entry];
  //cerr << " floor_tiles["<<x_entry<<"]["<<y_entry<<"] = " << floor_tiles[x_entry][y_entry] << endl;
}


/* ---------------------------------------------------------------------------------------- */

void FLOOR_Sensors::add_noise(const vector<double> floor_sensor_readings,
                              vector<double> &inputs, int *count_inputs) {
    //At the moment there is no noise added to the readings
    for (int i = 0; i < floor_sensor_readings.size(); i++) {
        *count_inputs = *count_inputs + 1;
        inputs[*count_inputs] = floor_sensor_readings[i];
    }
}

/* ---------------------------------------------------------------------------------------- */

FLOOR_Sensors &FLOOR_Sensors::operator=(const FLOOR_Sensors &other) {
    if (this != &other) {
        destroy();
        copy(other);
    }
    return *this;
}

/* ---------------------------------------------------------------------------------------- */
