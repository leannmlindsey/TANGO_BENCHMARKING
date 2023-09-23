#!/bin/bash
#SBATCH -N 1
#SBATCH --mem=0
#SBATCH --account=owner-guest
#SBATCH --partition=notchpeak-guest
#SBATCH -C c32
#SBATCH -C zpu
#SBATCH --job-name=ssw_set1
#SBATCH --time=4:00:00
#SBATCH --exclusive
#SBATCH --output=/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/FINAL_BENCHMARKING_SCRIPTS/ssw_set1%j.out

echo "CPU Node Information"
uname -a
echo "HARDWARE SPECS"
lscpu

#script_dir="/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/LEANN_TEST/Complete-Striped-Smith-Waterman-Library/src"
#input_dir="/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/DATA/large_datasets/DNA"
#output_dir="/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/BENCHMARKING_RESULTS/FINAL_RESULTS/SSW_32_test1"

script_dir=$1
intput_dir=$2
output_dir=$3

mkdir $output_dir

cd $input_dir
bins=$(ls *_read.fasta)
for file in $bins; do
     echo "Starting tests on SSW with traceback on DNA data"
     cd $script_dir
     echo "Using scripts in directory: "
     pwd
     prefix=${file%_read.fasta}
     echo "Running with traceback"
     $script_dir/ssw_test ${input_dir}/${prefix}_ref.fasta ${input_dir}/${prefix}_read.fasta ${output_dir}/ssw_traceback_dna_results_${prefix}.txt 1 32 > ${output_dir}/FINAL_ssw_traceback_dna_${prefix}.txt
     echo "Running without traceback"
     $script_dir/ssw_test ${input_dir}/${prefix}_ref.fasta ${input_dir}/${prefix}_read.fasta ${output_dir}/ssw_notraceback_dna_results_${prefix}.txt 0 32 > ${output_dir}/FINAL_ssw_notraceback_dna_${prefix}.txt

done

