#include "NormalizedMessage.h"
#include "LobsterMessageReader.h"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <sstream>
#include <charconv>
#include <cstdint>

//TODO: Add trys and catch, error handlings

int LobsterMessageReader(vector<NormalizedMessage> &reader, string pathway){

    ifstream file(pathway);

    if(!file.is_open()){
        cerr << "Error opening, why tho?" << endl;
        return 1;
    }


    string temp_file_read;

    //TODO: comment out i when done or when testing.
    //int i = 0; //i is only for debugging purpose 
    //TODO: when you do remove the i, do a reserve(1,000,00);
    //TODO: add try and catch error handlers
    reader.reserve(1000000);
    while(getline(file, temp_file_read)){

        stringstream ss(temp_file_read);

        string t_time, t_type, t_id, t_size, t_price, t_side;
        string delimiter = ".";

        getline(ss, t_time, ',');
        getline(ss, t_type, ',');
        getline(ss, t_id, ',');
        getline(ss, t_size, ',');
        getline(ss, t_price, ',');
        getline(ss, t_side, ',');

        size_t index = t_time.find(delimiter);

        string p1, p2;

        if(index != string::npos){
            p1 = t_time.substr(0,index);
            p2 = t_time.substr(index + delimiter.length());
        }

        if(p2.length() < 9){
            p2.append(9 - p2.length(), '0');
        }

        uint64_t t1 = 0;
        uint64_t t2 = 0;
        uint64_t time = 0;

        auto [p1_end, ec1] = std::from_chars(p1.data(), p1.data() + p1.size(), t1);
        auto [p2_end, ec2] = std::from_chars(p2.data(), p2.data() + p2.size(), t2);


        if(ec1 == std::errc() && ec2 == std::errc()){
            time = (t1 * 1000000000) + (t2);
        }else{
            cout << "Ran into a error while converting time from string to int" << endl; 
            return 1;
        }
        


        uint8_t type = 0;

        auto [type_end, ec_type] = std::from_chars(t_type.data(), t_type.data()+t_type.size(), type);

        if(ec_type != std::errc()) {
            cout << "Ran into a error while converting type from string to int" << endl;
            return 1;
        }

        uint64_t id = 0;
        
        auto [id_end, ec_id] = std::from_chars(t_id.data(), t_id.data()+t_id.size(), id);

        if(ec_id != std::errc()) {
            cout << "Ran into a error while converting id from string to int" << endl;
            return 1;
        }

        uint32_t sz = 0;

        auto [sz_end, ec_sz] = std::from_chars(t_size.data(), t_size.data() + t_size.size(), sz);

        if(ec_sz != std::errc()) {
            cout << "Ran into a error while converting size from string to int" << endl;
            return 1;
        }

        uint64_t p = 0;
// may need to do a p = p/100
        auto [p_end, ec_p] = std::from_chars(t_price.data(), t_price.data() + t_price.size(), p);

        if(ec_p != std::errc()) {
            cout << "Ran into a error while converting price from string to int" << endl;
            return 1;
        }

        int8_t s = 0;

        auto [p_s, ec_s] = std::from_chars(t_side.data(), t_side.data() + t_side.size(), s);
//this is happening because its unsigned int, while side can have both 1 and -1 as the direction.
        if(ec_s != std::errc()) {
            cout << "Ran into a error while converting side from string to int" << endl;
            return 1;
        }

        reader.emplace_back(time, type, id, sz, p, s);


    }

    return 0;

}