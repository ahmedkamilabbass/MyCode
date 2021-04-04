#include "population.h"

/* -------------------------------------------------------------- */

Population::Population ( unsigned int id, const Parameters* params, const int nb_alleles, const int nb_bases_per_allele ) {
  process_id    = id;
  m_parameters  = params;
  resetStoreFitness();
  m_nb_groups               =  m_parameters->getNbGroups();
  m_nb_chromosome_per_group = m_parameters->get_nb_chromosome_per_group();

  //Chose type of breeding
  if ( !m_parameters->AreGroupsHeteroGroups() ){//Homogeneous groups
    m_breeding_fn_ptr = &Population::create_new_homo_population;
  }
  else{//Heterogeneous groups
    if( !m_parameters->IsSelectionIndividual() ){   //Group selection
      m_breeding_fn_ptr = &Population::create_new_hetero_population_group_selection;
    }
    else{//Individual selection
      m_breeding_fn_ptr = &Population::create_new_hetero_population_individual_selection;
    }
  }
  init_chromosomes ( nb_alleles, nb_bases_per_allele );
}

/* -------------------------------------------------------------- */

Population::~Population (){ }

/* -------------------------------------------------------------- */

void Population::init_chromosomes ( const int nb_alleles, const int nb_bases_per_allele ) {
  int count_solutions = 0;

  m_chromosome.resize     ( m_nb_groups );
  m_tmp_chromosome.resize ( m_nb_chromosome_per_group );
    
  Chromosome <chromosome_type> *local_chromosome = new Chromosome <chromosome_type>( m_parameters->AreSolutionDiploid() );
  for (int g = 0; g < m_parameters->getNbGroups(); g++){ 
    for(int ind = 0; ind < m_nb_chromosome_per_group; ind++){
      if( !m_parameters->AreGroupsHeteroGroups() ){ //Homogeneous Groups
	if( ind == 0 ){// Group Selection there is only 1 ind
	  local_chromosome->init_allele_values ( nb_alleles, nb_bases_per_allele, (double) g );	 
	}
      }
      else{
	local_chromosome->init_allele_values ( nb_alleles, nb_bases_per_allele, (double) g );
      }
      m_chromosome[g].push_back( *local_chromosome );
      count_solutions++;
    }
  }
  delete local_chromosome;

  /* for (int g = 0; g < m_parameters->getNbGroups(); g++){
    cerr << " groups = " << g << endl;
    for(int ind = 0; ind < m_nb_chromosome_per_group; ind++){
      m_chromosome[g][ind].print_allele();
    }
    }*/
  //exit(0);
  
  if(count_solutions != (m_nb_groups * m_nb_chromosome_per_group ) ){
    cerr <<" In population.cpp (constructor) : pop size is = " << (m_nb_groups * m_nb_chromosome_per_group )
	 << " count_solutions = " << count_solutions
	 << " nb groups = " << m_parameters->getNbGroups()
	 << " nb_agents per group = " << m_nb_chromosome_per_group
	 << endl;
    exit(EXIT_FAILURE);
  }
}

/* -------------------------------------------------------------- */

void Population::dumpStatsGenome       ( void ){
  if( !(m_parameters->getCurrentGeneration() % m_parameters->getDumpStatsEvery() ) )
    dumpStats();
  if( !(m_parameters->getCurrentGeneration() % m_parameters->getDumpGenomeEvery() ) ){
    dumpGenome();
  }
}

/* -------------------------------------------------------------- */

void Population::resetStoreFitness( void ){
  for(int i = 0; i < store_fitness.size(); i++){
    store_fitness[i].erase(store_fitness[i].begin(), store_fitness[i].begin() + store_fitness[i].size() );
    store_fitness[i].clear();
  }
  store_fitness.clear();
  if( m_parameters->IsSelectionIndividual() ) 
    store_fitness.resize( m_parameters->getNbGroups() * m_parameters->getNbAgentsPerGroup() );
  else
    store_fitness.resize( m_parameters->getNbGroups() );
  
  for (int i = 0; i < store_fitness.size(); i++){
    store_fitness[i].assign( m_parameters->getNbFitObjectives(), 0.0 );
  }
}

/* -------------------------------------------------------------- */

void Population::create_new_homo_population ( vector < vector < Chromosome <chromosome_type> >> &chromo,
					      vector <ParentsStruct<int>> &elite ){
  int count_elite = 0;
  vector <ParentsStruct <int>> parents;//These are mum and dad index
  Chromosome <chromosome_type> *local_chromosome = new Chromosome <chromosome_type>( m_parameters->AreSolutionDiploid() );
  
  //cerr << " nb elite = " << m_parameters->getNbElite() << endl;
  for(int g = 0; g < m_parameters->getNbGroups(); g++){ //Retain the elite unchanged 
    if( g < m_parameters->getNbElite() ){//The unit of measure of the elite is the group size
      for(int ind = 0; ind < m_nb_chromosome_per_group; ind++){
	//m_tmp_chromosome[g][ind].copy_allele_values( chromo[elite[count_elite].group][elite[count_elite].subject].get_allele_values() );
	m_tmp_chromosome[g].push_back ( chromo[elite[count_elite].group][elite[count_elite].subject] );
	count_elite++;
      }
    }
    else{
      for(int ind = 0; ind < m_nb_chromosome_per_group; ind++){
	if( !ind ){
	  select_parents ( parents );
	  /* cerr << " g zero = " << parents[0].group
	     << " i zero = " << parents[0].subject
	     << " g 1 = " << parents[1].group
	     << " i 1 = " << parents[1].subject << endl;*/
	  if( gsl_rng_uniform(GSL_randon_generator::r_rand) < m_parameters->getProbCrossOver() ) {
	    local_chromosome->/*m_tmp_chromosome[g][ind].*/create_with_cross_over_and_mutate_operators ( chromo[parents[0].group][parents[0].subject], 
													 chromo[parents[1].group][parents[1].subject], 
													 m_parameters->getProbMutation() );
	  }
	  else{
	    local_chromosome->/*m_tmp_chromosome[g][ind].*/create_with_mutate_operator ( chromo[parents[0].group][parents[0].subject],
											 m_parameters->getProbMutation() );
	  }
	}
	m_tmp_chromosome[g].push_back( *local_chromosome );	
	/*else{
	  m_tmp_chromosome[g][ind].copy_allele_values( m_tmp_chromosome[g][0].get_allele_values() );
	  }*/
      }
    }
  }
  delete local_chromosome;
}

/* -------------------------------------------------------------- */

void Population::create_new_hetero_population_group_selection ( vector < vector < Chromosome <chromosome_type> >> &chromo,
								vector <ParentsStruct<int>> &elite ){
  int count_elite = 0;
  vector <ParentsStruct <int>> parents;//These are mum and dad index
  Chromosome <chromosome_type> *local_chromosome = new Chromosome <chromosome_type>( m_parameters->AreSolutionDiploid() );
	
  for(int g = 0; g < m_parameters->getNbGroups(); g++){ //Retain the elite unchanged 
    if( g < m_parameters->getNbElite() ){//The unit of measure of the elite is the group size
      for(int ind = 0; ind < m_nb_chromosome_per_group; ind++){
	m_tmp_chromosome[g].push_back( chromo[elite[count_elite].group][elite[count_elite].subject] );
	count_elite++;
      }
    }
    else{
      for(int ind = 0; ind < m_nb_chromosome_per_group; ind++){
	select_parents ( parents );
	if( gsl_rng_uniform(GSL_randon_generator::r_rand) < m_parameters->getProbCrossOver() ) {
	  local_chromosome->/*m_tmp_chromosome[g][ind].*/create_with_cross_over_and_mutate_operators ( chromo[parents[0].group][parents[0].subject], 
												       chromo[parents[1].group][parents[1].subject], 
												       m_parameters->getProbMutation() );
	}
	else{
	  local_chromosome->/*m_tmp_chromosome[g][ind].*/create_with_mutate_operator ( chromo[parents[0].group][parents[0].subject],
										       m_parameters->getProbMutation() );
	}
	m_tmp_chromosome[g].push_back( *local_chromosome );
      }
    }
  }
  delete local_chromosome;
}

/* -------------------------------------------------------------- */

void Population::create_new_hetero_population_individual_selection ( vector < vector < Chromosome <chromosome_type> >> &chromo,
								     vector <ParentsStruct<int>> &elite ){
  vector <int> d_vector;
  for(int i = 0; i < (m_nb_groups*m_nb_chromosome_per_group); i++){
    d_vector.push_back(i);
  }

  int count_elite = 0;
  vector <ParentsStruct <int>> parents;//These are mum and dad index
  Chromosome <chromosome_type> *local_chromosome = new Chromosome <chromosome_type>( m_parameters->AreSolutionDiploid() );

  int counter = 0;
  do{
    int index_d_vector    = counter;//gsl_rng_uniform_int (GSL_randon_generator::r_rand, d_vector.size() );
    int element_d_vector  = d_vector[index_d_vector];
    int d_group           = (int)(floor((double)(element_d_vector)/(double)(m_nb_chromosome_per_group)));
    int d_ind             = (int)(element_d_vector % m_nb_chromosome_per_group);
    
    d_vector.erase       ( d_vector.begin() + index_d_vector );
    
    if( count_elite < (m_parameters->getNbElite()*m_nb_chromosome_per_group)  ){
      m_tmp_chromosome[d_group]/*[d_ind]*/.push_back( chromo[elite[count_elite].group][elite[count_elite].subject] );
      count_elite++;
    }
    else{
      select_parents ( parents );
      if( gsl_rng_uniform(GSL_randon_generator::r_rand) < m_parameters->getProbCrossOver() ) {
	local_chromosome->/*m_tmp_chromosome[d_group][d_ind].*/create_with_cross_over_and_mutate_operators ( chromo[parents[0].group][parents[0].subject], 
													     chromo[parents[1].group][parents[1].subject], 
													     m_parameters->getProbMutation() );
      }
      else{
	local_chromosome->/*m_tmp_chromosome[d_group][d_ind].*/create_with_mutate_operator ( chromo[parents[0].group][parents[0].subject],
											     m_parameters->getProbMutation() );
      }
      m_tmp_chromosome[d_group].push_back( local_chromosome );
    }
    counter++;
  }while( !d_vector.empty() );
  delete local_chromosome;
}

/* -------------------------------------------------------------- */
