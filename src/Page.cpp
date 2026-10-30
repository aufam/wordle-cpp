#include <mup/html.h>
#include "wordle.h"

using namespace mup::html;


std::string Wordle::Page() {
    return html<lang_ == "en">(
        head<>(
            meta<charset_ == "utf-8">,
            meta<name_ == "htmx-config", R"(content='{"allowNestedOobSwaps": false}')">,
            meta<name_ == "viewport", content_ == "width=device-width, initial-scale=1">,

            link_<rel_ == "preconnect", href_ == "https://fonts.googleapis.com">,
            link_<rel_ == "preconnect", href_ == "https://fonts.gstatic.com", crossorigin_ == true>,
            link_<
                rel_ == "stylesheet",
                href_ == "https://fonts.googleapis.com/"
                         "css2?family=Bricolage+Grotesque:opsz,wght@12..96,500;12..96,800&display=swap",
                crossorigin_ == true
            >,
            link_<rel_ == "stylesheet", href_ == "style.css">,

            title<>{"Wordle"},
            script<src_ == "https://unpkg.com/htmx.org@2.0.4">{}
        ),

        body<>(
            header<class_ == "masthead">{
                h1<>{"Wordle"},
                p<>{"Guest the five-letter word in six tries."},
            },

            main<>{
                Game(),
            },

            script<src_ == "app.js">{}
        )
    );
}
