#include "NormalizedMessage.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;


int main(){
    cout << "Hello, World!" << endl;

    // vector<NormalizedMessage> reader;

    ifstream file("data/INTC_2012-06-21_34200000_57600000_message_10.csv");

    if(!file.is_open()){
        cerr << "Error opening, why tho?" << endl;
        return 1;
    }

    string firstLine; 
    string secondLine;
    if(getline(file, firstLine)){
        cout << "First Line is: " << firstLine << endl;
    }if(getline(file, secondLine)){
        cout << "Second Line is ? " << secondLine << endl;
    }





    return 0;

}