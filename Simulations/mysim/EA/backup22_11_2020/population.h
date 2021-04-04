#ifndef _POPULATION_
#define _POPULATION_

#include "../MISC/general.h"
#include "../MISC/parameters.h"
#include "chromosome.h"

using namespace std;

template<class T>
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
}


class Population
{
 protected:
  const Parameters *m_parameters;

  vector < vector < Chromosome <chromosome_type> > > m_chromosome;
  vector < vector < Chromosome <chromosome_type> > > m_tmp_chromosome;

  int m_nb_groups;
  int m_nb_chromosome_per_group;
  vector < vector <double> >store_fitness;
  unsigned int process_id;

  void (Population::*m_breeding_fn_ptr)(vector < vector < Chromosome <chromosome_type> >> &, vector <ParentsStruct <int>> & );
  
 public:
  Population ( unsigned int id, const Parameters* params, const int nb_alleles, const int nb_bases_per_allele );     
  virtual ~Population();

  inline vector <chromosome_type> &                        get_solution     ( const int g, const int ind )  { return m_chromosome[g][ind].allele; }
  inline vector < vector < Chromosome <chromosome_type> >> get_chromosome   ( void )  { return m_chromosome; }  
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
  virtual void select_parents     ( vector <ParentsStruct<int>> &parents  ) = 0;
  

  void init_chromosomes                                  ( const int nb_alleles, const int nb_bases_per_allele );
  void create_new_homo_population                        ( vector < vector < Chromosome <chromosome_type> >> &chromo,
							   vector <ParentsStruct <int>> &elite );
  void create_new_hetero_population_group_selection      ( vector < vector < Chromosome <chromosome_type> >> &chromo,
							   vector <ParentsStruct<int>> &elite );
  void create_new_hetero_population_individual_selection ( vector < vector < Chromosome <chromosome_type> >> &chromo,
							   vector <ParentsStruct<int>> &elite );
};
#endif

