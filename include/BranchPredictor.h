#ifndef BRANCH_PREDICTOR_H
#define BRANCH_PREDICTOR_H

#include <vector>
#include <cstdint>



class BranchPredictor {
    public:
        BranchPredictor(int m, int n);
        bool predictBranch(unsigned long pc) const;
        void updateCounter(unsigned long pc, bool taken);
        void updateHistory(bool taken);
        void printContents() const;
        

    private:
    std::vector<uint8_t> branchCounters;
    int m;
    int n;
    unsigned long history;

    unsigned long getIndex(unsigned long pc) const;

};
#endif // BRANCH_PREDICTOR_H