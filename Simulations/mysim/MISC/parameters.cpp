#include "parameters.h"

/* ---------------------------------------- */

void Parameters::usage(void) {
    cerr << " Bad input format - Options are required \n"
         << "-e evolutionary mode, the parameter -n and -s has to be specified  \n "
         << "-v viewing mode, the parameter -n and -s has to be specified  \n "
         << "-r evaluation mode, the parameter -n and -s has to be specified  \n "
         << " -------------------------- \n"
         << "-n requires a evolutionary run name \n "
         << "-s requires a seed number different from zero."
         << endl;
    exit(EXIT_FAILURE);
}

/* ---------------------------------------- */

char *Parameters::getCmdOption(char **begin, char **end, const std::string &option) {
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end) {
        return *itr;
    } else usage();
    return 0;
}

/* ---------------------------------------- */

bool Parameters::cmdOptionExists(char **begin, char **end, const std::string &option) {
    return std::find(begin, end, option) != end;
}

/* ---------------------------------------- */

void Parameters::parse_command_line(int argc, char **argv) {
    

  if( cmdOptionExists(argv, argv+argc, "-e") ||
     cmdOptionExists(argv, argv+argc, "-v") ||
      cmdOptionExists(argv, argv+argc, "-r") ){
    switch(argv[1][1]) {
    case 'e':
      m_mode_evolution  = true;
      m_mode_viewing    = false;
      m_mode_evaluation = false;
     break;
   case 'v':
      m_mode_evolution  = false;
    m_mode_viewing    = true;
      m_mode_evaluation = false;
     break;
    case 'r':
     m_mode_evolution  = false;
      m_mode_viewing    = false;
     m_mode_evaluation = true;
      break;
    default:
     usage();
      break;
    }
   if(cmdOptionExists(argv, argv+argc, "-n")){
      m_runName = getCmdOption(argv, argv + argc, "-n");
      if ( m_runName.empty()  ) usage();
    }
   else usage();
    if(cmdOptionExists(argv, argv+argc, "-s")){
      m_rootSeed = atoi( getCmdOption(argv, argv + argc, "-s") );
     if( m_rootSeed == 0 ) usage();
    }
    else usage();
  }
  else usage();
}

/* ---------------------------------------- */

void Parameters::read_run_parameter_file(void) {

    std::string absolute_path = (home + local_path).c_str();
    ifstream I((absolute_path + "/MISC/init_run.txt").c_str());
    if (!I) {
        cerr << "File for Run Parameters not found" << endl;
        exit(EXIT_FAILURE);
    }

    /* ------------------------------- */
    //Load parameters from init_run.txt
    /* ------------------------------- */
    m_nbMaxGenerations = getInt('=', I);
    m_nbMaxEvaluations = getInt('=', I);
    m_nbMaxIterations = getInt('=', I);

    m_nbGroups = getInt('=', I);
    m_nbAgentsPerGroup = getInt('=', I);
    m_diploidSolutions = getYesNo('=', I);
    m_heteroGroups = getYesNo('=', I);
    m_individualSelection = getYesNo('=', I);
    m_nbEvolvingPopulations = getInt('=', I);

    m_nbFitObjectives = getInt('=', I);
    m_tournamentSize = getInt('=', I);
    m_typeOfDominance.resize(m_nbFitObjectives);
    for (int i = 0; i < m_nbFitObjectives; i++) {
        getStr('=', I, m_typeOfDominance[i]);
    }

    m_nbSolutionsTruncated = getInt('=', I);
    m_Elite = getYesNo('=', I);
    m_probMutation = getDouble('=', I);
    m_probCrossOver = getDouble('=', I);

    m_dumpStatsEvery = getInt('=', I);
    m_dumpGenomeEvery = getInt('=', I);

    std::string stats_str, genome_str, reval_str;

    getStr('=', I, stats_str);
    getStr('=', I, genome_str);

    m_statsFileDir = (absolute_path + "/" + stats_str).c_str();
    m_genomeFileDir = (absolute_path + "/" + genome_str).c_str();

    //getStr('=', I, stats_str );
    //strcpy(m_statsFileDir,absolute_path);
    //strcat(m_statsFileDir,stats_str);

    // getStr('=', I, reval_str );
    // strcpy(revalFileDir,absolute_path);
//  strcat(revalFileDir,reval_str);


//  getStr('=', I, genome_str );
//  strcpy(m_genomeFileDir,absolute_path);
//  strcat(m_genomeFileDir,genome_str);

    getStr('=', I, m_typeOfGA);
    getStr('=', I, m_typeOfController);

    m_simulationTimeStep = getDouble('=', I);

    m_nbBricks = getInt('=', I);
    m_nbCylinders = getInt('=', I);
    m_nbSpheres = getInt('=', I);
    m_nbObjects = m_nbBricks + m_nbCylinders + m_nbSpheres;
    m_nbObjProperties = getInt('=', I);

    if (m_nbObjects) {
        for (int g = 0; g < m_objectsDatas.size(); g++)
            m_objectsDatas[g].erase(m_objectsDatas[g].begin(), m_objectsDatas[g].begin() + m_objectsDatas[g].size());
        m_objectsDatas.erase(m_objectsDatas.begin(), m_objectsDatas.begin() + m_objectsDatas.size());
        m_objectsDatas.resize(m_nbObjects);

        for (int c = 0; c < m_nbObjProperties; c++) {
            for (int b = 0; b < m_nbObjects; b++) {
                m_objectsDatas[b].push_back(getDouble('=', I));
            }
        }
    }
    /* ------------------------------- */
    //End of loading parameters
    /* ------------------------------- */

    if (m_typeOfGA == "emoo") {
        m_Elite = false;
        m_nbSolutionsTruncated = 0;
    }

}

/* ---------------------------------------- */

Parameters::Parameters(int argc, char **argv) {
    parse_command_line(argc, argv);
    read_run_parameter_file(); //Open the first configuration file called init_run.txt

    if (!m_heteroGroups) { //Homogeneous Groups
        m_nb_chromosome_per_group = 1;
    } else {
        m_nb_chromosome_per_group = m_nbAgentsPerGroup;
    }

    if ((m_nbGroups * m_nb_chromosome_per_group) <= m_nbSolutionsTruncated) {
        cerr << "In parameters.cpp : Not enough genotypes : pop size too small \n";
        cerr << " or number of soution truncated too big; " << endl;
        exit(EXIT_FAILURE);
    }
    if (m_nbEvolvingPopulations != 1) {
        cerr << "In parameters.cpp : For time being, you can only have one single evolving population " << endl;
        exit(EXIT_FAILURE);
    }

    m_currentGeneration = 0;

    init_random_generator();
    if (!m_runName.empty() && m_mode_evolution) {
        dump_simulation_seed(); //This is to dump the seed number in dir DATA
    }
}

/* ---------------------------------------- */

Parameters::~Parameters() {
    GSL_randon_generator::free_generator();
}

/* ---------------------------------------- */

void Parameters::init_random_generator(void) {
    GSL_randon_generator::init_generator(m_rootSeed);
}

/* ---------------------------------------- */

void Parameters::dump_simulation_seed(void) {
    //Create the Directory to save seed
    //struct stat st = {0};
    //if( stat (m_statsFileDir.c_str(), &st) == -1 ){
    //mkdir(m_statsFileDir.c_str(), 0700 );
    //}
    std::string fileName = m_statsFileDir + m_runName + ".seed";
    ofstream outfile(fileName, ios::app);
    outfile.setf(ios::fixed);
    outfile << "m_rootSeed = " << m_rootSeed << endl;
    outfile.close();
}

/* ---------------------------------------- */

void Parameters::reset_seed(void) {
    GSL_randon_generator::reset_seed(m_rootSeed);
}

/* ---------------------------------------- */
/* ---------------------------------------- */
