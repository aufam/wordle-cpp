#include <mup/html.h>
#include <fmt/format.h>
#include "wordle.h"

using namespace mup::html;


std::string Wordle::GameOver(std::string status) {
    using game_over = div_<(id = "game-over", class_ = "game-over", hx_swap_oob = "outerHTML")>;
    using end_title = div_<(class_ = "end-title")>;

    prev_status = status;

    if (status.empty())
        return game_over{};

    return game_over{
        [&]() -> std::string {
            if (status == "won")
                return end_title{
                    fmt::format("Solved in {}.", guesses.size()),
                };
            else if (status == "lost")
                return end_title{
                    "The word was",
                    span<"class=answer">{answer},
                };

            return "";
        }(),

        button<(class_ = "again", type = "button", hx_post = "new", hx_target = "#game", hx_swap = "outerHTML")>{"Play again"},
    };
}
