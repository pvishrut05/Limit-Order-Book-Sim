#include "ReferenceBook.h"
#include "ReferenceBookReader.h"

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <charconv>
#include <cstdint>


int referenceBookReader(vector<TopOfBook> &reader, string pathway){
    ifstream file(pathway);

    if(!file.is_open()){
        cerr << "Error opening, why tho?" << endl;
        return 1;
    }

    string temp_file_read;



    //Needs try and catch
    reader.reserve(1000000);
    while(getline(file, temp_file_read)){
        
            const char* cur = temp_file_read.data();
            const char* end = cur + temp_file_read.size();

            TopOfBook& raw = reader.emplace_back();
            
            //TODO: need a edge case for huge negative or huge positive, negative for bids and positive for asks
            for(int i = 0; i < 10; i++){
                
                auto r = std::from_chars(cur, end, raw.ask[i].price);
                if(r.ec != std::errc()){ cout << "ran into a error at ask_price " << i << endl; return 1;}
                cur = r.ptr + 1;
                r = std::from_chars(cur, end, raw.ask[i].size);
                if(r.ec != std::errc()){ cout << "ran into a error at ask_size " << i << endl; return 1;}
                cur = r.ptr + 1;
                raw.n_ask++;
                r = std::from_chars(cur, end, raw.bid[i].price);
                if(r.ec != std::errc()){ cout << "ran into a error at bid_price " << i << endl; return 1;}
                cur = r.ptr + 1;
                r = std::from_chars(cur, end, raw.bid[i].size);
                if(r.ec != std::errc()){ cout << "ran into a error at bid_size " << i << endl; return 1;}
                cur = r.ptr + 1;
                raw.n_bid++;
            }
    }

    return 0;



}
