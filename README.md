# TANGO_BENCHMARKING

This repository contains all of the code to reproduce the benchmarking results for TANGO.

## GASAL2

```
salloc -M notchpeak --account=soc-gpu-np --partition=soc-gpu-np --nodes=1 --ntasks=8  --gres=gpu:a100:1 -t 0:30:00 

module load cuda

git clone https://github.com/nahmedraja/GASAL2.git

cd GASAL2

./configure.sh /uufs/chpc.utah.edu/sys/spack/linux-rocky8-nehalem/gcc-8.5.0/cuda-11.6.2-hgkn7czv7ciyy3gtpazwk2s72msbw6l2

make GPU_SM_ARCH='sm_80' MAX_QUERY_LEN=160 N_CODE='0x4E' N_PENALTY=-1  

cd test_prof

gunzip  query_batch.fasta.gz target_batch.fasta.gz

make 

./test_prog.out -a 1 -b 4 -q 6 -r 1 -s -t -p -y "local" query_batch.fasta target_batch.fasta
```
