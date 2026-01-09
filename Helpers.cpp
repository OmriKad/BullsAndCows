#include "Helpers.h"
#include "Settings.h"
#include <string>
#include <ctime>
#include <cstdlib>

using namespace std;

int const K = Settings::K;
int const N = Settings::N;

int Helpers::generateSecret()
{
    bool usedDigits[K + 1] = {false};
    int secret = 0;

    for (int i = 0; i < N; ++i)
    {
        int digit;
        do
        {
            digit = (rand() % K) + 1;
        } while (usedDigits[digit]);

        usedDigits[digit] = true;
        secret = secret * 10 + digit;
    }

    return secret;
}

Stats Helpers::calculateStatistics(const std::vector<int> &history)
{
    Stats stats = {0.0, 0.0, 0};
    if (history.empty()) return stats;

    int totalGuesses = 0;
    stats.max = history[0];

    for (int guesses : history)
    {
        totalGuesses += guesses;
        if (guesses > stats.max)
        {
            stats.max = guesses;
        }
    }

    stats.average = static_cast<double>(totalGuesses) / history.size();

    std::vector<int> sortedHistory = history;
    std::sort(sortedHistory.begin(), sortedHistory.end());
    size_t mid = sortedHistory.size() / 2;
    if (sortedHistory.size() % 2 == 0)
    {
        stats.median = (sortedHistory[mid - 1] + sortedHistory[mid]) / 2.0;
    }
    else
    {
        stats.median = sortedHistory[mid];
    }

    return stats;
}

Feedback Helpers::getFeedback(int guess, int secret)
{
    // Convert the guess and secret to strings
    const string guessStr = to_string(guess);
    const string secretStr = to_string(secret);
    Feedback result = {0, 0};

    // Calculate bulls and cows
    const size_t len = (guessStr.size() < secretStr.size()) ? guessStr.size() : secretStr.size();
    for (size_t i = 0; i < len; ++i)
    {
        const int g_digit = guessStr[i] - '0';
        const int s_digit = secretStr[i] - '0';

        if (g_digit == s_digit)
        {
            result.bulls++;
        }
        else
        {
            // Find if g_digit exists in secretStr
            const size_t g_pos = secretStr.find(guessStr[i]);
            if (g_pos != string::npos)
            {
                result.cows++;
            }
        }
    }

    return result;
}

bool Helpers::dfsFindGuess(int currentVal, int length, bool usedDigits[],
                         const std::vector<int>& historyGuesses,
                         const std::vector<Feedback>& historyFeedbacks,
                         int& outResult)
{
    // Base case: Full number generated
    if (length == Settings::N)
    {
        // Double check using the robust question_1 check (optional but recommended safety)
        // Convert vectors to arrays for question_1 API
        // Note: Using const_cast or creating temp arrays because API expects int*
        std::vector<int> h_guesses = historyGuesses;
        std::vector<Feedback> h_feedbacks = historyFeedbacks;

        if (Questions::question_1(currentVal, h_feedbacks.data(), h_guesses.data(), (int)historyGuesses.size())) {
            outResult = currentVal;
            return true;
        }
        return false;
    }

    int maxDigit = Settings::K;

    for (int d = 1; d <= maxDigit; ++d)
    {
        if (!usedDigits[d])
        {
            int nextVal = currentVal * 10 + d;

            // PRUNING: Check if this path is valid so far
            if (isPromising(nextVal, length + 1, historyGuesses, historyFeedbacks))
            {
                usedDigits[d] = true;
                if (dfsFindGuess(nextVal, length + 1, usedDigits, historyGuesses, historyFeedbacks, outResult))
                {
                    return true;
                }
                usedDigits[d] = false; // Backtrack
            }
        }
    }

    return false;
}

bool Helpers::isPromising(int currentVal, int length,
                        const std::vector<int>& historyGuesses,
                        const std::vector<Feedback>& historyFeedbacks)
{
    std::string s_curr = std::to_string(currentVal);

    for (size_t i = 0; i < historyGuesses.size(); ++i)
    {
        int pastGuess = historyGuesses[i];
        int pastBulls = historyFeedbacks[i].bulls;
        int pastCows = historyFeedbacks[i].cows;
        int pastTotal = pastBulls + pastCows;

        std::string s_past = std::to_string(pastGuess);

        int currentBulls = 0;
        int currentShared = 0;

        // Check constraints based on the digits filled so far (length)
        for (int k = 0; k < length; ++k)
        {
            // 1. Bulls check
            if (s_curr[k] == s_past[k]) {
                currentBulls++;
            }

            // 2. Shared digits check (Cows + Bulls logic)
            // Does s_curr[k] exist anywhere in s_past?
            if (s_past.find(s_curr[k]) != std::string::npos) {
                currentShared++;
            }
        }

        // --- Constraint 1: Too many bulls ---
        if (currentBulls > pastBulls) return false;

        // --- Constraint 2: Too many shared digits (bulls + cows) ---
        // Crucial for pruning branches with digits that shouldn't exist
        if (currentShared > pastTotal) return false;

        // --- Constraint 3: Future feasibility ---
        // Can we reach the required number of bulls/hits with remaining spots?
        int remainingSlots = Settings::N - length;

        if (currentBulls + remainingSlots < pastBulls) return false;
        if (currentShared + remainingSlots < pastTotal) return false;
    }

    return true;
}


