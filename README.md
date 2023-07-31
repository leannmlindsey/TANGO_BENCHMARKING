# TANGO_BENCHMARKING

This repository contains all of the code to reproduce the benchmarking results for TANGO. 

Instructions for cloning and installing the libraries:

## GASAL2
These instructions are for the A100 GPUs on the Notchpeak Cluster of the University of Utah CHPC

```
salloc -M notchpeak --account=soc-gpu-np --partition=soc-gpu-np --nodes=1 --ntasks=8  --gres=gpu:a100:1 -t 0:30:00

module load cuda

git clone https://github.com/nahmedraja/GASAL2.git

cd GASAL2

./configure.sh <path to cuda>

make GPU_SM_ARCH='sm_80' MAX_QUERY_LEN=160 N_CODE='0x4E' N_PENALTY=-1
# sm_80 is the archecture code for Nvidia A100

cd test_prog

gunzip  query_batch.fasta.gz target_batch.fasta.gz

make 

# test to see if it runs without error
./test_prog.out -a 1 -b 4 -q 6 -r 1 -s -t -p -y "local" query_batch.fasta target_batch.fasta
# -s flag to get the start positions
# -t flag to use traceback
# -p flag to print the results including the CIGAR string
```
## PARASAIL

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

## SEQAN3
```
salloc -M notchpeak --account=sundar-np --partition=soc-np --nodes=1 --ntasks=16 -t 0:30:00

mkdir seqan3_code

cd seqan3_code

mkdir build

mkdir source

git clone --recurse-submodules https://github.com/seqan/seqan3.git
```
## SSW
```
salloc -M notchpeak --account=sundar-np --partition=soc-np --nodes=1 --ntasks=16 -t 0:30:00

git clone https://github.com/mengyao/Complete-Striped-Smith-Waterman-Library.git

cd Complete-Striped-Smith-Waterman-Library.git

cd src

make
```

