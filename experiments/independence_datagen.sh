#!/bin/bash
#BATCH --partition=parallel
#BATCH --constraint="intel20|broadwell"

#SBATCH --partition=test

#SBATCH --ntasks=1
#SBATCH --cpus-per-task=10
#SBATCH --mem-per-cpu=5800
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

srun -o out_%j_%t ./autocorr_analysis2.py --pus 10 --thins 8 --runlength 1000 --nodes 200 --runs 10 --samples 1 -cg
