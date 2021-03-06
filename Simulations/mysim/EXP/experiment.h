#ifndef _EXPERIMENT_
#define _EXPERIMENT_

#include "../MISC/parameters.h"
#include <iterator>

//Evolutionary Algorithms
#include "../EA/population.h"
#include "../EA/roulette_wheel.h"
#include "../EA/tournament.h"
#include "../EA/emoo.h"

// Controllers
#include "../CONTROLLERS/controller.h"
#include "../CONTROLLERS/ctrnn3Layers.h"
#include "../CONTROLLERS/ctrnn3LayersHebb.h"
#include "../CONTROLLERS/ctrnnFullyRecurrent.h"
#include "../CONTROLLERS/perceptron.h"
#include "../CONTROLLERS/simple3Layers.h"
#include "../CONTROLLERS/elman.h"
#include "../CONTROLLERS/pfsm.h"

//World
#include "../WORLD/world_entity.h"
#ifdef _BULLET_WORLD_
#include "../WORLD/simple_agents.h"
#include "../WORLD/simple_objects.h"
#else
#include "../WORLD/kinetic_agent.h"
#include "../WORLD/kinetic_objects.h"
#endif

//Kinetic sensors
#ifndef _BULLET_WORLD_
#include "../KINETIC_SENSORS/ir_sensors.h"
#include "../KINETIC_SENSORS/al_sensors.h"
#include "../KINETIC_SENSORS/linear_camera.h"
#include "../KINETIC_SENSORS/floor_sensors.h"
#endif

class Experiment {
  
 private:
 protected:
  unsigned int m_evaluation;
  double       m_physicsStep;
  static const int NbPhysicsWorldUpdates = 6;
  static const int m_id_population       = 1;
  
  Parameters *m_parameters;
  std::vector< Controller* >     m_networks;
  std::vector< Population* >     m_population;
#ifdef _BULLET_WORLD_
  std::vector< SIMPLE_Agents* >  m_agents;
  std::vector< SIMPLE_Objects*>  m_plane;
  std::vector< SIMPLE_Objects*>  m_objects;
#else
  std::vector< Kinetic_Agents* >  m_agents;
  std::vector< Kinetic_Objects*>  m_objects;
  IR_Sensors* m_irSensor;
  AL_Sensors* m_alSensor;
  Linear_Camera* m_camera;
  FLOOR_Sensors* m_floorSensor;
#endif
  
  unsigned int m_root;
  unsigned int m_NbRepetitions;
  unsigned int m_NbAlleles;
  unsigned int m_NbBasesPerAllele;
  unsigned int m_NbGenesPerInd;
  unsigned int m_count_rep;
  unsigned int m_start;
  unsigned int m_end;
  vector <chromosome_type> m_genes;
  vector <double> m_fitness;
  vector <int> list_of_solutions_to_each_node;
  
  /* ---------------------------------------------- */
  vector < int >         m_pop_one_solutions;
  vector< vector <int> > m_teamTuples;
  vector <double>        m_tmp_get_fitness;
  vector< vector <int> > m_listOfTuplesPerProcess;
  /* ---------------------------------------------- */
  
  unsigned int m_nbActiveAgents;
  //World with Bullet
#ifdef _BULLET_WORLD_
  btCollisionConfiguration*  m_collisionConfig;
  btDispatcher*              m_dispatcher;
  btBroadphaseInterface*     m_broadphase;
  btConstraintSolver*        m_solver;
  btDynamicsWorld*           m_world;
#endif
 public:
  Experiment( Parameters* params );
  virtual ~Experiment();

  virtual void evaluate_solutions           ( void ) = 0;
  virtual void init_evaluations_loop        ( void ) = 0;
  virtual void init_single_evaluation       ( void ) = 0;
  virtual void set_agents_position          ( int  ) = 0;
  virtual void adv                          ( void ) = 0;
  virtual bool stop_evaluations_loop        ( void ) = 0;
  virtual vector <double> get_ir_ray_length ( int r) = 0;
  
  inline unsigned int getEvaluation       ( void ) const { return m_evaluation; }
  
  void initAgentsController                 ( void );
  void initEvolutionaryAlgorithm            ( void );
  //void send_new_population_to_all_processes ( void );
  void send_solutions_to_all_processes      ( void );
  void receive_fitness_from_all_processes   ( void );

  void runOneGeneration                     ( void );

  //Bullet world
#ifdef _BULLET_WORLD_
  void init_physics_param                     ( void );
  void resetPhysicsState                      ( void );
  void init_bullet_agents                     ( void );
  void init_bullet_objects                    ( void );
  void add_agent                              ( SIMPLE_Agents* agent);
  void remove_agent                           ( SIMPLE_Agents *agent);
#endif
  
  void from_genome_to_controllers             ( vector < vector <chromosome_type> > &genes, const int which_genotype );
  void defineHeteroTeamsRandomlyAssembled     ( void );
  void defineHeteroTeamsSequentiallyAssembled ( void );
  void defineHomogeneousTeams                 ( void );
  void draw_arena                             ( void );

  //Non bullet world
#ifndef _BULLET_WORLD_
  void init_kinetic_ir_sensor                 ( void );
  void init_kinetic_al_sensor                 ( void );
  void init_kinetic_camera                    ( void );
  void init_kinetic_floor_sensor              ( void );
  void init_kinetic_agents                    ( void );
  void init_kinetic_objects                   ( void );
  inline IR_Sensors*                     getIrSensor()     { return m_irSensor;}
  inline Linear_Camera*                  getCamera() const { return m_camera; }
#endif
  
  inline std::vector< Controller* >     getNetworks ()   { return m_networks;}
  inline std::vector< Population* >     getPopulation()  { return m_population;}
  
#ifdef _BULLET_WORLD_
  inline std::vector< SIMPLE_Agents* >  getAgents()      { return m_agents;}
  inline std::vector< SIMPLE_Objects*>  getPlane()       { return m_plane;}
  inline std::vector< SIMPLE_Objects*>  getObjects()     { return m_objects;}
#else
  inline std::vector< Kinetic_Agents* >  getAgents()       { return m_agents;}
  inline std::vector< Kinetic_Objects*>  getObjects()      { return m_objects;}
  
  
#endif
  
};
#endif
