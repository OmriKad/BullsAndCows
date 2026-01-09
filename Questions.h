#ifndef ALGO_QUESTIONS_H
#define ALGO_QUESTIONS_H
#include <vector>

struct Feedback
{
    int bulls;
    int cows;
};

struct Stats
{
    double average;
    double median;
    int max;
};

struct S_element
{
    std::vector<int> possible_guesses;
    int max_cost;
};


class Questions
{
public:
    static bool question_1(int newGuess, Feedback* feedbacks, int* guesses, int n);
    static void question_2();
    static void question_3();
    static void question_4();
};

#endif //ALGO_QUESTIONS_H