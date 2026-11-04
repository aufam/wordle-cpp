#include <mup/html.h>
#include "wordle.h"

using namespace mup::html;


std::string Wordle::Page() {
    return html<(lang = "en")>(
        head<>(
            meta<(charset = "utf-8")>,
            meta<(name = "htmx-config", R"(content='{"allowNestedOobSwaps": false}')")>,
            meta<(name = "viewport", content_ = "width=device-width, initial-scale=1")>,

            link_<(rel = "preconnect", href = "https://fonts.googleapis.com")>,
            link_<(rel = "preconnect", href = "https://fonts.gstatic.com", crossorigin)>,
            link_<(
                rel  = "stylesheet",
                href = "https://fonts.googleapis.com/"
                       "css2?family=Bricolage+Grotesque:opsz,wght@12..96,500;12..96,800&display=swap",
                crossorigin
            )>,
            link_<(rel = "stylesheet", href = "style.css")>,

            title<>{"Wordle"},
            script<(src = "https://cdn.jsdelivr.net/npm/htmx.org@4.0.1/dist/htmx.min.js", crossorigin = "anonymous")>{}
        ),

        body<>(
            header<(class_ = "masthead")>{
                h1<>{"Wordle"},
                p<>{"Guest the five-letter word in six tries."},
            },

            main<>{
                Game(),
            },

            script<(src = "app.js")>{}
        )
    );
}
