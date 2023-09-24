#include "ssw_cpp.h"
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
static const int8_t mat50[] = {
	//  A   R   N   D   C   Q   E   G   H   I   L   K   M   F   P   S   T   W   Y   V   B   Z   X   *
     	5, -2, -1, -2, -1, -1, -1,  0, -2, -1, -2, -1, -1, -3, -1,  1,  0, -3, -2,  0, -2, -1, -1, -5,	// A
       -2,  7, -1, -2, -4,  1,  0, -3,  0, -4, -3,  3, -2, -3, -3, -1, -1, -3, -1, -3, -1,  0, -1, -5,	// R
       -1, -1,  7,  2, -2,  0,  0,  0,  1, -3, -4,  0, -2, -4, -2,  1,  0, -4, -2, -3,  5,  0, -1, -5,	// N
       -2, -2,  2,  8, -4,  0,  2, -1, -1, -4, -4, -1, -4, -5, -1,  0, -1, -5, -3, -4,  6,  1, -1, -5,	// D
       -1, -4, -2, -4, 13, -3, -3, -3, -3, -2, -2, -3, -2, -2, -4, -1, -1, -5, -3, -1, -3, -3, -1, -5,	// C
       -1,  1,  0,  0, -3,  7,  2, -2,  1, -3, -2,  2,  0, -4, -1,  0, -1, -1, -1, -3,  0,  4, -1, -5,	// Q
       -1,  0,  0,  2, -3,  2,  6, -3,  0, -4, -3,  1, -2, -3, -1, -1, -1, -3, -2, -3,  1,  5, -1, -5,	// E
     	0, -3,  0, -1, -3, -2, -3,  8, -2, -4, -4, -2, -3, -4, -2,  0, -2, -3, -3, -4, -1, -2, -1, -5,	// G
       -2,  0,  1, -1, -3,  1,  0, -2, 10, -4, -3,  0, -1, -1, -2, -1, -2, -3,  2, -4,  0,  0, -1, -5,	// H
       -1, -4, -3, -4, -2, -3, -4, -4, -4,  5,  2, -3,  2,  0, -3, -3, -1, -3, -1,  4, -4, -3, -1, -5,	// I
       -2, -3, -4, -4, -2, -2, -3, -4, -3,  2,  5, -3,  3,  1, -4, -3, -1, -2, -1,  1, -4, -3, -1, -5,	// L
       -1,  3,  0, -1, -3,  2,  1, -2,  0, -3, -3,  6, -2, -4, -1,  0, -1, -3, -2, -3,  0,  1, -1, -5,	// K
       -1, -2, -2, -4, -2,  0, -2, -3, -1,  2,  3, -2,  7,  0, -3, -2, -1, -1,  0,  1, -3, -1, -1, -5,	// M
       -3, -3, -4, -5, -2, -4, -3, -4, -1,  0,  1, -4,  0,  8, -4, -3, -2,  1,  4, -1, -4, -4, -1, -5,	// F
       -1, -3, -2, -1, -4, -1, -1, -2, -2, -3, -4, -1, -3, -4, 10, -1, -1, -4, -3, -3, -2, -1, -1, -5,	// P
     	1, -1,  1,  0, -1,  0, -1,  0, -1, -3, -3,  0, -2, -3, -1,  5,  2, -4, -2, -2,  0,  0, -1, -5,	// S
    	0, -1,  0, -1, -1, -1, -1, -2, -2, -1, -1, -1, -1, -2, -1,  2,  5, -3, -2,  0,  0, -1, -1, -5, 	// T
       -3, -3, -4, -5, -5, -1, -3, -3, -3, -3, -2, -3, -1,  1, -4, -4, -3, 15,  2, -3, -5, -2, -1, -5, 	// W
       -2, -1, -2, -3, -3, -1, -2, -3,  2, -1, -1, -2,  0,  4, -3, -2, -2,  2,  8, -1, -3, -2, -1, -5, 	// Y
     	0, -3, -3, -4, -1, -3, -3, -4, -4,  4,  1, -3,  1, -1, -3, -2,  0, -3, -1,  5, -3, -3, -1, -5, 	// V
       -2, -1,  5,  6, -3,  0,  1, -1,  0, -4, -4,  0, -3, -4, -2,  0,  0, -5, -3, -3,  6,  1, -1, -5, 	// B
       -1,  0,  0,  1, -3,  4,  5, -2,  0, -3, -3,  1, -1, -4, -1,  0, -1, -2, -2, -3,  1,  5, -1, -5, 	// Z
       -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -5, 	// X
       -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5,  1 	// *
	};

	/* This table is used to transform amino acid letters into numbers. */
	int8_t aa_table[128] = {
		23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
		23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
		23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
		23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
		23, 0,  20, 4,  3,  6,  13, 7,  8,  9,  23, 11, 10, 12, 2,  23,
		14, 5,  1,  15, 16, 23, 19, 17, 22, 18, 21, 23, 23, 23, 23, 23,
		23, 0,  20, 4,  3,  6,  13, 7,  8,  9,  23, 11, 10, 12, 2,  23,
		14, 5,  1,  15, 16, 23, 19, 17, 22, 18, 21, 23, 23, 23, 23, 23
	};

  int8_t* table = aa_table;
  const int8_t* mat = mat50;

//printf("reading in the arguments\n");
string refFile = argv[1];
string queFile = argv[2];
string out_file = argv[3];
bool CIGAR = (stoi(argv[4]) == 1) ? true : false; 
int threads = stoi(argv[5]);

vector<string> ref_sequences, que_sequences;
string myInLine;
ifstream ref_file(refFile);
ifstream quer_file(queFile);
unsigned largestA = 0, largestB = 0;

int totSizeA = 0, totSizeB = 0;

//printf("reading in the ref file\n");
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

printf("reading in the query file\n");
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

//printf("declaring result vector\n");
auto start = NOW;
// //vector<int> global_strtA;
// //vector<int> global_endA;
// //vector<int> global_strtB;
// //vector<int> global_endB;
vector<result_sw> results[threads];

omp_set_num_threads(threads);
//printf("starting the for loop to align the sequences.\n");
#pragma omp parallel for
for(long unsigned i = 0; i < ref_sequences.size(); i++)
{
//printf("Sequence # i = %d\n",i);
string query = que_sequences[i];
string ref = ref_sequences[i];

int thread_id = omp_get_thread_num();

int32_t maskLen = strlen(query.c_str()) / 2;
maskLen = maskLen < 15 ? 15 : maskLen;
uint8_t gap_open = 6, gap_close = 1;
//printf("declaring aligner\n");
// Declares a default Aligner
StripedSmithWaterman::Aligner aligner(mat,24,table,128);
// Declares a default filter
//printf("declaring filter\n");
StripedSmithWaterman::Filter filter;
if(CIGAR)
filter= StripedSmithWaterman::Filter(true, true, 0, 32767);
else
filter = StripedSmithWaterman::Filter(true, false, 0, 32767);

//printf("declaring alignment\n");
// Declares an alignment that stores the result
StripedSmithWaterman::Alignment alignment;
//printf("aligning sequences\n");
//printf("%s\n",ref.c_str());
//printf("ref.size()\n",ref.size());
//printf("%s, %s, %d, %d, %d, %d\n",query.c_str(), ref.c_str(), ref.size(), filter, &alignment, maskLen);
aligner.Align(query.c_str(), ref.c_str(), ref.size(), filter, &alignment, maskLen);
////printf("recording results\n");
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
printf("printing results\n");
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

