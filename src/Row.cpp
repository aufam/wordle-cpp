#include <mup/html.h>
#include <fmt/ranges.h>
#include <ranges>
#include "wordle.h"

using namespace mup::html;

std::string Wordle::Row(size_t i, bool flash, bool shake) {
    return div_<(role = "row", hx_swap_oob = "outerHTML")>{
        (id = fmt::format("row-{}", i), class_ = fmt::format("row{}", shake ? " shake" : "")),

        mup::each(std::views::iota(0ul, answer.size()), [&](size_t j) -> std::string { return Tile(i, j, flash); }),
    };
}
