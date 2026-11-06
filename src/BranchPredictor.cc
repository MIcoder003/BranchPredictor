#include "BranchPredictor.h"

#include <cstdio>

BranchPredictor::BranchPredictor(int m, int n)
    : branchCounters(1u << m, 2), m(m), n(n), history(0) {}

unsigned long BranchPredictor::getIndex(unsigned long pc) const {
    unsigned long pcBits = (pc >> 2) & ((1lu << m) - 1);
    unsigned long upper  = pcBits >> (m - n);
    unsigned long lower  = pcBits & ((1lu << (m - n)) - 1);
    return ((upper ^ history) << (m - n)) | lower;
}

void BranchPredictor::updateHistory(bool taken) {
    if (n == 0) return;
    history = (history >> 1) | (static_cast<unsigned long>(taken) << (n - 1));
}

void BranchPredictor::updateCounter(unsigned long pc, bool taken) {
    unsigned long index = getIndex(pc);

    if (taken) {
        if (branchCounters[index] < 3) {
            branchCounters[index]++;
        }
    } else {
        if (branchCounters[index] > 0) {
            branchCounters[index]--;
        }
    }

    updateHistory(taken);
}

void BranchPredictor::printContents() const {
    for (size_t i = 0; i < branchCounters.size(); i++) {
        printf(" %zu\t%d\n", i, (int)branchCounters[i]);
    }
}

bool BranchPredictor::predictBranch(unsigned long pc) const {
    unsigned long index = getIndex(pc);
    return branchCounters[index] >= 2;
}