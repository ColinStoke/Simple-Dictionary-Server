#include <iostream>
using namespace std;

int main(int argc, char* argv[]){

    if(argc != 2){
        cout << "Incorrect usage. See the README for correct command line usage.\n";
        exit(-1);
    }

    cout << "Server3 Started On Port: " << argv[1] << endl;

    return 0;
}