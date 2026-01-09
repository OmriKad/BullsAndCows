#include "Questions.h"
#include "Helpers.h"
#include "Settings.h"
#include "S.h"
#include <vector>
#include <iostream>
#include <string>


bool Questions::question_1(int newGuess, Feedback* feedbacks, int* guesses, int n)
{
    bool result = true;
    for (int i = 0; i < n; i++)
    {
        Feedback current_feedback = Helpers::getFeedback(newGuess, guesses[i]);
        if (current_feedback.bulls != feedbacks[i].bulls || current_feedback.cows != feedbacks[i].cows)
        {
            result = false;
            break;
        }
    }
    return result;
}

void Questions::question_2()
{
    srand(time(0));
    std::vector<int> roundsGuesses; // Store results for statistics

    for (int i = 0; i < Settings::numOfRounds; i++)
    {
        int secret = Helpers::generateSecret();
        int attempts = 0;
        bool solved = false;

        std::vector<int> historyGuesses;
        std::vector<Feedback> historyFeedbacks;

        // Print Secret for control (Requirement 5)
        std::cout << "Round " << i + 1 << " Secret: " << secret << std::endl;

        while (!solved)
        {
            attempts++;
            int nextGuess = 0;

            // Optimization: First guess logic
            if (historyGuesses.empty())
            {
                // Generate 1234... based on N
                for (int k = 1; k <= Settings::N; ++k) nextGuess = nextGuess * 10 + k;
            }
            else
            {
                // Use DFS to find next consistent guess
                bool used[10] = {false};
                bool found = Helpers::dfsFindGuess(0, 0, used, historyGuesses, historyFeedbacks, nextGuess);

                if (!found) {
                    std::cout << "Error: No consistent guess found. Logic issue?" << std::endl;
                    break;
                }
            }

            // Play the guess
            Feedback fb = Helpers::getFeedback(nextGuess, secret);

            // Store history
            historyGuesses.push_back(nextGuess);
            historyFeedbacks.push_back(fb);

            // Print Guess for control
            std::cout << " Guess: " << nextGuess << " (B: " << fb.bulls << ", C: " << fb.cows << ")" << std::endl;

            if (fb.bulls == Settings::N) {
                solved = true;
            }
        }
        roundsGuesses.push_back(attempts);
    }

    // --- Statistics ---
    Stats stats = Helpers::calculateStatistics(roundsGuesses);

    std::cout << "Average: " << stats.average << std::endl;
    std::cout << "Median: " << stats.median << std::endl;
    std::cout << "Max: " << stats.max << std::endl;
}


void Questions::question_3()
{
    std::vector<int> roundsGuesses; // Store results for statistics

    for (int i = 0; i < Settings::numOfRounds; i++)
    {
        int secret = Helpers::generateSecret();
        int attempts = 0;
        bool solved = false;
        S state; // Initialize with all valid candidates and precomputed costs

        std::cout << "Round " << (i + 1) << " - Secret: " << secret << std::endl;

        while (!solved && state.size() > 0)
        {
            attempts++;

            // Get the min-max optimal guess
            int guess = state.GetNextGuess();

            // Get feedback from the secret
            Feedback fb = Helpers::getFeedback(guess, secret);

            std::cout << "  Guess " << attempts << ": " << guess
                      << " (B: " << fb.bulls << ", C: " << fb.cows
                      << ") - Remaining: " << state.size() << std::endl;

            if (fb.bulls == Settings::N) {
                solved = true;
            } else {
                // Filter candidates based on feedback
                state.UpdateCost(guess, fb);
            }
        }

        roundsGuesses.push_back(attempts);
        std::cout << "  Solved in " << attempts << " attempts\n" << std::endl;
    }

    // --- Statistics ---
    Stats stats = Helpers::calculateStatistics(roundsGuesses);

    std::cout << "\n=== Min-Max Algorithm Statistics ===" << std::endl;
    std::cout << "Average: " << stats.average << std::endl;
    std::cout << "Median: " << stats.median << std::endl;
    std::cout << "Max: " << stats.max << std::endl;
}
