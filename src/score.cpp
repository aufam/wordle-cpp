#include <mup/html.h>
#include <fmt/ranges.h>
#include "wordle.h"
#include "word_list.h"

std::pair<std::vector<int>, std::string> Wordle::score(std::string word) {
    const auto answer_len = answer.size();
    if (word.size() < answer_len) {
        return {{}, "Not enough letters"};
    }

    if (word.size() > answer_len) {
        return {{}, "Word is too long"};
    }

    if (lst->valid_guesses.count(word) == 0) {
        return {{}, "Not in the word list"};
    }

    if (hard_mode) {
        // check for green
        for (size_t i = 0; i < answer.size(); ++i) {
            if (keys[answer[i]] == 3 && answer[i] != word[i]) {
                return {
                    {},
                    fmt::format(
                        "{}{} letter must be {:c}",
                        i + 1,
                        i == 0   ? "st"
                        : i == 1 ? "nd"
                        : i == 2 ? "rd"
                                 : "th",
                        std::toupper(answer[i])
                    )
                };
            }
        }

        // check for yellow
        for (size_t i = 0; i < answer.size(); ++i) {
            if (keys[answer[i]] == 2 && word.find(answer[i]) == std::string::npos) {
                return {{}, fmt::format("Guess must contain {:c}", std::toupper(answer[i]))};
            }
        }
    }

    std::vector<int> marks, remaining;
    marks.resize(answer.size(), 0);
    remaining.resize(26ul, 0);

    for (size_t i = 0; i < answer_len; ++i) {
        if (word[i] == answer[i]) {
            marks[i] = 3;
        } else {
            remaining[answer[i] - 'a']++;
        }
    }

    for (size_t i = 0; i < answer_len; ++i) {
        if (marks[i] == 3)
            continue;

        auto c = word[i] - 'a';
        if (remaining[c] > 0) {
            marks[i] = 2;
            remaining[c]--;
        } else {
            marks[i] = 1;
        }
    }
    return {std::move(marks), {}};
}
