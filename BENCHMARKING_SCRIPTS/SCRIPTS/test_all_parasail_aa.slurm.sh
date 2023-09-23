#!/bin/bash
#SBATCH -N 1
#SBATCH --mem=0
#SBATCH --account=owner-guest
#SBATCH --partition=notchpeak-guest
#SBATCH --job-name=psail_aa
#SBATCH -C c32
#SBATCH -C zpu
#SBATCH --time=2:00:00
#SBATCH --exclusive
#SBATCH --output=/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/FINAL_BENCHMARKING_SCRIPTS/traceback_benchmark_parasail_aa%j.out

echo "CPU Node Information"
uname -a
echo "HARDWARE SPECS"
lscpu


#script_dir="/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/LEANN_TEST/PairwiseAlignmentStub/Parasail"
#input_dir="/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/DATA/large_datasets/PROTEIN"
#output_dir="/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/BENCHMARKING_RESULTS/FINAL_RESULTS/PARASAIL_32core_test1"

script_dir=$1
input_dir=$2
output_dir=$3

export LD_LIBRARY_PATH=~/mylibs:$LD_LIBRARY_PATH

mkdir $output_dir

cd $input_dir
num_nodes=( 1 )
bins=$(ls *_read.fasta)
echo "Starting tests"
for file in $bins; do
   for i in ${num_nodes[@]}; do	
   	prefix=${file%_read.fasta}
  	echo "file is " $file
   	echo "prefix is " $prefix
   	echo "read file is " ${input_dir}/${prefix}k_read.fasta
   	echo "ref file is " ${input_dir}/${prefix}k_ref.fasta
   	echo "Running test on file:" $file
	echo "with nodes:" $i

	$script_dir/multithreaded_aa ${input_dir}/${prefix}_ref.fasta ${input_dir}/${prefix}_read.fasta ${output_dir}/${prefix}_results.fasta > ${output_dir}/parasail_aa_${prefix}_output.txt
   done 
done
