# TANGO_BENCHMARKING

This repository contains all of the code and instructions to reproduce the results for TANGO:A GPU optimized traceback approach for sequence alignment algorithms, accepted as a short paper for the 13th Workshop on Irregular Applications: Architectures and Algorithms, a workshop at SC '23.

These instructions are for the A100 GPUs on the Notchpeak Cluster of the University of Utah CHPC. When installing on a different system, you should make appropriate changes to the 'salloc' and 'module load cuda' commands to make sure that you have an interactive allocation to a node with access to a GPU and that cuda is available.

## Overview

Step 1: Clone this github repository which holds the benchmarking scripts

Step 2: Clone GPU libraries and compile

Step 3: Clone CPU libraries, copy over multithreaded test programs and compile

Step 4: Submit the bash scripts for benchmarking

Step 5: Install UPC++ and Metahipmer

Step 6: Run Bash Scripts for Metahipmer Benchmarking

## Dependencies

- CMAKE 3.27.0 or greater

- NVCC 11.6 or greater

- GCC 10.0 or greater (required for SEQAN3)

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

The TANGO binary should now be located at

~/TANGO_BENCHMARKING/BENCHMARKING_SCRIPTS/SRC/TANGO/build/program_gpu

### ADEPT 

```
git clone https://github.com/mgawan/GPU-BSW.git

cd GPU-BSW

mkdir build

cd build

cmake CMAKE_BUILD_TYPE=Release ..

make
```

The ADEPT binary should now be located at

~/TANGO_BENCHMARKING/BENCHMARKING_SCRIPTS/SRC/GPU-BSW/build/program_gpu
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
The GASAL2 binary should now be located at

~/TANGO_BENCHMARKING/BENCHMARKING_SCRIPTS/SRC/GASAL2/test_prog/test_prog.out

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
Now link the parasail library and compile multithreaded_aa.cpp and multithreaded_dna.cpp

```
cd ~/TANGO_BENCHMARKING/BENCHMARKING_SCRIPTS/SRC/PARASAIL

gcc multithreaded_dna.cpp -I <include path>  -L <parasail lib path> -lparasail -lstdc++ -fopenmp -o multithreaded_dna

gcc multithreaded_aa.cpp -I <include path>  -L <parasail lib path> -lparasail -lstdc++ -fopenmp -o multithreaded_aa
```
Here are the instructions about the include path and the lib path taken directly from the parasail github

>By default, running "make install" will install parasail into /usr/local. You will find the parasail.h header in /usr/local/include and the parasail library, e.g., libparasail.a, in /usr/local/lib. If you specify a different prefix during configure, for example configure --prefix=/some/other/path, then look within the include and lib directories there for the parasail.h header and libparasail.so library, respectively.

>Don't forget to link your application to the parasail library. For example, gcc foo.c -I/where/you/installed/include -L/where/you/installed/lib -lparasail. Otherwise, you'll see errors such as undefined reference to 'parasail_sw'

The parasail binaries will now be located at

~/TANGO_BENCHMARKING/BENCHMARKING_SCRIPTS/SRC/PARASAIL/multithreaded_dna

~/TANGO_BENCHMARKING/BENCHMARKING_SCRIPTS/SRC/PARASAIL/multithreaded_aa

### SEQAN3
These set up instructions are taken from the Seqan3 Documentation [Quick Setup](https://docs.seqan.de/seqan3/3-master-user/setup.html)

```
cd SEQAN3

mkdir tutorial

cd tutorial

mkdir build

mkdir source

git clone --recurse-submodules https://github.com/seqan/seqan3.git

cp CMakeLists.txt source

cp seqan_aa_align_multithread.cpp source

cp seqan_dna_align_multithread.cpp source

cd build

cmake -DCMAKE_BUILD_TYPE=Release ../source

make

```

The binaries for SEQAN3 will now be located at

~/TANGO_BENCHMARKING/BENCHMARKING_SCRIPTS/SRC/SEQAN3/tutorial/build/seqan_aa_align_multithread

~~/TANGO_BENCHMARKING/BENCHMARKING_SCRIPTS/SRC/SEQAN3/tutorial/build/seqan_dna_align_multithread

### SSW
```
salloc -M notchpeak --account=sundar-np --partition=soc-np --nodes=1 --ntasks=16 -t 0:30:00

cd SSW

git clone https://github.com/mengyao/Complete-Striped-Smith-Waterman-Library.git

cp Makefile Complete-Striped-Smith-Waterman-Library/src

cp example_dna.cpp Complete-Striped-Smith-Waterman-Library/src

cp example_aa.cpp Complete-Striped-Smith-Waterman-Library/src

cd Complete-Striped-Smith-Waterman-Library

cd src

make ssw_test_aa_multi ssw_test_dna_multi
```

The SSW binaries will now be located at

~/TANGO_BENCHMARKING/BENCHMARKING_SCRIPTS/SRC/SSW/Complete-Striped-Smith-Waterman-Library/src/ssw_test_aa_multi

~/TANGO_BENCHMARKING/BENCHMARKING_SCRIPTS/SRC/SSW/Complete-Striped-Smith-Waterman-Library/src/ssw_test_dna_multi


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

```

## Step 5: Install UPC++ and Metahipmer

MetaHipMer2 and UPC++, a required dependency, can be downloaded from [MHM2 github](https://bitbucket.org/berkeleylab/mhm2/src/master/).

Installation and build instructions are available in the [user guide](https://bitbucket.org/berkeleylab/mhm2/src/1e50f8aa805917e0a4d8ee16a9ad1ec34f48d407/docs/mhm_guide.md).

Test after installation to make sure that the installation is working correctly.

Note: An earlier version of the TANGO code (with diagonal major indexing (DMI), not stacked DMI) is integrated into the most current version of MHM2.  To run the comparisons for this paper we compared against 

```
commit 56179b9f4fb679c3ec034b709aa34058113d72d9 (HEAD -> master, origin/master, origin/HEAD)
Merge: 6e9fcaf 5e7ebab
Author: Rob Egan <rsegan@lbl.gov>
Date:   Wed Jun 22 18:43:01 2022 +0000
```
Install the current version of mhm into a directory called mhm_tango

Install the mhm2 from the above listed commit into a directory called mhm_no_tango
## Step 6: Run Bash Scripts for Metahipmer Benchmarking

Benchmark both versions by running mhm2 with the --post-asm-align flag (TANGO is not used if the SAM file with CIGAR strings is not requested).

```
cd ~/mhm_tango
mhm2.py -r <metagenomic fastq file> --post-asm-align

cd ~/mhm_no_tango
mhm2.py -r <metagenomic fastq file> --post-asm-align

```

