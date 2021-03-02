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


const int NUMB_MEMORY_BLOCK = 22;
vector< vector<string> > ptr_tmp_files;

int which_relation = 0; // 0 if relation is Emp and 1 if relation is Dept

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

// get set of M blocks and sort them accorking to the specified attribute (column) 
vector <vector <string> > sort_blocks(vector<Block> blocks, int sort_column){
    //ofstream sorted_blocks_file;
    //sorted_blocks_file.open ("sorted_blocks.txt");

    /*
    string sorted_file = "sorted_blocks.txt";
    ofstream sorted_blocks_file(sorted_file, ofstream::out | ofstream::app);
    */

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

    vector<string> tmp_filenames;

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

        Block block;
        //block.add((int) &line, line);
        block.add(line);
        if (vectMblocks.size() < NUMB_MEMORY_BLOCK){
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
            tmp_filenames.push_back(sorted_filename);
            //ptr_tmp_files[which_relation].push_back(sorted_filename);

            count++;

            //empty vectMblocks for next run
            while(vectMblocks.size() > 0){
                vectMblocks.pop_back();
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
        tmp_filenames.push_back(sorted_filename);
        //ptr_tmp_files[which_relation].push_back(sorted_filename);

        //empty vectMblocks for next run
        while(vectMblocks.size() > 0){
            vectMblocks.pop_back();
        }
    }

    ptr_tmp_files.push_back(tmp_filenames);
    //return parseCSV;
}


vector<vector<string>> left_join(vector<vector<string>> tuples_rel1, int attr_rel1, vector<vector<string>> tuples_rel2, int attr_rel2){

    vector<vector<string>> joined_relation;
    
    //for (vector<string> tuple_r1: tuples_rel1){
    for (auto tuple_r1: tuples_rel1){
        vector<string> tuple1 = tuple_r1;
        //for (vector<string> tuple_r2: tuples_rel2){
        for (auto tuple_r2: tuples_rel2){
            //cout << "attr rel1: " << stoi(tuple_r1[attr_rel1]) << "\n"  << "attr rel2: " << stoi(tuple_r2[attr_rel2]) << "\n\n"; //***************
            if (stoi(tuple_r1[attr_rel1]) == stoi(tuple_r2[attr_rel2])){
                tuple1.insert( tuple1.end(), tuple_r2.begin(), tuple_r2.end() );
                joined_relation.push_back(tuple1);
            }
        }
    }
    
    return joined_relation;
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