#ifndef _PARAMETERS_
#define _PARAMETERS_

#include "general.h"
#include <algorithm>

class Parameters
{
  
 private:
  
  unsigned int  m_currentGeneration;
  unsigned int  m_nb_chromosome_per_group;
  
  /* parameters read from file init_run.txt */
  unsigned int  m_nbMaxGenerations;
  unsigned int  m_nbMaxEvaluations;
  unsigned int  m_nbMaxIterations;

  unsigned int  m_nbGroups;
  unsigned int  m_nbAgentsPerGroup;
  bool          m_diploidSolutions;
  bool          m_heteroGroups;
  bool          m_individualSelection;
  unsigned int  m_nbEvolvingPopulations;
  unsigned int  m_nbFitObjectives;
  unsigned int  m_tournamentSize;
  vector <string> m_typeOfDominance;
  
  unsigned int  m_nbSolutionsTruncated;
  unsigned int  m_Elite;
  double        m_probMutation;
  double        m_probCrossOver;

  unsigned int  m_dumpStatsEvery;
  unsigned int  m_dumpGenomeEvery;
  std::string   m_statsFileDir;
  std::string   m_genomeFileDir;
  std::string   m_typeOfGA;
  std::string   m_typeOfController;

  //double m_simulationTimeStep;
  int    m_nbBricks;
  int    m_nbCylinders;
  int    m_nbSpheres;
  int    m_nbObjects;
  int    m_nbObjProperties;
  vector < vector <double> > m_objectsDatas;  
  /* ---------------------------------- */

  /* parameters from command line at run time*/
  unsigned long m_rootSeed;
  bool m_mode_evolution;
  bool m_mode_viewing;
  bool m_mode_evaluation;
  std::string   m_runName;
  /* ---------------------------------- */
  
 public:
  Parameters                                ( int argc, char** argv );
  ~Parameters                               ( void );
  void  usage                               ( void );
  void  parse_command_line                  ( int argc, char** argv );
  void  read_run_parameter_file             ( void );
  void  init_random_generator               ( void );
  void  reset_seed                          ( void );
  void  dump_simulation_seed                ( void );
  char* getCmdOption                        (char ** begin, char ** end, const std::string & option);
  bool  cmdOptionExists                     (char** begin, char** end, const std::string& option);
    
  // Accessors
  inline void increment_current_generation          ( void ) { m_currentGeneration++; }
  inline void setRunName                            ( char* runName )    { m_runName = runName; }
  inline void setCurrentGeneration                  ( const int v )      { m_currentGeneration = v; } 
  inline void setObjectsDatas                       ( int index1, int index2, double value ) {m_objectsDatas[index1][index2] = value; }
  
  inline bool isModeEvolution                       () const { return m_mode_evolution;}
  inline bool isModeViewing                         () const { return m_mode_viewing;}
  inline bool isModeEvaluation                      () const { return m_mode_evaluation;}
  inline unsigned int  getCurrentGeneration         () const { return m_currentGeneration; } 
  inline unsigned long getRootSeed                  () const { return m_rootSeed; }
  inline unsigned int  get_nb_chromosome_per_group  () const { return m_nb_chromosome_per_group;}
  inline unsigned int  getNbGroups                  () const { return m_nbGroups; }
  inline unsigned int  getNbAgentsPerGroup          () const { return m_nbAgentsPerGroup; }
  inline unsigned int  getNbMaxGenerations          () const { return m_nbMaxGenerations; }
  inline unsigned int  getNbMaxEvaluations          () const { return m_nbMaxEvaluations; }
  inline unsigned int  getNbMaxIterations           () const { return m_nbMaxIterations; }
  inline std::string   getRunName                   () const { return m_runName; }
  inline bool          AreSolutionDiploid           () const { return m_diploidSolutions; }
  inline bool          AreGroupsHeteroGroups        () const { return m_heteroGroups; }
  inline bool          IsSelectionIndividual        () const { return m_individualSelection;}
  inline unsigned int  getNbEvolvingPopulations     () const { return m_nbEvolvingPopulations;}
  inline unsigned int  getNbFitObjectives           () const { return m_nbFitObjectives;}
  inline unsigned int  getTournamentSize            () const { return m_tournamentSize; }
  inline vector <string> getTypeOfDominance         () const { return m_typeOfDominance;}
  inline unsigned int  getNbSolutionsTruncated      () const { return m_nbSolutionsTruncated;}
  inline bool          getElite                     () const { return m_Elite;}
  inline double        getProbMutation              () const { return m_probMutation;}
  inline double        getProbCrossOver             () const { return m_probCrossOver;}
  inline unsigned int  getDumpStatsEvery            () const { return m_dumpStatsEvery;}
  inline unsigned int  getDumpGenomeEvery           () const { return m_dumpGenomeEvery;}
  inline std::string   getStatsFileDir              () const { return m_statsFileDir;}
  inline std::string   getGenomeFileDir             () const { return m_genomeFileDir;}
  inline std::string   getTypeOfGA                  () const { return m_typeOfGA;}
  inline std::string   getTypeOfController          () const { return m_typeOfController;}
  //inline double        getSimulationTimeStep        () const { return m_simulationTimeStep;}
  inline int           getNbBricks                  () const { return m_nbBricks;}
  inline int           getNbCylinders               () const { return m_nbCylinders;}
  inline int           getNbSpheres                 () const { return m_nbSpheres;}
  inline int           getNbObjects                 () const { return m_nbObjects;}
  inline int           getNbObjProperties           () const { return m_nbObjProperties;}
  inline vector < vector <double> > getObjectsDatas () const { return m_objectsDatas;}

  
};
#endif
