#include "NormalizedMessage.h"
#include "LobsterMessageReader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;


int main(){
    cout << "Hello, World!" << endl;

    vector<NormalizedMessage> reader;

    int readerErr = LobsterMessageReader(reader);

    if(readerErr != 0){
        cout << "Ran into a error";
    }else{
        for(auto i = 0; i < reader.size(); i++){
            cout << reader[i].timestamp_ns << endl;
        }
        
    }









    return 0;

}