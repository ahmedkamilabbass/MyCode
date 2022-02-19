
/////////////////////////Libraries//////////////////////////////////

#include <webots/DistanceSensor.hpp>
#include <webots/Receiver.hpp>
#include <webots/Emitter.hpp>
#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <webots/LED.hpp>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cassert>
#include <random>
#include <chrono>
#include <vector>
#include <array>
using namespace webots;
/////////////////////////Constant//////////////////////////////////

#define NB_LEDS 10 
#define NUM_SENSORS 8
#define Emitter_Range 0.6
#define TIME_STEP 128
#define GROUND_SENSORS_NUMBER 3
#define MAX_SPEED 6.28
#define COMMUNICATION_CHANNEL 1
#define BLACK 0
#define WHITE 1
#define g 10

//////////////Webots Node Difinitions///////////////////////////////

DistanceSensor *ps[NUM_SENSORS];
Receiver       *receiver;  
Emitter        *emitter;   
Motor          *left_motor, *right_motor; 
Robot          *robot;
DistanceSensor *ground_sensors[GROUND_SENSORS_NUMBER];
LED            *led[NB_LEDS];  

//////////////////////// Difinitions///////////////////////////////

static std::vector<std::array<float,3>> My_Record;
static std::vector<float> Received_Color;
static std::vector<bool> Result_Vector;
static double ground_sensors_values[GROUND_SENSORS_NUMBER] = {0.0, 0.0, 0.0};
static double psValues[NUM_SENSORS] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
float  Black = 0, White = 0, Pr_Black = 0, Pr_White = 0;
static int    My_ID = -1;
static float  *inbuffer;
static float  outbuffer[3];
static float  Super_Outbuffer[3];
float  Final_Opinion = -1, Current_Opinion = -1;
float  Final_Opinion_Probability = -1, Current_Opinion_Probability = -1;
int    f = 0;
int    Previous = -1;
static int Register_Time = 20;;
static int Run_Time = 300;
std::ofstream Result_File;
std::string Results_Path = "/home/ahmed/Desktop/Results/SimpleSetup/DMVD/";
gsl_rng * r = gsl_rng_alloc (gsl_rng_mt19937);

//////////////////////// Enumeration///////////////////////////////

typedef enum {
  RANDOM_WALK = 0, 
  SPOT_ROTATION,
  OBSTACLE_AVOIDANCE,        
  QUALITY_ESTIMATION    
} RobotState;
RobotState State = RANDOM_WALK;

typedef enum {
  Exploration = 0,
  Dissemination
} RunMode;
static RunMode mode = Exploration;

typedef enum {
  DMMD_F = 0,
  DMVD_F,
  DC_F
} CDM_Algorthim;
static CDM_Algorthim Strategy = DMVD_F;

/////////////////////////Function Prototypes//////////////////////////

static float Random_Uniform(float LowerBound, float UpperBound);
static int   Random_Exponential(double mean);
static void  Reset();
static bool  LeftObstacle();
static bool  RightObstacle();
static void  RandomWalk();
static void  SpotRotation();
static void  ObstacleAvoidance();
static void  ReadDistanseSensors ();
static void  ReadGroundSensors ();
static void  switch_off_all_leds();
static void  switch_on_all_leds();
static void  Exchange_Message();
static void  DMVD();
static void  DMMD();
static void  DC();
static void  Run_Exploration();
static void  Run_Dissemination();
static void  Quality_Estimation();
static void  Get_Color();
static void  Record_Result();

////////////////////////Main Function/////////////////////////////////////

int main() {
  srand((unsigned)time(0));
  robot = new Robot();
  Reset();
  while(robot->step(TIME_STEP) != -1){
    switch(mode){
      case Exploration  : Run_Exploration();
           break;
      case Dissemination: Run_Dissemination();
           break;
      default:
           break;
    }
   }
  delete robot;
  return 0;
} 

//////////////////////////Reset///////////////////////////////////////////

void Reset(){
  receiver = robot->getReceiver("receiver");
  receiver->enable(TIME_STEP);
  emitter = robot->getEmitter("emitter");
  emitter->setChannel(COMMUNICATION_CHANNEL);
  emitter->setRange(Emitter_Range);
  
  static char psNames[NUM_SENSORS][4] = {
    "ps0", "ps1", "ps2", "ps3",
    "ps4", "ps5", "ps6", "ps7"
  };

  for (int i = 0; i < NUM_SENSORS; i++) {
    ps[i] = robot->getDistanceSensor(psNames[i]);
    ps[i]->enable(TIME_STEP);
  }
  
  static const char *ground_sensors_names[GROUND_SENSORS_NUMBER] = {"gs0", "gs1", "gs2"};
  for (int i = 0; i < GROUND_SENSORS_NUMBER; i++){
    ground_sensors[i]= robot->getDistanceSensor(ground_sensors_names[i]);
    ground_sensors[i]->enable(TIME_STEP);
 }
 
  left_motor = robot->getMotor("left wheel motor");
  right_motor = robot->getMotor("right wheel motor");
  left_motor->setPosition(INFINITY);
  right_motor->setPosition(INFINITY);
  left_motor->setVelocity(0.0);
  right_motor->setVelocity(0.0);
  
  char text[5] = "led0";
  for (int it  = 0; it < NB_LEDS; it++) {
    led[it]    = robot->getLED(text); 
    text[3]++;                        
  }
  
  std::string  robot_name, robotNumber1, robotNumber2;
  static int number1, number2;
  robot_name = robot->getName();
  if(robot_name.length() < 7){
    robotNumber1 = robot_name[5];
    My_ID   = std::stoi(robotNumber1);
  }
  else{
    robotNumber1 = robot_name[5];
    robotNumber2 = robot_name[6];
    number1 = std::stoi(robotNumber1);
    number2 = std::stoi(robotNumber2);
    My_ID = (number1 * 10) + number2;
  }
  //std::cout<<"ready......\n";
   // string path = "/home/ahmed/Desktop/Results/";
  Result_File.open (Results_Path + "myresult.txt", std::ios::trunc);
  Result_File.close();
 } 
//////////////////////////Run_Exploration//////////////////////////////////

static void  Run_Exploration(){
   if(!f)
   Get_Color();
   f = 1;
   float Exp_time = Random_Exponential(10);
   float Exp_Start_Time = robot->getTime();
   State = RANDOM_WALK;
   //std::cout<<"The Exploration mode is begin for: "<<Exp_time<<" seconds.........\n";
   while ((robot->getTime() - Exp_Start_Time) < Exp_time){
     switch(State)
     {
       case RANDOM_WALK:
          RandomWalk(); 
          break; 
        case SPOT_ROTATION:
          SpotRotation();    
          break;
        case OBSTACLE_AVOIDANCE:
         ObstacleAvoidance();
          break;
        default:
          break;
      }
    }
  mode = Dissemination;
  Quality_Estimation();
}

//////////////////////////Run_Exploration//////////////////////////////////

static void  Run_Dissemination(){
 float Diss_time;
 if(Strategy == DC_F)
    Diss_time = Random_Exponential(10);
 else 
   Diss_time = Random_Exponential(Final_Opinion_Probability * 10);  
   float Diss_Start_Time = robot->getTime();
   State = RANDOM_WALK;
  // std::cout<<"The Dissemination mode is begin for: "<<Diss_time<<" seconds.........\n";
   while ((robot->getTime() - Diss_Start_Time) < Diss_time){
     switch(State)
     {
       case RANDOM_WALK:
         RandomWalk(); 
          break; 
        case SPOT_ROTATION:
         SpotRotation();    
          break;
        case OBSTACLE_AVOIDANCE:
         ObstacleAvoidance();
          break;
        default:
          break;
      }
    }
  mode = Exploration;
  if(Strategy == DMMD_F)
    DMMD();
  else  if(Strategy == DMVD_F)
    DMVD();
  else
    DC();
}

//////////////////////////Exploration_RandomWalk/////////////////////////////

void RandomWalk(){
 int RandomWalk_Duratioin = Random_Exponential(40);
 int Walk_Start_Time = robot->getTime();
 while(((robot->getTime() - Walk_Start_Time) < RandomWalk_Duratioin) && State == RANDOM_WALK)
        {
          if((int)robot->getTime() % Register_Time == 0 && (int)robot->getTime() != Previous)
            Record_Result();
          if(RightObstacle() || LeftObstacle())
          {
          State = OBSTACLE_AVOIDANCE; 
          return;
          }
          right_motor->setVelocity( 0.8 * MAX_SPEED);
          left_motor->setVelocity ( 0.8 * MAX_SPEED);
          if(mode == Dissemination)
            Exchange_Message();
          else if(mode == Exploration){
            ReadGroundSensors();
            if((ground_sensors[1]->getValue()) < 500)
            Black++;
            else
            White++;
            robot->step(TIME_STEP);
          }
       robot->step(TIME_STEP);   
       }
 
 State = SPOT_ROTATION;
}

///////////////////////Exploration_SpotRotation//////////////////////////////

void SpotRotation(){
 int Rotation_Duratioin = Random_Uniform(0, 4.5);
 int Rotation_Start_Time = robot->getTime();
 if((int)Random_Uniform(0,2))
 {
  while(((robot->getTime() - Rotation_Start_Time) < Rotation_Duratioin) && State == SPOT_ROTATION)
        {
          if((int)robot->getTime() % Register_Time == 0 && (int)robot->getTime() != Previous)
            Record_Result();
          right_motor->setVelocity(  +0.5  * MAX_SPEED);
          left_motor->setVelocity(   -0.5  * MAX_SPEED);
          robot->step(TIME_STEP);
        }
       State = RANDOM_WALK;
       }
 else{
 while(((robot->getTime() - Rotation_Start_Time) < Rotation_Duratioin) && State == SPOT_ROTATION)
        {
          if((int)robot->getTime() % Register_Time == 0 && (int)robot->getTime() != Previous)
            Record_Result();
          right_motor->setVelocity(  -0.5  * MAX_SPEED);
          left_motor->setVelocity(   +0.5  * MAX_SPEED);
          robot->step(TIME_STEP);
        }
      State = RANDOM_WALK;
      }
}

/////////////////////Exploration_ObstacleAvoidance////////////////////////////

void ObstacleAvoidance(){
  if(RightObstacle() && !LeftObstacle())
  {
    while(RightObstacle() && State == OBSTACLE_AVOIDANCE)
          {
            if((int)robot->getTime() % Register_Time == 0 && (int)robot->getTime() != Previous)
            Record_Result();
            right_motor->setVelocity(  +0.5  * MAX_SPEED);
            left_motor->setVelocity(   -0.5  * MAX_SPEED);
            robot->step(TIME_STEP);
          }
     State = RANDOM_WALK;
     return;
  }
  else if(LeftObstacle() && !RightObstacle())
  {
    while(LeftObstacle() && State == OBSTACLE_AVOIDANCE)
          {
            if((int)robot->getTime() % Register_Time == 0 && (int)robot->getTime() != Previous)
              Record_Result();
            right_motor->setVelocity(  -0.5  * MAX_SPEED);
            left_motor->setVelocity (  +0.5  * MAX_SPEED);
            robot->step(TIME_STEP);
          }
     State = RANDOM_WALK;
     return;
  }
  else 
  {
  while(LeftObstacle() && RightObstacle() && State == OBSTACLE_AVOIDANCE)
          {
            if((int)robot->getTime() % Register_Time == 0 && (int)robot->getTime() != Previous)
              Record_Result();
            right_motor->setVelocity(  -0.5  * MAX_SPEED);
            left_motor->setVelocity (  -0.5  * MAX_SPEED);
            robot->step(TIME_STEP);
          }
     State = RANDOM_WALK;
     return;
  }
}

//////////////////////////ReadDistanseSensors/////////////////////////////

 void ReadDistanseSensors (){
    for (int i = 0; i < NUM_SENSORS ; i++)
    psValues[i] = ps[i]->getValue();
 }
 
////////////////////////////ReadGroundSensors////////////////////////////

void ReadGroundSensors (){
  for (int i = 0; i < GROUND_SENSORS_NUMBER; i++)
      ground_sensors_values[i] = ground_sensors[i]->getValue();
 }
 
///////////////////////////LeftObstacle//////////////////////////////////

bool LeftObstacle(){
  ReadDistanseSensors();
   bool left_obstacle =
      psValues[5] > 80.0 ||
      psValues[6] > 80.0 ||
      psValues[7] > 80.0;
      return left_obstacle;
}

//////////////////////////RightObstacle/////////////////////////////////

bool RightObstacle(){
  ReadDistanseSensors();
   bool right_obstacle =
      psValues[0] > 80.0 ||
      psValues[1] > 80.0 ||
      psValues[2] > 80.0;
      return right_obstacle;
}

/////////////////////////Random_Uniform/////////////////////////////////

float Random_Uniform(float LowerBound, float UpperBound)
{
    srand((unsigned int) time(0));
    std::chrono::system_clock::now().time_since_epoch().count();
    std::random_device random_device;
    std::default_random_engine random_engine(random_device());
    std::uniform_real_distribution<float> uniform_real_distribution(LowerBound, UpperBound);
    auto random_Float = uniform_real_distribution(random_engine);
    return random_Float;
}

//////////////////////Random_Exponential////////////////////////////////

static int Random_Exponential(double mean)
{
    unsigned long randSeed;
    randSeed = rand();
    gsl_rng_set (r, randSeed);
    unsigned int k = gsl_ran_exponential (r, mean);
    return k;
}

/////////////////////switch_off_all_leds////////////////////////////////

static void switch_off_all_leds() {
  int it;  // index
  for (it = 0; it < 8; it++)
   led[it]->set(0);  // switch off led[it]
}

////////////////////switch_off_all_leds/////////////////////////////////

static void switch_on_all_leds() {
  int it;  // index
  for (it = 0; it < 8; it++)
   led[it]->set(1);  // switch off led[it]
}

////////////////////Quality_Estimation/////////////////////////////////

static void  Quality_Estimation(){
  Pr_Black = Black / (Black + White);
  Pr_White = White / (Black + White);
  if(Black >= White)
    {
    Current_Opinion = BLACK;
    Current_Opinion_Probability = Pr_Black;
    //std::cout<<"My ID is: "<<My_ID <<" My opinion is: Black  and the probability value: "<<Pr_Black<<std::endl;
    }
    else if( White > Black)
    {
    Current_Opinion = WHITE;
    Current_Opinion_Probability = Pr_White;
    //std::cout<<"My ID is: "<<My_ID <<" My opinion is: White  and the probability value: "<<Pr_White<<std::endl;
    }
  //Black = 0;
  //White = 0;
  //Pr_Black = 0;
  //Pr_White = 0;
}

////////////////////Exchange_Message/////////////////////////////////

static void  Exchange_Message(){
  //Send Message
  outbuffer[0] = My_ID;
  outbuffer[1] = Current_Opinion;
  outbuffer[2] = Current_Opinion_Probability;
  emitter->send(outbuffer, 3 * sizeof(float));
  robot->step(TIME_STEP);
  //Receive Message
  if (receiver->getQueueLength() > 0){
    inbuffer = (float *)receiver->getData();
     //std::cout<<inbuffer[0]<<std::endl;
   //if(inbuffer[0] != 77){
    for(unsigned int i = 0; i < My_Record.size(); ++i){
      if(inbuffer[0] == My_Record[i][0]){
        receiver->nextPacket();
        return;
      }
     }    
  My_Record.push_back({inbuffer[0], inbuffer[1], inbuffer[2]});
  receiver->nextPacket();
  //}
 }
}

////////////////////DMVD//////////////////////////////////////////

static void  DMVD(){
 if(My_Record.size() > 0){
  int Index = (int)Random_Uniform(0, My_Record.size());
   Final_Opinion = My_Record[Index][1];
   Current_Opinion_Probability = My_Record[Index][2];
   }
 else {
   Final_Opinion = Current_Opinion;
   Final_Opinion_Probability = Current_Opinion_Probability;
   }
   
  //std::cout<<"the choosen index is:  "<<Index<<"  i will change to: "<<My_Record[Index][1]<<std::endl;
  if(Final_Opinion){
    //Final_Opinion = WHITE;
    led[8]->set(1);
    switch_off_all_leds();
    led[9]->set(0);
    robot->step(TIME_STEP);
  }
  else {
    //Final_Opinion = BLACK;
    led[8]->set(0);
    switch_on_all_leds();
    led[9]->set(0);
    robot->step(TIME_STEP);
  }
  My_Record.clear();   
}

////////////////////DMMD//////////////////////////////////////////

static void  DMMD(){
 My_Record.push_back({(float)My_ID, Current_Opinion, Current_Opinion_Probability});
 int BlackSum = 0, Whitesum = 0;
  for(int i = 0; i < My_Record.size(); ++i){
    if(My_Record[i][1] == BLACK) BlackSum++;
    else Whitesum++;
    }
  if(BlackSum < Whitesum){
    Final_Opinion = WHITE;
    led[8]->set(1);
    switch_off_all_leds();
    led[9]->set(0);
    robot->step(TIME_STEP);
  }
  else if(BlackSum >= Whitesum){
    Final_Opinion = BLACK;
    led[8]->set(0);
    switch_on_all_leds();
    led[9]->set(0);
    robot->step(TIME_STEP);
  }
  My_Record.clear();   
}

//////////////////////DC//////////////////////////////////////////

static void  DC(){
 if(My_Record.size() > 0){
  int Index = (int)Random_Uniform(0, My_Record.size());
  //std::cout<<"the choosen index is:  "<<Index<<"  i will change to: "<<My_Record[Index][1]<<std::endl;
 if(My_Record[Index][2] > Current_Opinion_Probability){
   Final_Opinion = My_Record[Index][1];
   Final_Opinion_Probability = My_Record[Index][2];
   }
 else{
   Final_Opinion = Current_Opinion;
   Final_Opinion_Probability = Current_Opinion_Probability;
   }
 }
 else{
   Final_Opinion = Current_Opinion;
   Final_Opinion_Probability = Current_Opinion_Probability;
   }
 
  if(Final_Opinion){
    led[8]->set(1);
    switch_off_all_leds();
    led[9]->set(0);
    robot->step(TIME_STEP);
  }
  else {
    led[8]->set(0);
    switch_on_all_leds();
    led[9]->set(0);
    robot->step(TIME_STEP);
  }
My_Record.clear();   
}

//////////////////////Get_Color//////////////////////////////////

static void  Get_Color(){
   //Receive all color from supervisor
   if (receiver->getQueueLength() > 0){
   //std::cout<<" inside Receiver------------ "<<std::endl;
    int *Color_Inbuffer = (int *)receiver->getData();
    for(unsigned int i = 1; i <= Color_Inbuffer[0]; ++i)
     Received_Color.push_back(Color_Inbuffer[i]);
    receiver->nextPacket();
   }
  
  //select my color and turn on the led
  int My_Color;
  for(unsigned int i = 0; i < Received_Color.size(); ++i)
  if(My_ID == i) My_Color = Received_Color[i];
  
  if(My_Color){
     Final_Opinion = WHITE;
     led[8]->set(1);
     switch_off_all_leds();
     led[9]->set(0);
     robot->step(TIME_STEP);
     // //std::cout<<" My ID is: "<< My_ID<<"  , Initial color is White..........  "<<std::endl;
  }
  else {
    Final_Opinion = BLACK;
    led[8]->set(0);
    switch_on_all_leds();
    led[9]->set(0);
    robot->step(TIME_STEP);
    //std::cout<<" My ID is: "<< My_ID<<"  , Initial color is Black..........  "<<std::endl;
  }
}

//////////////////////Record_Result//////////////////////////////////

static void Record_Result(){
  Previous = (int)robot->getTime();
  Result_Vector.push_back(Final_Opinion);
  if(Previous == Run_Time){
  Result_File.open (Results_Path + "myresult.txt", std::ios::app);
  Result_File<<std::endl;
  for(int i = 0; i < Result_Vector.size(); i++){
    Result_File<<Result_Vector[i];
    }
    Result_File.close();
   }
  //Super_Outbuffer[0] = 77;
  //Super_Outbuffer[1] = My_ID;
  //Super_Outbuffer[2] = Final_Opinion;
  //std::cout<<" My ID is: "<<My_ID<<"  My opinion is: "<<Final_Opinion<<std::endl;
  //emitter->send(Super_Outbuffer, 3 * sizeof(float));
  //robot->step(TIME_STEP);
  //std::cout<<" the time is:  "<<(int)robot->getTime()<<std::endl;
}





