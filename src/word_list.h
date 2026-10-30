#pragma once

#include <string>
#include <vector>
#include <unordered_set>

struct WordList {
    std::vector<std::string>        answers;
    std::unordered_set<std::string> valid_guesses;
};
