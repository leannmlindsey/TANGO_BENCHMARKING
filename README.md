# TANGO_BENCHMARKING

This repository contains all of the code and instructions to reproduce the results for TANGO:A GPU optimized traceback approach for sequence alignment algorithms, accepted as a short paper for the 13th Workshop on Irregular Applications: Architectures and Algorithms, a workshop at SC '23.

These instructions are for the A100 GPUs on the Notchpeak Cluster of the University of Utah CHPC. When installing on a different system, you should make appropriate changes to the 'salloc' and 'module load cuda' commands to make sure that you have an interactive allocation to a node with access to a GPU and that cuda is available.

## Overview

Step 1: Clone this github repository which holds the benchmarking scripts

Step 2: Clone GPU libraries and compile

Step 3: Clone CPU libraries, copy over multithreaded test programs and compile

Step 4: Submit the bash scripts for benchmarking

## Dependencies

- CMAKE 3.27.0 or greater

- NVCC 11.6 or greater

## Step 1: Clone this github repository which holds the benchmarking scripts

```
salloc -M notchpeak --account=soc-gpu-np --partition=soc-gpu-np --nodes=1 --ntasks=8  --gres=gpu:a100:1 -t 0:30:00

module load cuda

git clone https://github.com/leannmlindsey/TANGO_BENCHMARKING.git

cd TANGO_BENCHMARKING

cd SRC

```

## Step 2: Clone GPU libraries and compile

Instructions for cloning and installing the libraries:

Notes: All of the GPU libraries have test programs that do pairwise alignment, so they are compiled exactly as the instructions in the corresponding githubs

### TANGO

```
git clone https://github.com/leannmlindsey/TANGO.git

cd TANGO

mkdir build

cd build

cmake CMAKE_BUILD_TYPE=Release ..

make 

```

### ADEPT 

```
git clone https://github.com/mgawan/GPU-BSW.git

cd GPU-BSW

mkdir build

cd build

cmake CMAKE_BUILD_TYPE=Release ..

make
```

### GASAL2


```
git clone https://github.com/nahmedraja/GASAL2.git

cd GASAL2

./configure.sh <path to cuda>

make GPU_SM_ARCH='sm_80' MAX_QUERY_LEN=160 N_CODE='0x4E' N_PENALTY=-1
# sm_80 is the archecture code for Nvidia A100, if you are using a different GPU, you will need to use the appropriate architecture code
# you can find the architecture codes for each NVIDIA machine here: https://arnon.dk/matching-sm-architectures-arch-and-gencode-for-various-nvidia-cards/

cd test_prog

gunzip  query_batch.fasta.gz target_batch.fasta.gz

make 

# test to see if it runs without error
./test_prog.out -a 1 -b 4 -q 6 -r 1 -s -t -p -y "local" query_batch.fasta target_batch.fasta
# -s flag to get the start positions
# -t flag to use traceback
# -p flag to print the results including the CIGAR string
```

## Step 3: Clone CPU libraries, copy over multithreaded test programs and compile

Notes: The CPU libraries did not have pre-written multithreaded test programs so we wrote custom programs and these must be copied into the appropriate directory and compiled.  Instructions are below. 

### PARASAIL

```
git clone https://github.com/jeffdaily/parasail.git
# Create a directory for the include and lib files

cd parasail/

autoreconf -fi

./configure --prefix=<directory for include and lib files>

make -j8

make install -j8

```

### SEQAN3
```
mkdir seqan3_code

cd seqan3_code

mkdir build

mkdir source

git clone --recurse-submodules https://github.com/seqan/seqan3.git
```
### SSW
```
salloc -M notchpeak --account=sundar-np --partition=soc-np --nodes=1 --ntasks=16 -t 0:30:00

git clone https://github.com/mengyao/Complete-Striped-Smith-Waterman-Library.git

cd Complete-Striped-Smith-Waterman-Library.git

cd src

make
```

## Step 4: Submit the bash scripts for benchmarking

### Instructions to run the slurm scripts:

Modify the header of each of the scripts to work with your SLURM account and partition 

Example Header:
```
#!/bin/bash
#SBATCH -N 1                                        // Requests one CPU node
#SBATCH --mem=0	                                    // Requests all memory on the node
#SBATCH --account=soc-gpu-np                        // GPU account
#SBATCH --partition=soc-gpu-np                      // GPU partition
#SBATCH --job-name=adept_tango                      // Job Name
#SBATCH --time=8:00:00                              // Time requested
#SBATCH --gres=gpu:a100:1                           // Specifications of GPU requested
#SBATCH -w notch369                                 // Requested a specific node so that all results would be on the same node
#SBATCH --exclusive                                 // exclusive access
```

**To Test the GPU Libraries (ADEPT, TANGO, GASAL2)**

Notes: 

     - ADEPT and TANGO only do SHORT READ alignment, they will fail with reads > 1024 because of the Nvidia architecture limitation of 1024 threads per block.

     - The read and ref files must be pairwise (matching pairs of equal length, one read corresponding to one ref) and must have the extension _read.fasta and _ref.fasta for this bash script to work correctly

```

sbatch test_all_gpu_libraries.slurm.sh <path to ADEPT binary> <path to TANGO binary> <path to GASAL2 test program> <aa directory> <dna directory> <output_dir>

```

**To Test the CPU Libraries (SSW, Parasail, SEQAN3)**

### PARASAIL

```
sbatch test_all_parasail_aa.slurm.sh <path to Parasail binary> <aa directory> <output directory>

sbatch test_all_parasail_dna.slurm.sh <path to Parasail binary> <dna directory> <output directory>

```

### SSW

```

sbatch test_all_ssw_aa.slurm.sh <path to SSW binary> <aa directory> <output directory>

sbatch test_all_ssw_dna.slurm.sh <path to SSW binary> <dna directory> <output directory>

```

### SEQAN3

```

sbatch test_all_seqan3_aa_slurm.sh <path to SEQAN3 binary> <aa directory> <output directory>

sbatch test_all_seqan3_dna_slurm.sh <path to SEQAN3 binary> <dna directory> <output directory>

``

## Step 5: Install UPC++ and Metahipmer

MetaHipMer2 and UPC++, a required dependency, can be downloaded from:

https://bitbucket.org/berkeleylab/mhm2/src/master/

Installation and build instructions are available in the user guide:

https://bitbucket.org/berkeleylab/mhm2/src/1e50f8aa805917e0a4d8ee16a9ad1ec34f48d407/docs/mhm_guide.md

The TANGO code is integrated into the most recent version of MHM2.

## Step 6: Run Bash Scripts for Metahipmer Benchmarking


