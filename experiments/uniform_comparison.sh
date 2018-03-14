#!/bin/bash
#SBATCH --partition=parallel
#SBATCH --constraint="intel20|broadwell"

#BATCH --partition=test

#SBATCH --ntasks=1
#SBATCH --cpus-per-task=10
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

./autocorr_analysis2.py --label uniform_comparison --pus 10 --thins 2 4 8 16 32 64 --runlength 1000 --a 5 --b 500 --nodes 10000 --runs 9 --samples 9 -e -cu
