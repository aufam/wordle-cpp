#include <mup/html.h>
#include <fmt/ranges.h>
#include "wordle.h"

using namespace mup::html;

auto tile_label(char letter, int m) {
    const char *desc;
    switch (m) {
    case 3: desc = "correct spot"; break;
    case 2: desc = "in the word, wrong spot"; break;
    case 1: desc = "not in the word"; break;
    default: desc = ""; break;
    }

    return fmt::format("{:c}, {}", letter - 'a' + 'A', desc);
};

std::string Wordle::Tile(size_t i, size_t j, bool flash) {
    auto &g = *this;

    auto div = div_<(role = "gridcell", hx_swap_oob = "outerHTML")>{};
    div.attributes.push_back(fmt::format("id=\"tile-{}-{}\"", i, j));

    std::vector<std::string> classes = {"tile"};
    if (flash)
        classes.push_back("flip");

    std::string style, aria_label;

    if (i < g.guesses.size()) {
        auto &guessed = g.guesses[i];
        div.content   = guessed.word[j];

        auto m = guessed.marks[j];
        if (m == 3)
            classes.push_back("correct");
        else if (m == 2)
            classes.push_back("present");
        else if (m == 1)
            classes.push_back("absent");

        style      = fmt::format("animation-delay: {}ms", j * 280);
        aria_label = tile_label(guessed.word[j], guessed.marks[j]);
    } else if (i == g.guesses.size()) {
        classes.push_back("typed");
        if (j < g.current.size())
            div.content = g.current[j];
    }

    div.attributes.push_back(fmt::format("class=\"{}\"", fmt::join(classes, " ")));
    if (!style.empty())
        div.attributes.push_back(fmt::format("style=\"{}\"", style));
    if (!aria_label.empty())
        div.attributes.push_back(fmt::format("aria-label=\"{}\"", aria_label));

    return div;
}
