#!/bin/bash
#SBATCH -N 1
#SBATCH --mem=0
#SBATCH --account=owner-guest
#SBATCH --partition=notchpeak-guest
#SBATCH --job-name=seqan3_aa
#SBATCH --time=4:00:00
#SBATCH -C c32
#SBATCH -C zpu
#SBATCH --exclusive
#SBATCH --output=/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/FINAL_BENCHMARKING_SCRIPTS/seqan3_aa%j.out

script_dir="/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/LEANN_TEST/SEQAN3/tutorial/build"
input_dir="/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/DATA/large_datasets/PROTEIN"
output_dir="/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/BENCHMARKING_RESULTS/SEQAN3_32core_test1"

mkdir $output_dir

module load cmake
module load gcc/11.2.0
#module swap PrgEnv-${PE_ENV,,} PrgEnv-gnu

lscpu

cd $script_dir

echo "TIME: START seqan3 aa on set 1 = `date +"%Y-%m-%d %T"`"
srun -n 1 ./seqan_aa_align_multithread $input_dir/set_1_ref.fasta $input_dir/set_1_read.fasta > ${output_dir}/seqan3_traceback_aa_out_set_1.txt
echo "TIME: START seqan3 aa on set 2 = `date +"%Y-%m-%d %T"`"
srun -n 1 ./seqan_aa_align_multithread $input_dir/set_2_ref.fasta $input_dir/set_2_read.fasta > ${output_dir}/seqan3_traceback_aa_out_set_2.txt
echo "TIME: START seqan3 aa on set 3 = `date +"%Y-%m-%d %T"`"
srun -n 1 ./seqan_aa_align_multithread $input_dir/set_3_ref.fasta $input_dir/set_3_que.fasta > ${output_dir}/seqan3_traceback_aa_out_set_3.txt
