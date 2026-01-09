#ifndef ALGO_S_H
#define ALGO_S_H
#include <vector>
#include <unordered_map>
#include "Helpers.h"

struct GuessCost {
    int guess;
    int maxBucketSize;  // worst-case remaining candidates
};

class S {
    std::vector<int> S_group;
    std::vector<GuessCost> costs;  // precomputed costs for each guess in S_group

    void computeCosts();  // compute/recompute costs for all guesses
public:
    S();
    void UpdateCost(int guess, Feedback fb);
    int GetNextGuess();
    size_t size() const { return S_group.size(); }
};
#endif //ALGO_S_H