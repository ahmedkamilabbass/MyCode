#include "population.h"

/* -------------------------------------------------------------- */

Population::Population(unsigned int id,const Parameters* params,const int nb_alleles,const int nb_bases_per_allele){
  process_id                = id;
  m_parameters              = params;
  m_nb_groups               = m_parameters->getNbGroups();
  m_nb_chromosome_per_group = m_parameters->get_nb_chromosome_per_group();
  elite_size                = 0;
  
  //Chose type of breeding
  if ( !m_parameters->AreGroupsHeteroGroups() && !m_parameters->IsSelectionIndividual() ) {
    // Homogeneous groups and Group selection
    m_breeding_fn_ptr = &Population::create_new_pop_homo_group_hete_ind;
    if( m_parameters->getElite() ) elite_size = 1;
    nb_new_ind = m_nb_groups - elite_size;
    if( (elite_size + nb_new_ind ) != (m_nb_groups*m_nb_chromosome_per_group) ){
      cerr << " Problem in Population eliteSize= " << elite_size << " ParentsSize= " << nb_new_ind << endl;
      exit(EXIT_FAILURE);
    }
  }
  else if ( m_parameters->AreGroupsHeteroGroups() &&  m_parameters->IsSelectionIndividual() ) {
    // Hetero groups and Individual selection
    m_breeding_fn_ptr = &Population::create_new_pop_homo_group_hete_ind;
    if( m_parameters->getElite() ) elite_size = m_nb_chromosome_per_group;
    nb_new_ind = (m_nb_groups * m_nb_chromosome_per_group) - elite_size;
    if( (elite_size + nb_new_ind ) != (m_nb_groups*m_nb_chromosome_per_group) ){
      cerr << " Problem in Population eliteSize= " << elite_size << " ParentsSize= " << nb_new_ind << endl;
      exit(EXIT_FAILURE);
    }
  }
  else if ( !m_parameters->AreGroupsHeteroGroups() && m_parameters->IsSelectionIndividual() ){
    // Homogeneous groups and Individual selection
    m_breeding_fn_ptr = &Population::create_new_pop_homo_ind;
    if( m_parameters->getElite() ) elite_size = m_parameters->getNbAgentsPerGroup();
    nb_new_ind = m_nb_groups - elite_size;
    if( (elite_size + nb_new_ind ) != (m_nb_groups*m_nb_chromosome_per_group) ){
      cerr << " Problem in Population eliteSize= " << elite_size << " ParentsSize= " << nb_new_ind << endl;
      exit(EXIT_FAILURE);
    }
  }
  else if ( m_parameters->AreGroupsHeteroGroups() && !m_parameters->IsSelectionIndividual() ){
    // Hetero groups and Group selection
    m_breeding_fn_ptr = &Population::create_new_pop_hete_group;
    if( m_parameters->getElite() ) elite_size = 1;
    nb_new_ind = m_nb_groups - elite_size;
    if( ( (elite_size*m_nb_chromosome_per_group) + (nb_new_ind*m_nb_chromosome_per_group)) !=
	(m_nb_groups*m_nb_chromosome_per_group) ){
      cerr << " Problem in Population eliteSize= " << elite_size << " ParentsSize= " << nb_new_ind << endl;
      exit(EXIT_FAILURE);
    }
  }
  
  resetStoreFitness();
  init_chromosomes ( nb_alleles, nb_bases_per_allele );
}

/* -------------------------------------------------------------- */

Population::~Population (){ }

/* -------------------------------------------------------------- */

void Population::init_chromosomes ( const int nb_alleles, const int nb_bases_per_allele ) {
  Chromosome <chromosome_type> *local_chromosome=new Chromosome <chromosome_type>(m_parameters->AreSolutionDiploid());
  for (int i = 0; i < (m_nb_groups * m_nb_chromosome_per_group); i++){ 
    local_chromosome->init_allele_values ( nb_alleles, nb_bases_per_allele );	 
    m_chromosome.push_back( *local_chromosome );
    m_old_new_chromosome.push_back( m_chromosome.back() );
  }
  delete local_chromosome;
}

/* -------------------------------------------------------------- */

void Population::dumpStatsGenome       ( void ){
  if( !(m_parameters->getCurrentGeneration() % m_parameters->getDumpStatsEvery() ) )
    dumpStats();
  //if( !(m_parameters->getCurrentGeneration() % m_parameters->getDumpGenomeEvery() ) ){
  //dumpGenome();
  //}
}

/* -------------------------------------------------------------- */

void Population::resetStoreFitness( void ){
  for(int i = 0; i < store_fitness.size(); i++){
    store_fitness[i].erase(store_fitness[i].begin(), store_fitness[i].begin() + store_fitness[i].size() );
    store_fitness[i].clear();
  }
  store_fitness.clear();
  if( m_parameters->IsSelectionIndividual() ) 
    store_fitness.resize( m_nb_groups * m_parameters->getNbAgentsPerGroup()  );
  else
    store_fitness.resize( m_nb_groups );
  for (int i = 0; i < store_fitness.size(); i++){
    store_fitness[i].assign( m_parameters->getNbFitObjectives(), 0.0 );
  }
}

/* -------------------------------------------------------------- */

void Population::empty_m_tmp_chromosome( void ){
  for(int g = 0; g < m_tmp_chromosome.size(); g++){
    m_tmp_chromosome.erase (m_tmp_chromosome.begin(), m_tmp_chromosome.begin() + m_tmp_chromosome.size() );
    m_tmp_chromosome.clear();
  }
}

/* -------------------------------------------------------------- */

void Population::update_m_chromosome_list( const vector <int> index_list ){
  empty_m_tmp_chromosome( );
  m_chromosome.erase (m_chromosome.begin(), m_chromosome.begin() + m_chromosome.size() );
  m_chromosome.clear();
  if ( (!m_parameters->AreGroupsHeteroGroups() && !m_parameters->IsSelectionIndividual()) ||
       ( m_parameters->AreGroupsHeteroGroups() &&  m_parameters->IsSelectionIndividual()) ) {
    //Homogeneous groups Group selection OR Heterogeneous groups Individual selection
    for(int i=0; i < index_list.size(); i++) {
      m_chromosome.push_back( m_old_new_chromosome[index_list[i]] );
    }
  }
  else if ( !m_parameters->AreGroupsHeteroGroups() && m_parameters->IsSelectionIndividual() ){
    for(int i=0; i < index_list.size(); i++) {
      int ind = (int)(floor((double)(index_list[i])/(double)(m_parameters->getNbAgentsPerGroup())));
      m_chromosome.push_back( m_old_new_chromosome[ind] );
    }
  }
  else if ( m_parameters->AreGroupsHeteroGroups() && !m_parameters->IsSelectionIndividual() ){
    for(int i=0; i < index_list.size(); i++) {
      for(int j = 0; j < m_nb_chromosome_per_group; j++){
	int ind = (index_list[i] * m_nb_chromosome_per_group) + j;
	m_chromosome.push_back( m_old_new_chromosome[ind] );
      }
    }
  }
}

/* -------------------------------------------------------------- */

void Population::copy_chromosome_into_working_pop( void ){

  m_old_new_chromosome.erase (m_old_new_chromosome.begin(), m_old_new_chromosome.begin()+m_old_new_chromosome.size());
  m_old_new_chromosome.clear();
  for(int g = 0; g < m_chromosome.size(); g++)
    m_old_new_chromosome.push_back( m_chromosome[g] );

  //cerr << " In COPY CHROMO m_x.size = " << m_chromosome.size();
    
  m_chromosome.erase (m_chromosome.begin(), m_chromosome.begin() + m_chromosome.size() );
  m_chromosome.clear();
  
  for(int g = 0; g < m_tmp_chromosome.size(); g++){
    m_chromosome.push_back( m_tmp_chromosome[g] );
    m_old_new_chromosome.push_back( m_tmp_chromosome[g] );
  }

  
  //cerr << " m_x_old.size = " << m_old_new_chromosome.size() 
  //   << " m_tmp_chromosome.size = " << m_tmp_chromosome.size()  << endl;
  /*  cerr << " OLD NEW CHROMO " << endl;
  for(int g = 0; g < m_old_new_chromosome.size(); g++)
    m_old_new_chromosome[g].print_allele( );

  cerr << " NEW CHROMO " << endl;
  for(int g = 0; g < m_chromosome.size(); g++)
    m_chromosome[g].print_allele( );
    getchar(); */
}

/* -------------------------------------------------------------- */

void Population::create_new_pop_homo_group_hete_ind(vector <SingleStr <int>> &elite,vector <CoupleStr <int>> &parents){
  empty_m_tmp_chromosome();
  
  for(int g = 0; g < elite.size(); g++){ //Retain the elite unchanged 
    m_tmp_chromosome.push_back ( m_chromosome[elite[g].group] );
  }

  /*  cerr << " In create new pop m_chromo  eliste sizew = " << elite.size() << endl;
  for(int g = 0; g < m_chromosome.size(); g++)
    m_chromosome[g].print_allele( );
    getchar();*/
  
  Chromosome <chromosome_type> *local_chromosome=new Chromosome <chromosome_type>(m_parameters->AreSolutionDiploid());
  for(int ind = 0; ind < parents.size(); ind++){
    //cerr << " ind = " << ind << " mum = " << parents[ind].mum << " dad = " << parents[ind].dad << " parents_size = " << parents.size() << endl;
    if( gsl_rng_uniform(GSL_randon_generator::r_rand) < m_parameters->getProbCrossOver() ) {
      local_chromosome->create_with_cross_over_and_mutate_operators ( m_chromosome[parents[ind].mum], 
								      m_chromosome[parents[ind].dad], 
								      m_parameters->getProbMutation() );
    }
    else{
      local_chromosome->create_with_mutate_operator (m_chromosome[parents[ind].mum], m_parameters->getProbMutation());
    }
    m_tmp_chromosome.push_back( *local_chromosome );
  }
  delete local_chromosome;
  
  /*
  cerr << " TMP CHROMO in create new pop " << endl;
  for(int i = 0; i < m_tmp_chromosome.size(); i++)
    m_tmp_chromosome[i].print_allele( );
    getchar();*/
}

/* -------------------------------------------------------------- */

void Population::create_new_pop_homo_ind ( vector <SingleStr <int>> & elite, vector <CoupleStr <int>> &parents) {
  
  empty_m_tmp_chromosome();

  for(int g = 0; g < elite.size(); g++){ //Retain the elite unchanged
    int ind = (int)(floor((double)(elite[g].group)/(double)(m_parameters->getNbAgentsPerGroup())));
    m_tmp_chromosome.push_back ( m_chromosome[ind] );
  }
  
  Chromosome <chromosome_type> *local_chromosome=new Chromosome <chromosome_type>(m_parameters->AreSolutionDiploid());
  for(int ind = 0; ind < parents.size(); ind++){
    int g_mum = (int)(floor((double)(parents[ind].mum)/(double)(m_parameters->getNbAgentsPerGroup())));
    int g_dad = (int)(floor((double)(parents[ind].dad)/(double)(m_parameters->getNbAgentsPerGroup())));

    /* cerr << " ind = " << ind
      	 << " mum = " << parents[ind].mum
	 << " g_mum = " << g_mum
	 << " dad = " << parents[ind].dad
	 << " g_ dad = " << g_dad << " size = " << parents.size() << endl;
	 getchar(); */
    if( gsl_rng_uniform(GSL_randon_generator::r_rand) < m_parameters->getProbCrossOver() ) {
      local_chromosome->create_with_cross_over_and_mutate_operators ( m_chromosome[g_mum], 
 								      m_chromosome[g_dad], 
 								      m_parameters->getProbMutation() );
    }
    else{
      local_chromosome->create_with_mutate_operator (m_chromosome[g_mum], m_parameters->getProbMutation());
    }
    m_tmp_chromosome.push_back( *local_chromosome );
  }
  delete local_chromosome;

  /*
  cerr << " TMP CHROMO in create new pop " << endl;
  for(int i = 0; i < m_tmp_chromosome.size(); i++)
    m_tmp_chromosome[i].print_allele( );
    getchar();*/
  
}

/* -------------------------------------------------------------- */

void Population::create_new_pop_hete_group ( vector <SingleStr <int>> & elite, vector <CoupleStr <int>> &parents) {
  empty_m_tmp_chromosome();
  
  //Retain the elite unchanged
  for(int g = 0; g < elite.size(); g++){ //Retain the elite unchanged 
    for(int j = 0; j < m_nb_chromosome_per_group; j++){
      int ind = (elite[g].group*m_nb_chromosome_per_group)+j;
      m_tmp_chromosome.push_back ( m_chromosome[ind] );
    }
  }

  Chromosome <chromosome_type> *local_chromosome=new Chromosome <chromosome_type>(m_parameters->AreSolutionDiploid());
  for(int ind = 0; ind < parents.size(); ind++){
    for(int j = 0; j < m_nb_chromosome_per_group; j++){
      int g_mum = (parents[ind].mum*m_nb_chromosome_per_group) + j;
      int g_dad = (parents[ind].dad*m_nb_chromosome_per_group) + j;

      /* cerr << " ind = " << ind
	 << " mum = " << parents[ind].mum
	 << " g_mum = " << g_mum
	 << " dad = " << parents[ind].dad
	 << " g_ dad = " << g_dad << " size = " << parents.size() << endl;
	 getchar();*/
      if( gsl_rng_uniform(GSL_randon_generator::r_rand) < m_parameters->getProbCrossOver() ) {
	local_chromosome->create_with_cross_over_and_mutate_operators ( m_chromosome[g_mum], 
									m_chromosome[g_dad], 
									m_parameters->getProbMutation() );
      }
      else{
	local_chromosome->create_with_mutate_operator (m_chromosome[g_mum], m_parameters->getProbMutation());
      }
      m_tmp_chromosome.push_back( *local_chromosome );
    }
  }
  delete local_chromosome;
  
  /* cerr << " TMP CHROMO in create new pop " << endl;
  for(int i = 0; i < m_tmp_chromosome.size(); i++){
    cerr << " ind = " << i;
    m_tmp_chromosome[i].print_allele( );
  }
  getchar(); */
}

/* -------------------------------------------------------------- */
/* -------------------------------------------------------------- */
