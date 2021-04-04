#ifndef _TOURNAMENT_SELECTION__
#define _TOURNAMENT_SELECTION__

#include <numeric>
#include "population.h"


class Tournament : public Population { 

 private:
  vector <double> wheelProbList;
  double m_sum_of_fitness;
  int    m_tournament_size;
  
 public:
  Tournament ( unsigned int id, const Parameters* params, const int nb_alleles, const int nb_bases_per_allele );
  ~Tournament( );

  inline vector < ValueWithIndex <double> > getFitness      ( void )                        { return fitness; }
  
  void assign_fitness                      ( void );
  void breeding                            ( void );
  void select_parents                      ( vector <int> &selected );
  void dumpStats                           ( void );
  void dumpGenome                          ( void );
 
};

#endif
