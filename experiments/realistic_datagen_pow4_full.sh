#!/bin/bash
#SBATCH --partition=parallel
#SBATCH --constraint="intel20|broadwell"

#BATCH --partition=test

#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=60000
#SBATCH --time=48:00:00
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

srun -o out_%j_%t ./autocorr_analysis2.py --pus 10 --thins 1 2 3 4 5 6 7 8 9 10 --runlength 1000 --a 5 --b 500 --nodes 10000 --runs 20 --samples 1 -e -cbg