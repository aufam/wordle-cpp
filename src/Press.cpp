#include <mup/html.h>
#include <fmt/ranges.h>
#include "wordle.h"

using namespace mup::html;


std::string Wordle::Press(std::string key) {
    const auto active_row = guesses.size();
    const auto active_col = current.size();

    bool game_over = (active_row > 0 && guesses.back().word == answer) || active_row >= max_guess;
    if (game_over)
        return "";

    std::vector<std::string> rerender;
    std::string              message;

    if (key == "back") {
        if (active_col > 0) {
            current.pop_back();
            rerender.push_back(Tile(active_row, active_col - 1));
        }
    } else if (key == "enter") {
        const auto word = current;

        bool do_shake = false;
        bool do_flash = false;

        auto [marks, err] = score(word);

        if (err.empty()) {
            do_flash = true;

            auto &guess = guesses.emplace_back(current, marks);
            current.clear();

            fmt::println("score={}", fmt::join(guess.marks, " "));
            for (size_t i = 0; i < answer.size(); ++i) {
                auto  m = guess.marks[i];
                auto &k = keys[word[i]];

                k = std::max(m, k);

                rerender.push_back(Key(std::string(1, word[i])));
            }

            if (word == answer) {
                rerender.push_back(GameOver("won"));
                game_over = true;
            } else if (guesses.size() >= max_guess) {
                rerender.push_back(GameOver("lost"));
                game_over = true;
            } else if (active_row + 1 <= max_guess) {
                rerender.push_back(Row(active_row + 1));
            }

        } else {
            do_shake = true;
            message  = err;
        }

        rerender.push_back(Row(active_row, do_flash, do_shake));
    } else if (key.size() == 1 && key[0] >= 'a' && key[0] <= 'z') {
        if (active_col < answer.size()) {
            current.append(key);
            rerender.push_back(Tile(active_row, active_col));
        }
    } else {
        return Toast("Invalid key: " + key);
    }

    rerender.push_back(Key(key));
    rerender.push_back(Toast(message)); // update toast message or hide if empty
    if (game_over) {
        for (auto &keys : key_rows)
            for (auto &key : keys)
                rerender.push_back(Key(key, true)); // disable all keys
    }

    return fmt::format("{}", fmt::join(rerender, "\n"));
}
