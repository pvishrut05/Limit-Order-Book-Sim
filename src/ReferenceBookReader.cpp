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

//  int64_t ask_px[10];
//     uint64_t ask_sz[10];
//     int64_t bid_px[10];
//     uint64_t bid_sz[10];

int referenceBookReader(vector<ReferenceBook> &reader){
    ifstream file("data/INTC_2012-06-21_34200000_57600000_orderbook_10.csv");

    if(!file.is_open()){
        cerr << "Error opening, why tho?" << endl;
        return 1;
    }

    string temp_file_read;
    int j = 0;


    //Needs try and catch
    while(getline(file, temp_file_read) && j < 25){
        
            const char* cur = temp_file_read.data();
            const char* end = cur + temp_file_read.size();

            ReferenceBook& raw = reader.emplace_back();
            
            //TODO: need a edge case for huge negative or huge positive, negative for bids and positive for asks
            for(int i = 0; i < 10; i++){
                
                auto r = std::from_chars(cur, end, raw.ask_px[i]);
                if(r.ec != std::errc()){ cout << "ran into a error at ask_price " << i << " " << j << endl; return 1;}
                cur = r.ptr + 1;
                r = std::from_chars(cur, end, raw.ask_sz[i]);
                if(r.ec != std::errc()){ cout << "ran into a error at ask_size " << i << " " << j << endl; return 1;}
                cur = r.ptr + 1;
                r = std::from_chars(cur, end, raw.bid_px[i]);
                if(r.ec != std::errc()){ cout << "ran into a error at bid_price " << i << " " << j << endl; return 1;}
                cur = r.ptr + 1;
                r = std::from_chars(cur, end, raw.bid_sz[i]);
                if(r.ec != std::errc()){ cout << "ran into a error at bid_size " << i << " " << j << endl; return 1;}
                cur = r.ptr + 1;

            }
     
        j++;
    }

    return 0;



}
