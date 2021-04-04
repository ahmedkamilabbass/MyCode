#ifndef _ROULETTE_WHEEL_
#define _ROULETTE_WHEEL_

#include <numeric>
#include "population.h"

/* template<class T> */
/* struct ParentsInfo */
/* { */
/*   T mum_index; */
/*   T dad_index; */
/* }; */

/* template<class T> */
/* ParentsInfo<T> MakeMumAndDad(T mum_ind, T dad_ind ) */
/* { */
/*   ParentsInfo <T> ret; */
/*   ret.mum_index = mum_ind; */
/*   ret.dad_index = dad_ind; */
/*   return ret; */
/* } */

template<class T>
struct ValueWithIndex
{
  T value;
  int index;
};
//template<class T>
//bool operator < (const ValueWithIndex<T>& v1, const ValueWithIndex<T>& v2)
//{
//return v1.value < v2.value;
//}

template<class T>
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
}

class Roulette_wheel : public Population { 

 private:
  vector <double> ProbSelection;
  //vector < ParentsInfo <int> > m_parents;
  vector< ValueWithIndex <double> > fitness;
  double m_sum_of_fitness;
  
 public:
  Roulette_wheel ( unsigned int id, const Parameters* params, const int nb_alleles, const int nb_bases_per_allele );
  ~Roulette_wheel( );

  inline vector < ValueWithIndex <double> > getFitness      ( void )                        { return fitness; }
  
  //void (Roulette_wheel::*breeding_fn_ptr)();

  void assign_fitness                      ( void );
  void breeding                            ( void );
  void proporational_fitness_selection     ( void );
  void rank_based_selection                ( void );
  void compute_probabilities_of_selection  ( void );

  //void breeding_HomoGroupSelection         ( void );
  //void breeding_HomoIndividualSelection    ( void );
  //void breeding_HeteroGroupSelection       ( void );
  //void breeding_HeteroIndividualSelection  ( void );
  
  //void select_groups                       ( int *mum, int *dad );
  void dumpStats                           ( void );
  void dumpGenome                          ( void );
  inline vector <double> getProbSelection  ( void ) const {return ProbSelection;}

  void select_parents ( vector <ParentsStruct <int>> &parents );

};

#endif
