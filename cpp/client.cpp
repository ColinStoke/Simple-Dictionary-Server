
#include <iostream>
using namespace std;

int main(int argc, char* argv[]){

    if(argc != 3){
        cout << "Incorrect usage. See the README for correct command line usage.\n";
        exit(-1);
    }

    cout << "IP: " << argv[1] << " Port: " << argv[2] << endl;

    return 0;
}