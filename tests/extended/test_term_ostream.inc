// Term ostream tests — require the linked rang impl.
// The enclosing TU must include <sstream> before `import collab;`.

TEST_CASE("term[ext]: operator<< on a non-TTY ostream emits without throwing", "[term][ext]") {
    std::ostringstream os;
    os << collab::term::fg::red << "hi" << collab::term::reset_color << "\n";
    // rang suppresses color codes on non-TTY streams; we just verify the text
    // round-trips.
    REQUIRE(os.str().find("hi") != std::string::npos);
}

TEST_CASE("term[ext]: style manipulators stream without throwing", "[term][ext]") {
    std::ostringstream os;
    os << collab::term::bold << "loud" << collab::term::reset_style;
    REQUIRE(os.str().find("loud") != std::string::npos);
}
