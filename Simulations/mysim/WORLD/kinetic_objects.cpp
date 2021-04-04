#include "kinetic_objects.h"

/* ------------------------------------------------------------ */

Kinetic_Objects::Kinetic_Objects( void ) : World_Entity ( )
{
    rot.assign    (3, 0.0);
    dim.assign    (3, 0.0);
    colour.assign (3, 0.0);
    removed       = false;
    safe_dist     = 0.0;
}

Kinetic_Objects::Kinetic_Objects(const Kinetic_Objects& other)
    : World_Entity(other)
{
    copy(other);
}

Kinetic_Objects::~Kinetic_Objects()
{
    destroy();
}

/* ------------------------------------------------------------ */

void Kinetic_Objects::copy(const Kinetic_Objects &other)
{
    removed = other.removed;
    pos = other.pos;
    rot = other.rot;
    dim = other.dim;
    type_id = other.type_id;
    safe_dist = other.safe_dist;
}

/* ------------------------------------------------------------ */

void Kinetic_Objects::destroy()
{
    pos.clear();
    rot.clear();
    dim.clear();
}

/* ------------------------------------------------------------ */

Kinetic_Objects& Kinetic_Objects::operator=(const Kinetic_Objects &other)
{
    if(this != &other)
    {
        destroy();
        copy(other);
    }
    return *this;
}

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------- */
/* BRICK   BRICK   BRICK   BRICK   BRICK   BRICK   BRICK   BRICK */
/* ------------------------------------------------------------- */

Kinetic_Brick::Kinetic_Brick( const vector <double> & data )
    : Kinetic_Objects( )
{
    pos.assign    (9, 0.0);
    pos[0] = data[0];//X position of the segment centre
    pos[1] = data[1];//Y position of the segment centre
    pos[2] = data[2];//Z position of the segment centre

    dim[0] = data[3]; //Length on the X axis
    dim[1] = data[4]; //Width on the Y axis
    dim[2] = data[5]; //Height on the Z axis
    safe_dist = dim[0] + DIST_OFFSET_SECURITY;

    rot[2] = data[6]; //Rotation on the XY plain
    double angle = (rot[2]*PI)/180.0;
     
    pos[3] = pos[0] + ((dim[0]*0.5) * cos(angle)); //X position of one segment end
    pos[4] = pos[1] + ((dim[0]*0.5) * sin(angle)); //Y position of one segment end
    pos[5] = pos[2];                               //Z position of the segment centre
    if(pos[3] > 0.0 && pos[3] < 0.00001) pos[3] = 0.0;
    else if(pos[3] < 0.0 && pos[3] > -0.00001) pos[3] = 0.0;
    if(pos[4] > 0.0 && pos[4] < 0.00001) pos[4] = 0.0;
    else if(pos[4] < 0.0 && pos[4] > -0.00001) pos[3] = 0.0;
 
    pos[6] = pos[0] - ((dim[0]*0.5) * cos(angle)); //X position of the opposite segment end
    pos[7] = pos[1] - ((dim[0]*0.5) * sin(angle)); //Y position of the opposite segment end
    pos[8] = pos[2];                               //Z position of the segment centre
    if(pos[6] > 0.0 && pos[6] < 0.00001) pos[6] = 0.0;
    else if(pos[6] < 0.0 && pos[6] > -0.00001) pos[6] = 0.0;
    if(pos[7] > 0.0 && pos[7] < 0.00001) pos[7] = 0.0;
    else if(pos[7] < 0.0 && pos[7] > -0.00001) pos[7] = 0.0;
      
    colour[0] = data[7]; //RED
    colour[1] = data[8]; //GREEN
    colour[2] = data[9]; //BLUE

    type_id  = BRICK;
}

/* ------------------------------------------------------------ */

Kinetic_Brick::Kinetic_Brick( const Kinetic_Brick& other ) 
	: Kinetic_Objects (other)
{
}

/* ------------------------------------------------------------ */

Kinetic_Brick::~Kinetic_Brick()
{
    destroy();
}

/* ------------------------------------------------------------ */

void Kinetic_Brick::copy(const Kinetic_Brick &other)
{
}

/* ------------------------------------------------------------ */

void Kinetic_Brick::destroy()
{
}

/* ------------------------------------------------------------ */

Kinetic_Brick& Kinetic_Brick::operator=(const Kinetic_Brick &other)
{
    if(this != &other)
    {
        destroy();
        copy(other);
    }
    return *this;
}

/* ------------------------------------------------------------ */

#ifdef _GRAPHICS_
void Kinetic_Brick::render ( void ){
  glColor3f(colour[0], colour[1], colour[2]);
    float m[16];
    m[ 0] = 1.0;
    m[ 1] = 0.0;
    m[ 2] = 0.0;
    m[ 3] = 0;
    m[ 4] = 0.0;
    m[ 5] = 1.0;          
    m[ 6] = 0.0;
    m[ 7] = 0;
    m[ 8] = 0.0;
    m[ 9] = 0.0;
    m[10] = 1.0;
    m[11] = 0;
    m[12] = pos[0];
    m[13] = pos[1];
    m[14] = pos[2];
    m[15] = 1.0;
    glPushMatrix();
    glMultMatrixf(m);
    glRotatef(rot[2], 0, 0, 1);// (rot[2]*180)/PI
    glScalef(dim[0],dim[1],dim[2]);
    glutSolidCube(1.0f);
    glPopMatrix();
}
#endif
/* ------------------------------------------------------------ */
/* CYLINDER    CYLINDER    CYLINDER    CYLINDER    CYLINDER     */
/* ------------------------------------------------------------ */

Kinetic_Cylinder::Kinetic_Cylinder( const vector <double> & data ) : Kinetic_Objects(  )
{
    pos.assign    (3, 0.0);
    pos[0] = data[0]; //X position
    pos[1] = data[1]; // Y position
    pos[2] = data[2]; // Z position

    dim[0] = data[4]; //width = dim[0] = radius
    dim[1] = data[5]; //height = dim[1] = height
    dim[2] = data[3];
    safe_dist = dim[0] + DIST_OFFSET_SECURITY;

    rot[2] = data[6]; // XY rotation

    colour[0] = data[7]; // RED
    colour[1] = data[8]; // GREEN
    colour[2] = data[9]; // BLUE

    type_id  = CYLINDER;
}

/* ------------------------------------------------------------ */

Kinetic_Cylinder::Kinetic_Cylinder( const Kinetic_Cylinder& other )
    : Kinetic_Objects (other)
{
}

/* ------------------------------------------------------------ */

Kinetic_Cylinder::~Kinetic_Cylinder()
{
    destroy();
}

/* ------------------------------------------------------------ */

void Kinetic_Cylinder::copy(const Kinetic_Cylinder &other)
{
}

/* ------------------------------------------------------------ */

void Kinetic_Cylinder::destroy()
{
}

/* ------------------------------------------------------------ */

Kinetic_Cylinder& Kinetic_Cylinder::operator=(const Kinetic_Cylinder &other)
{
    if(this != &other)
    {
        destroy();
        copy(other);
    }
    return *this;
}

/* ------------------------------------------------------------ */

#ifdef _GRAPHICS_
void Kinetic_Cylinder::render ( void ){
    glColor3f(colour[0], colour[1], colour[2]);
    /*float m[16];
  m[ 0] = 1.0;
  m[ 1] = 0.0;
  m[ 2] = 0.0;
  m[ 3] = 0;
  m[ 4] = 0.0;
  m[ 5] = 1.0;
  m[ 6] = 0.0;
  m[ 7] = 0;
  m[ 8] = 0.0;
  m[ 9] = 0.0;
  m[10] = 1.0;
  m[11] = 0;
  m[12] = pos[0];
  m[13] = pos[1];
  m[14] = pos[2];
  m[15] = 1;*/

    glPushMatrix();
    //glMultMatrixf(m);
    glTranslatef(pos[0], pos[1], pos[2]);
    GLUquadric* params_quadric = gluNewQuadric();
    glColor3f(colour[0], colour[1], colour[2] );
    gluCylinder(params_quadric, dim[0], dim[0], dim[1], robot_slices, 1);
    gluDisk(params_quadric, 0, dim[0], robot_slices, 1);
    glTranslated(0,0,dim[1]);
    gluDisk(params_quadric, 0, dim[0], robot_slices, 1);
    gluDeleteQuadric(params_quadric);
    /*glBegin(GL_TRIANGLES);
  for (int i = 0; i<100; i++)
    {
      float n = i*(M_PI/50);
      //glColor3f((n/2),1-(n/2),sin(n));
      glVertex3f((dim[0]*sin(n)),(dim[0]*cos(n)), 0);
      n = (1+i)*(M_PI/50);
      glVertex3f((dim[0]*sin(n)),(dim[0]*cos(n)), 0);
      glVertex3f(0, 0, 0);
    }
  glEnd();
  GLUquadric* quad = gluNewQuadric();
  gluCylinder( quad, dim[0], dim[0], dim[1], 30, 30 );
  gluDeleteQuadric( quad );
  glTranslatef( 0, 0, dim[1] );
  glBegin(GL_TRIANGLES);
  for (int i = 0; i<100; i++)
    {
      float n = i*(M_PI/50);
      //glColor3f((n/2),1-(n/2),sin(n));
      glVertex3f((dim[0]*sin(n)),(dim[0]*cos(n)), 0);
      n = (1+i)*(M_PI/50);
      glVertex3f((dim[0]*sin(n)),(dim[0]*cos(n)), 0);
      glVertex3f(0, 0, 0);
    }
  glEnd();*/
    glPopMatrix();
}
#endif

/* ------------------------------------------------------------ */
/* SPHERE    SPHERE    SPHERE    SPHERE    SPHERE     SPHERE    */
/* ------------------------------------------------------------ */

Kinetic_Sphere::Kinetic_Sphere( const vector <double> & data ) : Kinetic_Objects( ) 
{
    pos.assign    (3, 0.0);
    pos[0] = data[0]; // X position
    pos[1] = data[1]; // Y position
    pos[2] = data[2]; // Z position

    dim[0] = data[3]; 
    dim[1] = data[4];
    dim[2] = data[5];
    safe_dist = dim[0] + DIST_OFFSET_SECURITY;

    rot[2] = data[6]; // XY rotation

    colour[0] = data[7]; // RED
    colour[1] = data[8]; // GREEN
    colour[2] = data[9]; // BLUE

    type_id  = SPHERE;
}

Kinetic_Sphere::Kinetic_Sphere( const Kinetic_Sphere& other )
    : Kinetic_Objects (other)
{
}

/* ------------------------------------------------------------ */

Kinetic_Sphere::~Kinetic_Sphere()
{
    destroy();
}

/* ------------------------------------------------------------ */

void Kinetic_Sphere::copy(const Kinetic_Sphere &other)
{
}

/* ------------------------------------------------------------ */

void Kinetic_Sphere::destroy()
{
}

/* ------------------------------------------------------------ */

Kinetic_Sphere& Kinetic_Sphere::operator=(const Kinetic_Sphere &other)
{
    if(this != &other)
    {
        destroy();
        copy(other);
    }
    return *this;
}

/* ------------------------------------------------------------ */

#ifdef _GRAPHICS_
void Kinetic_Sphere::render ( void ){
    float m[16];
    m[ 0] = 1.0;
    m[ 1] = 0.0;
    m[ 2] = 0.0;
    m[ 3] = 0;
    m[ 4] = 0.0;
    m[ 5] = 1.0;
    m[ 6] = 0.0;
    m[ 7] = 0;
    m[ 8] = 0.0;
    m[ 9] = 0.0;
    m[10] = 1.0;
    m[11] = 0;
    m[12] = pos[0];
    m[13] = pos[1];
    m[14] = pos[2];
    m[15] = 1;

    glColor3f(colour[0], colour[1], colour[2]);
    glPushMatrix();
    glMultMatrixf(m);
    glutSolidSphere(dim[0],100,100);
    glPopMatrix();
}
#endif
/* ------------------------------------------------------------ */

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
