# TANGO_BENCHMARKING

This repository contains all of the code and instructions to reproduce the results for TANGO:A GPU optimized traceback approach for sequence alignment algorithms, accepted as a short paper for the 13th Workshop on Irregular Applications: Architectures and Algorithms, a workshop at SC '23.

These instructions are for the A100 GPUs on the Notchpeak Cluster of the University of Utah CHPC. When installing on a different system, you should make appropriate changes to the 'salloc' and 'module load cuda' commands to make sure that you have an interactive allocation to a node with access to a GPU and that cuda is available.

## Overview

Step 1: Clone this github repository which holds the benchmarking scripts

Step 2: Clone TANGO and comparison libraries and compile binaries

Step 3: Submit the bash scripts for benchmarking
 

## Step 1: Clone this github repository which holds the benchmarking scripts

```
salloc -M notchpeak --account=soc-gpu-np --partition=soc-gpu-np --nodes=1 --ntasks=8  --gres=gpu:a100:1 -t 0:30:00

module load cuda

git clone https://github.com/leannmlindsey/TANGO_BENCHMARKING.git

cd TANGO_BENCHMARKING

cd SRC

```

## Step 2: Clone TANGO and comparison libraries and compile binaries

Instructions for cloning and installing the libraries:

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
### PARASAIL

```
salloc -M notchpeak --account=sundar-np --partition=soc-np --nodes=1 --ntasks=16 -t 0:30:00 

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
salloc -M notchpeak --account=sundar-np --partition=soc-np --nodes=1 --ntasks=16 -t 0:30:00

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

## Step 3: Submit the bash scripts for benchmarking

