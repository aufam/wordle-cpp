#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

struct WordList;

struct Wordle {
    static Wordle New(std::string answer, std::shared_ptr<const WordList> lst = {}, bool hard_mode = true, size_t max_guess = 6) {
        return {.answer = std::move(answer), .lst = lst, .hard_mode = hard_mode, .max_guess = max_guess};
    }

    struct Guess {
        std::string      word;
        std::vector<int> marks;
    };

    const std::string               answer;
    std::shared_ptr<const WordList> lst;

    const bool   hard_mode;
    const size_t max_guess;

    std::vector<Guess>            guesses     = {};
    std::string                   current     = {};
    std::unordered_map<char, int> keys        = {};
    std::string                   prev_status = {};

    const std::vector<std::vector<std::string>> key_rows = {
        {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p"},
        {"a", "s", "d", "f", "g", "h", "j", "k", "l"},
        {"enter", "z", "x", "c", "v", "b", "n", "m", "back"},
    };

    // render initial page
    std::string Page();

    // render new game
    std::string Game();

    // render toast
    std::string Toast(std::string msg);

    // render a single row
    std::string Row(size_t row, bool flash = false, bool shake = false);

    // render a single tile
    std::string Tile(size_t row, size_t col, bool flash = false);

    // render a single key
    std::string Key(std::string k, bool disable = false);

    // render game over
    std::string GameOver(std::string status);

    // render htmx oob
    std::string Press(std::string key);

    std::pair<std::vector<int>, std::string> score(std::string word);
};
