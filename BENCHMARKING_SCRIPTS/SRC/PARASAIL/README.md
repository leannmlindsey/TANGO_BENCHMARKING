# Compile Instructions

export LD_LIBRARY_PATH=~/mylibs:$LD_LIBRARY_PATH

gcc multithreaded_dna.cpp -I<dir to include files> -L<dir to lib files> -lparasail -lstdc++ -fopenmp -o multithreaded_dna

gcc multithreaded_aa.cpp -I<dir to include files> -L<dir to lib files> -lparasail -lstdc++ -fopenmp -o multithreaded_aa
# Run Instructions

./multithreaded_dna ../test-data/dna-reference.fasta ../test-data/dna-query.fasta outfile_parasail_dna.txt

./multithreaded_aa ../test-data/protein-reference.fasta ../test-data/protein-query.fasta outfile_parasail_aa.txt

