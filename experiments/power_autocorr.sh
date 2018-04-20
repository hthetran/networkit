#!/bin/bash
#SBATCH --partition=parallel
#BATCH --partition=test
#SBATCH --constraint=intel20
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=64000
#BATCH --tmp=4000
#SBATCH --time=1:00:00
#SBATCH --mail-type=FAIL

export OMP_NUM_THREADS=1
export USER=$(whoami)
scriptdir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
user=$(whoami)
dir="/scratch/memhierarchy/$user/curveball_$SLURM_JOB_ID/"
mkdir $dir
cp power.gml $dir/
cp power_autocorr.py $dir/
cp ${BASH_SOURCE[0]} $dir/

cd $dir
env

srun -o out_%j_%t ./power_autocorr.py 
