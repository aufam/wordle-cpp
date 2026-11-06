#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/uuid.hpp>
#include <boost/random.hpp>
#include <iostream>
#include <fstream>
#include <mutex>
#include "wordle.h"
#include "word_list.h"

import brb;
import brb.static_files;
import fmt;

std::shared_ptr<const WordList> load_word_list(std::string path) {
    auto words = std::make_shared<WordList>();

    std::ifstream file(path);
    if (!file)
        throw std::runtime_error("Failed to open word list: " + path);

    std::string word;
    while (std::getline(file, word)) {
        if (!word.empty() && word.back() == '\r')
            word.pop_back();

        if (word.empty())
            continue;

        words->valid_guesses.insert(word);
        words->answers.push_back(std::move(word));
    }

    if (file.bad())
        throw std::runtime_error("Failed to read word list: " + path);

    if (words->answers.empty())
        throw std::runtime_error("Word list contains no answers: " + path);

    return words;
}

int main() {
    auto words = load_word_list("words.txt");

    boost::asio::io_context io;
    std::mutex              mtx;
    brb::Engine             engine{io};

    std::unordered_map<std::string, std::shared_ptr<Wordle>> all;

    const auto logger = [](brb::Context &c) -> boost::asio::awaitable<void> {
        const auto  ep  = c.stream->socket().remote_endpoint();
        const auto &req = c.req();
        fmt::println("[{}:{}] {} {}", ep.address().to_string(), ep.port(), req.method_string(), req.target());

        try {
            co_await c.next();
        } catch (boost::system::system_error &e) {
            fmt::println("[{}:{}] {}", ep.address().to_string(), ep.port(), e.code().message());
            c.res().result(brb::http::status::internal_server_error);
            co_return;
        } catch (std::exception &e) {
            fmt::println("[{}:{}] {}", ep.address().to_string(), ep.port(), e.what());
            c.res().result(brb::http::status::internal_server_error);
            co_return;
        }

        const auto &res = c.res();
        fmt::println("[{}:{}] {} {}", ep.address().to_string(), ep.port(), (int)res.result(), res.reason());
    };

    const auto static_files = brb::middlewares::static_files{
        .dir           = "static",
        .last_modified = false,
    };

    engine.use(logger, static_files, brb::middlewares::request_reader{.string_body = true});

    const auto get_id = [](brb::Context &c) {
        std::string id;

        std::string_view cookie = c.req()[brb::http::field::cookie];
        std::string_view key    = "playground_id=";

        auto pos = cookie.find(key);
        if (pos != std::string_view::npos) {
            cookie = cookie.substr(pos + key.size());
            cookie = cookie.substr(0, cookie.find(';'));
            id     = std::string(cookie);
        }

        if (id.empty()) {
            id = boost::uuids::to_string(boost::uuids::random_generator()());
            c.res().set(brb::http::field::set_cookie, fmt::format("{}{}; Max-Age=31536000", key, id));
        }

        return id;
    };

    const auto new_random = [&](bool hard_mode = false) {
        boost::random::mt19937                          rng{boost::uuids::detail::random_device{}()};
        boost::random::uniform_int_distribution<size_t> dist{0, words->answers.size() - 1};

        return std::make_shared<Wordle>(Wordle::New(words->answers[dist(rng)], words, hard_mode));
    };

    const auto remove_oob = [](std::string &&html) {
        constexpr std::string_view attr = R"(hx-swap-oob="outerHTML")";

        size_t pos = 0;
        while ((pos = html.find(attr, pos)) != std::string::npos)
            html.erase(pos, attr.size());

        return html;
    };

    engine.route(brb::http::verb::get, "/", [&](brb::Context &c) -> brb::awaitable<void> {
        auto id = get_id(c);

        auto w = [&]() {
            std::lock_guard<std::mutex> lock(mtx);
            if (all.count(id))
                return all.at(id);

            return all[id] = new_random();
        }();

        c.response_string().body() = remove_oob(w->Page());
        c.res().set(brb::http::field::content_type, "text/html");

        co_return;
    });

    engine.route(brb::http::verb::post, "/new", [&](brb::Context &c) -> brb::awaitable<void> {
        auto id = get_id(c);

        auto w = [&]() {
            std::lock_guard<std::mutex> lock(mtx);
            return all[id] = new_random(all.count(id) == 0 ? false : all.at(id)->hard_mode);
        }();

        c.response_string().body() = remove_oob(w->Game());
        c.res().set(brb::http::field::content_type, "text/html");

        co_return;
    });

    engine.route(brb::http::verb::post, "/press/:key", [&](brb::Context &c) -> brb::awaitable<void> {
        auto id = get_id(c);

        auto [w, err] = [&]() -> std::pair<std::shared_ptr<Wordle>, std::string> {
            std::lock_guard<std::mutex> lock(mtx);
            if (all.count(id))
                return {all.at(id), ""};
            else
                return {nullptr, "User data not found, please reload the page"};
        }();

        auto key = std::string(c.get(":key"));
        c.res().set(brb::http::field::content_type, "text/html");

        if (!err.empty()) {
            c.response_string().body() = w->Toast(err);
        } else if (key == "toggle-hard-mode") {
            c.response_string().body() = w->ToggleHardMode(true);
            fmt::println("hardmode={}", w->hard_mode);
        } else {
            c.response_string().body() = w->Press(key);
        }

        co_return;
    });

    const auto run_opts = brb::Engine::run_opts{
        .host                 = "0.0.0.0",
        .port                 = 7000,
        .cancellation_signals = {SIGINT, SIGTERM},
    };
    boost::asio::co_spawn(io, engine.run(run_opts), boost::asio::detached);

    io.run();
}
