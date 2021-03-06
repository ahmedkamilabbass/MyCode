#include "ctrnn3Layers.h"

/* -------------------------------------------------------------------------------------- */

void  Ctrnn3Layers::read_from_file ( void )
{
  std::string absolute_path = (home + local_path).c_str();
  ifstream I ( (absolute_path + "/CONTROLLERS/ctrnn3Layers").c_str() );
  if(!I) {
    cerr << "File with network structure not found" <<endl;
    exit(0);
  }
  
  delta_t                 = getDouble   ('=', I);
  num_input               = getInt      ('=', I);
  num_hidden              = getInt      ('=', I);
  num_output              = getInt      ('=', I);
  
  low_bound_inputWts      = getDouble   ('=', I);
  upper_bound_inputWts    = getDouble   ('=', I);
  
  low_bound_hiddenWts     = getDouble   ('=', I);
  upper_bound_hiddenWts   = getDouble   ('=', I);
  low_bound_hiddenTau     = getDouble   ('=', I);
  upper_bound_hiddenTau   = getDouble   ('=', I);
  low_bound_hiddenBias    = getDouble   ('=', I);
  upper_bound_hiddenBias  = getDouble   ('=', I);
  
  low_bound_outputBias    = getDouble   ('=', I);
  upper_bound_outputBias  = getDouble   ('=', I);
  
  int check = getInt('=',I);
  I.close();
  if( check != 999 ){
    cerr << " In ../CONTROLLERS/ctrnn3Layers specifications' file --- End check was not 999" << endl;
    exit(0);
  }
}

/* -------------------------------------------------------------------------------------- */

void Ctrnn3Layers::compute_genotype_length ( void ){
  genotype_length = 0;
  
  //weights input-hidden
  genotype_length += num_input * num_hidden;
  
  //weights hidden-hidden
  genotype_length += num_hidden * num_hidden;
  
  //weights hidden-output
  genotype_length += num_hidden * num_output;
  
  //tau - it only applies to hidden nodes
  for(int h = 0; h < num_hidden; h++) genotype_length++;
  
  //bias - it applies to hidden and output nodes 
  for(int h = 0; h < num_hidden; h++) genotype_length++;
  for(int out = 0; out < num_output; out++) genotype_length++;

}

/* -------------------------------------------------------------------------------------- */

Ctrnn3Layers::Ctrnn3Layers( )
  : Controller()
{
  read_from_file ( );
  compute_genotype_length ( );
  
  allocate(num_input, num_hidden, num_output);
}

Ctrnn3Layers::Ctrnn3Layers(const Ctrnn3Layers& other)
{
  allocate(other.num_input, other.num_hidden, other.num_output);
  copy(other);
}

void Ctrnn3Layers::copy(const Ctrnn3Layers &other)
{
  num_input  = other.num_input;
  num_hidden = other.num_hidden;
  num_output = other.num_output;
  
  inputLayer  = other.inputLayer;
  hiddenLayer = other.hiddenLayer;
  outputLayer = other.outputLayer;
  
  low_bound_inputWts = other.low_bound_inputWts;
  upper_bound_inputWts = other.upper_bound_inputWts;
  
  low_bound_hiddenWts = other.low_bound_hiddenWts;
  upper_bound_hiddenWts = other.upper_bound_hiddenWts;
  low_bound_hiddenTau = other.low_bound_hiddenTau;
  upper_bound_hiddenTau = other.upper_bound_hiddenTau;
  low_bound_hiddenBias = other.low_bound_hiddenBias;
  upper_bound_hiddenBias = other.upper_bound_hiddenBias;
  
  low_bound_outputBias = other.low_bound_outputBias;
  upper_bound_outputBias = other.upper_bound_outputBias;
}

void Ctrnn3Layers::allocate(int numInput, int numHidden, int numOutput)
{
  inputLayer.resize(numInput);
  for(int i = 0 ; i < numInput ; i++){
    inputLayer[i].weightsOut.assign(numHidden, 0.0);
  }
  
  hiddenLayer.resize(numHidden);
  for(int i = 0 ; i < numHidden ; i++) {
    hiddenLayer[i].weightsOut.assign(numOutput, 0.0);
    hiddenLayer[i].weightsSelf.assign(numHidden, 0.0);
  }
  
  outputLayer.resize(numOutput);
}

void Ctrnn3Layers::destroy()
{
  for(int i = 0; i < inputLayer.size() ; i++)
    {
      inputLayer[i].weightsOut.clear();
    }
  inputLayer.clear();
  
  for(int i = 0; i < hiddenLayer.size() ; i++)
    {
      hiddenLayer[i].weightsOut.clear();
      hiddenLayer[i].weightsSelf.clear();
    }
  hiddenLayer.clear();
  
  outputLayer.clear();
}

/* -------------------------------------------------------------------------------------- */

Ctrnn3Layers::~Ctrnn3Layers()
{
    destroy();
}

/* -------------------------------------------------------------------------------------- */

void Ctrnn3Layers::init ( const vector <chromosome_type> &genes ){
  
  int    counter     = 0;
  /* ------------------ INPUT_LAYER -------------------- */
  for( int i=0; i<inputLayer.size(); i++){
    for(int j=0; j< inputLayer[i].weightsOut.size(); j++){
      inputLayer[i].weightsOut[j] =  get_value(genes, counter)*(upper_bound_inputWts - low_bound_inputWts) + low_bound_inputWts;
      counter++;
    }
  }
  /* --------------------------------------------------------------- */
  
  /* ------------------ CONTROLLER::HIDDEN_LAYER -------------------- */
  for( int i=0; i<hiddenLayer.size() ; i++){
    for(int j=0; j<hiddenLayer[i].weightsOut.size() ; j++){
      hiddenLayer[i].weightsOut[j] = get_value(genes, counter)*(upper_bound_hiddenWts - low_bound_hiddenWts) + low_bound_hiddenWts;
      counter++;
    }
    hiddenLayer[i].tau         = pow(10, (low_bound_hiddenTau + (upper_bound_hiddenTau * get_value(genes, counter) )));
    counter++;
    hiddenLayer[i].bias        = get_value(genes, counter)*(upper_bound_hiddenBias - low_bound_hiddenBias) + low_bound_hiddenBias;
    counter++;
    for( int j=0; j<hiddenLayer.size() ; j++){
      hiddenLayer[i].weightsSelf[j] = get_value(genes, counter)*(upper_bound_hiddenWts - low_bound_hiddenWts) + low_bound_hiddenWts;
      counter++;
    }
  }
  /* ---------------------------------------------------------------- */
  
  /* ------------------ CONTROLLER::OUTPUT_LAYER -------------------- */
  for(int i=0; i<outputLayer.size() ; i++){
    outputLayer[i].bias =  get_value(genes, counter)*(upper_bound_outputBias - low_bound_outputBias) + low_bound_outputBias;
    counter++;
  }
  /* --------------------------------------------------------------- */
  
  if( counter != genotype_length ){
    cerr << "'In ctrnn3Layers.cpp init():: the number of genes is wrong"
	 << " " << genotype_length << " " << counter << endl;
    exit(0);
  }
}

/* -------------------------------------------------------------------------------------- */

void Ctrnn3Layers::step ( const vector <double> &inputs, vector <double> &outputs ){
  
  for(int h=0; h < num_hidden; h++) {
    hiddenLayer[h].s = -hiddenLayer[h].state;
    for( int i=0; i < num_input; i++)
      hiddenLayer[h].s += inputs[i] * inputLayer[i].weightsOut[h];
    for(int oh=0;  oh< num_hidden; oh++)
      hiddenLayer[h].s += f_sigmoid (hiddenLayer[oh].state + hiddenLayer[oh].bias) * hiddenLayer[oh].weightsSelf[h];
  }
  
  for(int h=0; h < num_hidden; h++)
    hiddenLayer[h].state += (hiddenLayer[h].s * (delta_t/hiddenLayer[h].tau));
  
  for( int out = 0; out < num_output; out++ ){
    outputs[out] = 0.0;
    for(int h=0; h < num_hidden; h++)
      outputs[out] +=  f_sigmoid (hiddenLayer[h].state + hiddenLayer[h].bias) * hiddenLayer[h].weightsOut[out];
    outputs[out] = f_sigmoid( outputs[out] + outputLayer[out].bias );
  }
}

/* -------------------------------------------------------------------------------------- */

void Ctrnn3Layers::reset ( void ){
  for(int i=0; i<num_hidden; i++){
    hiddenLayer[i].state = 0.0;
    hiddenLayer[i].s = 0.0;
  }
}

/* -------------------------------------------------------------------------------------- */

Ctrnn3Layers& Ctrnn3Layers::operator=(const Ctrnn3Layers &other)
{
  if(this != &other)
    {
      destroy();
      allocate(other.num_input, other.num_hidden, other.num_output);
      copy(other);
    }
  return *this;
}

/* -------------------------------------------------------------------------------------- */
