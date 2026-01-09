#include "S.h"
#include "Settings.h"
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cmath>

S::S(bool useExpectedValueStrategy) : currentAttempt(0), useExpectedValue(useExpectedValueStrategy) {
    S_group.clear();
    int maxNumber = 1;
    for (int i = 0; i < Settings::N; ++i)
    {
        maxNumber *= 10;
    }

    for (int num = 0; num < maxNumber; ++num)
    {
        std::string s_num = std::to_string(num);
        // Pad with leading zeros
        while (s_num.length() < Settings::N)
        {
            s_num = "0" + s_num;
        }

        // Check for valid digits (1-6 only) and unique digits
        bool usedDigits[10] = {false};
        bool isValid = true;
        for (char ch : s_num)
        {
            int digit = ch - '0';
            if (digit == 0 || digit > 6 || usedDigits[digit])
            {
                isValid = false;
                break;
            }
            usedDigits[digit] = true;
        }

        if (isValid)
        {
            S_group.push_back(num);
        }
    }

    // Precompute costs for all initial guesses
    if (useExpectedValue) {
        computeCostsExpected();
    } else {
        computeCosts();
    }
}

void S::computeCosts() {
    costs.clear();
    costs.reserve(S_group.size());

    for (int guess : S_group) {
        std::unordered_map<int, int> bucketSizes; // key = bulls*10 + cows

        // For this guess, compute feedback distribution across all candidates
        for (int secret : S_group) {
            Feedback fb = Helpers::getFeedback(guess, secret);
            int key = fb.bulls * 10 + fb.cows;
            bucketSizes[key]++;
        }

        // Find the largest bucket (worst-case)
        int maxBucket = 0;
        for (const auto& pair : bucketSizes) {
            if (pair.second > maxBucket) {
                maxBucket = pair.second;
            }
        }

        costs.push_back({guess, maxBucket, 0.0});
    }

    // Sort by maxBucketSize, then by guess value (for tie-breaking)
    std::sort(costs.begin(), costs.end(), [](const GuessCost& a, const GuessCost& b) {
        if (a.maxBucketSize != b.maxBucketSize)
            return a.maxBucketSize < b.maxBucketSize;
        return a.guess < b.guess;
    });
}

void S::computeCostsExpected() {
    costs.clear();
    costs.reserve(S_group.size());

    for (int guess : S_group) {
        std::unordered_map<int, int> bucketSizes; // key = bulls*10 + cows

        // For this guess, compute feedback distribution across all candidates
        for (int secret : S_group) {
            Feedback fb = Helpers::getFeedback(guess, secret);
            int key = fb.bulls * 10 + fb.cows;
            bucketSizes[key]++;
        }

        // Calculate expected value cost
        double expectedCost = 0.0;
        int maxBucket = 0;

        for (const auto& pair : bucketSizes) {
            double probability = static_cast<double>(pair.second) / S_group.size();
            double bucketCost = pair.second;

            // Apply the cost formula based on S size and current attempt
            // cost(x,y) ~ t/100 * |S|
            if (S_group.size() > 100) {
                bucketCost = bucketCost * currentAttempt / 100.0;
            }

            expectedCost += probability * bucketCost;

            if (pair.second > maxBucket) {
                maxBucket = pair.second;
            }
        }

        costs.push_back({guess, maxBucket, expectedCost});
    }

    // Sort by expectedCost, then by guess value (for tie-breaking)
    std::sort(costs.begin(), costs.end(), [](const GuessCost& a, const GuessCost& b) {
        if (std::abs(a.expectedCost - b.expectedCost) > 1e-9)
            return a.expectedCost < b.expectedCost;
        return a.guess < b.guess;
    });
}

void S::UpdateCost(int guess, Feedback fb) {
    std::vector<int> filtered;
    filtered.reserve(S_group.size());
    for (int candidate : S_group) {
        Feedback candFb = Helpers::getFeedback(guess, candidate);
        if (candFb.bulls == fb.bulls && candFb.cows == fb.cows) {
            filtered.push_back(candidate);
        }
    }
    S_group.swap(filtered);

    // Increment attempt counter for expected value strategy
    currentAttempt++;

    // Recompute costs for the narrowed candidate set
    if (useExpectedValue) {
        computeCostsExpected();
    } else {
        computeCosts();
    }
}

int S::GetNextGuess() {
    if (costs.empty()) return -1;

    // Return the best guess (already sorted)
    return costs.front().guess;
}


