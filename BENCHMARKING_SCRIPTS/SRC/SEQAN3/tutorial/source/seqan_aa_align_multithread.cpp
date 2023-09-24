#include <string> // for std::string
#include <string.h> // for std::string
#include <tuple>  // for std::tie
#include <vector> // for std::vector
#include <omp.h>
#include <seqan3/alignment/aligned_sequence/all.hpp> // for alignment stream operator <<
#include <seqan3/alignment/configuration/all.hpp>    // for all configs in the seqan3::align_cfg namespace
#include <seqan3/alignment/pairwise/all.hpp>
#include <seqan3/alignment/scoring/all.hpp>
#include <seqan3/alphabet/aminoacid/aa27.hpp>
#include <seqan3/argument_parser/all.hpp>      // for argument_parser
#include <seqan3/core/debug_stream.hpp>        // for debug_stream
#include <seqan3/io/sequence_file/all.hpp>   // for sequence_file_input
#include <chrono>
#include <seqan3/io/sequence_file/input.hpp>
#define NOW std::chrono::high_resolution_clock::now()
using namespace std;
using std::cout;
using std::endl;
using std::string;

struct result_sw{
  int ref_number;
  int ref_begin;
  int ref_end;
  int que_begin;
  int que_end;
  int score;
  string cigar;
};

int main(int argc, char * argv[])
{
    using namespace seqan3::literals;

     // Receive the filename as program argument.
    std::string que_filename{};
    std::string ref_filename{};
    std::string out_file = "/global/homes/l/lindsey/FINAL_repos/SEQAN3/tutorial/build/out.txt";

    bool CIGAR = true;
    seqan3::argument_parser parser("My-first-program", argc, argv);
    parser.add_positional_option(ref_filename, "The reference filename of the file to read.");
    parser.add_positional_option(que_filename, "The reference filename of the file to read.");
    int threads=32;
 
    try
    {
        parser.parse();
    }
    catch (seqan3::argument_parser_error const & ext)
    {
        seqan3::debug_stream << "[PARSER ERROR] " << ext.what() << '\n';
        return 0;
    }
 
    seqan3::debug_stream << "Reading file " <<"\n";
 
    // Create the vector to store sequences of type seqan3::dna5_vector.
    //std::vector<seqan3::aa27_vector> ref_sequences;
    //std::vector<seqan3::aa27_vector> que_sequences;
 
    // Iterate through the file and store the sequences.
    using traits_type = seqan3::sequence_file_input_default_traits_aa;
    seqan3::sequence_file_input<traits_type> file_in{ref_filename};
    
    using record_type = decltype(file_in)::record_type;
    std::vector<record_type> ref_sequences{};
   
    for (auto & record : file_in)
    {
        ref_sequences.push_back(std::move(record));
    }
    using traits_type = seqan3::sequence_file_input_default_traits_aa;
    seqan3::sequence_file_input<traits_type> fin{que_filename};

    using record_type = decltype(fin)::record_type;
    std::vector<record_type> que_sequences{};
    for (auto & record : fin)
    {
        que_sequences.push_back(std::move(record));
    }
    
    auto start = NOW;
    vector<result_sw> results[threads];

    auto min_cfg = seqan3::align_cfg::method_local{}
                     | seqan3::align_cfg::scoring_scheme{
                       seqan3::aminoacid_scoring_scheme{seqan3::aminoacid_similarity_matrix::blosum62}} 
                     | seqan3::align_cfg::gap_cost_affine{seqan3::align_cfg::open_score{-9}, seqan3::align_cfg::extension_score{-2}}
                     | seqan3::align_cfg::output_score{}
                     | seqan3::align_cfg::output_end_position{}
                     | seqan3::align_cfg::output_begin_position{}
                     | seqan3::align_cfg::output_sequence1_id{}
                     | seqan3::align_cfg::output_sequence2_id{}
                     | seqan3::align_cfg::output_alignment{};
    
    //Need to add Open penalty and Extend penalty and I can't find an example of that in the SEQAN3 documentation

    omp_set_num_threads(threads);
    #pragma omp parallel for
    for(long unsigned int i=0; i<ref_sequences.size(); i++){

        int thread_id = omp_get_thread_num();
        result_sw temp_res;
        
	    for (auto && res : align_pairwise(std::tie(que_sequences[i].sequence(), ref_sequences[i].sequence()),min_cfg))
			{
            auto aligned_seq = res.alignment();
            temp_res.ref_number = i;
            temp_res.ref_begin = res.sequence1_begin_position();
            temp_res.ref_end = res.sequence1_end_position();
            temp_res.que_begin = res.sequence2_begin_position();
            temp_res.que_end = res.sequence2_end_position();
            temp_res.score = res.score();
	        //if(CIGAR){
                //temp_res.cigar = alignment.cigar_string;
            //}
            results[thread_id].push_back(temp_res);
        }
    }
    #pragma omp barrier
    auto end=NOW;
    std::chrono::duration<double>diff=end-start;
    cout << "Total Alignments:"<<que_sequences.size()<<" Total Time:"<< diff.count() <<endl;

    ofstream results_file(out_file);
    for(int j = 0; j < threads; j++){
        for(int k = 0; k < results[j].size(); k++){
            auto loc_vec = results[j];
            results_file << loc_vec[k].ref_number << '\t' << loc_vec[k].score<<"\t"<<loc_vec[k].ref_begin<<"\t"<<loc_vec[k].ref_end<<"\t"<<loc_vec[k].que_begin<<"\t"<<loc_vec[k].que_end;
            if(CIGAR)
                results_file<<'\t'<<loc_vec[k].cigar<<endl;
            else
                results_file<<endl;
        }
    }


    return 0;
}
 
