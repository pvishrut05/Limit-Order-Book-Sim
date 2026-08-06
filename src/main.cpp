#include <iostream>

using namespace std;


int main(){
    cout << "Hello, World!" << endl;

    size_t size = 16;

    char* buffer = new char[size];
    char* buffer2 = new char[size];

    for(size_t i = 0; i <= size; ++i){
        buffer[i] = 'A';
    }

    delete[] buffer;
    delete[] buffer2;
    return 0;

}