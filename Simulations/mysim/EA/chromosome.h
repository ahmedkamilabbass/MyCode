#ifndef _GENE_H_
#define _GENE_H_

#include <stdlib.h>
#include <math.h>
#include <vector>
#include "../MISC/general.h"

#define STD  0.1

using namespace std;

/* ------------------------------------------------------------------------------------------ */
// Gene Base class (float or double)
/* ------------------------------------------------------------------------------------------ */
template <class T>
class BasesT {
  
 public:

  T get_new_value  ( void ) {
    return get_new_real_value  ( );
    //return get_new_binary_value( );
  }

  void mutate_base ( T *base_, const double prob_mutation ){
    mutate_base_real_value   ( base_, prob_mutation );
    //mutate_base_binary_value ( base_, prob_mutation );
  }
    
  /* Init a random base in between (0, 1] */
  T  get_new_real_value  ( void ) {
    //static std::mt19937 generator(686452231);
    //std::uniform_real_distribution<> distribution(0.0, 1.0);
    //return (T)( distribution(generator) );
    return (T)(gsl_rng_uniform_pos( GSL_randon_generator::r_rand ));
  }

  T get_new_binary_value( void ){
    return (T)(gsl_rng_uniform_int(GSL_randon_generator::r_rand, 2) );
  }

  void mutate_base_binary_value ( T *base_, const double prob_mutation ){
    if( gsl_rng_uniform( GSL_randon_generator::r_rand ) < prob_mutation ) {
      if( *base_ == 1.0 ) *base_ = 0.0;
      else *base_ = 1.0;
    }
  }
  
  /* Mutate the base within [0, 1] */
  void mutate_base_real_value ( T *base_, const double prob_mutation ){
    if( gsl_rng_uniform( GSL_randon_generator::r_rand ) < prob_mutation ) {
      T offset    = (T)(gsl_ran_gaussian( GSL_randon_generator::r_rand, STD));
      T new_value = ( *base_ + offset );
      if( new_value > 1 ) {
	*base_ = 1;
      }
      else {
	if ( new_value < 0 ) {
	  *base_ = 0;
	}
	else {
	  *base_ = new_value;
	}
      }
    }
  }
};
/* ------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------ */
//Gene Base Class Specialisation (int)
/* ------------------------------------------------------------------------------------------ */
template <>
class BasesT <int> {

 public:

  int get_new_value  ( void ) {
    //return get_new_real_value  ( );
    return get_new_binary_value( );
  }

  void mutate_base ( int *base_, const double prob_mutation ){
    //mutate_base_real_value   ( base_, prob_mutation );
    mutate_base_binary_value ( base_, prob_mutation );
  }
  
  int get_new_real_value  ( void ){
    //generate a random int in 0 to (n-1) inclusive
    return (int)((gsl_rng_uniform_int(GSL_randon_generator::r_rand, (MAX_INT+1)) & 0xFF) );
    //return (int)(rand()%(MAX_INT+1) );
  }

  /* Init a random binary base */
  int get_new_binary_value( void ){
    return gsl_rng_uniform_int(GSL_randon_generator::r_rand, 2);
  }

  void mutate_base_binary_value ( int *base_, const double prob_mutation ){
    if( gsl_rng_uniform( GSL_randon_generator::r_rand ) < prob_mutation ) {
      if( *base_ == 1 ) *base_ = 0;
      else *base_ = 1;
    }
  }

  /* Mutate the base */
  void mutate_base_real_value ( int *base_, const double prob_mutation ){
    //for (int x = 0; x < 8; x++){
    if( gsl_rng_uniform(GSL_randon_generator::r_rand) < prob_mutation ) {
      /* 	int my_mask = 1; // 00 00 00 01 */
      /*   	my_mask = (my_mask << x); //swifth the 1 of x position on the left */
      /*   	*base_ = (*base_ ^ my_mask); // this is the XOR operator */
      /*   } */
      /* } */
      //cerr << " Init Base = " << ((double)(*base_)/255.0);
      double offset    = gsl_ran_gaussian( GSL_randon_generator::r_rand, 0.1);
      if( offset > 0.1 ) offset = 0.1;
      else if (offset < -0.1) offset = -0.1;
      double new_value = ( ((double)(*base_)/255.0) + offset );
      if( new_value > 1.0 ) { *base_ = 255;}
      else {
    	if ( new_value < 0 ) {*base_ = 0;}
    	else {
    	  *base_ = (int)(nearbyint(new_value*255.0));
    	}
      }
    }
  }
  
};
/* ------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------ */
template <class T> 
class Chromosome {
  
 private:
  bool diploid;
  vector <T> allele;
  BasesT <T> base_value;
  
 public:
  //Class Constructor
  Chromosome( bool isDiploid ){
    if( isDiploid ) diploid = true;
    else diploid = false;
    this->allele.erase( this->allele.begin(), this->allele.begin() + this->allele.size() );
    this->allele.clear();
  }
  
  //Class Destructor
  virtual ~Chromosome(){};
    
  friend class Population;
  //friend class Tournament;
  //friend class Swarm_ga;

  //Copy constructor
  Chromosome(const Chromosome <T> &obj)
    {
      this->diploid = obj.diploid;
      this->allele.erase( this->allele.begin(), this->allele.begin() + this->allele.size() );
      this->allele.clear();
      for (unsigned int a = 0; a < obj.allele.size(); a++){
	this->allele.push_back( obj.allele[a] );
      }
    }
  
  //Create a new assign operator
  Chromosome <T> & operator = (const Chromosome <T> &other)
    {
      this->allele.erase( this->allele.begin(), this->allele.begin() + this->allele.size() );
      this->allele.clear();
      if (this != &other) // protect against invalid self-assignment
        {
	  this->allele.resize( other.allele.size() );
	  for (unsigned int a = 0; a < other.allele.size(); a++){
	    //this->allele.push_back( other.allele[a] );
	    this->allele[a] = other.allele[a];
	  }
	  this->diploid = other.diploid;
	}
      return *this;
    }
  
  void print_allele( void ){
    for (unsigned int a = 0; a < this->allele.size(); a++){
      //cerr << "allele["<<a<<"] = " << allele[a] << endl;
      cerr << " " << allele[a];
    }
    cerr << endl;
  }
  
  inline void set_diploid ( bool d ) {diploid = d;}

  vector <T> & get_allele_values( void ){
    return allele;
  }

  void set_allele_values( vector <T> & alleles_){
    for (int a=0; a < alleles_.size(); a++)
      allele[a] = alleles_[a];
  }
  
  /* ---------------------------------------------------------------------------- */
  /*                                INIT ALLELE FUNCTION                          */
  /* ---------------------------------------------------------------------------- */
  void init_allele_values( const int num_alleles_, const int num_bases_per_allele_ ) {
    unsigned int totItems = 0;
    this->allele.erase( this->allele.begin(), this->allele.begin() + this->allele.size() );
    this->allele.clear();
    if( !diploid ){
      totItems = (num_alleles_ * num_bases_per_allele_);
    }
    else{
      totItems = (2.0 * num_alleles_ * num_bases_per_allele_);
    }
    for (unsigned int a = 0; a < totItems; a++){
      this->allele.push_back( this->base_value.get_new_value( ) );
    }
  }

  /* ---------------------------------------------------------------------------- */
  /*                                COPY ALLELE VALUES                            */
  /* ---------------------------------------------------------------------------- */
  void copy_allele_values( const vector <T> &obj ) {
    this->allele.erase( this->allele.begin(), this->allele.begin() + this->allele.size() );
    this->allele.clear();
    for (unsigned int a = 0; a < obj.size(); a++){
      this->allele.push_back( obj[a] );
    }
  }
  
  /* ---------------------------------------------------------------------------- */
  /*                                MUTATION FUNCTION                             */
  /* ---------------------------------------------------------------------------- */
  void create_with_mutate_operator ( const Chromosome <T> &other, const double prob_mutation ){
    this->allele.erase( this->allele.begin(), this->allele.begin() + this->allele.size() );
    this->allele.clear();
    /* cerr << " In mutate " << endl;
    for (unsigned int a = 0; a < other.allele.size(); a++)
      cerr << " mum.allele[" << a << "]= " << other.allele[a] << endl;
      getchar(); */
    for (unsigned int a = 0; a < other.allele.size(); a++){
      this->allele.push_back ( other.allele[a] );
      base_value.mutate_base ( &this->allele[a], prob_mutation );
    }
  }
  
  /* ---------------------------------------------------------------------------- */
  /*                               CROSSOVER FUNCTIONS                            */
  /* ---------------------------------------------------------------------------- */
  
  void create_with_cross_over_and_mutate_operators ( const Chromosome <T> &mum, const Chromosome <T> &dad, const double prob_mutation ){
    if( diploid ) { //the child is DIPLOID (d)
      if( dad.diploid ){ //the mum is DIPLOID (d) - the dad is DIPLOID (d)
        ddd_crossover( mum, dad, prob_mutation );
      }
      else if( !dad.diploid ){ //the mum is DIPLOID (d) - the dad is APLOID (a)
	dda_crossover( mum, dad, prob_mutation );
      }
    }
    else{ //the child is APLOID (a)
      if( !mum.diploid && !dad.diploid ){
	aaa_crossover( mum, dad, prob_mutation );//the mum is APLOID (a) - the dad is APLOID (a)
      }
      else if ( mum.diploid ) {
	adx_crossover( mum, dad, prob_mutation );//the mum is DIPLOID (d) - the dad is IRRELEVANT (x)
      }
    }
  } 
  
  /* ---------------------------------------------------------------------------- */
  /* FIRST CASE :: The child is DIPLOID - the mum is DIPLOID - the dad is DIPLOID */
  /* ---------------------------------------------------------------------------- */
  void ddd_crossover ( const Chromosome <T> &mum, const Chromosome <T> &dad, const double prob_mutation ){
    this->allele.erase( this->allele.begin(), this->allele.begin() + this->allele.size() );
    this->allele.clear();
    this->allele.resize( mum.allele.size() );	  
    for (unsigned int a = 0; a < this->allele.size(); a+=2){
      if( gsl_rng_uniform(GSL_randon_generator::r_rand) < 0.5 ) {
	this->allele[a] = mum.allele[a];
      }
      else{
	this->allele[a] = mum.allele[a+1];
      }
      if( gsl_rng_uniform(GSL_randon_generator::r_rand) < 0.5 ) {
	this->allele[a+1] = dad.allele[a];
      }
      else{
	this->allele[a+1] = dad.allele[a+1];
      }
      base_value.mutate_base ( &allele[a], prob_mutation );
      base_value.mutate_base ( &allele[a+1], prob_mutation );
    }
  }
  
  /* ---------------------------------------------------------------------------- */
  /* SECOND CASE :: The child is DIPLOID - the mum is DIPLOID - the dad is APLOID */
  /* ---------------------------------------------------------------------------- */
  void dda_crossover ( const Chromosome <T> &mum, const Chromosome <T> &dad, const double prob_mutation ){
    allele.resize( mum.allele.size() );
    int half = (int)(mum.allele.size()*0.5);
    for (unsigned int a = 0; a < this->allele.size(); a++){
      if ( a <  half ) {
	if( gsl_rng_uniform(GSL_randon_generator::r_rand) < 0.5 ) {
	  this->allele[a] = mum.allele[a];
	  base_value.mutate_base ( &allele[a], prob_mutation );
	}
	else{
	  this->allele[a] = mum.allele[half + a];
	  base_value.mutate_base ( &allele[a], prob_mutation );
	}
      }
      else {
	this->allele[a] = dad.allele[a - half];
	base_value.mutate_base ( &allele[a], prob_mutation );
      }
      //cerr << " this->allele["<<a<<"] = " <<  (int)this->allele[a] << endl;
    }
  }

  /* ------------------------------------------------------------------------------ */
  /* THIRD CASE :: The child is APLOID - the mum is DIPLOID - the dad is IRRELEVANT */
  /* ------------------------------------------------------------------------------ */
  void adx_crossover ( const Chromosome <T> &mum, const Chromosome <T> &dad, const double prob_mutation ){
    int half = (int)(mum.allele.size()*0.5);
    allele.resize( half, 0 );
    for (unsigned int a = 0; a < this->allele.size(); a++){
      if( gsl_rng_uniform(GSL_randon_generator::r_rand) < 0.5 ) {
	this->allele[a] = mum.allele[a];
	base_value.mutate_base ( &allele[a], prob_mutation );
      }
      else{
	this->allele[a] = mum.allele[half + a];
	base_value.mutate_base ( &allele[a], prob_mutation );
      }
      //cerr << " this->allele["<<a<<"] = " <<  (int)this->allele[a] << endl;
    }
  }
  
  /* ------------------------------------------------------------------------ */
  /* FORTH CASE ::The child is APLOID - the mum is APLOID - the dad is APLOID */
  /* ------------------------------------------------------------------------ */
  void aaa_crossover ( const Chromosome <T> &mum, const Chromosome <T> &dad, const double prob_mutation ){
    this->allele.erase( this->allele.begin(), this->allele.begin() + this->allele.size() );
    this->allele.clear();
    //cerr << " In cross over " << endl;
    //for (unsigned int a = 0; a < mum.allele.size(); a++)
    //cerr << " mum.allele[" << a << "]= " << mum.allele[a] << endl;
    //getchar();
    
    int cross_over_point = gsl_rng_uniform_int (GSL_randon_generator::r_rand, 1 + (dad.allele.size()-1) );
    if( gsl_rng_uniform(GSL_randon_generator::r_rand) < 0.5 ) {
      for (unsigned int a = 0; a < cross_over_point; a++){
	this->allele.push_back( dad.allele[a]);
	base_value.mutate_base ( &this->allele[a], prob_mutation );
      }
      for (unsigned int a = cross_over_point; a < mum.allele.size(); a++){
	this->allele.push_back( mum.allele[a]);
	base_value.mutate_base ( &this->allele[a], prob_mutation );
      }
    }
    else {
      for (unsigned int a = 0; a < cross_over_point; a++){
	this->allele.push_back( mum.allele[a]);
	base_value.mutate_base ( &this->allele[a], prob_mutation );
      }
      for (unsigned int a = cross_over_point; a < dad.allele.size(); a++){
	this->allele.push_back( dad.allele[a]);
	base_value.mutate_base ( &this->allele[a], prob_mutation );
      }
    }
  }

};

/* ------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------ */

#endif
