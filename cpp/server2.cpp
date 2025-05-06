#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
using namespace std;

void parseTsv(const string inFile, vector<pair<string, string>>& dict);

int main(int argc, char* argv[]){

    vector<pair<string, string>> dict;

    if(argc != 2){
        cout << "Incorrect usage. See the README for correct command line usage.\n";
        exit(-1);
    }

    cout << "Server1 Started On Port: " << argv[1] << endl;

    parseTsv("dictionary.tsv", dict);

    for(size_t i = 0; i < dict.size(); i++){
        cout << dict[i].first << ' ' << dict[i].second << endl;
    }

    return 0;
}

void parseTsv(const string inFile, vector<pair<string, string>>& dict){

    string line;

    ifstream file(inFile);

    stringstream ss;

    pair<string, string> temp;

    if(!file){
        cout << "Error opening file.\n";
        exit(-1);
    }

    while(!file.eof()){
        getline(file, line, '\n'); //get full line

        ss.clear();
        ss.str(line); //put line into stringstream

        getline(ss, temp.first, '\t'); //get word

        getline(ss, temp.second, '\n'); //get defenition

        dict.push_back(temp); //put into main dict object
    }

}