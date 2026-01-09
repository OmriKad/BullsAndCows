#ifndef ALGO_HELPERS_H
#define ALGO_HELPERS_H
#include "Questions.h"
#include <vector>


class Helpers
{
public:
    static int generateSecret();
    static Stats calculateStatistics(const std::vector<int>& history);
    static Feedback getFeedback(int guess, int secret);
    static bool dfsFindGuess(int currentVal, int length, bool usedDigits[],
                         const std::vector<int>& historyGuesses,
                         const std::vector<Feedback>& historyFeedbacks,
                         int& outResult);
    static bool isPromising(int currentVal, int length,
                            const std::vector<int>& historyGuesses,
                            const std::vector<Feedback>& historyFeedbacks);
};

#endif //ALGO_HELPERS_H