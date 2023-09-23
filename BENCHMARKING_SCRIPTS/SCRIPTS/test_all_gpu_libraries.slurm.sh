#!/bin/bash
#SBATCH -N 1
#SBATCH --mem=0
#SBATCH --account=soc-gpu-np
#SBATCH --partition=soc-gpu-np
#SBATCH --job-name=adept_tango
#SBATCH --time=8:00:00
#SBATCH --gres=gpu:a100:1
#SBATCH -w notch369
#SBATCH --exclusive
#SBATCH --output=/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/FINAL_BENCHMARKING_SCRIPTS/traceback_benchmark_slurm%j.out

echo "CPU Node Information"
uname -a
echo "GPU Node Information"
nvidia-smi
module load cuda
#script_dir_adept="/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/LEANN_TEST/GPU-BSW/build"
#script_dir_tango="/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/LEANN_TEST/TANGO/build"
#script_dir_gasal2="/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/LEANN_TEST/GASAL2/CLEAN/GASAL2/test_prog"
#input_dir_aa="/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/DATA/large_datasets/PROTEIN"
#input_dir_dna="/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/DATA/large_datasets/DNA"
#output_dir="/uufs/chpc.utah.edu/common/home/u1323098/sundar-group-space2/ADEPT/BENCHMARKING_RESULTS/REPLICATES/ADEPT_TANGO_a100_80gb"

script_dir_adept=$1
script_dir_tango=$2
script_dir_gasal2=$3
input_dir_aa=$4
intput_dir_dna=$5
output_dir=$6

mkdir $output_dir
cd $input_dir_dna
iterations=(1 2 3 4 5 6 7 8 9 10)
#bins=$(ls *_read.fasta)
bins=$(ls *_read.fasta)
echo "Starting tests"
for file in $bins; do
   for i in ${iterations[@]}; do
        prefix=${file%_read.fasta}
	echo "DNA ITERATION " $i
        echo "file is " $file
        echo "prefix is " $prefix
        echo "read file is " ${input_dir}/${prefix}_read.fasta
        echo "ref file is " ${input_dir}/${prefix}_ref.fasta
        echo "Running test on file:" $file
        echo "with nodes:" 1
        echo "output in: " $output_dir
        srun -N 1 --gpus-per-node=1 ${script_dir_adept}/program_gpu dna ${input_dir_dna}/${prefix}_ref.fasta ${input_dir_dna}/${prefix}_read.fasta ${output_dir}/adept_results_dna_iteration_${i}_${prefix}.txt > ${output_dir}/FINAL_adept_executionTime_dna_iteration_${i}_${prefix}.txt
        srun -N 1 --gpus-per-node=1 ${script_dir_tango}/program_gpu dna ${input_dir_dna}/${prefix}_ref.fasta ${input_dir_dna}/${prefix}_read.fasta ${output_dir}/tango_results_dna_iteration_${i}_${prefix}.txt > ${output_dir}/FINAL_tango_executionTime_dna_iteration_${i}_${prefix}.txt


done
done


mkdir $output_dir
cd $input_dir_aa
iterations=(1 2 3 4 5 6 7 8 9 10)
#bins=$(ls *_read.fasta)
bins=$(ls *_read.fasta)
echo "Starting tests"
for file in $bins; do
   for i in ${iterations[@]}; do
        prefix=${file%_read.fasta}
        echo "AA ITERATION " $i
        echo "file is " $file
        echo "prefix is " $prefix
        echo "read file is " ${input_dir}/${prefix}_read.fasta
        echo "ref file is " ${input_dir}/${prefix}_ref.fasta
        echo "Running test on file:" $file
        echo "with nodes:" 1
        echo "output in: " $output_dir
        srun -N 1 --gpus-per-node=1 ${script_dir_adept}/program_gpu aa ${input_dir_aa}/${prefix}_ref.fasta ${input_dir_aa}/${prefix}_read.fasta ${output_dir}/adept_results_aa_iteration_${i}_${prefix}.txt > ${output_dir}/FINAL_adept_executionTime_aa_iteration_${i}_${prefix}.txt
        srun -N 1 --gpus-per-node=1 ${script_dir_tango}/program_gpu dna ${input_dir_aa}/${prefix}_ref.fasta ${input_dir_aa}/${prefix}_read.fasta ${output_dir}/tango_results_aa_iteration_${i}_${prefix}.txt > ${output_dir}/FINAL_tango_executionTime_aa_iteration_${i}_${prefix}.txt


done
done


cd $input_dir
iteration=( 1 2 3 4 5 6 7 8 9 10)
bins=$(ls set_2_read.fasta)
echo "Starting tests"
for file in $bins; do
   for i in ${iteration[@]}; do
   	prefix=${file%_read.fasta}
	echo "GASAL2 ITERATION" ${i}
   	echo "file is " $file
   	echo "prefix is " $prefix
   	echo "read file is " ${input_dir}/${prefix}k_read.fasta
   	echo "ref file is " ${input_dir}/${prefix}k_ref.fasta
   	echo "Running test on file:" $file
	echo "with nodes:" 1

	srun -N 1 --gpus-per-node=1 ${script_dir_gasal2}/test_prog.out -a 3 -b 3 -q 6 -r 1 -n 32 -y "local" ${input_dir}/${prefix}_read.fasta ${input_dir}/${prefix}_ref.fasta > ${output_dir}/gasal2_forwardonly_iteration_${i}_${prefix}_out.txt

	echo "Results above for score and endpoints but no start points - FORWARD ONLY"

	srun -N 1 --gpus-per-node=1 ${script_dir_gasal2}/test_prog.out -a 3 -b 3 -q 6 -r 1 -n 32 -s -y "local" ${input_dir}/${prefix}_read.fasta ${input_dir}/${prefix}_ref.fasta > ${output_dir}/gasal2_notraceback_iteration_${i}_${prefix}_out.txt

	echo "Results above for score, endpoints, startpoints - FORWARD + REVERSE SCORING"

	srun -N 1 --gpus-per-node=1 ${script_dir_gasal2}/test_prog.out -a 3 -b 3 -q 6 -r 1 -n 32 -s -t -y "local" ${input_dir}/${prefix}_read.fasta ${input_dir}/${prefix}_ref.fasta > ${output_dir}/gasal2_traceback_iteration_${i}_${prefix}_out.txt

	echo "Results above for score, endpoints, startpoints, CIGAR - FORWARD + REVERSE SCORE + TRACEBACK"

	srun -N 1 --gpus-per-node=1 ${script_dir_gasal2}/test_prog.out -a 3 -b 3 -q 6 -r 1 -n 32 -s -p -y "local" ${input_dir}/${prefix}_read.fasta ${input_dir}/${prefix}_ref.fasta > ${output_dir}/gasal2_results_notraceback_iteration_${i}_${prefix}_results.txt

	echo "Results above for score, endpoint, startpoints, CIGAR + printing resulings - FORWARD + REVERSE SCORE + TRACEBACK + PRINT"

	srun -N 1 --gpus-per-node=1 ${script_dir_gasal2}/test_prog.out -a 3 -b 3 -q 6 -r 1 -n 32 -s -t -p -y "local" ${input_dir}/${prefix}_read.fasta ${input_dir}/${prefix}_ref.fasta > ${output_dir}/gasal2_results_traceback_iteration_${i}_${prefix}_results.txt


done
done

