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

const int NUMB_MEMORY_BLOCK = 3;

#include "utils.h"


int main(){

    string filename = "Emp.csv";

    int sort_col = 0;
    ReadAndSortRelation(filename, sort_col);
    
    // Open output file to write the results of the join
    //string join_filename = "join_result.txt";
    string output_filename = "final_sorted_result.csv";

    merge(ptr_tmp_files, output_filename);

    return 0;

}