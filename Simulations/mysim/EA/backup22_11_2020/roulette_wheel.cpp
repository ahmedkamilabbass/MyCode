#include "roulette_wheel.h"

/* -------------------------------------------------------------- */

Roulette_wheel::Roulette_wheel ( unsigned int id, const Parameters* params, const int nb_alleles, const int nb_bases_per_allele ) : Population ( id, params, nb_alleles, nb_bases_per_allele ){ }

/* -------------------------------------------------------------- */

Roulette_wheel::~Roulette_wheel ( void ){}

/* -------------------------------------------------------------- */

void Roulette_wheel::assign_fitness( void ){
  m_sum_of_fitness = 0.0;
  fitness.erase(fitness.begin(), fitness.begin() + fitness.size() );
  for(int i = 0; i < store_fitness.size(); i++){
    fitness.push_back( MakeValueWithIndex(store_fitness[i][0], i) );
    m_sum_of_fitness += store_fitness[i][0]; // store_fitness[i][0] is the fitness of individual i for the objective 0
  }
  if( m_parameters->getTypeOfDominance()[0] == "maximise")
    std::sort(fitness.begin(), fitness.end(), acompare<double> ); // sort in ascending order from lower to higher value
  else //minimise
    std::sort(fitness.begin(), fitness.end(), dcompare<double> ); // sort in descending order from higher to lower value
  
  /* for(int i = 0; i < fitness.size(); i++){
     cerr << " index = " << fitness[i].index << " fit value = " << fitness[i].value << endl;
     }
     getchar();*/
  
}

/* -------------------------------------------------------------- */

void Roulette_wheel::breeding ( void ) {
  
  compute_probabilities_of_selection();

  //Set here the elite
  vector <ParentsStruct <int>> elite;
  elite.erase( elite.begin(), elite.begin() + elite.size() );
  elite.clear();
  
  if( !m_parameters->AreGroupsHeteroGroups() ){//Homogeneous groups
    if( !m_parameters->IsSelectionIndividual() ){//Group selection - just select the best groups - ind always = 0
      for(int e = 0; e < m_parameters->getNbElite(); e++){
	for(int i = 0; i < m_nb_chromosome_per_group; i++){// In this case m_nb_chromosome_per_group = 1
	  elite.push_back( MakePairs ( fitness[fitness.size()-1-e].index , 0 ) );
	}
      }
    }
    else{//Individual selection
      for(int e = 0; e < m_parameters->getNbElite(); e++){
	int selected_group = (int)(floor( (double)(fitness[fitness.size()-1-e].index)/(double)(m_nb_chromosome_per_group)));//extract the group ID
	int selected_ind   = (int)(fitness[fitness.size()-1-e].index % m_nb_chromosome_per_group);//extract the subject ID
	for(int i = 0; i < m_nb_chromosome_per_group; i++){
	  elite.push_back( MakePairs ( selected_group, selected_ind ) );
	}
      }
    }
  }
  else{//Heterogeneous groups
    if( !m_parameters->IsSelectionIndividual() ){//Group selection
      for(int e = 0; e < m_parameters->getNbElite(); e++){
	int selected_group = (int)(floor( (double)(fitness[fitness.size()-1-(m_nb_chromosome_per_group*e)].index)/(double)(m_nb_chromosome_per_group)));
	for(int i = 0; i < m_nb_chromosome_per_group; i++){
	  elite.push_back( MakePairs ( selected_group, i ) );
	}
      }
    }
    else{//Individual selection
      for(int e = 0; e < m_parameters->getNbElite(); e++){
	for(int i = 0; i < m_nb_chromosome_per_group; i++){      
	  int selected_group = (int)(floor( (double)(fitness[fitness.size()-1-e].index)/(double)(m_nb_chromosome_per_group)));//extract the group ID
	  int selected_ind   = (int)(fitness[fitness.size()-1-e].index % m_nb_chromosome_per_group);//extract the subject ID
	  elite.push_back( MakePairs ( selected_group, selected_ind ) );
	}
      }
    }
  }

  for(int g = 0; g < m_tmp_chromosome.size(); g++){
    m_tmp_chromosome[g].erase (m_tmp_chromosome[g].begin(), m_tmp_chromosome[g].begin() + m_tmp_chromosome[g].size() );
    m_tmp_chromosome[g].clear();
  }
  
  (this->*m_breeding_fn_ptr)( m_chromosome, elite );

  //Copy new cromosomes (m_tmp_chromosome) into working population m_chromosome
  for(int g = 0; g < m_chromosome.size(); g++){
    m_chromosome[g].erase (m_chromosome[g].begin(), m_chromosome[g].begin() + m_chromosome[g].size() );
    m_chromosome[g].clear();
    for(int ind = 0; ind < m_nb_chromosome_per_group; ind++){  
      m_chromosome[g].push_back( m_tmp_chromosome[g][ind] );
    }
  }  
}

/* -------------------------------------------------------------- */

void Roulette_wheel::compute_probabilities_of_selection ( void ){
  //transform all fitness values in positive numbers
  if( m_parameters->getTypeOfDominance()[0] == "maximise"){
    if( fitness[0].value < 0.0 ){
      double min_val = fabs ( fitness[0].value );
      for(int i = 0; i < fitness.size(); i++){
	fitness[i].value += min_val;
      }
    }
  }
  else{//minimise
    if( fitness[fitness.size()-1].value < 0.0 ){
      cerr <<" In Roulette_wheele.cpp : the fitness can not be smaller than zero " << endl;
      exit(EXIT_FAILURE);
    }
    else{
      m_sum_of_fitness = 0.0;
      for(int i = 0; i < fitness.size(); i++){
	fitness[i].value = fitness[0].value - fitness[i].value;
	m_sum_of_fitness += fitness[i].value; 
      }
    }
  }
  proporational_fitness_selection();
  
  //if( m_parameters->getCurrentGeneration() == 0 )
  //rank_based_selection( );
}

/* -------------------------------------------------------------- */

void Roulette_wheel::proporational_fitness_selection( void ){
  ProbSelection.erase(ProbSelection.begin(), ProbSelection.begin() + ProbSelection.size() );
  ProbSelection.clear();
  ProbSelection.push_back( fitness[0].value / m_sum_of_fitness );
  
  /*  cerr << " ProbSelection[ "<< 0 <<"] = " << ProbSelection[0]
   << " Fit = " << fitness[0].value
   << endl; */
  for(int i = 1; i < fitness.size(); i++){
    ProbSelection.push_back( ProbSelection[i-1] + ( fitness[i].value / m_sum_of_fitness) );

    /*   cerr << " ProbSelection[ "<< i <<"] = " << ProbSelection[i]
    	 << " diff = " << ProbSelection.back() - ProbSelection[i-1]
    	 << " Fit = " << fitness[i].value
    	 << endl; */
  }
  //getchar();
}

/* -------------------------------------------------------------- */

void Roulette_wheel::rank_based_selection( void ){
  double selective_pressure = 2.0;
  
  ProbSelection.erase(ProbSelection.begin(), ProbSelection.begin() + ProbSelection.size() );
  ProbSelection.clear();
  for(int i = 0; i < fitness.size(); i++){
    ProbSelection.push_back( (2.0 - selective_pressure) + 
			     2.0 * (selective_pressure - 1.0)*
			     ((double)(i)/(double)(fitness.size()-1)) );
   
  }
  for(int i = 0; i < fitness.size(); i++){
    ProbSelection[i] /= ProbSelection.back();
    //cerr << " ProbSelection[ "<< i <<"] = " << ProbSelection[i] << endl;
  }
  //getchar();
}

/* -------------------------------------------------------------- */

void Roulette_wheel::select_parents ( vector <ParentsStruct <int>> &parents ){

  parents.erase(parents.begin(), parents.begin() + parents.size() );
  parents.clear();
  
  //Select mum
  double NbRnd = 0.0;
  int mum;
  if( m_parameters->getNbSolutionsTruncated() ){
    NbRnd = ProbSelection[m_parameters->getNbSolutionsTruncated()-1] +
      (gsl_rng_uniform (GSL_randon_generator::r_rand) * (ProbSelection.back() -
							 ProbSelection[m_parameters->getNbSolutionsTruncated()-1]) );
    if( NbRnd > 1.0 ){
      cerr <<" In Roulette_wheele.cpp : in select parents something went wrong  " << endl;
      exit(EXIT_FAILURE);
    }
  }
  else{
    NbRnd = gsl_rng_uniform (GSL_randon_generator::r_rand) * ProbSelection.back();
  }
  
  for (int i = m_parameters->getNbSolutionsTruncated(); i < ProbSelection.size(); i++){
    if( NbRnd < ProbSelection[i] ){
      mum = i;
      break;
    }
  }

  //Select dad different from mum
  int dad = mum;
  unsigned int iter = 0;
  do{
    if( m_parameters->getNbSolutionsTruncated()){
      NbRnd = ProbSelection[m_parameters->getNbSolutionsTruncated()-1] +
	(gsl_rng_uniform (GSL_randon_generator::r_rand) * (ProbSelection.back() -
							   ProbSelection[m_parameters->getNbSolutionsTruncated()-1]) );
      if( NbRnd > 1.0 ){
	cerr <<" In Roulette_wheele.cpp : select group wrong  " << endl;
	exit(EXIT_FAILURE);
      }
    }
    else{
      NbRnd = gsl_rng_uniform (GSL_randon_generator::r_rand) * ProbSelection.back();
    }
    
    
    for (int i = m_parameters->getNbSolutionsTruncated(); i < ProbSelection.size(); i++){
      if( NbRnd < ProbSelection[i]){
	dad = i;
	break;
      }
   }
    iter++;
  }while( mum == dad && iter < 10);

  int selected_mum_group, first_ind, selected_dad_group, second_ind;
  if( !m_parameters->IsSelectionIndividual() ){//Group selection
    if( m_parameters->AreGroupsHeteroGroups() ){//Heterogeneous groups
      //Select the group based on fitness
      //Select the individual (i.e., solutions) randomly choosing between 0 and (m_nb_chromosome_per_group - 1)
      selected_mum_group = (int)(floor( (double)(fitness[mum].index)/(double)(m_nb_chromosome_per_group)));
      first_ind          = (int)(gsl_rng_uniform_int (GSL_randon_generator::r_rand, m_nb_chromosome_per_group ));
      selected_dad_group = (int)(floor( (double)(fitness[dad].index)/(double)(m_nb_chromosome_per_group)));
      second_ind         = (int)(gsl_rng_uniform_int (GSL_randon_generator::r_rand, m_nb_chromosome_per_group ));
    }
    else{//Homogeneous groups
      //Select the group based on fitness
      //The individual is always 0 in this case, becasue there is only 1 solution per each group
      selected_mum_group  = fitness[mum].index;
      first_ind           = 0;
      selected_dad_group  = fitness[dad].index;
      second_ind          = 0;
    }
  }
  else{//Individual selection
    //Select the group based on fitness
    //Select the individual based on fitness;
    selected_mum_group  = (int)(floor( (double)(fitness[mum].index)/(double)(m_nb_chromosome_per_group)));
    first_ind           = (int)(fitness[mum].index % m_nb_chromosome_per_group);
    selected_dad_group  = (int)(floor( (double)(fitness[dad].index)/(double)(m_nb_chromosome_per_group)));
    second_ind          = (int)(fitness[dad].index % m_nb_chromosome_per_group);
  }
  
  parents.push_back( MakePairs ( selected_mum_group, first_ind  ) );
  parents.push_back( MakePairs ( selected_dad_group, second_ind ) );
}

/* -------------------------------------------------------------- */

void Roulette_wheel::dumpStats( void )
{
  char fname_statistics[500];
  sprintf(fname_statistics, "%s%s%d.data", m_parameters->getStatsFileDir().c_str(), m_parameters->getRunName().c_str(), process_id ); 
  ofstream outfile;
  outfile.open (fname_statistics, ios::app);
  outfile.setf(ios::fixed);
  
  outfile << " " << m_parameters->getCurrentGeneration()
	  << " " << fitness[fitness.size()-1].value //The fitness of the best
	  << " " << m_sum_of_fitness/(double)((m_parameters->getNbGroups() * m_nb_chromosome_per_group)) // the mean fitness
	  << " " << fitness[0].value //the fitness of the worst
	  << endl;
  outfile.close();
}

/* ---------------------------------------- */

void Roulette_wheel::dumpGenome( void )
{
  char fname_genome[500];
  sprintf(fname_genome, "%s%s%d_pop_G%d.geno", m_parameters->getGenomeFileDir().c_str(), m_parameters->getRunName().c_str(), process_id, m_parameters->getCurrentGeneration() );
  ofstream out ( fname_genome );
  out.setf( ios::fixed );
  
  for(unsigned int p=0; p< (m_nb_groups * m_nb_chromosome_per_group ); p++) {
    int group_id, ind_id;
    
    //Choose type of breeding
    if((m_nb_groups*m_nb_chromosome_per_group) == (m_parameters->getNbGroups()* m_parameters->getNbAgentsPerGroup()) ){
      group_id  = (int)(floor( (double)(fitness[fitness.size()-1-p].index)/(double)( m_nb_chromosome_per_group )));
      ind_id    = (int)(fitness[fitness.size()-1-p].index % m_nb_chromosome_per_group);
    }
    else if((m_nb_groups*m_nb_chromosome_per_group) == m_parameters->getNbGroups() ){
      group_id  = p;
      ind_id    = 0;
    }
    else{
      cerr <<" In Roulette_wheele.cpp (dumpGenome) : select group/ind wrong  " << endl;
      exit(EXIT_FAILURE);
    }
    
    out << "" << m_chromosome[group_id][ind_id].get_allele_values().size();
    for(int n=0; n<m_chromosome[group_id][ind_id].get_allele_values().size(); n++){
      out << " " << setprecision(15) << m_chromosome[group_id][ind_id].get_allele_values()[n]; 
    }
    out << " " << fitness[fitness.size()-1-p].index;
    out << " " << fitness[fitness.size()-1-p].value << endl;
  }
  out.close();
}

/* ---------------------------------------- */
