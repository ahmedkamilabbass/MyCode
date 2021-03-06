#include "simple3Layers.h"

/* -------------------------------------------------------------------------------------- */

void  Simple3Layers::read_from_file ( void ) { 
  std::string absolute_path = (home + local_path).c_str();
  ifstream I ( (absolute_path + "/CONTROLLERS/simple3Layers").c_str() );
  if(!I) {
        cerr << "File with network structure not found" <<endl;
        exit(0);
    }

    num_input               = getInt      ('=', I);
    num_hidden              = getInt      ('=', I);
    num_output              = getInt      ('=', I);
    low_bound_Wts           = getDouble   ('=', I);
    upper_bound_Wts         = getDouble   ('=', I);

    int check = getInt('=',I);
    I.close();
    if( check != 999 ){
        cerr << " In ../CONTROLLERS/simple3layers specifications' file --- End check was not 999" << endl;
        exit(0);
    }
}

/* -------------------------------------------------------------------------------------- */

void Simple3Layers::compute_genotype_length ( void )
{
    genotype_length = 0;
    //weights input-hidden
    for(int i = 0; i < num_input; i++)
        for(int h = 0; h < num_hidden; h++)
            genotype_length++;

    //weights hidden-output
    for(int h = 0; h < num_hidden; h++)
        for(int o = 0; o < num_output; o++)
            genotype_length++;

    //weigth bias-hidden
    for(int h = 0; h < num_hidden; h++) genotype_length++;

    //weigths bias-output
    for(int out = 0; out < num_output; out++) genotype_length++;
}

/* -------------------------------------------------------------------------------------- */

Simple3Layers::Simple3Layers( )
    : Controller()
{
    read_from_file ( );
    compute_genotype_length ( );

    allocate(num_input, num_hidden, num_output);
}

/* -------------------------------------------------------------------------------------- */

Simple3Layers::Simple3Layers(const Simple3Layers& other)
{
    allocate(other.num_input, other.num_hidden, other.num_output);
    copy(other);
}

/* -------------------------------------------------------------------------------------- */

void Simple3Layers::allocate(int numInput, int numHidden, int numOutput)
{
    inputLayer.resize(numInput);
    for(int i = 0; i < numInput ; i++)
        inputLayer[i].weightsOut.assign(numHidden, 0.0);

    hiddenLayer.resize(numHidden);
    for(int i = 0; i < numHidden ; i++)
        hiddenLayer[i].weightsOut.assign(numOutput, 0.0);

    outputLayer.resize(numOutput);

    biasLayer.resize(1);
    biasLayer[0].weightsOut.assign(numHidden+numOutput, 0.0);
}

/* -------------------------------------------------------------------------------------- */

Simple3Layers::~Simple3Layers()
{
    destroy();
}

/* -------------------------------------------------------------------------------------- */

void Simple3Layers::copy(const Simple3Layers &other)
{
    Controller::copy(other);

    num_hidden = other.num_hidden;
    biasLayer = other.biasLayer;
    inputLayer = other.inputLayer;
    hiddenLayer = other.hiddenLayer;
    outputLayer = other.outputLayer;

    low_bound_Wts = other.low_bound_Wts;
    upper_bound_Wts = other.upper_bound_Wts;
}

/* -------------------------------------------------------------------------------------- */

void Simple3Layers::destroy()
{
    Controller::destroy();

    for(int i = 0; i < num_input ; i++)
        inputLayer[i].weightsOut.clear();
    inputLayer.clear();

    for(int i = 0; i < num_hidden ; i++)
        hiddenLayer[i].weightsOut.clear();
    hiddenLayer.clear();

    outputLayer.clear();

    biasLayer[0].weightsOut.clear();
    biasLayer.clear();
}

/* -------------------------------------------------------------------------------------- */

void Simple3Layers::init( const vector <chromosome_type> &genes ){
    int    counter  = 0;
    /* ------------------ INPUT_LAYER -------------------- */
    for( int i=0; i<num_input; i++){
        inputLayer[i].state        = 0.0;
        for(int j=0; j< num_hidden; j++){
            inputLayer[i].weightsOut[j] = get_value(genes, counter)*(upper_bound_Wts - low_bound_Wts) + low_bound_Wts;
            counter++;
        }
    }

    /* ------------------ CONTROLLER::HIDDEN_LAYER -------------------- */
    for( int i=0; i<num_hidden; i++){
        hiddenLayer[i].state        = 0.0;
        for(int j=0; j<num_output; j++){
            hiddenLayer[i].weightsOut[j] = get_value(genes, counter)*(upper_bound_Wts - low_bound_Wts) + low_bound_Wts;
            counter++;
        }
    }

    /* ------------------ CONTROLLER::OUTPUT_LAYER -------------------- */
    for(int i=0; i<num_output; i++)
        outputLayer[i].state         = 0.0;

    /* ------------------ CONTROLLER::BIAS_LAYER -------------------- */
    biasLayer[0].state         = 1.0;
    for(int i=0; i<(num_hidden+num_output); i++){
        biasLayer[0].weightsOut[i] = get_value(genes, counter)*(upper_bound_Wts - low_bound_Wts) + low_bound_Wts;
        counter++;
    }

    /* --------------------------------------------------------------- */
    if( counter != genotype_length ){
        cerr << "'In simple3Layers.cpp init():: the number of genes is wrong"
             << " " << genotype_length << " " << counter << endl;
        exit(0);
    }
}

/* -------------------------------------------------------------------------------------- */

void Simple3Layers::step ( const vector <double> &inputs, vector <double> &outputs ){

    for( int h=0; h<num_hidden; h++){
        hiddenLayer[h].state = biasLayer[0].weightsOut[h];
        for( int i=0; i<num_input; i++){
            hiddenLayer[h].state += inputs[i] * inputLayer[i].weightsOut[h];
        }
        hiddenLayer[h].state = f_sigmoid( hiddenLayer[h].state );
    }

    for(int out=0; out<num_output; out++){
        outputLayer[out].state = biasLayer[0].weightsOut[num_hidden+out];
        for( int h=0; h<num_hidden; h++){
            outputLayer[out].state += hiddenLayer[h].state * hiddenLayer[h].weightsOut[out];
        }
        outputs[out] = f_sigmoid( outputLayer[out].state );
    }
}

/* -------------------------------------------------------------------------------------- */

void Simple3Layers::reset ( void )
{
    for(int i=0; i<num_input; i++)
        inputLayer[i].state = 0.0;
    for(int i=0; i<num_hidden; i++)
        hiddenLayer[i].state = 0.0;
    for(int i=0; i<num_output; i++)
        outputLayer[i].state = 0.0;
}

/* -------------------------------------------------------------------------------------- */

Simple3Layers& Simple3Layers::operator=(const Simple3Layers &other)
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
/* -------------------------------------------------------------------------------------- */
