#ifndef _ROULETTE_WHEEL_
#define _ROULETTE_WHEEL_

#include <numeric>
#include "population.h"

/* template<class T>
struct ValueWithIndex
{
  T value;
  int index;
  };*/
//template<class T>
//bool operator < (const ValueWithIndex<T>& v1, const ValueWithIndex<T>& v2)
//{
//return v1.value < v2.value;
//}

/* template<class T>
bool dcompare(const ValueWithIndex<T>& v1, const ValueWithIndex<T>& v2) { return v1.value > v2.value; }
template<class T>
bool acompare(const ValueWithIndex<T>& v1, const ValueWithIndex<T>& v2) { return v1.value < v2.value; }

template<class T> ValueWithIndex<T>
MakeValueWithIndex(const T& value, int index)
{
  ValueWithIndex<T> ret;
  ret.value = value;
  ret.index = index;
  return ret; 
  }*/

class Roulette_wheel : public Population { 

 private:
  vector <double> wheelProbList;
  //vector< ValueWithIndex <double> > fitness;
  double m_sum_of_fitness;
  double trunc_threshold;
  
 public:
  Roulette_wheel ( unsigned int id, const Parameters* params, const int nb_alleles, const int nb_bases_per_allele );
  ~Roulette_wheel( );

  inline vector < ValueWithIndex <double> > getFitness      ( void )                        { return fitness; }
  
  void assign_fitness                      ( void );
  void breeding                            ( void );
  void proporational_fitness_selection     ( void );
  void rank_based_selection                ( void );
  void compute_probabilities_of_selection  ( void );
  void select_parents                      ( vector <int> &selected );
  void dumpStats                           ( void );
  void dumpGenome                          ( void );
 
};

#endif
