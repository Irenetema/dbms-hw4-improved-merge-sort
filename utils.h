#include <iostream>
#include <cstdlib>
#include <string>
#include <ostream>
#include <list>
#include <fstream>
#include <sstream>
#include <cmath>
#include<algorithm>
#include <limits>

#include <sys/types.h>
#include <sys/stat.h>


vector<string> ptr_tmp_files;

int sort_col = 0;


class Block {
    // each block contains at most one tuple so we will create a vector of size 1 containing strings
    //vector<int> records_ptr;
    vector<string> records;

    public:
    Block() {
        //records_ptr.clear();
        records.clear();
    }

    string get_record(int i){
        return records[i];
    }

    /*
    void add(int x, string value) {
        //records_ptr.push_back(x);
        records.push_back(value);
    }
    */

    void add(string value) {
        records.push_back(value);
    }

    void clear() {
        //records_ptr.clear();
        records.clear();
    }

    void clearElements(vector<string> &v) {
        for(unsigned int i = 0; i < records.size(); i++) {
            v.push_back(records[i]);
        }
        records.clear();
    }

};


// used to sort a 2d vector according to a given column
//bool sortcol( const vector<int>& v1, const vector<int>& v2, int col ) { 
bool sortcol( const vector<string>& v1, const vector<string>& v2) { 
    return stoi(v1[sort_col]) < stoi(v2[sort_col]); 
}

// get sets of M blocks and sort them according to the specified attribute (column) 
vector <vector <string> > sort_blocks(vector<Block> blocks, int sort_column){

    vector <vector <string> > array_of_tuples;
    for (auto block : blocks){
        string cell;
        stringstream tuple(block.get_record(0));
        vector<string> row_parse;
        while (getline(tuple, cell, ','))
        {
            row_parse.push_back(cell);

            //sorted_blocks_file << cell << "\t";
        }
        array_of_tuples.push_back(row_parse);
        //sorted_blocks_file << "\n";
    }

    // Use of "sort()" for sorting on basis 
    // of 2nd column in descending order 
    sort(array_of_tuples.begin(), array_of_tuples.end(), sortcol); 

    return array_of_tuples;
}


//vector<vector<string> > ReadAndSortRelation(string filename)
void ReadAndSortRelation(string filename, int col)
{
    ifstream data(filename);

    //vector<string> tmp_filenames;

    sort_col = col;

    // check if the file exist
    if (data.good()){
        cout << "File \"" << filename << "\" found! \n";
    }
    else{
        cout << "File \"" << filename << "\" NOT FOUND! \n";
    }

    // Create a temporary directory to store the sorted file if none exist
    struct stat info;
    if( stat( "./tmp", &info ) != 0 ){
        cout << "Please create and empty directory named \"tmp\" to store the sorted runs \n\n";
        exit(1);
    }
    else{
        cout << "\"tmp\" directory found! \n\n";
    }

    string line;
    vector<Block> vectMblocks;
    int count = 0;
    while (getline(data, line))
    {
        
        if (vectMblocks.size() < NUMB_MEMORY_BLOCK){
            Block block;
            //block.add((int) &line, line);
            block.add(line);
            vectMblocks.push_back(block);
            block.clear(); // free memory
        }
        else if(data.eof() || vectMblocks.size() == NUMB_MEMORY_BLOCK){
            vector<vector <string>> sorted_blocks = sort_blocks(vectMblocks, col);

            string sorted_filename = "tmp/sorted_blocks_" + filename.substr(0, filename.length()-4) + "_" + to_string(count) + ".txt";
            ofstream sorted_blocks_file;
            sorted_blocks_file.open (sorted_filename);

            for (auto tuple: sorted_blocks){
                for (string attr: tuple){
                    sorted_blocks_file << attr << "\t";
                }
                sorted_blocks_file << "\n";
            }

            sorted_blocks_file.close();
            ptr_tmp_files.push_back(sorted_filename);

            count++;

            //empty vectMblocks for next run
            while(vectMblocks.size() > 0){
                vectMblocks.pop_back();
            }

            if (!data.eof()){
                Block block;
                //block.add((int) &line, line);
                block.add(line);
                vectMblocks.push_back(block);
                block.clear(); // free memory
            }

        }
    }
    
    // check if the last block was sorted
    if (vectMblocks.size() > 0){
        vector<vector <string>> sorted_blocks = sort_blocks(vectMblocks, col);

        string sorted_filename = "tmp/sorted_blocks_" + filename.substr(0, filename.length()-4) + "_" + to_string(count) + ".txt";
        ofstream sorted_blocks_file;
        sorted_blocks_file.open (sorted_filename);

        for (auto tuple: sorted_blocks){
            for (string attr: tuple){
                sorted_blocks_file << attr << "\t";
            }
            sorted_blocks_file << "\n";
        }

        sorted_blocks_file.close();
        ptr_tmp_files.push_back(sorted_filename);

        //empty vectMblocks for next run
        while(vectMblocks.size() > 0){
            vectMblocks.pop_back();
        }
    }

    //ptr_tmp_files.push_back(tmp_filenames);
    //return parseCSV;
}


// Get the line number "num" in file "fname"
string GetLine(string fname, unsigned int num){
    string sLine = "";
    ifstream read;

    read.open(fname);
    int line_no = 0;
    while (line_no != num && getline(read, sLine)) {
        ++line_no;
    }

    //if (line_no == num) {
    //    // sLine contains the second line in the file.
    //} else {
    //    // The file contains fewer than two lines.
    //}
    return sLine;
}

// convert string to vector of string
vector<string> StringToVect(string str_tuple)
{
    vector<vector<string> > parseCSV;

    stringstream LongTuple(str_tuple);
    string Attr;
    vector<string> row_parse;
    while (getline(LongTuple, Attr, '\t'))
    {
        row_parse.push_back(Attr);
    }

    return row_parse;
}


// given the temporarily for filenames, merge M-1 of them in a sorted way
void merge(vector<string> vect_file_names, string final_filename){
    vector<string> files_name = vect_file_names;
    int pass = 0;
    bool is_final = false; // used to determine when it is the last pass and stop the iterative merging operation

    while (files_name.size() > 0)
    {
        // Number of runs for the current pass
        float nb_runs = (float)files_name.size()/ (float) (NUMB_MEMORY_BLOCK - 1);
        int nbr_runs = ceil( nb_runs );
        
        nbr_runs = max(nbr_runs, 1);
        vector<string> new_files_name;

        // check if last pass then use the final file name instead of tmp names
        if (files_name.size() <= NUMB_MEMORY_BLOCK - 1){
            is_final = true;
        }
        string sorted_filename = final_filename;

        for (int i = 0;  i <  nbr_runs; i++){
            vector<string> current_run_filenames;
            // current files to merge
            // iterate through all the previous run sorted file and read them iteratively M-1 blocks a the time
            for (int ii=i*(NUMB_MEMORY_BLOCK - 1); ii < (i+1)*(NUMB_MEMORY_BLOCK - 1); ii++){
                if (ii < files_name.size()){
                    current_run_filenames.push_back(files_name[ii]);
                }
            }

            // Open output file to write the results of the merge for the current pass
            if (files_name.size() > NUMB_MEMORY_BLOCK - 1){
                sorted_filename = "tmp/sorted_pass_" + to_string(pass) + "_run_" + to_string(i) + ".txt";
            }
            
            //string sorted_filename = "tmp/sorted_pass_" + to_string(pass) + "_run_" + to_string(i) + ".txt";
            ofstream merge_sorted_file(sorted_filename, ofstream::out | ofstream::app);
            new_files_name.push_back(sorted_filename);


            // Keep track of position of the read blocks in the runs of each relation.
            vector<int> read_runs(current_run_filenames.size(), 1); 
            bool merge_complete = false;

            //while (all_blocks_rel1_read && all_blocks_rel2_read){
            while (!merge_complete){

                vector<vector<string> > tuples;
                vector<bool> is_file_empty(current_run_filenames.size(), false);
                vector<int> non_empty;

                // Loop through all the runs and get the jth block
                for (unsigned int i = 0; i < current_run_filenames.size(); i++){
                    string read_tuple = GetLine(current_run_filenames[i], read_runs[i]);
                    if (read_tuple != ""){
                        tuples.push_back( StringToVect(read_tuple) );
                        cout << "tuple read: " + read_tuple + " \n read_runs[" + to_string(i) + "]: " + to_string(read_runs[i]) + "\n";

                        non_empty.push_back(i);
                    }
                   else if (read_runs[i] > 1){
                       is_file_empty[i] = true;
                   }
                } 
                
                if ( tuples.size() > 0 ){
                    
                    // Check the minimum of each the M-1 blocks merged and write it to disk file
                    for (int i = 0;  i < tuples.size(); i++){
                        int count = 0; // count # id_rel1 < id_rel2
                        for (int j = 0; j < tuples.size(); j++){
                            bool condition1 = stoi(tuples[i][sort_col]) != stoi(tuples[j][sort_col]);
                            bool condition2 = stoi(tuples[i][sort_col]) < stoi(tuples[j][sort_col]);
                            if ( condition1 && condition2 ){
                                count++;
                            }
                        }

                        // the id at the current value of i is the minimum and we should call another block of the same run
                        //if ( (count > 0) && (count == tuples.size() - 1) ){
                        if ( (count == tuples.size() - 1) || (tuples.size() == 1) ){
                        //if ( count == tuples.size() - 1 ){
                            // tuple with smallest Id found, now write it to disk
                            int c = 1; // to avoid adding "," at the end of the line in file
                            for (string attribute: tuples[i]){
                                if (c != tuples[i].size()){
                                    merge_sorted_file << attribute << ",";
                                }
                                else{
                                    merge_sorted_file << attribute;
                                }
                                c++;
                            }
                            merge_sorted_file << "\n";

                            read_runs[non_empty[i]]++;

                        }
                    }
                }
                else{
                    merge_complete = true;
                }
                
                for (unsigned int i = 0; i < current_run_filenames.size(); i++){
                    if (is_file_empty[i]){
                        read_runs.erase(read_runs.begin() + i);
                        current_run_filenames.erase(current_run_filenames.begin() + i);
                    }
                }


            }
            
            merge_sorted_file.close();

        }
        files_name = new_files_name;
        if (is_final && files_name.size() <= NUMB_MEMORY_BLOCK - 1){
            for (auto f: files_name){
                files_name.pop_back();
            }
        }
        pass++;
    }
    
    
}



