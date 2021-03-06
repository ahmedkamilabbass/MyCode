#include "emoo.h"

Emoo::Emoo ( unsigned int id, const Parameters* params, const int nb_alleles, const int nb_bases_per_allele ) :
  Population ( id, params, nb_alleles, nb_bases_per_allele ){

  m_pop_p.erase(m_pop_p.begin(), m_pop_p.begin() + m_pop_p.size() );
  m_pop_p.clear();
  m_type_of_dominance = m_parameters->getTypeOfDominance();
  m_tournament_size   = m_parameters->getTournamentSize();
  m_first_round       = true;
  if( m_parameters->getElite() ){
    cerr << " There is a problem in Emoo:: Elite has to be 'N' " << endl;
    exit(EXIT_FAILURE);
  }
}

/* -------------------------------------------------------------- */

Emoo::~Emoo ( void ){}

/* -------------------------------------------------------------- */

void Emoo::assign_fitness     ( void ){

  for(int i=0; i< m_pop_p.size(); i++)
    m_pop_p[i].clearRelevantValues();
  
  int m_pop_p_initial_size = m_pop_p.size();
  
  for(int i = 0; i < store_fitness.size(); i++){
    m_pop_p.push_back( MakeStruct( store_fitness[i], (m_pop_p_initial_size + i) ) );
  }

  update_domination_counter_and_dominated_set ();
  compute_pareto_fronts( m_pop_p );
  
  /*for(int i = 0; i < m_pop_p.size(); i++) {
    cerr << " ind = "<< i <<" index = " << m_pop_p[i].index;
    for(int ob = 0; ob < m_pop_p[i].objective.size(); ob++)
      cerr << " obj["<<ob<<"]="<< m_pop_p[i].objective[ob];
    cerr << " np = " << m_pop_p[i].domination_counter << " rank ="<< m_pop_p[i].rank
	 << " crow_d = " << m_pop_p[i].crowding_distance << endl;
    //for(int j = 0; j < m_pop_p[i].dominated_set.size(); j++) {
    //cerr << " d_set["<<j<<"] = " << m_pop_p[i].dominated_set[j];
    //}
    cerr << endl;
  }
  getchar();*/
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

      if(pop[single_front_solutions[single_front_solutions.size()-1]].objective[f] == pop[single_front_solutions[0]].objective[f] ){
	for(int ind = 1; ind < single_front_solutions.size()-1; ind++){
	  pop[single_front_solutions[ind]].crowding_distance = 0.0;
	}
      }
      else{
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
}

/* -------------------------------------------------------------- */

void Emoo::tournament_selection ( vector < pop_struct <double> > &pop, vector <int> &selected ){
  vector <int> s;
  selected.erase(selected.begin(), selected.begin() + selected.size());
  selected.clear();
  selected.resize( 2 );
  int p = 0;
  do{
    bool flag_continue;
    for(int k = 0; k < m_tournament_size; k++){
      int tmp_random_int;
      do{
	flag_continue = true;
	tmp_random_int = gsl_rng_uniform_int( GSL_randon_generator::r_rand, pop.size() );
	for(int i=0; i<k; i++){
	  if( tmp_random_int == s[i]){
	    flag_continue = false;
	    break;
	  }
	}
      }while(flag_continue == false);

      s.push_back( tmp_random_int );
      //cerr << " s["<<k<<"]= "<< s[k] << endl;
      //getchar();
	  
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
    s.erase( s.begin(), s.begin() + s.size() );
    //cerr << " selected["<< p << "]=" << selected[p] << endl;
    //getchar();
    p++;
  }while(p < 2);
}

/* -------------------------------------------------------------- */

void Emoo::sort_selected_solutions_for_crowding_dist( vector < pop_struct <double> > &pop,
						      vector <int> &solutions_in_front_i ){
  int tmp_index;
  for(int i = 0; i < solutions_in_front_i.size()-1; i++){
    for(int j = 0; j < solutions_in_front_i.size()-i-1; j++){
      if ( pop[solutions_in_front_i[j]].crowding_distance > pop[solutions_in_front_i[j+1]].crowding_distance ){
	tmp_index = solutions_in_front_i[j];
	solutions_in_front_i[j] =  solutions_in_front_i[j+1];
	solutions_in_front_i[j+1] = tmp_index;
      }
    }
  }
}

/* -------------------------------------------------------------- */

void Emoo::nsga2_select( vector < pop_struct <double> > &pop, vector <int> &selected, int nb_to_select ){
    vector <int> solutions_in_front_i;
  //int nb_to_select = pop.size() * 0.5; //(m_nb_groups * m_nb_chromosome_per_group);
  bool flag_stack_full = false;
  int rank = 1;
  do{
    int i = 0;
    while(i < pop.size() ){
      if( pop[i].rank == rank ){
	solutions_in_front_i.push_back( i );
      }
      i++;
    }
    compute_crowding_distance( pop, solutions_in_front_i );
    sort_selected_solutions_for_crowding_dist( pop, solutions_in_front_i );
    
    while( selected.size() < nb_to_select ) {
      selected.push_back( solutions_in_front_i.back() );
      solutions_in_front_i.pop_back();
      
      if( solutions_in_front_i.empty() )
	break;
    }
    
    if( selected.size() == nb_to_select )
      flag_stack_full = true;
    else rank++;
  }while( flag_stack_full == false);
}

/* -------------------------------------------------------------- */

void Emoo::breeding( void ){  
  if( !m_first_round ){
    vector <int> selected_solutions;
    nsga2_select( m_pop_p, selected_solutions, nb_new_ind );
    /* for(int i = 0; i < selected_solutions.size(); i++)
       cerr << " s["<<i<<"] = " << selected_solutions[i] << " index = " << m_pop_p[selected_solutions[i]].index << endl;
       getchar(); */

    vector < pop_struct <double> > tmp_pop_p;
    for(int i=0; i< selected_solutions.size(); i++){
      tmp_pop_p.push_back( m_pop_p[selected_solutions[i]] );
    }
    m_pop_p.erase(m_pop_p.begin(), m_pop_p.begin() + m_pop_p.size());
    
    if (!m_parameters->AreGroupsHeteroGroups() && m_parameters->IsSelectionIndividual()){
      for(int p = 0; p < tmp_pop_p.size(); p++){
	for(int a =0; a < m_parameters->getNbAgentsPerGroup(); a++){
	  m_pop_p.push_back( tmp_pop_p[p] );
	  m_pop_p.back().index = (p*m_parameters->getNbAgentsPerGroup())+a;
	}
      }
    }
    else{
      for(int p = 0; p < tmp_pop_p.size(); p++){
	m_pop_p.push_back( tmp_pop_p[p] );
	m_pop_p.back().index = p;
      }
    }
    /*for(int p = 0; p < m_pop_p.size(); p++){
      int i = (int)(floor((double)(p)/(double)(m_parameters->getNbAgentsPerGroup()) ));
      cerr << " index = " << m_pop_p[p].index << " s["<<p<< "] = " << selected_solutions[i]
	   << " tmp_index = " << tmp_pop_p[i].index 
	   << endl;
    }
    getchar();*/

    update_m_chromosome_list( selected_solutions  );
  }
  
  vector <CoupleStr <int>> parents;
  vector <SingleStr <int>> elite;
  vector <int> selected;
  for (int i = 0; i < nb_new_ind; i++ ){
    tournament_selection ( m_pop_p, selected );
    parents.push_back( MakeCouple (m_pop_p[selected[0]].index, m_pop_p[selected[1]].index ) );
    //cerr << " ind = " << i << " mum = " << parents[i].mum << " dad = " << parents[i].dad << endl; 
  }
  //getchar();
  
  (this->*m_breeding_fn_ptr)( elite, parents );
  copy_chromosome_into_working_pop();

  if( !(m_parameters->getCurrentGeneration() % m_parameters->getDumpGenomeEvery() ) )
    dumpGenome();
  
  if( m_first_round )
    m_first_round = false;
}

/* -------------------------------------------------------------- */

void Emoo::dumpStats          ( void ){
  char fname_statistics[500];
  sprintf(fname_statistics, "%s%s%d.data", m_parameters->getStatsFileDir().c_str(), m_parameters->getRunName().c_str(), process_id );

  vector <double> best;
  vector <int>    best_ind;
  int nbObj = m_parameters->getNbFitObjectives();

  for(int ob=0; ob<nbObj; ob++){
    if( m_type_of_dominance[ob] == "minimise" )
      best.push_back( INFINITY );
    else
      best.push_back( 0.0 );
    best_ind.push_back( 0 );
    
    for(int p=0; p<m_pop_p.size()*0.5; p++){
      if( m_type_of_dominance[ob] == "minimise" ){
	if( m_pop_p[p].objective[ob] < best[ob] ){
	  best[ob] = m_pop_p[p].objective[ob];
	  best_ind[ob] = p;
	}
      }
      else{
	if( m_pop_p[p].objective[ob] > best[ob] ){
	  best[ob] = m_pop_p[p].objective[ob];
	  best_ind[ob] = p;
	}
      }
    }
  }
  
  ofstream outfile;
  outfile.open (fname_statistics, ios::app);
  outfile.setf(ios::fixed);
  if( outfile.is_open() ){
    outfile << " " << m_parameters->getCurrentGeneration();
    for(int p=0; p<nbObj; p++){
      for(int ob=0; ob<nbObj; ob++){
	outfile << " " << setprecision(15) << m_pop_p[best_ind[p]].objective[ob];
      }
    }
    outfile << endl;
  }
  outfile.close();
}

/* -------------------------------------------------------------- */

void Emoo::dumpGenome         ( void ){

  /*  for(int p = 0; p < m_pop_p.size(); p++){
    cerr << " p = " << p;
    for(int ob=0; ob<m_pop_p[p].objective.size(); ob++){
      cerr << " " << m_pop_p[p].objective[ob];
    }
    cerr << endl;
  }
  getchar();


  for(unsigned int p=0; p< m_old_new_chromosome.size(); p++) {
    cerr << " p = " << p;
    double pheno = 0.0;
    for(int n=0; n<m_old_new_chromosome[p].get_allele_values().size(); n++){
      pheno += m_old_new_chromosome[p].get_allele_values()[n];
    }
    pheno = (pheno/(double)(m_old_new_chromosome[p].get_allele_values().size()))*1000.0;
    cerr << " " << pheno*pheno << " " << (pheno-2)*(pheno-2);
  }
  cerr << endl;
  getchar(); */

  
  
  char fname_genome[500];
  sprintf(fname_genome, "%s%s%d_pop_G%d.geno", m_parameters->getGenomeFileDir().c_str(), m_parameters->getRunName().c_str(), process_id, m_parameters->getCurrentGeneration() );
  
  ofstream outfile;
  outfile.open ( fname_genome, ios::app);
  outfile.setf(ios::fixed);
  if( outfile.is_open() ){
    for(unsigned int p=0; p< (m_old_new_chromosome.size()) * 0.5; p++) {
      double pheno = 0.0;
      outfile << "" << m_old_new_chromosome[p].get_allele_values().size();
      for(int n=0; n<m_old_new_chromosome[p].get_allele_values().size(); n++){
	outfile << " " << setprecision(15) << m_old_new_chromosome[p].get_allele_values()[n];
	pheno += m_old_new_chromosome[p].get_allele_values()[n];
      }

      int p_size = p;
      if ( m_parameters->AreGroupsHeteroGroups() && !m_parameters->IsSelectionIndividual() )
	p_size = (int)(floor((double)(p)/(double)(m_nb_chromosome_per_group)));
      //else if( !m_parameters->AreGroupsHeteroGroups() && m_parameters->IsSelectionIndividual() )
      //p_size = ;
      
      for(int ob=0; ob<m_pop_p[p_size].objective.size(); ob++)
	outfile << " " << m_pop_p[p_size].objective[ob];
      
      //for(int ob=0; ob<m_pop_p[p*m_parameters->getNbAgentsPerGroup()].objective.size(); ob++)
      //outfile << " " << m_pop_p[p*m_parameters->getNbAgentsPerGroup()].objective[ob];
      
      pheno = ((pheno/(double)(m_old_new_chromosome[p].get_allele_values().size()))*2000.0);//-1000.0;
      outfile << " " << pheno*pheno << " " << (pheno-2)*(pheno-2);
      outfile << " " << m_pop_p[p_size].rank;
      outfile << endl;
    }
  }
  outfile.close();
}

/* -------------------------------------------------------------- */

