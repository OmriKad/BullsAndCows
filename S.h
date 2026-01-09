#ifndef ALGO_S_H
#define ALGO_S_H
#include <vector>
#include <unordered_map>
#include "Helpers.h"

struct GuessCost {
    int guess;
    int maxBucketSize;  // worst-case remaining candidates
    double expectedCost; // expected value cost
};

class S {
    std::vector<int> S_group;
    std::vector<GuessCost> costs;  // precomputed costs for each guess in S_group
    int currentAttempt;  // Track current attempt number for cost formula
    bool useExpectedValue;  // true for question_4, false for question_3

    void computeCosts();  // compute/recompute costs for all guesses
    void computeCostsExpected();  // compute expected value costs for question_4
public:
    S(bool useExpectedValueStrategy = false);
    void UpdateCost(int guess, Feedback fb);
    int GetNextGuess();
    size_t size() const { return S_group.size(); }
    void incrementAttempt() { currentAttempt++; }
};
#endif //ALGO_S_H