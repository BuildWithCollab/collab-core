// Term tests — header-only-compatible (just the enum / constexpr surface).
// The ostream operators that actually emit ANSI escape codes need the linked
// rang impl; those tests live in tests/extended/.
//
// Comparisons are wrapped in extra parens to prevent Catch2's expression
// decomposition from instantiating `operator<<(ostream&, color/style)` —
// the operators are only defined in the linked impl, and include-only mode
// has no library to resolve them against.

TEST_CASE("term: fg constants alias the right color enums", "[term]") {
    REQUIRE((collab::term::fg::red    == collab::term::color::red));
    REQUIRE((collab::term::fg::green  == collab::term::color::green));
    REQUIRE((collab::term::fg::blue   == collab::term::color::blue));
    REQUIRE((collab::term::reset_color == collab::term::color::reset));
}

TEST_CASE("term: style constants alias the right style enums", "[term]") {
    REQUIRE((collab::term::bold        == collab::term::style::bold));
    REQUIRE((collab::term::italic      == collab::term::style::italic));
    REQUIRE((collab::term::underline   == collab::term::style::underline));
    REQUIRE((collab::term::reset_style == collab::term::style::reset));
}
