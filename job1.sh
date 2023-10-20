#!/bin/bash
#PBS -N Poisson_test_1
#PBS -q serialq
#PBS -l select=1:ncpus=1:mpiprocs=1:ngpus=1
#PBS -j oe
#PBS -V
cd $PBS_O_WORKDIR
module load pgi/19.10
#####module load cuda10.1/toolkit/10.1.105
####module load openmpi/pgi/pgi19.10/4.0.1-cuda-10.1
#module load cuda91/toolkit/9.1.85
#module load openmpi/pgi/1.10.7-cuda_9.1

#export NGPUS=2

PREFIX=Poisson_test_1
mpirun -np 1 --hostfile $PBS_NODEFILE ./main 

##mkdir ../output/$PREFIX
##mv ../output/$PREFIX.* ../output/$PREFIX
##exit 0
