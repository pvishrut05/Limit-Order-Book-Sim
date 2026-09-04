#include "NormalizedMessage.h"
#include "LobsterMessageReader.h"
#include "ReferenceBook.h"
#include "ReferenceBookReader.h"
#include "Report.h"
#include "OrderBook.h"
#include "BookDiff.h"
#include "Apply.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <chrono>


//0 on clean
//1 on mismatch
//2 on usage or file error

int main(int argc, char* argv[]){
    size_t start = 0;
    size_t limit = 624040;
    size_t seedr = 0;
    int level = 10;
    string message, book;
    bool no_seed = false;

//maybe seed and start are the same, will have to come back to this
    for(int i = 1; i < argc; i++){
        string a = argv[i];
        if(a == "--levels" && argc > i + 1) level = stoi(argv[++i]);
        else if(a == "--start" && argc > i + 1) start = stoi(argv[++i]);
        else if(a == "--limit" && argc > i + 1) limit = stoi(argv[++i]);
        else if(a == "--messages" && argc > i + 1) message = argv[++i];
        else if(a == "--book" && argc > i + 1) book = argv[++i];
        else if( a == "--no-seed") no_seed = true;
        else if(a == "--seed" && argc > i + 1) seedr = stoull(argv[++i]);
        else {cerr << "Unknown command line argument "; return 2;}
    }

    if(message.empty() || book.empty()){
        cerr << "Usage Note: replay --messages <csv> --book <csv>\n"
                "You can also use, --limit N, --level N, --start N\n"
                "Look at ReadMe for more information\n";
        return 2;
    }


    if(level < 1 || level > 10){
        cerr << "Min level is 1 & Max level is 10\n";
        return 2;
    }

    using clock = chrono::steady_clock;

    vector<NormalizedMessage> msg;
    vector<TopOfBook> b;

    auto t_load_start = clock::now();
    if(LobsterMessageReader(msg, message) != 0){
        cerr << "LOBSTER Message Filename was not found / File couldn't be opened" << endl;
        return 2;
    }
    if(referenceBookReader(b, book) != 0){
        cerr << "LOBSTER Book Filename was not found / File couldn't be opened" << endl;
        return 2;
    }
    auto t_load_end = clock::now();


    print_histogram(cout, msg, b.size());

    if(msg.size() != b.size()){
        cerr << "Message and Order Book size are different";
        return 2;
    }

    

    OrderBook ob;
    int errorRecord = 0;
    auto t_run_start = clock::now();


    size_t first_event = start;
    if (!no_seed) {
        if (seedr >= b.size()) {
            std::cerr << "--seed-row " << seedr << " is past the end ("
                    << b.size() << " rows)\n";
            return 2;
        }
        ob.seed(b[seedr]);

        // Prove the seed landed: compare against the row we copied, before
        // applying anything.
        DiffResult d0 = BookDiff(ob.top(level), b[seedr], level);
        if (!d0.match) {
            std::cout << "seed did not reproduce reference row " << seedr << "\n";
            print_mismatch(std::cout, seedr, msg[seedr], d0,
                        ob.top(level), b[seedr], level);
            return 1;
        }
        std::cout << "seeded from reference row " << seedr << " -- exact match\n";
        first_event = std::max(start, seedr + 1);
    }

    size_t end = b.size();
    if(limit > 0 && first_event + limit < end) end = first_event + limit;

    for(auto i = first_event; i < end; i++){
        apply(ob, msg[i]);

        TopOfBook mine = ob.top(level);

        DiffResult d = BookDiff(mine, b[i], level);

        if(!d.match){
            print_mismatch(cout, i, msg[i], d, mine, b[i], level);
            errorRecord = 1;
            break;
        }

    }

    auto t_run_end = clock::now();

    auto ms = [](auto a, auto b){
        return chrono::duration_cast<chrono::milliseconds>(b-a).count();
    };

    cerr << "\nload  " << ms(t_load_start, t_load_end) << " ms\n"
         << "replay " << ms(t_run_start, t_run_end)   << " ms\n";



    if(errorRecord == 0) cout << "\nclean match over " << (end - start) << " events\n";

    return errorRecord;

}