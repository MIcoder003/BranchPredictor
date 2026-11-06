#include "Hybrid.h"

Hybrid::Hybrid(int k, int m1, int n, int m2)
    : gshare(m1, n),
      bimodal(m2, 0),
      chooser(1u << k, 1),   // chooser counters initialized to 1
      k(k) {}

unsigned long Hybrid::chooserIndex(unsigned long pc) const {
    return (pc >> 2) & ((1lu << k) - 1);
}

// Steps 1-3: get both predictions, consult the chooser, return the selected one.
bool Hybrid::predictBranch(unsigned long pc) const {
    bool gPred = gshare.predictBranch(pc);
    bool bPred = bimodal.predictBranch(pc);

    if (chooser[chooserIndex(pc)] >= 2) {
        return gPred;
    } else {
        return bPred;
    }
}

void Hybrid::printContents() const {
    printf("FINAL CHOOSER CONTENTS\n");
    for (size_t i = 0; i < chooser.size(); i++)
        printf(" %zu\t%d\n", i, (int)chooser[i]);
    printf("FINAL GSHARE CONTENTS\n");
    gshare.printContents();
    printf("FINAL BIMODAL CONTENTS\n");
    bimodal.printContents();
}

// Steps 4-6.
void Hybrid::update(unsigned long pc, bool actual) {
    // Re-read both individual predictions BEFORE anything is updated.
    bool gPred = gshare.predictBranch(pc);
    bool bPred = bimodal.predictBranch(pc);

    unsigned long cIdx    = chooserIndex(pc);
    bool          useG    = (chooser[cIdx] >= 2);

    // Step 4: only the selected predictor updates its counter.
    // Step 5: gshare's global history must advance either way.
    if (useG) {
        // gshare's updateCounter already advances its own history.
        gshare.updateCounter(pc, actual);
    } else {
        bimodal.updateCounter(pc, actual);   // n == 0, so no history effect
        gshare.updateHistory(actual);        // advance gshare history anyway
    }

    // Step 6: chooser moves only when the two disagreed.
    bool gCorrect = (gPred == actual);
    bool bCorrect = (bPred == actual);

    if (gCorrect && !bCorrect) {
        if (chooser[cIdx] < 3) chooser[cIdx]++;
    } else if (bCorrect && !gCorrect) {
        if (chooser[cIdx] > 0) chooser[cIdx]--;
    }
    // both correct or both incorrect -> no change
}