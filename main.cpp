#include <iostream>
#include <cstdlib>
#include <string>
#include <ostream>
#include <list>
#include <fstream>
#include <sstream>
#include <queue>
#include <cmath>
#include<algorithm>
//#include <filesystem>

#include <limits>

using namespace std;

const int NUMB_MEMORY_BLOCK = 22;

#include "utils.h"


/*
const int NUMB_MEMORY_BLOCK = 22;
vector< vector<string> > ptr_tmp_files;

int which_relation = 0; // 0 if relation is Emp and 1 if relation is Dept

int sort_col = 0;
*/


int main(){

    string filename1 = "Emp.csv";
    string filename2 = "Dept.csv";

    int sort_col1 = 0;
    which_relation = 0;
    ReadAndSortRelation(filename1, sort_col1);
    int sort_col2 = 3;
    which_relation = 1;
    ReadAndSortRelation(filename2, sort_col2);

    //// read the sorted blocks from file
    ////vector<vector<vector<string> >> All_runs;

    
    // Open output file to write the results of the join
    //string join_filename = "join_result.txt";
    string join_filename = "join_result.csv";
    ofstream join_rel_file(join_filename, ofstream::out | ofstream::app);

    // number of blocks available (In memory) to merge the sorted blocks of each relation
    float nbr_block_p_rel = (float)(NUMB_MEMORY_BLOCK-1) / (float) 2;
    const int N_BLOCK_PER_REL = floor( nbr_block_p_rel );

    // check if the number of runs is greater than M/2 for eahc of the relations
    if (ptr_tmp_files[0].size() > N_BLOCK_PER_REL || ptr_tmp_files[1].size() > N_BLOCK_PER_REL){
        cout << "Cannot have more than M-1 blocks from relations R1 and R2 in main memory for merge-join operation!";
        cout << "Number of memory blocks available doest allow to sort such huge relation";
        exit(1);
    }


    // Keep track of position of the read blocks in the runs of each relation.
    vector<int> read_runs_rel1(ptr_tmp_files[0].size(), 1);
    vector<int> read_runs_rel2(ptr_tmp_files[1].size(), 1);

    //bool all_blocks_rel1_read = false;
    //bool all_blocks_rel2_read = false;

    bool merge_join_complete = false;

    //while (all_blocks_rel1_read && all_blocks_rel2_read){
    while (!merge_join_complete){

        vector<vector<string> > tuples_rel1;
        vector<vector<string> > tuples_rel2;

        // Loop through all the runs and get the jth block
        for (unsigned int i = 0; i < ptr_tmp_files[0].size(); i++){
            string read_tuple_rel1 = GetLine(ptr_tmp_files[0][i], read_runs_rel1[i]);
            if (read_tuple_rel1 != ""){
                tuples_rel1.push_back( StringToVect(read_tuple_rel1) );
            }
            else if (i > 0) {
                read_runs_rel1.erase(read_runs_rel1.begin() + i);
                ptr_tmp_files[0].erase(ptr_tmp_files[0].begin() + i);
            }
            else{
                merge_join_complete = true;
            }
        } 

        // Loop through all the runs and get the jth block
        for (unsigned int i = 0; i < ptr_tmp_files[1].size(); i++){
            string read_tuple_rel2 = GetLine(ptr_tmp_files[1][i], read_runs_rel2[i]);
            if (read_tuple_rel2 != ""){
                tuples_rel2.push_back( StringToVect(read_tuple_rel2) );
            }
            else if (i > 0){
                read_runs_rel2.erase(read_runs_rel2.begin() + i);
                ptr_tmp_files[1].erase(ptr_tmp_files[1].begin() + i);
            }
            else{
                merge_join_complete = true;
            }
        } 

        if (tuples_rel1.size() > 0 && tuples_rel2.size() > 0 ){

            for (int i = 0;  i < tuples_rel1.size(); i++){
                int count1 = 0; // count # id_rel1 < id_rel2
                for (int j = 0; j < tuples_rel2.size(); j++){
                    if ( stoi(tuples_rel1[i][sort_col1]) == stoi(tuples_rel2[j][sort_col2]) ){
                        vector<vector<string>> join_rel2_rel1 = left_join(tuples_rel2, sort_col2, tuples_rel1, sort_col1);
        
                        for (auto tuple: join_rel2_rel1){
                            for (string attribute: tuple){
                                join_rel_file << attribute << ",";
                            }
                            join_rel_file << "\n";
                        }
                        read_runs_rel2[j]++;
                    }
                    else if ( stoi(tuples_rel1[i][sort_col1]) < stoi(tuples_rel2[j][sort_col2]) ){
                        count1++;
                    }
                }

                // the id at the current value of i is the minimum and we should call another block of the same run
                if ( count1 == tuples_rel2.size() ){
                    read_runs_rel1[i]++;
                }
            }

            for (int j = 0;  j < tuples_rel2.size(); j++){
                int count2 = 0; // count # id_rel1 < id_rel2
                for (int i = 0; i < tuples_rel1.size(); i++){
                    
                    if ( stoi(tuples_rel2[j][sort_col2]) < stoi(tuples_rel1[i][sort_col1]) ){
                        count2++;
                    }
                }

                // the id at the current value of i is the minimum and we should call another block of the same run
                if ( count2 == tuples_rel1.size() ){
                    read_runs_rel2[j]++;
                }
            }
        }
        else{
            merge_join_complete = true;
        }



    }

    join_rel_file.close();
    
    //filesystem::remove_all("tmp");

    return 0;

}