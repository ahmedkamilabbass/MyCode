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
  int index;
  
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
MakeStruct (const vector <T> &objs, int _index )
{
  pop_struct<T> ret;
  ret.clearRelevantValues();
  ret.index = _index;
  ret.objective.insert     ( ret.objective.begin(), objs.begin(), objs.end() );
  return ret; 
}

class Emoo : public Population { 

 private:  
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
  void sort_selected_solutions_for_crowding_dist   ( vector < pop_struct <double> > &pop,
						     vector <int> &solutions_in_front_i );
  void nsga2_select                                ( vector < pop_struct <double> > &pop, vector <int> &selected,
						     int nb_to_select );
  
  void assign_fitness          ( void );
  void dumpStats               ( void );
  void dumpGenome              ( void );
  void breeding                ( void );

};

#endif
