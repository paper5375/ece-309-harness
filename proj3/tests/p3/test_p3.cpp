// tests/p3/test_p3.cpp — YOUR P3 TEST SUITE.
//
// The five tests below are worked examples. Keep them, and add your own
// until you have AT LEAST 15 (spec §6 lists what must be covered).
// Name every test <area>_<what>: reg_ for the registry, ws_ for the
// workspace, calc_ for the calculator.
//
// Build & run:   cmake --build build && ./build/test_p3
// One area:      ./build/test_p3 reg_        (names starting with "reg_")
// One test:      ./build/test_p3 =reg_add_and_find

#include "../mini_test.h"
#include "../test_helpers.h"

#include "tools/calculator.h"
#include "tools/tool_registry.h"
#include "tools/workspace.h"

#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

// A tool whose name you choose — use it to fill the registry in tests.
class NamedTool : public Tool {
public:
    explicit NamedTool(std::string n) : n_(std::move(n)), schema_(JsonValue::object()) {}
    std::string name() const override { return n_; }
    std::string description() const override { return "test tool"; }
    const JsonValue& schema() const override { return schema_; }
    ToolResult invoke(const JsonValue&, ToolContext&) override { return ToolResult::success(n_); }
private:
    std::string n_;
    JsonValue   schema_;   // an empty JSON object: {}
};

// Tokenize with the provided tokenizer, then run YOUR calc_evaluate.
bool eval(const std::string& expr, double* value, std::string* error) {
    std::vector<CalcToken> tokens;
    if (!calc_tokenize(expr, &tokens, error)) return false;
    return calc_evaluate(tokens, value, error);
}

}  // namespace

// ---------------------------------------------------------------- examples

TEST(reg_add_and_find) {
    ToolRegistry r;
    auto t = std::make_unique<NamedTool>("alpha");
    Tool* raw = t.get();                        // remember the address before moving
    REQUIRE(r.add(std::move(t)));
    CHECK(r.size() == 1);
    CHECK(r.find("alpha") == raw);              // same object, not a copy
    CHECK(r.find("beta") == nullptr);
}

TEST(reg_duplicate_keeps_original) {
    ToolRegistry r;
    auto first = std::make_unique<NamedTool>("alpha");
    Tool* original = first.get();
    REQUIRE(r.add(std::move(first)));
    CHECK(!r.add(std::make_unique<NamedTool>("alpha")));
    CHECK(r.find("alpha") == original);
    CHECK(r.size() == 1);
}

TEST(ws_rejects_parent_escape) {
    TempDir d;
    Workspace ws(d.path());
    fs::path out;
    std::string reason;
    REQUIRE(ws.resolve("notes/todo.txt", &out, &reason));   // sanity: accepts *something*
    CHECK(out == ws.root() / "notes" / "todo.txt");
    CHECK(!ws.resolve("../etc/passwd", &out, &reason));
    CHECK(!reason.empty());
}

TEST(calc_precedence) {
    double v = 0;
    std::string err;
    REQUIRE(eval("2 + 3 * 4", &v, &err));
    CHECK(v == 14);
}

TEST(calc_division_by_zero) {
    double v = 0;
    std::string err;
    CHECK(!eval("1 / 0", &v, &err));
    CHECK(err == "division by zero");
}

// ------------------------------------------------------------- your tests

TEST(reg_10000_tools) {
    ToolRegistry r;
    Tool* firsttool = nullptr;

    for (int i = 0; i < 10000; i++) {
        std::unique_ptr<NamedTool> t = std::make_unique<NamedTool>("tool_" + std::to_string(i));
        if (firsttool == nullptr) firsttool = t.get();

        REQUIRE(r.add(std::move(t)));
        CHECK(r.load_factor() <= 1.0);
    }

    CHECK(r.size() == 10000);
    CHECK(r.find("tool_0") == firsttool);

    for (int i = 0; i < 10000; ++i) {
        CHECK(r.find("tool_" + std::to_string(i)) != nullptr);
    }
    CHECK(r.max_probe_length() <= 64);

    std::cout << "bucket_count: "     << r.bucket_count()     << std::endl;
    std::cout << "load_factor: "      << r.load_factor()      << std::endl;
    std::cout << "max_probe_length: " << r.max_probe_length() << std::endl;
}

TEST(reg_alternative_hash) {
    ToolRegistry r;
    for (int i = 0; i < 10000; i++) {
        std::unique_ptr<NamedTool> t = std::make_unique<NamedTool>("tool_" + std::to_string(i));
        r.add(std::move(t));
    }

    const std::size_t buckets = r.bucket_count();
    std::vector<int> counts(buckets, 0);

    for (int i = 0; i < 10000; i++) {
        const std::string s = "tool_" + std::to_string(i);
        const std::size_t weak_hash = s[0] + s.size();

        counts[weak_hash % buckets]++;
    }

    int longest = 0;
    for (auto c : counts) {
        if (c > longest) longest = c;
    }

    std::cout << "Weak hash longest chain: " << longest              << std::endl;
    std::cout << "FNV-1a longest chain: "    << r.max_probe_length() << std::endl;
    CHECK(static_cast<std::size_t>(longest) > r.max_probe_length());
}

TEST(reg_null_rejected) {
    ToolRegistry r;
    CHECK(!r.add(nullptr));
    CHECK(r.size() == 0);
    REQUIRE(r.add(std::make_unique<NamedTool>("mytool")));
    CHECK(!r.add(nullptr));
    CHECK(r.size() == 1);
}

TEST(find_absent_name) {
    ToolRegistry r;
    REQUIRE(r.add(std::make_unique<NamedTool>("present")));
    CHECK(r.find("absent") == nullptr);
}

TEST(reg_for_each_visits_each_once) {
    ToolRegistry r;
    for (auto i = 0; i < 10; i++) {
        r.add(std::make_unique<NamedTool>("tool_" + std::to_string(i)));
    }

    std::vector<std::string> seen_tools;
    r.for_each([&](const Tool& t) {
        seen_tools.push_back(t.name());
    });

    std::sort(seen_tools.begin(), seen_tools.end());
    CHECK(seen_tools.size() == 10);
    for (int i = 0; i < 10; i++) {
        CHECK(seen_tools.at(i) == "tool_" + std::to_string(i));
    }
}



// BELOW COMMENT FROM INSTRUCTOR
//
// Still to write (spec §6 has the full list). Suggested names:
//   reg_null_rejected, reg_for_each_visits_each_once
//   reg_10000_tools               REQUIRED: print bucket_count / load_factor /
//                                 max_probe_length, check the bound
//   reg_alternative_hash          REQUIRED: longest chain of a weak hash
//   ws_<case>                     REQUIRED: every row of the table in spec §4.4
//   calc_<case>                   left-to-right, parentheses, negatives


int main(int argc, char** argv) { return mt::run_all(argc, argv); }
