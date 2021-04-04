#include "emoo.h"

Emoo::Emoo ( unsigned int id, const Parameters* params, const int nb_alleles, const int nb_bases_per_allele ) :
  Population ( id, params, nb_alleles, nb_bases_per_allele ){

  m_pop_p.erase(m_pop_p.begin(), m_pop_p.begin() + m_pop_p.size() );
  m_pop_p.clear();
  m_type_of_dominance = m_parameters->getTypeOfDominance();
  m_tournament_size   = m_parameters->getTournamentSize();
  m_first_round       = true;
}

/* -------------------------------------------------------------- */

Emoo::~Emoo ( void ){}

/* -------------------------------------------------------------- */

void Emoo::assign_fitness     ( void ){
  
  for(int i = 0; i < store_fitness.size(); i++){
    m_pop_p.push_back( MakeStruct( store_fitness[i] ) );
    //m_chromosome[(i%m_nb_groups)][(i%m_nb_chromosome_per_group)].get_allele_values() ) );
  }

  update_domination_counter_and_dominated_set ();
  /* for(int i = 0; i < m_pop_p.size(); i++) {
     cerr << " np = " << m_pop_p[i].domination_counter;// << " rank ="<< m_pop_p[i].rank << endl;
     for(int j = 0; j < m_pop_p[i].dominated_set.size(); j++) {
     cerr << " d_set["<<j<<"] = " << m_pop_p[i].dominated_set[j] ;
     }
     cerr << endl;
     } */
  compute_pareto_fronts( m_pop_p );
    
  for(int i = 0; i < m_pop_p.size(); i++) {
    for(int ob = 0; ob < m_pop_p[i].objective.size(); ob++){
      cerr << " ind = "<< i << " obj["<<ob<<"]="<< m_pop_p[i].objective[ob];
    }
    cerr << " np = " << m_pop_p[i].domination_counter << " rank ="<< m_pop_p[i].rank
	 << " crow_d = " << m_pop_p[i].crowding_distance << endl;
    //   /*cerr << " np = " << m_current_pop[i].domination_counter;
    //   for(int j = 0; j < m_current_pop[i].dominated_set.size(); j++)
    //     cerr << " sp["<<j<<"] = " << m_current_pop[i].dominated_set[j];
    //     cerr  << endl;*/
    //   double pheno = 0.0;

    //Hete ind
    //for(int j = 0; j < 10; j++)
    //cerr << " " << m_chromosome[rint((double)i/2.0)][i%2].get_allele_values()[j];
    //cerr << endl;
    //     pheno += m_pop_p[i].genes[j];
    //   pheno = (pheno/10.0)*10;
    //   cerr << " " << (pheno * pheno) << " " << (pheno-2.0) * (pheno-2.0);
    //   cerr << endl;
  }
}

/* -------------------------------------------------------------- */

void Emoo::update_domination_counter_and_dominated_set ( void ){
  for(int i = 0; i < m_pop_p.size()-1; i++) {
    for(int j = i+1; j < m_pop_p.size(); j++) {
      int i_better_j = 0;
      int j_better_i = 0;
      
      for(int obj = 0; obj < m_parameters->getNbFitObjectives(); obj++){
	if( m_type_of_dominance[obj] == "minimise" ){
	  if( m_pop_p[i].objective[obj] < m_pop_p[j].objective[obj] )
	    i_better_j++;
	  else if ( m_pop_p[i].objective[obj] > m_pop_p[j].objective[obj] )
	    j_better_i++;
	}
	else{//maximise
	  if( m_pop_p[i].objective[obj] > m_pop_p[j].objective[obj] )
	    i_better_j++;
	  else if ( m_pop_p[i].objective[obj] < m_pop_p[j].objective[obj] )
	    j_better_i++;
	}
      }//end of for objectives
      if( i_better_j != 0 && j_better_i == 0 ){
	m_pop_p[i].dominated_set.push_back(j); //i dominates j
	m_pop_p[j].domination_counter++; //j is dominated by i
      }
      else if( i_better_j == 0 && j_better_i != 0 ){
	m_pop_p[j].dominated_set.push_back(i); //j dominates i
	m_pop_p[i].domination_counter++; //i is dominated by j
      }
    }
  }
}

/* -------------------------------------------------------------- */

void Emoo::compute_pareto_fronts( vector < pop_struct <double> > &pop ){
  vector <int> single_front_solutions;
  vector <int> tmp_single_front_solutions;
  for(int i=0; i<pop.size(); i++){
    if( pop[i].domination_counter == 0 ){
      pop[i].rank = 1;
      single_front_solutions.push_back(i);
    }
  }
  compute_crowding_distance( pop, single_front_solutions );
  
  int rank = 1;
  do{
    while ( !single_front_solutions.empty() ){
      int item = single_front_solutions.back();
      single_front_solutions.pop_back();
      for(int j=0; j<pop[item].dominated_set.size(); j++){
	pop[pop[item].dominated_set[j]].domination_counter -= 1;
	if( pop[pop[item].dominated_set[j]].domination_counter == 0 ){
	  pop[pop[item].dominated_set[j]].rank = rank + 1;
	  tmp_single_front_solutions.push_back( pop[item].dominated_set[j] );
	}
      }
    }
    if( !tmp_single_front_solutions.empty()  && single_front_solutions.empty() ){
      do{
	single_front_solutions.push_back( tmp_single_front_solutions.back() );
	tmp_single_front_solutions.pop_back();
      }while( !tmp_single_front_solutions.empty() );
      rank++;
      compute_crowding_distance( pop, single_front_solutions );
    }
  }while( !single_front_solutions.empty() );
}

/* -------------------------------------------------------------- */

void Emoo::compute_crowding_distance( vector < pop_struct <double> > &pop, vector <int> &single_front_solutions ){

  if( single_front_solutions.size() < 2){ //If there is only two agents in this front
    for(int i=0; i < single_front_solutions.size(); i++){
      pop[single_front_solutions[i]].crowding_distance = INFINITY;
    }
  }
  else{
    for(int i=0; i < single_front_solutions.size(); i++)
      pop[single_front_solutions[i]].crowding_distance = 0.0;
    
    //bubble sorting
    for(int f=0; f<m_parameters->getNbFitObjectives(); f++){
      int tmp;
      for(int i=0; i < (single_front_solutions.size()-1); i++){
	for(int j = 0; j < (single_front_solutions.size()-i-1); j++){
	  if(pop[single_front_solutions[j]].objective[f] > pop[single_front_solutions[j+1]].objective[f] ){
	    tmp = single_front_solutions[j];
	    single_front_solutions[j] = single_front_solutions[j+1];
	    single_front_solutions[j+1] = tmp;
	  }
	}
      }
      
      if( m_type_of_dominance[f] == "minimise" )
	pop[single_front_solutions[0]].crowding_distance = INFINITY;
      else
	pop[single_front_solutions[single_front_solutions.size()-1]].crowding_distance = INFINITY;
      
      for(int ind = 1; ind < single_front_solutions.size()-1; ind++){
	double tmp_dist =(  ( pop[single_front_solutions[ind+1]].objective[f] -
			      pop[single_front_solutions[ind-1]].objective[f] )/
			    ( pop[single_front_solutions[single_front_solutions.size()-1]].objective[f] -
			      pop[single_front_solutions[0]].objective[f] ) );
	pop[single_front_solutions[ind]].crowding_distance += tmp_dist;
      }
    }
  }
}

/* -------------------------------------------------------------- */

void Emoo::tournament_selection ( vector < pop_struct <double> > &pop, vector <int> &selected ){
  int s[m_tournament_size];
  selected.resize( 2 );
  int p = 0;
  do{
    bool flag_continue;
    for(int k = 0; k < m_tournament_size; k++){
      do{
	flag_continue = true;
	s[k] = gsl_rng_uniform_int( GSL_randon_generator::r_rand, pop.size() );
	for(int i=0; i<k; i++){
	  if(s[i] == s[k]){
	    flag_continue = false;
	    break;
	  }
	}
      }while(flag_continue == false);
      if( k == 0 ){
	selected[p] = s[k];
      }
      else{
	if( pop[s[k]].rank < pop[selected[p]].rank )
	  selected[p] = s[k];
	else if ( pop[s[k]].rank == pop[selected[p]].rank ){
	  if ( pop[s[k]].crowding_distance > pop[selected[p]].crowding_distance ){
	    selected[p] = s[k];
	  }
	  else{
	    if( gsl_rng_uniform(GSL_randon_generator::r_rand) < 0.5 )
	      selected[p] = s[k];
	  }
	}
      }
    }
    p++;
  }while(p < 2);
}


/* -------------------------------------------------------------- */

void Emoo::breeding( void ){
  int nb_new_ind = 0;
  if ( !m_parameters->AreGroupsHeteroGroups() )//Homogeneous groups
    nb_new_ind = m_nb_groups;
  else
    nb_new_ind = m_nb_groups * m_nb_chromosome_per_group;

  if( m_first_round ){
    m_first_round = false;
    vector <int> selected;
    for (int i = 0; i < nb_new_ind; i++ ){
      tournament_selection ( m_pop_p, selected );
      // Homo and group selection
      if ( !m_parameters->AreGroupsHeteroGroups() && !m_parameters->IsSelectionIndividual() ){
  	//m_chromosome[selected[0]][0].get_allele_values();
  	//m_chromosome[selected[1]][0].get_allele_values();
  	cerr << " g mum = " << selected[0] << " g dad = " << selected[1] << " ind = 0 " << endl;
	for( int t=0; t<m_chromosome[selected[0]][0].get_allele_values().size(); t++){
	  cerr << " " << m_chromosome[selected[0]][0].get_allele_values()[t];
	}
	cerr << endl;
	for( int t=0; t<m_chromosome[selected[1]][0].get_allele_values().size(); t++){
	  cerr << " " << m_chromosome[selected[1]][0].get_allele_values()[t];
	}
	cerr << endl;
      }
      
      // Homo and individual selection
      if ( !m_parameters->AreGroupsHeteroGroups() && m_parameters->IsSelectionIndividual() ){
	int na = m_parameters->getNbAgentsPerGroup();
  	//m_chromosome[(int)(rint((double)(selected[0])/(double)(na)))][0].get_allele_values();
  	//m_chromosome[(int)(rint((double)(selected[1])/(double)(na)))][0].get_allele_values();
  	cerr << " S0 = " << selected[0] << " g mum = " << (int)(rint((double)(selected[0])/(double)(na)))
  	     << " g dad = " << (int)(rint((double)(selected[1])/(double)(na))) << " ind = 0 "<< endl;
	for( int t=0; t<m_chromosome[(int)(rint((double)(selected[0])/(double)(na)))][0].get_allele_values().size(); t++){
	  cerr << " " << m_chromosome[(int)(rint((double)(selected[0])/(double)(na)))][0].get_allele_values()[t];
	}
	cerr << endl;
	for( int t=0; t<m_chromosome[(int)(rint((double)(selected[1])/(double)(na)))][0].get_allele_values().size(); t++){
	  cerr << " " << m_chromosome[(int)(rint((double)(selected[1])/(double)(na)))][0].get_allele_values()[t];
	}
	cerr << endl;
      }
      
      
      // Hete and group selection
      if ( m_parameters->AreGroupsHeteroGroups() && !m_parameters->IsSelectionIndividual() ){
  	int random_selected_mum = gsl_rng_uniform_int (GSL_randon_generator::r_rand, m_nb_chromosome_per_group );
  	//m_chromosome[selected[0]][random_selected_mum].get_allele_values();
  	cerr << " g mum = " << selected[0] << " i mum = " << random_selected_mum; 
  	int random_selected_dad = gsl_rng_uniform_int (GSL_randon_generator::r_rand, m_nb_chromosome_per_group );
  	//m_chromosome[selected[1]][random_selected_dad].get_allele_values();
  	cerr << " g dad = " << selected[1] << " i dad = " << random_selected_dad  << endl;

	for( int t=0; t<m_chromosome[selected[0]][random_selected_mum].get_allele_values().size(); t++){
	  cerr << " " << m_chromosome[selected[0]][random_selected_mum].get_allele_values()[t];
	}
	cerr << endl;
	for( int t=0; t<m_chromosome[selected[1]][random_selected_dad].get_allele_values().size(); t++){
	  cerr << " " << m_chromosome[selected[1]][random_selected_dad].get_allele_values()[t];
	}
	cerr << endl;


	
      }
      
      // Hete and individual selection
      if ( m_parameters->AreGroupsHeteroGroups() && m_parameters->IsSelectionIndividual() ){
  	//m_chromosome[(int)(rint((double)(selected[0])/(double)(m_nb_chromosome_per_group)))][selected[0]%m_nb_chromosome_per_group].get_allele_values();
  	//m_chromosome[(int)(rint((double)(selected[1])/(double)(m_nb_chromosome_per_group)))][selected[1]%m_nb_chromosome_per_group].get_allele_values();
  	cerr << " s0=" << selected[0] << " g mum = " << (int)(rint((double)(selected[0])/(double)(m_nb_chromosome_per_group)))
  	     << " i mum = " << selected[0]% m_nb_chromosome_per_group
  	     << " s1=" << selected[1] << " g dad = " << (int)(rint((double)(selected[1])/(double)(m_nb_chromosome_per_group)))
  	     << " i dad = " << selected[1]%m_nb_chromosome_per_group << endl;
  	for( int t=0; t<m_chromosome[(int)(rint((double)(selected[0])/(double)(m_nb_chromosome_per_group)))][selected[0]%m_nb_chromosome_per_group].get_allele_values().size(); t++){
  	  cerr << " " << m_chromosome[(int)(rint((double)(selected[0])/(double)(m_nb_chromosome_per_group)))][selected[0]%m_nb_chromosome_per_group].get_allele_values()[t];
  	}
  	cerr << endl;
  	for( int t=0; t<m_chromosome[(int)(rint((double)(selected[1])/(double)(m_nb_chromosome_per_group)))][selected[1]%m_nb_chromosome_per_group].get_allele_values().size(); t++){
  	  cerr << " " << m_chromosome[(int)(rint((double)(selected[1])/(double)(m_nb_chromosome_per_group)))][selected[1]%m_nb_chromosome_per_group].get_allele_values()[t];
  	}
  	cerr << endl;
		
      }
    }
    
    //(this->*m_breeding_fn_ptr)( m_chromosome, elite );
  }
  else{
    m_first_round = true;
    //reset m_pop_p
  }
}

/* -------------------------------------------------------------- */


void Emoo::select_parents ( vector <ParentsStruct <int>> &parents ){
  
}

/* -------------------------------------------------------------- */

void Emoo::clear_all_vectors( void ){}

/* -------------------------------------------------------------- */

void Emoo::dumpStats          ( void ){ }

// /* -------------------------------------------------------------- */

void Emoo::dumpGenome         ( void ){ }

// /* -------------------------------------------------------------- */
// /* -------------------------------------------------------------- */

