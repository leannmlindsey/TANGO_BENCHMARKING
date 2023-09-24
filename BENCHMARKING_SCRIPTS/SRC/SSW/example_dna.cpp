#include "ssw_cpp.h"
#include <stdio.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <string.h>
#include <string>
#include <vector>
using namespace std;
using std::cout;
using std::endl;
using std::string;
#define NOW std::chrono::high_resolution_clock::now()

struct result_sw{
int ref_begin;
int ref_end;
int que_begin;
int que_end;
int score;
string cigar;
};

int
main(int argc, char* argv[])
{
printf("DNA SSW\n");
string refFile = argv[1];
string queFile = argv[2];
string out_file = argv[3];
bool CIGAR = (stoi(argv[4]) == 1) ? true : false; 
int threads = stoi(argv[5]);
printf("Running with CIGAR %d, threads %d\n", CIGAR, threads);
vector<string> ref_sequences, que_sequences;
string myInLine;
ifstream ref_file(refFile);
ifstream quer_file(queFile);
unsigned largestA = 0, largestB = 0;

int totSizeA = 0, totSizeB = 0;

if(ref_file.is_open()){
while(getline(ref_file, myInLine)){
if(myInLine[0] == '>'){
continue;
}else{
string seq = myInLine;
ref_sequences.push_back(seq);
totSizeA += seq.size();
if(seq.size() > largestA){
largestA = seq.size();
}
}
}
ref_file.close();
}

if(quer_file.is_open()){
while(getline(quer_file, myInLine)){
if(myInLine[0] == '>'){
continue;
}else{
string seq = myInLine;
que_sequences.push_back(seq);
totSizeB += seq.size();
if(seq.size() > largestB){
largestB = seq.size();
}
}
}
quer_file.close();
}

auto start = NOW;
vector<int> global_strtA;
vector<int> global_endA;
vector<int> global_strtB;
vector<int> global_endB;
vector<result_sw> results[threads];

omp_set_num_threads(threads);
#pragma omp parallel for
for(long unsigned i = 0; i < ref_sequences.size(); i++)
{
string query = que_sequences[i];
string ref = ref_sequences[i];
//printf("i=%d\n",i);
//test`comment
int thread_id = omp_get_thread_num();

int32_t maskLen = strlen(query.c_str()) / 2;
maskLen = maskLen < 15 ? 15 : maskLen;
uint8_t match =3, mismatch=3, gap_open = 6, gap_close = 1;

// Declares a default Aligner
StripedSmithWaterman::Aligner aligner(match, mismatch, gap_open, gap_close);
// Declares a default filter
StripedSmithWaterman::Filter filter;
if(CIGAR)
filter= StripedSmithWaterman::Filter(true, true, 0, 32767);
else
filter = StripedSmithWaterman::Filter(true, false, 0, 32767);

// Declares an alignment that stores the result
StripedSmithWaterman::Alignment alignment;

aligner.Align(query.c_str(), ref.c_str(), ref.size(), filter, &alignment, maskLen);
result_sw temp_res;
temp_res.ref_begin = alignment.ref_begin;
temp_res.ref_end = alignment.ref_end;
temp_res.que_begin = alignment.query_begin;
temp_res.que_end = alignment.query_end;
temp_res.score = alignment.sw_score;
if(CIGAR){
temp_res.cigar = alignment.cigar_string;
}

results[thread_id].push_back(temp_res);
}

#pragma omp barrier
auto end = NOW;
  std::chrono::duration<double> diff = end - start;
    cout <<"Total Alignments:"<<ref_sequences.size()<<"\t largest Ref:"<<largestA<<"\t largest Que:"<<largestB<<"\t Total Time:"<< diff.count() <<endl;

    ofstream results_file(out_file);
    for(int j = 0; j < threads; j++){
    for(int k = 0; k < results[j].size(); k++){
    auto loc_vec = results[j];
    results_file << loc_vec[k].score<<"\t"<<loc_vec[k].ref_begin<<"\t"<<loc_vec[k].ref_end<<
    "\t"<<loc_vec[k].que_begin<<"\t"<<loc_vec[k].que_end;
    if(CIGAR)
    results_file<<"\t"<<loc_vec[k].cigar<<endl;
    else
    results_file<<endl;

    }
    }

    return 0;
    }

