#include "experiment.h"

/* ----------------------------------------------------------------------------------- */

Experiment::Experiment(Parameters *params) {

    m_root = 0;
    m_parameters = params;

    if (m_parameters->getNbGroups() % NbTotProcesses) {
        if (NbRank == m_root) {
            cout << " In Experiment.cpp constructor "
                 << "\n This is the total number of genotypes required by your setup : " << m_parameters->getNbGroups()
                 << " \n This is the number of computer node available : " << NbTotProcesses
                 << " \n Remember that (total number of genotypes  % nb of computer node available) = 0 "
                 << endl;
        }
        exit(EXIT_FAILURE);
    }
    m_NbRepetitions = (int) (ceil(m_parameters->getNbGroups() / NbTotProcesses));

    //Init Agents Controllers here
    initAgentsController();

    //Init Agents' World
#ifdef _BULLET_WORLD_
    init_physics_param();
    init_bullet_objects();
    init_bullet_agents();
    m_physicsStep = m_simulationTimeStep/(double)(NbPhysicsWorldUpdates);
    //m_parameters->getSimulationTimeStep()/(double)(NbPhysicsWorldUpdates);
#else
    init_kinetic_objects();
    init_kinetic_agents();
#endif

    //The number of alleles or genes per genotype is determined by the number of network parameters
    m_NbAlleles = m_networks[0]->get_genotype_length();
    m_NbBasesPerAllele = 1; //This should be set to 1
    m_NbGenesPerInd = m_NbAlleles * m_NbBasesPerAllele;

    //m_fitness is the vector in which each processor stores the fitness of the group or the individuals currently under evaluation;
    m_fitness.erase(m_fitness.begin(), m_fitness.begin() + m_fitness.size());
    m_fitness.clear();
    if (m_parameters->IsSelectionIndividual())
        m_fitness.assign(m_parameters->getNbAgentsPerGroup() * m_parameters->getNbFitObjectives(), 0.0);
    else
        m_fitness.assign(m_parameters->getNbFitObjectives(), 0.0);

    //m_genes is the vector in which each core stores all the genes (i.e., total number of genotyeps * genotype lenght);
    m_genes.erase(m_genes.begin(), m_genes.begin() + m_genes.size());
    m_genes.clear();
    m_genes.assign((m_NbGenesPerInd * m_parameters->get_nb_chromosome_per_group()), 0.0);

    if (m_parameters->isModeEvolution()) {
        //init GAs only if we are runnig the simulator in evolutionary mode
        if (NbRank == m_root) {
            initEvolutionaryAlgorithm();
        }
    }
    /* cerr << " fit size = " << m_fitness.size()
         << " genes size = " << m_genes.size()
         << " nb chromo = " << m_parameters->get_nb_chromosome_per_group() << endl;*/
}

/* ----------------------------------------------------------------------------------- */

Experiment::~Experiment(void) {
    // Delete network objects
    m_networks.erase(m_networks.begin(), m_networks.begin() + m_networks.size());
    m_networks.clear();

    // Delete agents
    m_agents.erase(m_agents.begin(), m_agents.begin() + m_agents.size());
    m_agents.clear();

    // Delete objects
    m_objects.erase(m_objects.begin(), m_objects.begin() + m_objects.size());
    m_objects.clear();

    // Delete plane
#ifdef _BULLET_WORLD_
    for(int n = 0; n < m_plane.size() ; n++)
      delete m_plane[n];
    m_plane.clear();
#endif

    if (NbRank == m_root) {
        m_population.erase(m_population.begin(), m_population.begin() + m_population.size());
        m_population.clear();
    }
#ifdef _BULLET_WORLD_
    delete m_collisionConfig;
    delete m_dispatcher;
    delete m_broadphase;
    delete m_solver;
    delete m_world;
#endif

#ifndef _BULLET_WORLD_
    if (m_irSensor != NULL) delete m_irSensor;
    if (m_alSensor != NULL) delete m_alSensor;
    if (m_camera != NULL) delete m_camera;
    if (m_floorSensor != NULL) delete m_floorSensor;
#endif
}

/* ----------------------------------------------------------------------------------- */

void Experiment::initAgentsController(void) {
    std::string typeOfController = m_parameters->getTypeOfController();

    if (typeOfController == "ctrnn_three_layers") {
        for (int r = 0; r < m_parameters->getNbAgentsPerGroup(); r++)
            m_networks.push_back(new Ctrnn3Layers());
    } else if (typeOfController == "ctrnn_three_layers_hebb") {
        for (int r = 0; r < m_parameters->getNbAgentsPerGroup(); r++)
            m_networks.push_back(new Ctrnn3LayersHebb());
    } else if (typeOfController == "simple_three_layers") {
        for (int r = 0; r < m_parameters->getNbAgentsPerGroup(); r++)
            m_networks.push_back(new Simple3Layers());
    } else if (typeOfController == "perceptron") {
        for (int r = 0; r < m_parameters->getNbAgentsPerGroup(); r++)
            m_networks.push_back(new Perceptron());
    } else if (typeOfController == "elman") {
        for (int r = 0; r < m_parameters->getNbAgentsPerGroup(); r++)
            m_networks.push_back(new Elman());
    } else if (typeOfController == "ctrnn_fully_recurrent") {
        for (int r = 0; r < m_parameters->getNbAgentsPerGroup(); r++)
            m_networks.push_back(new CtrnnFullyRecurrent());
    } else if (typeOfController == "pfsm")//Prababilistic finite state machine
    {
        for (int r = 0; r < m_parameters->getNbAgentsPerGroup(); r++)
            m_networks.push_back(new Pfsm(r));
    } else {
        cerr << "In Experiment::initControllers - Controller type not found " << endl;
        exit(EXIT_FAILURE);
    }
}

/* ----------------------------------------------------------------------------------- */

void Experiment::initEvolutionaryAlgorithm(void) {
    std::string typeOfGA = m_parameters->getTypeOfGA();

    if (typeOfGA == "roulette_wheel") {
        for (int r = 0; r < m_parameters->getNbEvolvingPopulations(); r++)
            m_population.push_back(new Roulette_wheel(r, m_parameters, m_NbAlleles, m_NbBasesPerAllele));
    } else if (typeOfGA == "emoo") {
        for (int r = 0; r < m_parameters->getNbEvolvingPopulations(); r++)
            m_population.push_back(new Emoo(r, m_parameters, m_NbAlleles, m_NbBasesPerAllele));
    } else if (typeOfGA == "tournament") {
        for (int r = 0; r < m_parameters->getNbEvolvingPopulations(); r++)
            m_population.push_back(new Tournament(r, m_parameters, m_NbAlleles, m_NbBasesPerAllele));
    } else {
        cerr << "In Experiment::initEvolutionaryAlgorithm - GAs type not found " << endl;
        exit(EXIT_FAILURE);
    }
}

/* ----------------------------------------------------------------------------------- */
#ifdef _BULLET_WORLD_
void Experiment::resetPhysicsState( void )
{
  int numObjects = 0;
  int i;
  
  if (m_world)
    {
      for (i=0; i<m_world->getNumConstraints(); i++)
        {
      m_world->getConstraint(0)->setEnabled(true);
        }
      numObjects = m_world->getNumCollisionObjects();

      ///create a copy of the array, not a reference!
      btCollisionObjectArray copyArray = m_world->getCollisionObjectArray();
      for (i=0;i<numObjects;i++)
        {
      btCollisionObject* colObj = copyArray[i];
      btRigidBody* body = btRigidBody::upcast(colObj);
      if (body)
            {
          if (body->getMotionState())
                {
          btDefaultMotionState* myMotionState = (btDefaultMotionState*)body->getMotionState();
          myMotionState->m_graphicsWorldTrans = myMotionState->m_startWorldTrans;
          body->setCenterOfMassTransform( myMotionState->m_graphicsWorldTrans );
          colObj->setInterpolationWorldTransform( myMotionState->m_startWorldTrans );
          colObj->forceActivationState(ACTIVE_TAG);
          colObj->activate();
          colObj->setDeactivationTime(0);
          //colObj->setActivationState(WANTS_DEACTIVATION);
                }
          //removed cached contact points (this is not necessary if all objects have been removed from the dynamics world)
          if (m_world->getBroadphase()->getOverlappingPairCache())
        m_world->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(colObj->getBroadphaseHandle(), m_dispatcher);

          btRigidBody* body = btRigidBody::upcast(colObj);
          if (body && !body->isStaticObject())
                {
          btRigidBody::upcast(colObj)->setLinearVelocity(btVector3(0,0,0));
          btRigidBody::upcast(colObj)->setAngularVelocity(btVector3(0,0,0));
                }
            }
        }
      
      //reset some internal cached data in the broadphase
      m_world->getBroadphase()->resetPool(m_dispatcher);
      m_world->getConstraintSolver()->reset();
    }
}
#endif

/* ------------------------------------------------------------------------ */

#ifdef _BULLET_WORLD_
bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2);
#endif

/* ------------------------------------------------------------------------ */

#ifdef _BULLET_WORLD_
bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2)
{
  //cerr << " callback " << endl;
  // World_Entity* object1 = (World_Entity*)obj1->getCollisionObject()->getUserPointer();
  // World_Entity* object2 = (World_Entity*)obj2->getCollisionObject()->getUserPointer();

  // if( object1->get_type_id() == ROBOT ){
  //   object1->collision_memory.push_back( object1->MakeIndexWithId ( object2->get_index(), object2->get_type_id() ) );
  // }

  // if( object2->get_type_id() == ROBOT ){
  //   object2->collision_memory.push_back( object2->MakeIndexWithId( object1->get_index(), object1->get_type_id() ) );
  // }
//   if(object1->get_type_id() != PLANE && object2->get_type_id() != PLANE ){  
//     object1->collision_objects[object1->collision_counter] = new int[2];
//     object1->collision_objects[object1->collision_counter][0] = object2->get_index();
//     object1->collision_objects[object1->collision_counter][1] = object2->get_type_id();
//     object1->collision_counter++;
    
//     object2->collision_objects[object2->collision_counter] = new int[2];
//     object2->collision_objects[object2->collision_counter][0] = object1->get_index();
//     object2->collision_objects[object2->collision_counter][1] = object1->get_type_id();
//     object2->collision_counter++;
//   }
  return false;
}
#endif

/* ----------------------------------------------------------------------------------- */

#ifdef _BULLET_WORLD_
void Experiment::init_physics_param( void ){
  this->m_collisionConfig = new btDefaultCollisionConfiguration();
  this->m_dispatcher      = new btCollisionDispatcher(m_collisionConfig);
  btVector3 worldMin(-5,-1,-5);
  btVector3 worldMax(5,1,5);
  this->m_broadphase      = new btAxisSweep3(worldMin,worldMax);
  this->m_solver          = new btSequentialImpulseConstraintSolver();
  this->m_world           = new btDiscreteDynamicsWorld(m_dispatcher,m_broadphase,m_solver,m_collisionConfig);
  this->m_world->setGravity(btVector3(0.0,-9.81,0.0));  //gravity on Earth
  this->m_world->getSolverInfo().m_numIterations = 110;
  m_world->getSolverInfo().m_solverMode |= SOLVER_USE_2_FRICTION_DIRECTIONS  | SOLVER_SIMD;// | SOLVER_RANDMIZE_ORDER;
  //gContactAddedCallback = callbackFunc; //Comment this function in case you do not need to record collisions.
}
#endif

/* ----------------------------------------------------------------------------------- */

#ifdef _BULLET_WORLD_
void Experiment::init_bullet_agents( void ){
  vector <double> colour;
  colour.assign(3, 1.0);
  m_nbActiveAgents = 0;
  for (int r = 0; r < m_parameters->getNbAgentsPerGroup(); r++){
    m_agents.push_back( new SIMPLE_Agents ( r, m_simulationTimeStep /* m_parameters->getSimulationTimeStep()*/, m_world) );
    m_agents.back()->body->setUserPointer( m_agents.back() );
    m_agents.back()->left_wheel->setUserPointer( m_agents.back() );
    m_agents.back()->right_wheel->setUserPointer( m_agents.back() );    
    //m_agents.back()->set_crashed(false);
    colour[0] = 1.0;
    colour[1] = 0.0;
    colour[2] = 0.0;
    m_agents.back()->set_colour( colour );
    m_nbActiveAgents++;
  }
}
#endif

/* ----------------------------------------------------------------------------------- */

#ifdef _BULLET_WORLD_
void Experiment::add_agent(SIMPLE_Agents* agent){
  vector <double> pos;
  pos.assign(3,0.0);
  vector <double> rot;
  rot.assign(3,0.0);
  pos[0] = 4.80;
  pos[2] = 4.80;
  agent->set_robot_pos_rot( pos, rot );
  agent->right_hinge->setLimit(1,-1,1.0,0.3,1);
  agent->left_hinge->setLimit(1,-1,1.0,0.3,1);
  agent->body->setActivationState(1);
  agent->right_wheel->setActivationState(1);
  agent->left_wheel->setActivationState(1);
  agent->set_removed(false);
  m_nbActiveAgents++;
}
#endif

/* ----------------------------------------------------------------------------------- */

#ifdef _BULLET_WORLD_
void Experiment::remove_agent(SIMPLE_Agents *agent){
  vector <double> pos;
  pos.assign(3,0.0);
  vector <double> rot;
  rot.assign(3,0.0);
  pos[0] = 5.8;
  pos[2] = 5.8;
  agent->set_robot_pos_rot( pos, rot );
  agent->right_hinge->setLimit(0.0, 0.0);
  agent->left_hinge->setLimit(0.0, 0.0);
  agent->body->setActivationState(0);
  agent->right_wheel->setActivationState(0);
  agent->left_wheel->setActivationState(0);
  agent->set_removed(true);
  m_nbActiveAgents--;
}
#endif

/* ----------------------------------------------------------------------------------- */

/* void Experiment::draw_arena ( void ){
  double radius =  (2.0 * m_parameters->getObjectsDatas()[0][3])/(sqrt(6)-sqrt(2));
  double apotema = (radius/4.0) * (sqrt(6)+sqrt(2));
  double x_pos = apotema;
  double z_pos = 0.0;
  double angle = PI/6;
  double xz_rot = PI/2;  
  
  for(int ob = 0; ob < 12; ob++){
    if (ob < 12 ) {
#ifdef _BULLET_WORLD_
      m_parameters->setObjectsDatas (0, 0, x_pos );
      m_parameters->setObjectsDatas (0, 2, z_pos );
      m_parameters->setObjectsDatas (0, 6, xz_rot );

      m_objects.push_back( new SIMPLE_Brick( ob, m_parameters->getObjectsDatas()[0], m_world ) );
      m_objects.back()->body[0]->setUserPointer( m_objects.back() );
      m_objects.back()->set_crashed(false);
      x_pos = apotema * cos(angle*(ob+1));
      z_pos = apotema * sin(angle*(ob+1));
      xz_rot += -PI/6;
#else
      m_parameters->setObjectsDatas (0, 0, x_pos );
      m_parameters->setObjectsDatas (0, 1, z_pos );
      m_parameters->setObjectsDatas (0, 6, xz_rot );
      m_objects.push_back( new Kinetic_Brick( m_parameters->getObjectsDatas()[0] ) );
      x_pos = apotema * cos(angle*(ob+1));
      z_pos = apotema * sin(angle*(ob+1));
      xz_rot += PI/6;
#endif
      

    }

    
    // else if(ob == 12){
    //   m_parameters->setObjectsDatas (0, 0, 0 ); // x pos
    //   m_parameters->setObjectsDatas (0, 2, 0 ); // z pos
    //   m_parameters->setObjectsDatas (0, 6, 0 ); // xz plane rot
    //   m_parameters->setObjectsDatas (0, 3, 2.1 ); //lenght
      
    //   m_objects.push_back( new SIMPLE_Brick( ob, m_parameters->getObjectsDatas()[0], m_world ) );
    //   m_objects.back()->body[0]->setUserPointer( m_objects.back() );
    //   m_objects.back()->set_crashed(false);
    // }
    // else if(ob == 13){
    //   m_parameters->setObjectsDatas (0, 0, 0 ); // x pos
    //   m_parameters->setObjectsDatas (0, 2, 0.6 ); // z pos
    //   m_parameters->setObjectsDatas (0, 6, PI/2 ); // xz plane rot
    //   m_parameters->setObjectsDatas (0, 3, 1.02 ); //lenght
      
    //   m_objects.push_back( new SIMPLE_Brick( ob, m_parameters->getObjectsDatas()[0], m_world ) );
    //   m_objects.back()->body[0]->setUserPointer( m_objects.back() );
    //   m_objects.back()->set_crashed(false);
    // }
    // else if(ob == 14){
    //   m_parameters->setObjectsDatas (0, 0, 0 ); // x pos
    //   m_parameters->setObjectsDatas (0, 2, -0.6 ); // z pos
    //   m_parameters->setObjectsDatas (0, 6, PI/2 ); // xz plane rot
    //   m_parameters->setObjectsDatas (0, 3, 1.02 ); //lenght
      
    //   m_objects.push_back( new SIMPLE_Brick( ob, m_parameters->getObjectsDatas()[0], m_world ) );
    //   m_objects.back()->body[0]->setUserPointer( m_objects.back() );
    //   m_objects.back()->set_crashed(false);
    // }
  }
  }*/

/* ----------------------------------------------------------------------------------- */

#ifdef _BULLET_WORLD_
void Experiment::init_bullet_objects( void ){
  
  m_plane.push_back( new SIMPLE_Plane( m_world ) );
  m_plane[0]->body[0]->setUserPointer( m_plane[0] );
  
  if( m_parameters->getNbObjects() )
    {
      //draw_arena(); //please leave this function here for the time being. It draws a kind of circular arena
     
      unsigned int count = 0, ob = 0;
      while( count < m_parameters->getNbBricks() && ob < m_parameters->getNbObjects() )
        {
          m_objects.push_back( new SIMPLE_Brick( ob, m_parameters->getObjectsDatas()[ob], m_world ) );
          //body->setUserPointer(bodies[bodies.size()-1]);
          m_objects.back()->body[0]->setUserPointer( m_objects.back() );
          m_objects.back()->body[1]->setUserPointer( m_objects.back() );
          m_objects.back()->body[2]->setUserPointer( m_objects.back() );
          //m_objects.back()->set_crashed(false);
          ob++;
          count++;
        }
      count = 0;
      while( count < m_parameters->getNbCylinders() && ob < m_parameters->getNbObjects() )
        {
          m_objects.push_back( new SIMPLE_Cylinder ( ob, m_parameters->getObjectsDatas()[ob], m_world ) );
          m_objects.back()->body[0]->setUserPointer(m_objects.back() );
          //m_objects.back()->set_crashed(false);
          ob++;
          count++;
        }
      count = 0;
      while( count < m_parameters->getNbSpheres() && ob < m_parameters->getNbObjects() )
        {
          m_objects.push_back( new SIMPLE_Sphere ( ob, m_parameters->getObjectsDatas()[ob], m_world) );
          m_objects.back()->body[0]->setUserPointer( m_objects.back() );
          //m_objects.back()->set_crashed(false);
          ob++;
          count++;
    }
    } 
}
#endif

/* ----------------------------------------------------------------------------------- */

#ifndef _BULLET_WORLD_

void Experiment::init_kinetic_objects() {
    if (m_parameters->getNbObjects()) {
        //draw_arena(); //please leave this function here for the time being. It draws a kind of circular arena

        unsigned int count = 0, ob = 0;
        while (count < m_parameters->getNbBricks() && ob < m_parameters->getNbObjects()) {
            m_objects.push_back(new Kinetic_Brick(m_parameters->getObjectsDatas()[ob]));
            ob++;
            count++;
        }
        count = 0;
        while (count < m_parameters->getNbCylinders() && ob < m_parameters->getNbObjects()) {
            m_objects.push_back(new Kinetic_Cylinder(m_parameters->getObjectsDatas()[ob]));
            ob++;
            count++;
        }
        count = 0;
        while (count < m_parameters->getNbSpheres() && ob < m_parameters->getNbObjects()) {
            m_objects.push_back(new Kinetic_Sphere(m_parameters->getObjectsDatas()[ob]));
            ob++;
            count++;
        }
        count = 0;
    }
}

#endif

/* ----------------------------------------------------------------------------------- */

#ifndef _BULLET_WORLD_

void Experiment::init_kinetic_agents() {
    vector<double> colour;
    colour.assign(3, 0.0);
    m_nbActiveAgents = 0;
    int blackAgentCount = 0;
    int whiteAgentCount = 0;
    for (int r = 0; r < m_parameters->getNbAgentsPerGroup(); r++) {
        m_agents.push_back(new Kinetic_Agents( /*m_parameters->getSimulationTimeStep()*/ ));
        bool flag = true;
        while (flag) {
            int x = round(gsl_rng_uniform_pos(GSL_randon_generator::r_rand));
            if (x && whiteAgentCount < m_parameters->getNbAgentsPerGroup() / 2) {
                colour.assign(3, 1.0);
                whiteAgentCount++;
                flag = false;
            } else if (!x) {
                flag = false;
                blackAgentCount++;
                colour.assign(3, 0.0);
            }
        }

//    if( r == 0 ) {
//      colour[0] = 0.0;
//      colour[1] = 1.0;
//      colour[2] = 0.0;
//    }

        m_agents.back()->set_colour(colour);
        m_nbActiveAgents++;
    }
    init_kinetic_ir_sensor();
    init_kinetic_al_sensor();
    init_kinetic_camera();
    init_kinetic_floor_sensor();
}

#endif

/* ---------------------------------------------------------------------------------------- */

#ifndef _BULLET_WORLD_

void Experiment::init_kinetic_ir_sensor(void) {
    m_irSensor = new IR_Sensors();
}

#endif

/* ---------------------------------------------------------------------------------------- */

#ifndef _BULLET_WORLD_

void Experiment::init_kinetic_al_sensor(void) {
    //m_alSensor = new AL_Sensors();
    m_alSensor = NULL;
}

#endif

/* ---------------------------------------------------------------------------------------- */

#ifndef _BULLET_WORLD_

void Experiment::init_kinetic_floor_sensor(void) {
    m_floorSensor = new FLOOR_Sensors();
}

#endif

/* ---------------------------------------------------------------------------------------- */

#ifndef _BULLET_WORLD_

void Experiment::init_kinetic_camera(void) {
    //depth, num_pixels, pixel dimension, m_nbAgents, m_nbObjects
    m_camera = new Linear_Camera(0.1, 3, (PI / 18.0) * 2.0,
                                 m_parameters->getNbAgentsPerGroup(),
                                 m_parameters->getNbObjects());
}

#endif

/* ----------------------------------------------------------------------------------- */

void Experiment::from_genome_to_controllers(vector<vector<chromosome_type> > &genes, const int which_group) {
    if (m_parameters->AreGroupsHeteroGroups()) {//All hetero
        for (int ind = 0; ind < m_parameters->getNbAgentsPerGroup(); ind++) {
            m_networks[ind]->set_genotype_length(genes[ind].size());
            m_networks[ind]->init(genes[ind]);
        }
    } else {//Homogeneous group selection
        for (int ind = 0; ind < m_parameters->getNbAgentsPerGroup(); ind++) {
            m_networks[ind]->set_genotype_length(genes[which_group].size());
            m_networks[ind]->init(genes[which_group]);
        }
    }
    // cout<<endl;
    // for(int i = 0; i < genes[which_group].size(); i++)
    // cout<<genes[which_group][i]<<" ";
}


/* ----------------------------------------------------------------------------------- */

void Experiment::send_solutions_to_all_processes(void) {

    fill(m_genes.begin(), m_genes.end(), 0.0);

    //All processes must stop here and wait for the others
    MPI_Barrier(MPI_COMM_WORLD);

    if (NbRank == m_root) {
        vector<chromosome_type> tmp_genes;

        for (int i = m_start; i < m_end; i++) {
            int toRank = i % NbTotProcesses;
            tmp_genes.erase(tmp_genes.begin(), tmp_genes.begin() + tmp_genes.size());
            tmp_genes.clear();
            int counter = 0;
            for (int ind = 0; ind < m_parameters->get_nb_chromosome_per_group(); ind++) {
                for (int a = 0; a < m_NbGenesPerInd; a++) {
                    tmp_genes.push_back(m_population[0]->get_solution(
                            (i * m_parameters->get_nb_chromosome_per_group()) + counter)[a]);
                    //cerr << " i = " << i << " ind = " << ind << " " << tmp_genes.back();
                }
                counter++;
                //cerr << endl;
            }

            //Do all the sending
            if (toRank == m_root) {
                for (int j = 0; j < tmp_genes.size(); j++)
                    m_genes[j] = tmp_genes[j];
            } else {
#ifdef DOUBLE_GENES_TYPE
                MPI_Send(&tmp_genes[0], tmp_genes.size(), MPI_DOUBLE, toRank, 0, MPI_COMM_WORLD);
#endif
#ifdef INT_GENES_TYPE
                MPI_Send(&tmp_genes[0], tmp_genes.size(), MPI_INT, toRank, 0, MPI_COMM_WORLD);
#endif
            }
        }
    } else {
#ifdef DOUBLE_GENES_TYPE
        MPI_Recv(&m_genes[0], m_genes.size(), MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
#endif
#ifdef INT_GENES_TYPE
        MPI_Recv(&m_genes[0], m_genes.size(), MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
#endif
    }
}

/* ----------------------------------------------------------------------------------- */

void Experiment::receive_fitness_from_all_processes() {
    //Stop and wait for everyone to finalise the evalution
    MPI_Barrier(MPI_COMM_WORLD);
    if (NbRank == m_root) {

        m_tmp_get_fitness.erase(m_tmp_get_fitness.begin(), m_tmp_get_fitness.begin() + m_tmp_get_fitness.size());
        m_tmp_get_fitness.clear();
        m_tmp_get_fitness.resize(m_fitness.size() * NbTotProcesses);
        MPI_Gather(&m_fitness[0], m_fitness.size(), MPI_DOUBLE, &m_tmp_get_fitness[0], m_fitness.size(), MPI_DOUBLE, 0,
                   MPI_COMM_WORLD);

        int index = 0;
        for (int i = m_start; i < m_end; i++) {//These are the number of processors incrementally numbered
            int fitness_values_per_group = 1;
            if (m_parameters->IsSelectionIndividual())
                fitness_values_per_group = m_parameters->getNbAgentsPerGroup();
            for (int ind = 0; ind < fitness_values_per_group; ind++) {
                for (int obj = 0; obj < m_parameters->getNbFitObjectives(); obj++) {
                    m_population[0]->addAtStoreFitness((i * fitness_values_per_group) + ind, obj,
                                                       m_tmp_get_fitness[index]);
                    /*cerr << " i = " << i << " ind = " << ind << " fitsize = " << m_fitness.size()
                         << " n_group = " << (i*m_parameters->get_nb_chromosome_per_group())+ind
                         << " obj = " << obj
                         << " index = " << index
                         << " tmp_fit = " << m_tmp_get_fitness[index]
                         << " NbTotProcesses = " << NbTotProcesses
                         << endl;
                         getchar();*/
                    index++;
                }
            }
        }
    } else {
        //Each process send to process rank 0 the fitness
        MPI_Gather(&m_fitness[0], m_fitness.size(), MPI_DOUBLE, NULL, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
}

/* ------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------- */

void Experiment::runOneGeneration(void) {
    m_count_rep = 0;
    m_start = 0;
    m_end = m_start + NbTotProcesses;

    do {
        send_solutions_to_all_processes();
        evaluate_solutions();
        receive_fitness_from_all_processes();
        m_start = m_end;
        m_end += NbTotProcesses;
        m_count_rep++;
    } while (m_count_rep < m_NbRepetitions);

    if (NbRank == m_root) {
        for (int i = 0; i < m_population.size(); i++) {
            m_population[i]->assign_fitness();
            m_population[i]->dumpStatsGenome();
            m_population[i]->breeding();
            m_population[i]->resetStoreFitness();
        }
    }
}

/* ------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------------------------------- */
