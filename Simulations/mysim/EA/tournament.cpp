#include "tournament.h"

/* -------------------------------------------------------------- */

Tournament::Tournament ( unsigned int id, const Parameters* params, const int nb_alleles, const int nb_bases_per_allele ) : Population ( id, params, nb_alleles, nb_bases_per_allele ){
  m_tournament_size   = m_parameters->getTournamentSize();
}

/* -------------------------------------------------------------- */

Tournament::~Tournament ( void ){}

/* -------------------------------------------------------------- */

void Tournament::assign_fitness( void ){
  m_sum_of_fitness = 0.0;
  fitness.erase(fitness.begin(), fitness.begin() + fitness.size() );
  fitness.clear();
  
  for(int i = 0; i < store_fitness.size(); i++){
    fitness.push_back( MakeValueWithIndex(store_fitness[i][0], i) );
    m_sum_of_fitness += store_fitness[i][0]; // store_fitness[i][0] is the fitness of individual i for the objective 0
  }
  std::sort(fitness.begin(), fitness.end(), acompare<double> ); // sort in ascending order from lower to higher value
  /* for(int i = 0; i < fitness.size(); i++){
     cerr << " Ind = " << i << " index = " << fitness[i].index << " fit value = " << fitness[i].value << endl;
     }
     getchar();n*/
  
}

/* -------------------------------------------------------------- */

void Tournament::breeding ( void ) {
  
  //Set here the elite
  vector <SingleStr <int>> elite;
  if( m_parameters->getElite() ){
    for (int e = 0; e < elite_size; e++){
      if( m_parameters->getTypeOfDominance()[0] == "maximise"){
	elite.push_back( MakeSingle ( fitness[fitness.size()-1-e].index ) );
      }
      else{
	elite.push_back( MakeSingle ( fitness[e].index ) );
      }
    }
  }

  //Choose for reproduction
  vector <CoupleStr <int>> parents;
  vector <int> selected;
  for (int i = 0; i < nb_new_ind; i++ ){
    select_parents ( selected );
    parents.push_back( MakeCouple (fitness[selected[0]].index, fitness[selected[1]].index ) );
  }

  if( !(m_parameters->getCurrentGeneration() % m_parameters->getDumpGenomeEvery() ) )
    dumpGenome();
  
  (this->*m_breeding_fn_ptr)( elite, parents );
  copy_chromosome_into_working_pop();
}

/* -------------------------------------------------------------- */

void Tournament::select_parents ( vector <int> &selected ){
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
	tmp_random_int = gsl_rng_uniform_int( GSL_randon_generator::r_rand, fitness.size() );
	for(int i=0; i<k; i++){
	  if( tmp_random_int == s[i]){
	    flag_continue = false;
	    break;
	  }
	}
      }while(flag_continue == false);

      s.push_back( tmp_random_int );
	  
      if( k == 0 ){
	selected[p] = s[k];
      }
      else{
	if( m_parameters->getTypeOfDominance()[0] == "minimise"){
	  if( fitness[s[k]].value < fitness[selected[p]].value )
	    selected[p] = s[k];
	}
	else{
	  if( fitness[s[k]].value > fitness[selected[p]].value )
	    selected[p] = s[k];
	}
      }
    }
    s.erase( s.begin(), s.begin() + s.size() );
    p++;
  }while(p < 2);
}

/* -------------------------------------------------------------- */

void Tournament::dumpStats( void )
 {
   char fname_statistics[500];
   sprintf(fname_statistics, "%s%s%d.data", m_parameters->getStatsFileDir().c_str(), m_parameters->getRunName().c_str(), process_id ); 
   ofstream outfile;
   outfile.open (fname_statistics, ios::app);
   outfile.setf(ios::fixed);
  
   outfile << " " << m_parameters->getCurrentGeneration()
	   << " " << fitness.back().value //The highest fitness value
	   << " " << m_sum_of_fitness/(double)(fitness.size()) // The mean fitness
	   << " " << fitness[0].value //The lowest fitness value 
	   << endl;
   outfile.close();
}

/* ---------------------------------------- */

void Tournament::dumpGenome( void )
{
  char fname_genome[500];
  sprintf(fname_genome, "%s%s%d_pop_G%d.geno", m_parameters->getGenomeFileDir().c_str(), m_parameters->getRunName().c_str(), process_id, m_parameters->getCurrentGeneration() );
  ofstream out ( fname_genome );
  out.setf( ios::fixed );

  int element, index_chromo, nb_tot_solutions;

  if( (!m_parameters->AreGroupsHeteroGroups() && !m_parameters->IsSelectionIndividual()) ||
      ( m_parameters->AreGroupsHeteroGroups() &&  m_parameters->IsSelectionIndividual()) )
    nb_tot_solutions = m_nb_groups * m_nb_chromosome_per_group;
  else if( !m_parameters->AreGroupsHeteroGroups() && m_parameters->IsSelectionIndividual() )
    nb_tot_solutions = m_nb_groups * m_parameters->getNbAgentsPerGroup();
  else if( m_parameters->AreGroupsHeteroGroups() && !m_parameters->IsSelectionIndividual() )
    nb_tot_solutions = m_nb_groups;
  
  for(unsigned int p=0; p< nb_tot_solutions; p++ ) {

    if( m_parameters->getTypeOfDominance()[0] == "maximise")
      element = nb_tot_solutions-1-p;
    else
      element = p;
    
    if( (!m_parameters->AreGroupsHeteroGroups() && !m_parameters->IsSelectionIndividual()) ||
	( m_parameters->AreGroupsHeteroGroups() &&  m_parameters->IsSelectionIndividual()) ) {
      index_chromo = fitness[element].index;

      out << "" << m_chromosome[index_chromo].get_allele_values().size();
      for(int n=0; n<m_chromosome[index_chromo].get_allele_values().size(); n++){
	out << " " << setprecision(15) << m_chromosome[index_chromo].get_allele_values()[n]; 
      }
      out << " " << fitness[element].index;
      out << " " << fitness[element].value << endl;
    }
    else if( !m_parameters->AreGroupsHeteroGroups() && m_parameters->IsSelectionIndividual() ){
      index_chromo = ((int)(floor((double)(fitness[element].index) / (double)(m_parameters->getNbAgentsPerGroup()))));

      out << "" << m_chromosome[index_chromo].get_allele_values().size();
      for(int n=0; n<m_chromosome[index_chromo].get_allele_values().size(); n++){
	out << " " << setprecision(15) << m_chromosome[index_chromo].get_allele_values()[n]; 
      }
      out << " " << fitness[element].index;
      out << " " << fitness[element].value << endl;
    }
    else if( m_parameters->AreGroupsHeteroGroups() && !m_parameters->IsSelectionIndividual() ){
      index_chromo = fitness[element].index * m_parameters->getNbAgentsPerGroup();

      for(int j =0; j < m_parameters->getNbAgentsPerGroup(); j++ ){
	out << "" << m_chromosome[index_chromo+j].get_allele_values().size();
	for(int n=0; n<m_chromosome[index_chromo+j].get_allele_values().size(); n++){
	  out << " " << setprecision(15) << m_chromosome[index_chromo+j].get_allele_values()[n]; 
	}
	out << " " << fitness[element].index+j;
	out << " " << fitness[element].value << endl;
      }
    }
  }
  out.close();
}

/* ---------------------------------------- */
