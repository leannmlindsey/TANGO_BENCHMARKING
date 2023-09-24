#include <string> // for std::string
#include <tuple>  // for std::tie
#include <vector> // for st
#include <concepts>
#include <ranges>
#include <omp.h>
#include <seqan3/alignment/detail/pairwise_alignment_concept.hpp>
#include <seqan3/alphabet/cigar/cigar.hpp>
#include <seqan3/alignment/aligned_sequence/all.hpp> // for alignment stream operator <<
#include <seqan3/alignment/configuration/all.hpp>    // for all configs in the seqan3::align_cfg namespace
#include <seqan3/alignment/pairwise/all.hpp>
#include <seqan3/alphabet/nucleotide/dna5.hpp>
#include <seqan3/argument_parser/all.hpp>      // for argument_parser
#include <seqan3/core/debug_stream.hpp>        // for debug_stream
#include <seqan3/io/sequence_file/all.hpp>   // for sequence_file_input
#include <chrono>
#include <seqan3/io/sam_file/all.hpp>
#include <seqan3/io/sam_file/detail/cigar.hpp>
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
  std::string cigar;
};

int main(int argc, char * argv[])
{
    // Receive the filename as program argument.
    std::string que_filename{};
    std::string ref_filename{};
    std::string out_file = "/global/homes/l/lindsey/FINAL_repos/SEQAN3/tutorial/build/out.txt";
    //std::filesystem::path sam_path{"out.sam"};
    //auto filename = std::filesystem::current_path() / "out2.sam";
    //seqan3::sam_file_output fout{filename};
    

    bool CIGAR = true;
    seqan3::argument_parser parser("My-first-program", argc, argv);
    parser.add_positional_option(ref_filename, "The reference filename of the file to read.");
    parser.add_positional_option(que_filename, "The reference filename of the file to read.");
    int threads=68;
 
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
    std::vector<seqan3::dna5_vector> ref_sequences;
    std::vector<seqan3::dna5_vector> que_sequences;
 
    // Iterate through the file and store the sequences.
    
    //seqan3::sam_file_output sam_out{sam_path,
    //                                seqan3::fields<seqan3::field::seq,
    //                                               seqan3::field::id,
    //                                               seqan3::field::ref_id,
    //                                               seqan3::field::ref_offset,
    //                                               seqan3::field::alignment,
     //                                              seqan3::field::qual,
      //                                             seqan3::field::mapq>{}};
    seqan3::sequence_file_input file_in{ref_filename};
    for (auto & record : file_in)
    {
        ref_sequences.push_back(record.sequence());
    }

    seqan3::sequence_file_input fin{que_filename};
    for (auto & record : fin)
    {
        que_sequences.push_back(record.sequence());
    }

    auto start = NOW;
    //vector<int> global_strtA;
    //vector<int> global_endA;
    //vector<int> global_strtB;
    //vector<int> global_endB;
    vector<result_sw> results[threads];

    auto min_cfg = seqan3::align_cfg::method_local{}
                 | seqan3::align_cfg::scoring_scheme{
                     seqan3::nucleotide_scoring_scheme{seqan3::match_score{3}, seqan3::mismatch_score{-3}}}
                     | seqan3::align_cfg::gap_cost_affine{seqan3::align_cfg::open_score{-6}, seqan3::align_cfg::extension_score{-1}}
                     | seqan3::align_cfg::output_score{}
                     | seqan3::align_cfg::output_end_position{}
                     | seqan3::align_cfg::output_begin_position{}
                     | seqan3::align_cfg::output_sequence1_id{}
                     | seqan3::align_cfg::output_sequence2_id{}
                     | seqan3::align_cfg::output_alignment{};
                   

    omp_set_num_threads(threads);
    
    #pragma omp parallel for
    for(int i=0; i<que_sequences.size(); i++){
        int thread_id = omp_get_thread_num();
        result_sw temp_res;
        
        //std::cout << "Sequence Number" << i << '\n';
        //std::cout << "Query Seq: ";
	    //seqan3::debug_stream << que_sequences[i] <<'\n';
        //std::cout << "Ref Seq: ";
    	//seqan3::debug_stream << ref_sequences[i] <<'\n';
        
	    for (auto && res : align_pairwise(std::tie(que_sequences[i], ref_sequences[i]),min_cfg))
			{
            //seqan3::debug_stream << res.sequence1_id() << '\t';
            //seqan3::debug_stream << res.sequence2_id() << '\t';
			//seqan3::debug_stream << res.score() << '\t';
            //seqan3::debug_stream << res.sequence1_end_position() << '\t';
            //seqan3::debug_stream << res.sequence1_begin_position() << '\t';
            //seqan3::debug_stream << res.sequence2_end_position() << '\t';
            //seqan3::debug_stream << res.sequence2_begin_position() << '\n';
			//seqan3::debug_stream << res.alignment() << '\n';
            temp_res.ref_number = i;
            temp_res.ref_begin = res.sequence1_begin_position();
            temp_res.ref_end = res.sequence1_end_position();
            temp_res.que_begin = res.sequence2_begin_position();
            temp_res.que_end = res.sequence2_end_position();
            temp_res.score = res.score();
            //temp_res.cigar = res.output_alignment();
            //temp_res.cigar = res.cigar();
            //fout.push_back(res);
	    //if(CIGAR)
		  //  temp_res.cigar = seqan3::detail::get_cigar_string(std::make_pair(que_sequences[i], ref_sequences[i]));
			}
            results[thread_id].push_back(temp_res);
        }
    #pragma omp barrier
    auto end=NOW;
    std::chrono::duration<double>diff=end-start;
    cout << "Total Alignments:"<<que_sequences.size()<<"Total Time:"<< diff.count() <<endl;

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
 
