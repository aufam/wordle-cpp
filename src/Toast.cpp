#include <mup/html.h>
#include "wordle.h"

using namespace mup::html;

std::string Wordle::Toast(std::string msg) {
    return div_<id_ == "toast", class_ == "toast", hx_swap_oob == "outerHTML">{
        hidden_ = msg.empty(),
        msg,
    };
}
