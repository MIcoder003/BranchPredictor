#ifndef HYBRID_H
#define HYBRID_H

#include <vector>
#include <cstdint>
#include "BranchPredictor.h"

class Hybrid {
public:
    // k  = chooser index bits
    // m1 = gshare PC index bits, n = gshare history bits
    // m2 = bimodal PC index bits
    Hybrid(int k, int m1, int n, int m2);

    bool predictBranch(unsigned long pc) const;
    void update(unsigned long pc, bool actual);
    void printContents() const;

private:
    BranchPredictor gshare;
    BranchPredictor bimodal;
    std::vector<uint8_t> chooser;
    int k;

    unsigned long chooserIndex(unsigned long pc) const;
};

#endif // HYBRID_H