#ifndef _GENERAL_
#define _GENERAL_

#include <cstdlib>
#include <iomanip>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <sstream>
#include <vector>
#include <algorithm>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "AlgoString.h"
#include <mpi.h>

#define PI     3.14159265359
#define TWO_PI 6.28318530718

/* ------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------ */
//#define _DIPLOID_

//#define INT_GENES_TYPE
//typedef int chromosome_type;
#define DOUBLE_GENES_TYPE
typedef double chromosome_type;

#define MAX_INT         255
#define DOUBLE_MAX_INT  (MAX_INT*2)

/* ------------------------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------------------------ */

using namespace std;
const std::string home = getenv("HOME");
const std::string local_path = "/Simulations/mysim";

enum {BLACK_DOMINANT = 0, WHITE_DOMINANT = 1};

extern int NbRank;
extern int NbTotProcesses;
extern int m_iteration;
extern double m_simulationTimeStep;

double getDouble(char sep, istream &I);

int getInt(char sep, istream &I);

unsigned long getLongInt(char sep, istream &I);

void getStr(char sep, istream &I, char *s);

void getStr(char sep, istream &I, string &s);

char getChar(char sep, istream &I);

bool getYesNo(char sep, istream &I);

bool isIntString(char *s);

int string2Int(char *s);

bool fileExists(char *F);

double f_sigmoid(double x);

bool sortAscendingOrder(int i, int j);

bool sortDescendingOrder(int i, int j);

namespace GSL_randon_generator {
    extern gsl_rng *r_rand;

    void init_generator(const int root_seed);

    void free_generator(void);

    void reset_seed(const int root_seed);
}

/* ------------------------------------------------------------------------------------------ */

template<class T>
void upload_genome_from_file(const char *str_source, vector<vector<T> > &genes, int *num_groups) {

    for (int r = 0; r < genes.size(); r++) {
        genes[r].erase(genes[r].begin(), genes[r].end());
        genes[r].clear();
    }
    genes.clear();

    std::ifstream file(str_source, std::ios::in);  // Open file
    if (file)  // Open Success
    {
        std::string line;           // One line of the file
        unsigned int count_line = 0;
        while (!file.eof()) {
            std::getline(file, line); 
            //cout<<line<<endl;  // Read one line

            if (line.empty()) { // If last line empty it stops reading
                break;
            } else {
                line = ltrim(line);        //Remove space at beginning of the line
                genes.resize(count_line + 1);
                std::vector<std::string> lineSplited = split(line, ' ');    // Split the line on char ' '
                for (int i = 0; i < atoi(lineSplited[0].c_str()); i++) {
                    genes[count_line].push_back(atof(lineSplited[1 + i].c_str()));
                    //cout << " genes["<<count_line<<"]["<<i<<"] = " << genes[count_line][i] << " " << atof(lineSplited[1+i].c_str()) << endl;
                }

                if (atof(lineSplited[lineSplited.size() - 1].c_str()) == 0)
                    (*num_groups)++;

                ++count_line;
            }
        }
        file.close();  // Close file

        ///////////////////////////////////////////////////////////////////////////////////////
        std::ifstream file(str_source, std::ios::in); 
        vector<double> myTest;
        if (file)  // Open Success
        {
            string line; 
            getline(file, line);
            istringstream lineStream(line);
            double d;
            while(lineStream >> d)
            {
                myTest.push_back(d);
            }
            myTest.pop_back();
            myTest.pop_back();
            myTest.erase(myTest.begin());
        }
        ////////////////////////////////////////////////////////////////////////////////////////////

        //vector<double> mystest{0.103941000358997 0.000000000000000 0.029877269172923 0.547801353364701 0.000000000000000 0.170163771521969 0.088060329340863 0.000000000000000 0.349265874860292 0.051513659222604 0.589223347107844 0.000000000000000 0.000000000000000 0.053947352852419 0.248510499877950 0.930010353185668 0.973920034423251 0.838957250095616 0.000000000000000 1.000000000000000 0.650630632596625 0.766402013705648 0.936691936122310 0.714595037860870 0.000000000000000 1.000000000000000 0.434217884724037 1.000000000000000 1.000000000000000 0.989770042177019 1.000000000000000}
        //cout<<endl<<genes[0].size()<<endl;
        // vector<double> mystest{0.103941000358997, 0.000000000000000, 0.029877269172923,
        //  0.547801353364701, 0.000000000000000, 0.170163771521969, 0.088060329340863, 0.000000000000000,
        //  0.349265874860292, 0.051513659222604, 0.589223347107844, 0.000000000000000, 0.000000000000000,
        //  0.053947352852419, 0.248510499877950, 0.930010353185668, 0.973920034423251, 0.838957250095616,
        //  0.000000000000000, 1.000000000000000, 0.650630632596625, 0.766402013705648, 0.936691936122310,
        //  0.714595037860870, 0.000000000000000, 1.000000000000000, 0.434217884724037, 1.000000000000000,
        //  1.000000000000000, 0.989770042177019, 1.000000000000000};
        genes[0] = myTest;
        // cout<<endl;
        // for(int i = 0; i < genes[0].size(); i++)
        // cout<<genes[0][i]<<" ";

    } else
        throw std::runtime_error("Impossible to open file or file doesn't exist! : " + std::string(str_source));
}

/* ------------------------------------------------------------------------------------------ */

/* ---------------------------------------------------------------------------------------- */

template<class myType>
double find_distance(const vector<myType> &_pos1, const vector<myType> &_pos2) {
    return sqrt(((_pos2[0] - _pos1[0]) * (_pos2[0] - _pos1[0]) + (_pos2[2] - _pos1[2]) * (_pos2[2] - _pos1[2])));
}

/* /\* ------------------------------------------------------------------------------------------*\/ */

/* template <class myType> */
/* void swap ( myType *a, myType *b) { */
/*   myType temp = *a; */
/*   *a = *b; */
/*   *b = temp; */
/* } */

/* /\* ------------------------------------------------------------------------------------------*\/ */

/* template <class myType> */
/* void shuffle_array ( myType arr[], int arr_size) { */
/*   for (int i = (arr_size-1); i> 0; i--){ */
/*     int j = (int)(gsl_rng_uniform_int(GSL_randon_generator::r_rand, i+1 )); */
/*     swap <myType> (&arr[i], &arr[j]); */
/*   } */
/* } */

/* /\* ------------------------------------------------------------------------------------------*\/ */


#endif
