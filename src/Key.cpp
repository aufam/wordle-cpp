#include <mup/html.h>
#include <fmt/format.h>
#include "wordle.h"

using namespace mup::html;

std::string Wordle::Key(std::string k, bool disable) {
    auto mark_class = [](int m) {
        switch (m) {
        case 3: return "correct";
        case 2: return "present";
        case 1: return "absent";
        default: return "";
        }
    };

    auto label = k;
    if (label == "back")
        label = "⌫";
    else if (label == "enter")
        label = "Enter";

    return button<type_ == "button", hx_swap_oob == "outerHTML">{
        id_        = "key-" + k,
        class_     = fmt::format("key {} {}", k.size() == 1 ? mark_class(keys[k[0]]) : "", k.size() > 1 ? "wide" : ""),
        hx_post    = "press/" + k,
        aria_label = k,
        disabled_  = disable,

        label,
    };
}
