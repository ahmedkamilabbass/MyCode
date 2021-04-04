#!/bin/bash

# Submission script for NIC4 

#SBATCH --time=24:00:00 # hh:mm:ss

#SBATCH --ntasks=64 

#SBATCH --mem-per-cpu=2625 # megabytes 

#SBATCH --partition=defq 

# Specify the file name to write stdout to; %J is replaced by the JOBID

#SBATCH --output=output/job_output.%J

# Specify the file name to write stderr to; %J is replaced by the JOBID

#SBATCH --error=output/job_error.%J

# mpirun -np 64 build/dsim -e -n A -s 88902146

module load EasyBuild Bullet/2.83.7-foss-2016a
module load GSL/2.3-foss-2016b

mpirun -np 64 build/dsim -e -n B -s 12936334
