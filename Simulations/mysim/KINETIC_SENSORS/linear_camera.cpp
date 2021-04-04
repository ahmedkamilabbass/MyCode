#include "linear_camera.h"

/* ---------------------------------------------------------------------------------------- */
Linear_Camera::Linear_Camera(const double _depth, const int num_pix, const  double pix_dim, const int _num_agents, const int _num_objects ){  
  depth           = _depth;
  num_pixel       = num_pix;
  pixel_dimension = pix_dim;
  num_agents      = _num_agents;
  num_objects     = _num_objects;
    
  allocate(num_agents, num_objects, num_pixel);
  
  distance_nearest_obj = depth;
  count_obj = 0;
  obj_dist.clear();
}

/* ---------------------------------------------------------------------------------------- */

Linear_Camera::Linear_Camera(const Linear_Camera& other)
{
  allocate(other.num_agents, other.num_objects, other.num_pixel);
  copy(other);
}

/* ---------------------------------------------------------------------------------------- */

Linear_Camera::~Linear_Camera()
{
  destroy();
}

/* ---------------------------------------------------------------------------------------- */

void Linear_Camera::copy(const Linear_Camera &other)
{
  depth = other.depth;
  num_pixel = other.num_pixel;
  pixel_dimension = other.pixel_dimension;
  num_agents = other.num_agents;
  num_objects = other.num_objects;
  count_obj = other.count_obj;
  distance_nearest_obj = other.distance_nearest_obj;
  camera_pixels = other.camera_pixels;
  obj_dist = other.obj_dist;
  
  for(int i = 0; i < (num_agents + num_objects); i++)
    {
      for (int j = 0; j < num_pixel; j++)
        {
	  for (int k = 0 ; k < 3 ; k++)
	    pixels[i][j][k] = other.pixels[i][j][k];
	  pixels_touched[i][j] = other.pixels_touched[i][j];
        }
    }
}

/* ---------------------------------------------------------------------------------------- */

void Linear_Camera::allocate(int numAgents, int numObjects, int numPixels)
{
  camera_pixels.resize(numPixels);
  for (int i = 0; i < camera_pixels.size(); i++)
    camera_pixels[i].resize(3);
  
  pixels.resize(numAgents + numObjects);
  pixels_touched.resize(numAgents + numObjects);
  for (int i = 0; i < numAgents + numObjects; i++)
    {
      pixels[i].resize(numPixels);
      pixels_touched[i].assign(numPixels, false);
      for (int j = 0 ; j < numPixels; j++)
        {
	  pixels[i][j].assign(3, 0.0);
        }
    }
}

/* ---------------------------------------------------------------------------------------- */

void Linear_Camera::destroy()
{
  camera_pixels.clear();
  pixels.clear();
  pixels_touched.clear();
}

/* ---------------------------------------------------------------------------------------- */

void Linear_Camera::update_pixel_rgb_values( World_Entity *agent, World_Entity *target){
  
  const vector <double> agent_dim  = agent->get_dim();
  const vector <double> target_dim = target->get_dim();
  double camera_noise = 0.0;
  
  if( target_dim[1] >= agent_dim[1] ){  //If object high enough for the camera
  
    const vector <double> agent_pos  = agent->get_pos();
    const vector <double> target_pos = target->get_pos();
    double x_offset  = (target_pos[0] - agent_pos[0]) + gsl_ran_gaussian(GSL_randon_generator::r_rand, camera_noise);
    double y_offset  = (target_pos[1] - agent_pos[1]) + gsl_ran_gaussian(GSL_randon_generator::r_rand, camera_noise);
    
    //If object close enough for the camera
    double distance  = sqrt( (x_offset*x_offset)+(y_offset*y_offset) );
    if( distance < depth  ){
      //cerr << " dist = " << distance << endl;
      obj_dist.insert( make_pair( distance, count_obj) );
      
      for (int i = 0; i < num_pixel; i++)
	pixels_touched[count_obj][i] = false;
      
      //Semi-aperture
      double obj_halfA = asin( target_dim[0] / distance );
      
      // Camera angle and semi-aperture
      double camera_halfA = (num_pixel * 0.5) * pixel_dimension;
      
      // Object angle
      double obj_angle = atan2( y_offset, x_offset );
      if( obj_angle < 0.0 ) obj_angle += TWO_PI;//2.0*M_PI + obj_angle;
      //cerr << " obj_angle = " << obj_angle << endl;
      
      // Angular position of the object with respect to the camera (between -PI and PI)
      double obj_toCamera = obj_angle - agent->get_rot()[2];
      if( obj_toCamera > PI ) obj_toCamera = obj_toCamera - 2.0 * PI;
      else if( obj_toCamera < -PI ) obj_toCamera = obj_toCamera + 2.0 * PI;
      
      // Computing the angular distance of the left object edge from the left camera edge.
      // < 0.0 means over left edge, > num_camera_pixel*pixel_dimension means over right edge.
      double lower_angle_from_left_edge = camera_halfA - (obj_toCamera + obj_halfA);
      // Computing the angular distance of the right object edge from the left camera edge.
      // < 0.0 means over left edge, > num_camera_pixel*pixel_dimension means over right edge.
      // Note: this is the same as lower_angle_from_left_edge + 2.0 * obj_halfA
      double upper_angle_from_left_edge = camera_halfA - (obj_toCamera - obj_halfA);
      
      // Computing left and right edge pixel (left included, right excluded)
      int pixel_lower_edge = int(floor(lower_angle_from_left_edge / pixel_dimension));
      if( pixel_lower_edge < 0 ) pixel_lower_edge = 0;
      else if( pixel_lower_edge > num_pixel ) pixel_lower_edge = num_pixel;
      int pixel_upper_edge = int(floor(upper_angle_from_left_edge / pixel_dimension)) + 1;
      if( pixel_upper_edge < 0 ) pixel_upper_edge = 0;
      else if( pixel_upper_edge > num_pixel ) pixel_upper_edge = num_pixel;
      
      // ... and then activating the right ones
      for (int p = pixel_lower_edge; p < pixel_upper_edge; p++) {
	pixels_touched[count_obj][p] = true;
	//cerr << " " << target_dim[2] << " " << agent_dim[1]
	//   << " pix_touched["<<count_obj<<"]["<<p<<"] = " << pixels_touched[count_obj][p];
	for (int j = 0; j < 3; j++){
	  pixels[count_obj][p][j] = target->get_colour()[j];
	  //cerr << " \n pix["<<count_obj<<"]["<<p<<"]["<<j<<"] " << pixels[count_obj][p][j];
	}
	//cerr  << endl;
      }
      count_obj++;
    }
  }
}

/* ---------------------------------------------------------------------------------------- */
void Linear_Camera::reset_camera ( void ){
  count_obj = 0;
  obj_dist.clear();
  // Now resetting all pixels...
  for(int obj = 0; obj < (num_agents + num_objects); obj++){//num. elements that can be seen
    for (int i = 0; i < num_pixel; i++){//num. camera sector
      for(int j = 0; j < 3; j++){//red green blue
	pixels[obj][i][j] = 0.0;
	camera_pixels[i][j] = 0.0;
      }
      pixels_touched[obj][i] = false;
    }
  }
}

/* ---------------------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------------------- */
void Linear_Camera::merge_camera_pixels_view( void ){
  distance_nearest_obj = depth;
  for (multimap<double, int >::reverse_iterator it = obj_dist.rbegin(); it != obj_dist.rend(); it++){
    for (int p = 0; p < num_pixel; p++){
      if( pixels_touched[(*it).second][p] ){
	distance_nearest_obj  = (*it).first;
	//cerr << " obj = " << (*it).second << " dist = " << (*it).first << endl;
	for (int j = 0; j < 3; j++){
	  camera_pixels[p][j] = pixels[(*it).second][p][j];
	  //cerr << " cam_pix["<<p<<"]["<<j<<"] =  " << camera_pixels[p][j];
	}
	//cerr << endl;
      }
    }
  }
}
/* ---------------------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------------------- */
void Linear_Camera::get_pixel_rgb ( vector <double> &pixel, int p ){
  for(int j = 0; j < 3; j++){
    pixel[j] = camera_pixels[p][j];
    //cerr << " II cam_pix["<<p<<"]["<<j<<"] =  " << camera_pixels[p][j];
  }
  //cerr << endl;
}
/* ---------------------------------------------------------------------------------------- */

#ifdef _GRAPHICS_
/* ---------------------------------------------------------------------------------------- */
void Linear_Camera::render( World_Entity *agent ){
  const vector <double> agent_pos  = agent->get_pos();
  for (int p = 0;  p <= num_pixel; p++){
    double unit = (num_pixel*0.5) - p;
    
    double camera_edge = agent->get_rot()[2] + (unit*pixel_dimension);
    if( camera_edge > TWO_PI) camera_edge -= TWO_PI;
    else if ( camera_edge < 0.0 ) camera_edge += TWO_PI;

    //renderCylinder(agent_pos[0], agent_pos[1], 0.055, agent_pos[0] + cos(camera_edge)*depth, agent_pos[1] + sin(camera_edge)*depth, 0.055, 0.04, 30);
    
    double x_projection = agent->get_pos()[0] + (depth * cos( camera_edge ));
    double y_projection = agent->get_pos()[1] + (depth * sin( camera_edge ));
    glEnable( GL_LINE_SMOOTH );
    glLineWidth(1.25);
    glColor3f(0.0, 0.5, 0.5);
    glBegin(GL_LINES);
    glVertex3f(agent->get_pos()[0], agent->get_pos()[1], agent->get_dim()[1]*0.9);
    glVertex3f(x_projection, y_projection, agent->get_dim()[1]*0.9);
    glEnd();
      
    /*
      double sensor_heading = agent->get_rot()[2] + epuck_sensors_angular_pos[i];    
      if(sensor_heading < 0.0 ) sensor_heading += TWO_PI;
      else if(sensor_heading > TWO_PI ) sensor_heading -= TWO_PI;
      
      double dist =  ir_ray_length[i] + agent->get_dim()[0]; 
      double x_projection = agent->get_pos()[0] + (dist * cos(sensor_heading));
      double y_projection = agent->get_pos()[1] + (dist * sin(sensor_heading));
      
      glEnable( GL_LINE_SMOOTH );
      glLineWidth(1.25);
      glColor3f(0.0, 0.8, 0.8);
      glBegin(GL_LINES);
      glVertex3f(agent->get_pos()[0], agent->get_pos()[1], agent->get_dim()[1]*0.9);
      glVertex3f(x_projection, y_projection, agent->get_dim()[1]*0.9);
      glEnd();*/
    
      
    
  }
}
/* ---------------------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------------------- */
void Linear_Camera::renderCylinder(float x1, float y1, float z1, float x2,float y2, float z2, float radius,int subdivisions)
{
  float vx = x2-x1;
  float vy = y2-y1;
  float vz = z2-z1;
  //handle the degenerate case with an approximation
  if(vz == 0)
    vz = .00000001;
  float v = sqrt( vx*vx + vy*vy + vz*vz );
  float ax = 57.2957795*acos( vz/v );
  if ( vz < 0.0 )
    ax = -ax;
  float rx = -vy*vz;
  float ry = vx*vz;
  
  GLUquadricObj *quadric=gluNewQuadric();
  gluQuadricNormals(quadric, GLU_SMOOTH);
  
  glPushMatrix();
  glTranslatef( x1,y1,z1 );
  glRotatef(ax, rx, ry, 0.0);
  //draw the cylinder
  gluCylinder(quadric, radius, radius, v, 32, 1);
  gluQuadricOrientation(quadric,GLU_INSIDE);
  //draw the first cap
  gluDisk( quadric, 0.0, radius, 32, 1);
  glTranslatef( 0,0,v );
  //draw the second cap
  gluQuadricOrientation(quadric,GLU_OUTSIDE);
  gluDisk( quadric, 0.0, radius, 32, 1);
  glPopMatrix();
  
  gluDeleteQuadric(quadric);
}

/* ---------------------------------------------------------------------------------------- */

Linear_Camera& Linear_Camera::operator=(const Linear_Camera &other)
{
  if(this != &other)
    {
      destroy();
      allocate(other.num_agents, other.num_objects, other.num_pixel);
      copy(other);
    }
  return *this;
}

/* ---------------------------------------------------------------------------------------- */
#endif
