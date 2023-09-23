Instructions to run the slurm scripts:

Modify the header of each of the scripts to work with your SLURM account and partition 

Example Header:
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


To Test the GPU Libraries (ADEPT, TANGO, GASAL2)

Notes: 
     ADEPT and TANGO only do SHORT READ alignment, they will fail with reads > 1024 because of the Nvidia architecture limitation of 1024 threads per block.
     The read and ref files must be pairwise (matching pairs of equal length, one read corresponding to one ref) 
     and must have the extension _read.fasta and _ref.fasta for this bash script to work correctly

sbatch test_all_gpu_libraries.slurm.sh <path to ADEPT binary> <path to TANGO binary> <path to GASAL2 test program> <aa directory> <dna directory> <output_dir>

To Test the CPU Libraries (SSW, Parasail, SEQAN3)

PARASAIL

sbatch test_all_parasail_aa.slurm.sh <path to Parasail binary> <aa directory> <output directory>
sbatch test_all_parasail_dna.slurm.sh <path to Parasail binary> <dna directory> <output directory>

SSW
sbatch test_all_ssw_aa.slurm.sh <path to SSW binary> <aa directory> <output directory>
sbatch test_all_ssw_dna.slurm.sh <path to SSW binary> <dna directory> <output directory>

SEQAN3

sbatch test_all_seqan3_aa_slurm.sh <path to SEQAN3 binary> <aa directory> <output directory>
sbatch test_all_seqan3_dna_slurm.sh <path to SEQAN3 binary> <dna directory> <output directory>
