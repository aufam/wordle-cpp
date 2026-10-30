#include <mup/html.h>
#include <fmt/format.h>
#include <ranges>
#include "wordle.h"

using namespace mup::html;

std::string Wordle::Game() {
    return div_<id_ == "game", class_ == "game">{
        div_<class_ == "toast-slot", role_ == "status", aria_live == "polite">{Toast("")},

        div_<class_ == "board", role_ == "grid", aria_label == "Guesses">{

            mup::each(std::views::iota(0ul, max_guess), [&](size_t i) { return Row(i); })
        },

        GameOver(prev_status),

        div_<class_ == "keyboard">{

            mup::each(key_rows, [&](const std::vector<std::string> &keys) {
                return div_<class_ == "krow">{
                    mup::each(keys, [&](auto &k) { return Key(k); }),
                };
            })
        }
    };
}
