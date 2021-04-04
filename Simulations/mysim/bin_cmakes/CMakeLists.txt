cmake_minimum_required(VERSION 2.8)

SET ( EXECUTABLE pippo )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3") 
#This is for heterogenous groups
#add_definitions(-D_HETERO_)

# UNCOMMENT FOR GRAPHICS
# add_definitions(-D_GRAPHICS_)

#FIND_PACKAGE( Qt COMPONENTS QtOpenGL REQUIRED )
#FIND_PACKAGE( Qt4 REQUIRED )
#SET( QT_USE_QTOPENGL TRUE )
#SET(  QT_USE_QTXML TRUE )
#INCLUDE( ${QT_USE_FILE} )

FILE ( GLOB SOURCES ./*.cpp ./MISC/*.cpp ./EVOL_ALGORITHM/*.cpp ./ROB_SENSORS_OBJS/*.cpp ./EXP/*.cpp ./CONTROLLERS/*.cpp)  
FILE ( GLOB HEADERS ./*.h ./MISC/*.h ./EVOL_ALGORITHM/*.h ./ROB_SENSORS_OBJS/*.cpp  ./EXP/*.h ./CONTROLLERS/*.h) 
#FILE ( GLOB UIS ./*.ui ./EXP/RENDERING/*.ui) 

#QT4_WRAP_CPP( SOURCES ${HEADERS} )
#QT4_WRAP_UI( SOURCES  ${UIS} )
#FIND_PACKAGE ( GLUT )  
#FIND_PACKAGE ( OpenGL )

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} /app/libraries/gsl/1.15/sb/intel-13.0/include /app/libraries/openmpi/1.6.4/sb/intel-13.0/include /home/muhanad.mohammed/bullet/src/)
# UNCOMMENT FOR GRAPHICS


#  UNCOMMENT FOR NO GRAPHICS
#FILE ( GLOB SOURCES ./*.cpp ./MISC/*.cpp ./CONTROLLERS/*.cpp ./EVOL_ALGORITHM/*.cpp   ./EXP/*.cpp )
#FILE ( GLOB HEADERS ./*.h ./MISC/*.h ./CONTROLLERS/*.h ./EVOL_ALGORITHM/*.h   ./EXP/*.h )
#INCLUDE_DIRECTORIES( /usr/include/ ${CMAKE_CURRENT_BINARY_DIR} )
#  UNCOMMENT FOR NO GRAPHICS

ADD_EXECUTABLE( ${EXECUTABLE} ${SOURCES})

# UNCOMMENT FOR GRAPHICS
#TARGET_LINK_LIBRARIES( ${EXECUTABLE} QGLViewer ${OPENGL_glu_LIBRARY}  ${GLUT_LIBRARIES} -lGL -lGLU  ${QT_LIBRARIES} )#revel xvidcore
TARGET_LINK_LIBRARIES( ${EXECUTABLE} "-lm -L/usr/lib64 -L/home/muhanad.mohammed/bullet/lib -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath -L/app/libraries/openmpi/1.6.4/sb/intel-13.0/lib -L/app/libraries/gsl/1.15/sb/intel-13.0/lib -lmpi -Bstatic -lgsl -lgslcblas -openmp" ) #qwt-6.1.1-svn/lib -lNewton
# UNCOMMENT FOR GRAPHICS

#  UNCOMMENT FOR NO GRAPHICS
#TARGET_LINK_LIBRARIES( ${EXECUTABLE} )
#TARGET_LINK_LIBRARIES( ${EXECUTABLE} "-L/sw/lib -lgsl -lgslcblas -lm" )
#  UNCOMMENT FOR NO GRAPHICS

