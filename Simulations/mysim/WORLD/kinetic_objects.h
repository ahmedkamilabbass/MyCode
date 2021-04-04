#ifndef _KINETIC_OBJECTS_
#define _KINETIC_OBJECTS_

#include "world_entity.h"

using namespace std;

class Kinetic_Objects : public World_Entity
{
  
 protected:
  bool removed;
  vector <double> pos;
  vector <double> rot;
  vector <double> dim;
  double safe_dist;
  
  int type_id;
  static const int robot_slices    = 20;
  
  void copy(const Kinetic_Objects &other);
  void destroy();

private:

public:
    Kinetic_Objects( );
    Kinetic_Objects(const Kinetic_Objects& other);
    virtual ~Kinetic_Objects();
    
    inline bool is_removed                   ( void )      { return removed; }
    inline void set_removed                  ( bool flag ) {
      removed = flag;
    }
    
    inline void set_pos ( const vector <double> &_pos ){
        pos[0] = _pos[0];
        pos[1] = _pos[1];
        pos[2] = _pos[2];
    }
    inline void set_rot ( const vector <double> &_rot ){
        rot[0] = _rot[0];
        rot[1] = _rot[1];
        rot[2] = _rot[2];
    }
    inline const vector <double> get_pos     ( void ){ return pos;}
    inline const vector <double> get_tmp_pos ( void ){ return pos;}
    inline const vector <double> get_rot     ( void ){ return rot;}
    inline const vector <double> get_dim     ( void ){ return dim;}
    inline const vector <double> get_colour  ( void ){ return colour;}
    inline const int             get_type_id ( void ){ return type_id;}
    inline const double          get_safe_dist ( void ) {return safe_dist;}
    
    Kinetic_Objects& operator=(const Kinetic_Objects &other);

#ifdef _GRAPHICS_
    virtual void    render                   ( void ) = 0;
#endif

};

/* ------------------------------------------------------------ */

class Kinetic_Brick : public Kinetic_Objects
{

private:
    void copy(const Kinetic_Brick &other);
    void destroy();

public:
    Kinetic_Brick( const vector <double> & data );
    Kinetic_Brick( const Kinetic_Brick& other );
    virtual ~Kinetic_Brick();

    Kinetic_Brick& operator=(const Kinetic_Brick &other);
#ifdef _GRAPHICS_
    void    render                   ( void );
#endif
};

/* ------------------------------------------------------------ */

class Kinetic_Cylinder : public Kinetic_Objects
{
private:
    void copy(const Kinetic_Cylinder &other);
    void destroy();

public:
    Kinetic_Cylinder( const vector <double> & data );
    Kinetic_Cylinder( const Kinetic_Cylinder& other );
    virtual ~Kinetic_Cylinder();

    Kinetic_Cylinder& operator=(const Kinetic_Cylinder &other);
#ifdef _GRAPHICS_
    void    render                   ( void );
#endif
};

/* ------------------------------------------------------------ */

class Kinetic_Sphere : public Kinetic_Objects
{
private:
    void copy(const Kinetic_Sphere &other);
    void destroy();

public:
    Kinetic_Sphere( const vector <double> & data );
    Kinetic_Sphere( const Kinetic_Sphere& other );
    virtual ~Kinetic_Sphere();

    Kinetic_Sphere& operator=(const Kinetic_Sphere &other);
#ifdef _GRAPHICS_
    void    render                     ( void );
#endif
};

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */

#endif

