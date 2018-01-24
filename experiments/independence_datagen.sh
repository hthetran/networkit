#!/bin/bash
#SBATCH --partition=parallel
#SBATCH --constraint="intel20|broadwell"

#BATCH --partition=test

#SBATCH --ntasks=1
#SBATCH --cpus-per-task=10
#SBATCH --mem-per-cpu=3200
#SBATCH --time=0:20:00
#SBATCH --mail-type=END

export OMP_NUM_THREADS=10
export USER=$(whoami)
scriptdir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
user=$(whoami)
dir="/scratch/memhierarchy/$user/independence_$SLURM_JOB_ID/"
mkdir $dir
cp autocorr_analysis2.py $dir/
cp ${BASH_SOURCE[0]} $dir/

cd $dir
env

srun -o out_%j_%t ./autocorr_analysis2.py --pus 10 --thins 8 16 32 64 128 --runlength 1000 --nodes X --runs 10 --samples 10 -cg
