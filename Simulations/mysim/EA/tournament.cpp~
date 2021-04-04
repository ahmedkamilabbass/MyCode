#include "roulette_wheel.h"

/* -------------------------------------------------------------- */

Roulette_wheel::Roulette_wheel ( unsigned int id, const Parameters* params, const int nb_alleles, const int nb_bases_per_allele ) : Population ( id, params, nb_alleles, nb_bases_per_allele ){}

/* -------------------------------------------------------------- */

Roulette_wheel::~Roulette_wheel ( void ){}

/* -------------------------------------------------------------- */

void Roulette_wheel::assign_fitness( void ){
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

void Roulette_wheel::breeding ( void ) {
  
  compute_probabilities_of_selection();

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

void Roulette_wheel::compute_probabilities_of_selection ( void ){
  //transform all fitness values in positive numbers
  if( m_parameters->getTypeOfDominance()[0] == "maximise"){
    if( fitness[0].value < 0.0 ){
      m_sum_of_fitness = 0.0;
      double min_val = fabs ( fitness[0].value );
      for(int i = 0; i < fitness.size(); i++){
	fitness[i].value += min_val;
	m_sum_of_fitness += fitness[i].value;
      }
    }
  }
  else{//minimise
    if( fitness[0].value < 0.0 ){
      cerr <<" In Roulette_wheele.cpp : the fitness can not be smaller than zero " << endl;
      exit(EXIT_FAILURE);
    }
  }
  proporational_fitness_selection();
  
  //if( m_parameters->getCurrentGeneration() == 0 )
  //rank_based_selection( );
}

/* -------------------------------------------------------------- */

void Roulette_wheel::proporational_fitness_selection( void ){
  vector <double> ProbSelection;

  for(int i = 0; i < fitness.size(); i++){
    if( m_sum_of_fitness )
      ProbSelection.push_back(  fitness[i].value/m_sum_of_fitness  );
    else
      ProbSelection.push_back( 0.0 );
  }
  double probSum = 0.0;
  wheelProbList.erase(wheelProbList.begin(), wheelProbList.begin() + wheelProbList.size() );
  wheelProbList.clear();
  if( m_parameters->getTypeOfDominance()[0] == "maximise"){
    for(int i = 0; i < fitness.size(); i++){
      probSum += ProbSelection[i];
      wheelProbList.push_back( probSum );
    }
    if( !m_parameters->getNbSolutionsTruncated() )
      trunc_threshold = 0.0;
    else
      trunc_threshold = wheelProbList[m_parameters->getNbSolutionsTruncated()];
  }
  else{
    for(int i = 0; i < fitness.size(); i++){
      probSum += ((1.0 - ProbSelection[i])) / ((double)(fitness.size()-1));
      wheelProbList.push_back( probSum );
    }
    if( !m_parameters->getNbSolutionsTruncated() )
      trunc_threshold = 1.0;
    else
      trunc_threshold = wheelProbList[fitness.size() - m_parameters->getNbSolutionsTruncated() - 1];
  }

  /*  for(int i = 0; i < fitness.size(); i++){
    cerr << " wheelProblist["<< i <<"] = " << wheelProbList[i]
	 << "  ProbSelection["<<i<<"] = " << ProbSelection[i]
	 << " Fit = " << fitness[i].value
	 << " sum fit = " << m_sum_of_fitness
	 << endl;
  }
  getchar(); */
}

// /* -------------------------------------------------------------- */

// void Roulette_wheel::rank_based_selection( void ){
//   double selective_pressure = 2.0;
  
//   ProbSelection.erase(ProbSelection.begin(), ProbSelection.begin() + ProbSelection.size() );
//   ProbSelection.clear();
//   for(int i = 0; i < fitness.size(); i++){
//     ProbSelection.push_back( (2.0 - selective_pressure) + 
// 			     2.0 * (selective_pressure - 1.0)*
// 			     ((double)(i)/(double)(fitness.size()-1)) );
   
//   }
//   for(int i = 0; i < fitness.size(); i++){
//     ProbSelection[i] /= ProbSelection.back();
//     //cerr << " ProbSelection[ "<< i <<"] = " << ProbSelection[i] << endl;
//   }
//   //getchar();
// }

// /* -------------------------------------------------------------- */

void Roulette_wheel::select_parents ( vector <int> &selected ){
  selected.erase(selected.begin(), selected.begin() + selected.size());
  selected.clear();
  double NbRnd;
  bool flag_continue = true;
  do{
    do{
      flag_continue = true;
      if( m_parameters->getTypeOfDominance()[0] == "minimise") {
	NbRnd = gsl_rng_uniform (GSL_randon_generator::r_rand) * trunc_threshold;
	//cerr << " trunc_t = " << trunc_threshold << " nbRnd = " << NbRnd << endl;
	//getchar();
      }
      else
	NbRnd = trunc_threshold + (gsl_rng_uniform (GSL_randon_generator::r_rand) * (1.0 - trunc_threshold));
      for(int i=0; i < wheelProbList.size(); i++ ){
	if( NbRnd < wheelProbList[i] ){
	  selected.push_back( i );
	  break;
	}
      }
      for(int i=0; i<selected.size()-1; i++){
	if( selected.back() == selected[i] ){
	  flag_continue = false;
	  selected.pop_back();
	  break;
	}
      }
    }while(flag_continue == false);
  }while( selected.size() < 2);  
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
	   << " " << fitness.back().value //The highest fitness value
	   << " " << m_sum_of_fitness/(double)(fitness.size()) // The mean fitness
	   << " " << fitness[0].value //The lowest fitness value 
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
