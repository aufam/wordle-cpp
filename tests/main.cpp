#include "../src/wordle.h"
#include "../src/word_list.h"
#include <assert.h>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

using WordSet = std::unordered_set<std::string>;

static std::shared_ptr<const WordList> make_list(WordSet words) {
    auto lst              = std::make_shared<WordList>();
    lst->valid_guesses    = std::move(words);
    return lst;
}

// type a full word and press enter
static void press_word(Wordle &w, const std::string &word) {
    for (auto c : word) {
        auto res = w.Press(std::string(1, c));
        assert(!res.empty());
    }
    w.Press("enter");
}

// ---------------------------------------------------------------- score (hard mode disabled)

static void test_score_marks() {
    auto w = Wordle::New("abbey", make_list({"abbey", "babes", "kebab"}), false);

    {
        auto [marks, err] = w.score("abbey");
        assert(err.empty());
        assert((marks == std::vector<int>{3, 3, 3, 3, 3}));
    }

    // duplicated letters in the answer: only as many hits as exist
    {
        auto [marks, err] = w.score("babes"); // b a b e s vs a b b e y
        assert(err.empty());
        assert((marks == std::vector<int>{2, 2, 3, 3, 1}));
    }

    // duplicated letters in the guess: leftmost consume first
    {
        auto [marks, err] = w.score("kebab"); // k e b a b vs a b b e y
        assert(err.empty());
        assert((marks == std::vector<int>{1, 2, 3, 2, 2}));
    }
}

static void test_score_input_validation() {
    auto w = Wordle::New("abbey", make_list({"abbey"}), false);

    {
        auto [marks, err] = w.score("ab");
        assert(marks.empty());
        assert(err == "Not enough letters");
    }
    {
        auto [marks, err] = w.score("keyboard");
        assert(marks.empty());
        assert(err == "Word is too long");
    }
    {
        auto [marks, err] = w.score("zzzzz");
        assert(marks.empty());
        assert(err == "Not in the word list");
    }

    // invalid input must not disturb game state
    assert(w.guesses.empty() && w.current.empty() && w.keys.empty());
}

// ---------------------------------------------------------------- score (hard mode enabled)

static void test_score_hard_mode() {
    // simulate knowledge from previous guesses: c green, a green, r yellow, e yellow, t absent
    auto w  = Wordle::New("crate", make_list({"crate", "brace", "crabs", "crave"}), true);
    w.keys  = {{'c', 3}, {'a', 3}, {'r', 2}, {'e', 2}, {'t', 1}};

    // green constraint violated: 1st letter must be 'c'
    {
        auto [marks, err] = w.score("brace");
        assert(marks.empty());
        assert(err == "1st letter must be C");
    }

    // yellow constraint violated, greens respected: no 'e'
    {
        auto [marks, err] = w.score("crabs");
        assert(marks.empty());
        assert(err == "Guess must contain E");
    }

    // consistent word passes and is scored normally
    {
        auto [marks, err] = w.score("crave");
        assert(err.empty());
        assert((marks == std::vector<int>{3, 3, 3, 1, 3}));
    }
}

// ---------------------------------------------------------------- press (hard mode disabled)

static void test_press_hard_disabled() {
    auto w = Wordle::New("crate", make_list({"crate", "trace", "trane"}), false);

    // typing letters fills the current row
    assert(!w.Press("t").empty());
    assert(!w.Press("r").empty());
    assert(w.current == "tr");

    // backspace removes the last letter
    assert(!w.Press("back").empty());
    assert(w.current == "t");

    // backspace on an empty row is a no-op
    w.current.clear();
    assert(!w.Press("back").empty());
    assert(w.current.empty());

    // unknown keys are rejected and change nothing (rendering a key may
    // create a neutral mark entry, but it must not carry any knowledge)
    assert(!w.Press("?").empty());
    assert(!w.Press("Enter").empty()); // case matters
    assert(w.current.empty() && w.guesses.empty());
    for (auto &[k, v] : w.keys)
        assert(v == 0);

    // letters past the answer length are ignored
    for (auto c : std::string("crate"))
        assert(!w.Press(std::string(1, c)).empty());
    assert(w.current == "crate");
    assert(!w.Press("x").empty());
    assert(w.current == "crate");

    // valid non-winning guess is recorded and scored
    w.current.clear();
    for (auto c : std::string("trane"))
        assert(!w.Press(std::string(1, c)).empty());
    w.Press("enter");
    assert(w.guesses.size() == 1);
    assert(w.guesses[0].word == "trane");
    // t r a n e vs c r a t e: t yellow, r/a/e green, n absent
    assert((w.guesses[0].marks == std::vector<int>{2, 3, 3, 1, 3}));
    assert(w.current.empty());
    assert(w.keys.at('t') == 2 && w.keys.at('r') == 3 && w.keys.at('a') == 3);
    assert(w.keys.at('n') == 1 && w.keys.at('e') == 3);

    // incomplete word on enter: rejected, row kept for correction
    for (auto c : std::string("cra"))
        assert(!w.Press(std::string(1, c)).empty());
    w.Press("enter");
    assert(w.guesses.size() == 1); // unchanged
    assert(w.current == "cra");

    // winning guess ends the game and freezes input
    w.current.clear();
    for (auto c : std::string("crate"))
        assert(!w.Press(std::string(1, c)).empty());
    assert(!w.Press("enter").empty());
    assert(w.guesses.size() == 2 && w.guesses[1].word == "crate");
    assert(w.Press("a").empty()); // game over
    assert(w.Press("?").empty()); // even invalid keys
    assert(w.guesses.size() == 2 && w.current.empty());
}

// ---------------------------------------------------------------- press (hard mode enabled)

static void test_press_hard_enabled() {
    auto w = Wordle::New("crate", make_list({"crate", "trace", "truce", "urate"}), true);

    // seed knowledge from trace: greens r(1), a(2), e(4); yellows t(0), c(3)
    press_word(w, "trace");
    assert(w.guesses.size() == 1);
    assert((w.guesses[0].marks == std::vector<int>{2, 3, 3, 2, 3}));
    assert(w.keys.at('r') == 3 && w.keys.at('a') == 3 && w.keys.at('e') == 3);
    assert(w.keys.at('t') == 2 && w.keys.at('c') == 2);

    // green violation: 3rd letter must be 'a' (truce has 'u' there)
    press_word(w, "truce");
    assert(w.guesses.size() == 1); // rejected
    assert(w.current == "truce");
    w.current.clear();

    // yellow violation: matches greens, has 't', but lacks 'c'
    press_word(w, "urate");
    assert(w.guesses.size() == 1); // rejected
    assert(w.current == "urate");
    w.current.clear();

    // fully consistent guess is accepted and wins
    press_word(w, "crate");
    assert(w.guesses.size() == 2 && w.guesses[1].word == "crate");
    assert(w.Press("a").empty()); // game over
}

// ----------------------------------------------------------------

int main() {
    test_score_marks();
    test_score_input_validation();
    test_score_hard_mode();
    test_press_hard_disabled();
    test_press_hard_enabled();
}
