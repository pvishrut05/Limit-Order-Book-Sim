#ifndef REPORT_H
#define REPORT_H

#include<iosfwd>
#include<vector>
#include"NormalizedMessage.h"
#include"BookDiff.h"
void print_histogram(ostream& os, vector<NormalizedMessage>& nm, size_t ref_row);

void print_mismatch(ostream& os, NormalizedMessage& nm, DiffResult& d, TopOfBook& mine, TopOfBook& ref, int lvl); 

#endif