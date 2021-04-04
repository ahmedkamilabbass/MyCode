#ifndef _POPULATION_
#define _POPULATION_

#include "../MISC/general.h"
#include "../MISC/parameters.h"
#include "chromosome.h"

using namespace std;

template<class T>
struct SingleStr
{
  T group;
};
template<class T>
SingleStr <T> MakeSingle (T gr )
{
  SingleStr<T> ret;
  ret.group   = gr;
  return ret;
}

template<class T>
struct CoupleStr
{
  T mum;
  T dad;
};
template<class T>
CoupleStr <T> MakeCouple (T _mum, T _dad )
{
  CoupleStr<T> ret;
  ret.mum = _mum;
  ret.dad = _dad;
  return ret;
}

/* template<class T> */
/* struct CoupleStr */
/* { */
/*   vector < struct SingleStr <T>> mum; */
/*   vector < struct SingleStr <T>> dad; */
/* }; */
/* template<class T> */
/* CoupleStr <T> MakeCouple (T mum_gr, T mum_subj, T dad_gr, T dad_subj ) */
/* { */
/*   CoupleStr<T> Cret; */
/*   Cret.mum.push_back( MakeSingle(mum_gr, mum_subj) ); */
/*   Cret.dad.push_back( MakeSingle(dad_gr, dad_subj) ); */
/*   return Cret; */
/* } */



/*template<class T>
struct ParentsStruct
{
  T group;
  T subject;
};
template<class T>
ParentsStruct <T> MakePairs (T gr, T subj )
{
  ParentsStruct<T> ret;
  ret.group   = gr;
  ret.subject = subj;
  return ret;
  }*/


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



class Population
{
 protected:
  const Parameters *m_parameters;

  vector < Chromosome <chromosome_type> > m_chromosome;
  vector < Chromosome <chromosome_type> > m_old_new_chromosome;
  vector < Chromosome <chromosome_type> > m_tmp_chromosome;

  int m_nb_groups;
  int m_nb_chromosome_per_group;
  int elite_size;
  int nb_new_ind;
  vector < vector <double> >store_fitness;
  vector< ValueWithIndex <double> > fitness;
  unsigned int process_id;

  void (Population::*m_breeding_fn_ptr)( vector <SingleStr <int>> &, vector <CoupleStr <int>> & );
  
 public:
  Population ( unsigned int id, const Parameters* params, const int nb_alleles, const int nb_bases_per_allele );     
  virtual ~Population();

  inline vector <chromosome_type> &get_solution ( const int i )  {
    return m_chromosome[i].allele;
  }
  inline vector < Chromosome <chromosome_type> > get_chromosome   ( void )  { return m_chromosome; }  
  inline unsigned int               get_nb_chromosome_per_group ( void ) { return m_nb_chromosome_per_group;}
  inline void                       setStoreFitness         ( const int i, const int j, const double v ) { store_fitness[i][j] = v; }
  inline void                       addAtStoreFitness       ( const int i, const int j, const double v ) { store_fitness[i][j] += v; }
  inline double                     get_store_fitness       ( const int i, const int j ) { return store_fitness[i][j];}
  inline vector < vector <double>>  get_store_fitness       ( void ) { return store_fitness;}
  
  void resetStoreFitness          ( void );
  void dumpStatsGenome            ( void );
    
  virtual void assign_fitness     ( void ) = 0;
  virtual void dumpStats          ( void ) = 0;
  virtual void dumpGenome         ( void ) = 0;
  virtual void breeding           ( void ) = 0;

  void init_chromosomes                                  ( const int nb_alleles, const int nb_bases_per_allele );

  void update_m_chromosome_list                          ( const vector <int> index );
  void copy_chromosome_into_working_pop                  ( void );
  void empty_m_tmp_chromosome                            ( void );
  void create_new_pop_homo_group_hete_ind                ( vector <SingleStr <int>> &, vector <CoupleStr <int>> &);
  void create_new_pop_homo_ind                           ( vector <SingleStr <int>> &, vector <CoupleStr <int>> &);
  void create_new_pop_hete_group                         ( vector <SingleStr <int>> &, vector <CoupleStr <int>> &);
};
#endif

