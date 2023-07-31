//salloc -M kingspeak --account=sundar-kp --partition=soc-kp --nodes=1 --ntasks=16 -t 1:00:00 --pty /bin/bash -l
//export LD_LIBRARY_PATH=~/mylibs:$LD_LIBRARY_PATH
//gcc single_thread.c -I/uufs/chpc.utah.edu/common/home/u1369232/SOFTWARE_COMPARE/include -L/uufs/chpc.utah.edu/common/home/u1369232/SOFTWARE_COMPARE/lib -lparasail
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <parasail.h>
#include <parasail/matrices/blosum62.h>
#include <omp.h>
#include <chrono>
#include <vector>
#define NOW std::chrono::high_resolution_clock::now()
using namespace std;
using std::cout;
using std::endl;
using std::string;

// Define a struct to hold sequence information
struct Sequence {
    string header;
    string sequence;
};

// Define a struct to hold results information 
struct result_sw{
  int ref_number;
  int ref_begin;
  int ref_end;
  int que_begin;
  int que_end;
  int score;
  std::string cigar;
};

// Function to read a single FASTA file and return a vector of Sequence objects
vector<Sequence> read_fasta_file(string filename) {
    vector<Sequence> sequences;
    ifstream file(filename);

    if (file.is_open()) {
        string line;
        Sequence seq;

        while (getline(file, line)) {
            if (line[0] == '>') {
                if (!seq.header.empty()) {
                    sequences.push_back(seq);
                    seq.header.clear();
                    seq.sequence.clear();
                }
                seq.header = line.substr(1);
            } else {
                seq.sequence += line;
            }
        }

        if (!seq.header.empty()) {
            sequences.push_back(seq);
        }

        file.close();
    } else {
        cerr << "Error: could not open file " << filename << endl;
    }

    return sequences;
}

// Function to write a vector of result_sw structs to a file
void write_results_file(vector<result_sw> results, string filename) {
    ofstream outfile(filename);

    if (outfile.is_open()) {
        outfile << "score\tref_begin\tref_end\tque_begin\tque_end\tCIGAR" << endl;
        for (const auto& res : results) {
            outfile << res.score << '\t' << res.ref_begin << '\t' << res.ref_end << '\t'
                    << res.que_begin << '\t' << res.que_end << '\t' << res.cigar << endl;
        }
        outfile.close();
        cout << "Results written to " << filename << endl;
    } else {
        cerr << "Error: could not open file " << filename << " for writing" << endl;
    }
}

int main(int argc, char* argv[]) {
    // open the FASTA file for reading
    // Check that three command-line arguments were provided
    if (argc != 4) {
        cerr << "Error: expected three command-line arguments" << endl;
        cerr << "Usage: " << argv[0] << " Reference_file Query_file Output_file" << endl;
        return 1;
    }

   // Read the first FASTA file and store sequences in a vector
    vector<Sequence> ref = read_fasta_file(argv[1]);

    // Read the second FASTA file and store sequences in a vector
    vector<Sequence> que = read_fasta_file(argv[2]);

    // Open the output file for writing
    ofstream outfile(argv[3]);

    int threads=32;
    bool CIGAR = true;

    auto start=NOW;
   
    omp_set_num_threads(threads);
    vector<result_sw> results[threads];
#pragma omp parallel for
    for(int i = 0; i < ref.size(); i++) {
	int thread_id=omp_get_thread_num();
    	result_sw temp_res;

        parasail_result_t *result = NULL;
        parasail_cigar_t *c_result = NULL;
        result = parasail_sw_trace_striped_sat(que[i].sequence.c_str(), que[i].sequence.size(), ref[i].sequence.c_str(), ref[i].sequence.size(), 6, 1, &parasail_blosum62);
        c_result = parasail_result_get_cigar(result, que[i].sequence.c_str(), que[i].sequence.size(), ref[i].sequence.c_str(), ref[i].sequence.size(), &parasail_blosum62);
	temp_res.ref_number = i;
	temp_res.score = parasail_result_get_score(result);
        temp_res.ref_begin = c_result->beg_ref;
        temp_res.ref_end = result->end_ref;
        temp_res.que_begin = c_result->beg_query;
        temp_res.que_end = result->end_query;
	//temp_res.cigar = parasail_cigar_decode(c_result, que[i].sequence, que[i].sequence.size(), ref[i].sequence, ref[i].sequence.size(), parasail_blosum62);
	results[thread_id].push_back(temp_res);

	//parasail_cigar_free(c_result);
        //parasail_result_free(result);
    }
#pragma omp barrier
    auto end=NOW;
    std::chrono::duration<double>diff=end-start;

    cout << "Total Alignments:"<<que.size()<<"Total Time:"<< diff.count() <<endl;

    // Combine the results from all threads into a single vector
    vector<result_sw> final_results;
    for (auto& res_vec : results) {
        final_results.insert(final_results.end(), res_vec.begin(), res_vec.end());
    }

    // Write the final results to the output file
    write_results_file(final_results, argv[3]);
    return 0;    
}
