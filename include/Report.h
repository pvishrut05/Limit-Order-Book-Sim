#ifndef REPORT_H
#define REPORT_H

#include<iosfwd>
#include<vector>
#include"NormalizedMessage.h"
#include"BookDiff.h"
void print_histogram(ostream& os, vector<NormalizedMessage>& nm, size_t ref_row);

void print_mismatch(ostream& os, size_t event_idx, const NormalizedMessage& nm, const DiffResult& d, const TopOfBook& mine, const TopOfBook& ref, int lvl); 

#endif
