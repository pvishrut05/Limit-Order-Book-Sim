#include "NormalizedMessage.h"
#include "LobsterMessageReader.h"
#include "ReferenceBook.h"
#include "ReferenceBookReader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>




int main(){
    cout << "Hello, World!" << endl;

    vector<NormalizedMessage> message_reader;

    int readerErr = LobsterMessageReader(message_reader);

    if(readerErr != 0){
        cout << "Ran into a error";
    }else{
        for(auto i = 0; i < 24; i++){
            cout << message_reader[i].timestamp_ns << endl;
        }
        
    }


    cout<< endl << endl;

    vector<ReferenceBook> ob_reader;

    int ob_err = referenceBookReader(ob_reader);

    if(ob_err != 0){
        cout << "Error " << endl;
    }else{
        for(int i = 0; i < 5 && i < ob_reader.size(); i++){

            cout << ob_reader[i].n_ask << " testing " << ob_reader[i].n_bid << endl;
            for(int j = 0; j < 10; j++){
                cout << ob_reader[i].ask_px[j] << " " << ob_reader[i].ask_sz[j] << " " << ob_reader[i].bid_px[j] << " " << ob_reader[i].bid_sz[j] << "|";
            }
            cout << endl;
        }
    }












    return 0;

}