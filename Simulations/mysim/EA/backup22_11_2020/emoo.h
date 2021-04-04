#ifndef _EMOO_
#define _EMOO_

#include "population.h"
#include <limits>
#include <algorithm>

template<class T>
struct pop_struct
{
  vector <T> objective;
  //vector <T> genes;
  double crowding_distance;
  int domination_counter;
  vector <int> dominated_set;
  int rank;
  
  void clearRelevantValues ( void )
  {
    dominated_set.erase( dominated_set.begin(), dominated_set.begin() + dominated_set.size());
    dominated_set.clear();
    crowding_distance  = 0;
    domination_counter = 0;
    rank               = 0;
  }
};

  
template<class T> pop_struct<T>
MakeStruct (const vector <T> &objs /*, const vector <T> _genes */ )
{
  pop_struct<T> ret;
  ret.clearRelevantValues();
  ret.objective.insert     ( ret.objective.begin(), objs.begin(), objs.end() );
  return ret; 
}


class Emoo : public Population { 

 private:  
  vector < vector < Chromosome <chromosome_type> >> m_parents_offsprings_ch;
  vector <vector <pop_struct <double>* >> m_fronts;  
  vector <pop_struct <double>* >          m_selected_solutions;  
  vector <string>                         m_type_of_dominance;
  int                                     m_tournament_size;
  bool                                    m_first_round;
  vector < pop_struct <double> >          m_pop_p;  
  
 public:
  Emoo ( unsigned int id, const Parameters* params, const int nb_alleles, const int nb_bases_per_allele );
  virtual ~Emoo( );


  void update_domination_counter_and_dominated_set ( void );
  void compute_pareto_fronts                       ( vector < pop_struct <double> > &pop );
  void tournament_selection                        ( vector < pop_struct <double> > &pop, vector <int> &selected );
  void compute_crowding_distance                   ( vector < pop_struct <double> > &pop, vector <int> &single_front_solutions );



  
  void  fast_nondominated_sort                             ( void );
  bool  dominance                                          ( const int p, const int q );
  void  tournament_selection                               ( vector < vector < Chromosome <chromosome_type> >> &chromo );
  void  select_solutions_using_front_and_crowding_distance ( void );
  void  clear_all_vectors                                  ( void );
  void  sort_solutions_using_crowding_distance             ( const int current_front );
    
  void assign_fitness          ( void );
  void dumpStats               ( void );
  void dumpGenome              ( void );
  void breeding                ( void );


  void select_parents ( vector <ParentsStruct <int>> &parents  );
};

#endif
