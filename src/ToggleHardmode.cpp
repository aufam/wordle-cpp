#include <mup/html.h>
#include "wordle.h"

using namespace mup::html;

std::string Wordle::ToggleHardMode(bool do_toggle) {
    if (do_toggle)
        hard_mode = !hard_mode;

    return button<(
        type = "button", class_ = "toggle", id = "toggle-hard-mode", hx_post = "press/toggle-hard-mode", hx_swap_oob = "outerHTML"
    )>{(aria_pressed = hard_mode ? "true" : "false"), hard_mode ? "Hard Mode: ON" : "Hard Mode: OFF"};
}
