#!/bin/bash
#SBATCH -N 1
#SBATCH --mem=0
#SBATCH --account=owner-guest
#SBATCH --partition=notchpeak-guest
#SBATCH --job-name=parasail_dna
#SBATCH --time=2:00:00
#SBATCH --exclusive
#SBATCH --output=/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/FINAL_BENCHMARKING_SCRIPTS/traceback_benchmark_parasail_dna%j.out

echo "CPU Node Information"
uname -a
lscup

script_dir="/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/LEANN_TEST/PairwiseAlignmentStub/Parasail"
input_dir="/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/DATA/large_datasets/DNA"
output_dir="/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/BENCHMARKING_RESULTS/FINAL_RESULTS"

export LD_LIBRARY_PATH=~/mylibs:$LD_LIBRARY_PATH

mkdir $output_dir
echo "Node Number"
uname -u
echo "Hardware info"
lscpu

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

	$script_dir/multithreaded_dna ${input_dir}/${prefix}_ref.fasta ${input_dir}/${prefix}_read.fasta ${output_dir}/${prefix}_results.fasta > ${output_dir}/parasail_dna_${prefix}_output.txt
#	srun -N 1 --gpus-per-node=$i ${script_dir}/test_prog.out -a 3 -b 3 -q 6 -r 1 -s -y "local" ${input_dir}/${prefix}_read.fasta ${input_dir}/${prefix}_ref.fasta > ${output_dir}/gasal2_notraceback_${prefix}_out.txt

#	srun -N 1 --gpus-per-node=$i ${script_dir}/test_prog.out -a 3 -b 3 -q 6 -r 1 -s -t -y "local" ${input_dir}/${prefix}_read.fasta ${input_dir}/${prefix}_ref.fasta > ${output_dir}/gasal2_traceback_${prefix}_out.txt

#	srun -N 1 --gpus-per-node=$i ${script_dir}/test_prog.out -a 3 -b 3 -q 6 -r 1 -s -p -y "local" ${input_dir}/${prefix}_read.fasta ${input_dir}/${prefix}_ref.fasta > ${output_dir}/gasal2_results_notraceback_${prefix}_out.txt

#	srun -N 1 --gpus-per-node=$i ${script_dir}/test_prog.out -a 3 -b 3 -q 6 -r 1 -s -t -p -y "local" ${input_dir}/${prefix}_read.fasta ${input_dir}/${prefix}_ref.fasta > ${output_dir}/gasal2_results_traceback_${prefix}_out.txt
   done 
done
