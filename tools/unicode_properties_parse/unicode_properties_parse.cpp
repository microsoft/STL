#include <assert.h>
#include <charconv>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <istream>
#include <iterator>
#include <limits.h>
#include <locale>
#include <ranges>
#include <span>
#include <stdexcept>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>
#include <vector>
using namespace std;
using namespace std::filesystem;
using namespace std::string_literals;

struct unicode_properties_parse_error final : runtime_error {
    using runtime_error::runtime_error;
};

enum token_type {
    ttInvalid,
    ttEof,
    ttDots,
    ttSemicolon,
    ttIdent, // note: we can't distinguish "numbers" from "names"
    ttNumTokenTypes
};

constexpr const char* token_type_names[] = {"ttInvalid", "ttEof", "ttDots", "ttSemicolon", "ttIdent"};
static_assert(size(token_type_names) == ttNumTokenTypes);

struct token {
    token_type type = ttInvalid;
    string literal;
};

struct tokenizer {
    istream* str;
    static constexpr int eof = istream::traits_type::eof();

    explicit tokenizer(istream& str) : str(&str) {}

    void skip() {
        for (;;) {
            switch (str->peek()) {
            case ' ':
                (void) str->get();
                break;
            case '#':
                (void) str->get();
                for (int c = str->peek(); c != '\r' && c != '\n' && c != eof; c = str->get()) {
                }
                break;
            case '\r':
                (void) str->get();
                break;
            case '\n':
                (void) str->get();
                break;
            default:
                return;
            }
        }
    }

    string getString() {
        string result;
        do {
            result += static_cast<char>(str->get());
        } while (isalnum(str->peek(), locale::classic()) || str->peek() == '_');
        return result;
    }

    token getTok() {
        token result;
        skip();
        int c = str->peek();
        if (c == '.') {
            (void) str->get();
            if (str->peek() == '.') {
                result.type    = ttDots;
                result.literal = "..";
                (void) str->get();
            } else {
                str->unget();
            }
        } else if (c == ';') {
            result.type    = ttSemicolon;
            result.literal = ";";
            (void) str->get();
        } else if (isalnum(c, locale::classic())) {
            result.type    = ttIdent;
            result.literal = getString();
        } else if (c == eof) {
            result.type    = ttEof;
            result.literal = "<EOF>";
        }
        return result;
    }
};

void printToken(const token& t) {
    fputs("{ ", stdout);
    fputs(token_type_names[t.type], stdout);
    fputs(", ", stdout);
    fputs(t.literal.c_str(), stdout);
    fputs(" }", stdout);
}


constexpr const char* Grapheme_Cluster_Break_Names[] = {"Any", "CR", "LF", "Control", "Extend", "ZWJ",
    "Regional_Indicator", "Prepend", "SpacingMark", "L", "V", "T", "LV", "LVT", "E_Base", "E_Modifier",
    "Glue_After_Zwj", "E_Base_GAZ", nullptr};

constexpr const char* Extended_Pictographic_Names[] = {"Extended_Pictographic", nullptr};
struct property_range {
    uint32_t first;
    uint32_t last;
    uint8_t prop;
};

void print_property_range(const property_range& prop_range) {
    fprintf(stdout, "{%x,%x,%s}", prop_range.first, prop_range.last, Grapheme_Cluster_Break_Names[prop_range.prop]);
}

struct properties {
    vector<uint32_t> code_point_lower_bounds;
    vector<uint16_t> props_and_range;
};

properties convert_to_properties_table(span<property_range> ranges) {
    properties result;
    for (auto& r : ranges) {
        result.code_point_lower_bounds.push_back(r.first);
        // OK to use exclusive range, since 0x10FFFF is the largest unicode
        // code-point
        uint32_t size = (r.last - r.first) + 1;
        // the size of the range can't be over 4095 since we want to use the bottom
        // 12 bytes of a short to store the size (using the top 4 for the property
        // type)
        if (size > 0x0FFF) {
            throw unicode_properties_parse_error("Size of range can not exceed 4,095");
        }
        result.props_and_range.push_back(static_cast<uint16_t>(size) | (static_cast<uint16_t>(r.prop) << 12));
    }
    assert(result.code_point_lower_bounds.size() == result.props_and_range.size());
    return result;
}
struct parser : tokenizer {
    token tok;

    explicit parser(istream& str) : tokenizer(str) {
        nextTok();
    }

    void expectType(token_type type) {
        if (tok.type != type) {
            throw unicode_properties_parse_error(
                "Expected: "s + token_type_names[type] + " Got: "s + token_type_names[tok.type]);
        }
    }
    void nextTok() {
        tok = getTok();
    }
    void eatTok(token_type type) {
        expectType(type);
        nextTok();
    }
    void expectOneOfType(initializer_list<token_type> types) {
        for (auto tt : types) {
            if (tt == tok.type) {
                return;
            }
        }
        throw unicode_properties_parse_error("unexpected token type: "s + token_type_names[tok.type]);
    }
    uint32_t parseCodePoint() {
        uint32_t result;
        auto [ptr, ec] = from_chars(tok.literal.data(), tok.literal.data() + tok.literal.size(), result, 16);
        if (ptr != tok.literal.data() + tok.literal.size()) {
            throw unicode_properties_parse_error("Expected hex numeric literal but got: " + tok.literal);
        }
        if (ec != errc{}) {
            throw system_error(make_error_code(ec));
        }
        return result;
    }
    // prop names must be null terminated
    int8_t parseOptionalProperty(const char* const* prop_names) {
        const char* const* it = prop_names;
        for (; *it != nullptr; ++it) {
            if (strcmp(tok.literal.c_str(), *it) == 0) {
                break;
            }
        }
        ptrdiff_t index = it - prop_names;
        if (index > INT8_MAX) {
            throw unicode_properties_parse_error(
                "Property has too many values, max 255 per property got: " + to_string(index));
        }
        if (*it == nullptr) {
            return -1;
        }
        return static_cast<int8_t>(index);
    }
    property_range parseOptionalPropertyRange(const char* const* prop_names) {
        // returns a property value of uint8_t(-1) if the property is not matched
        property_range result{};
        expectType(ttIdent);
        result.first = parseCodePoint();
        result.last  = result.first;
        nextTok();
        if (tok.type == ttDots) {
            nextTok();
            expectType(ttIdent);
            result.last = parseCodePoint();
            nextTok();
        }
        eatTok(ttSemicolon);
        expectType(ttIdent);
        result.prop = parseOptionalProperty(prop_names);
        return result;
    }
    property_range parsePropertyRange(const char* const* prop_names) {
        property_range result = parseOptionalPropertyRange(prop_names);
        if (result.prop == static_cast<uint8_t>(-1)) {
            throw unicode_properties_parse_error("invalid property: " + tok.literal);
        }
        return result;
    }
    vector<property_range> parseAll(const char* const* prop_names) {
        vector<property_range> result;
        while (tok.type != ttEof) {
            result.push_back(parsePropertyRange(prop_names));
            nextTok();
        }
        return result;
    }
    vector<property_range> parseSelectedProp(const char* const* prop_names) {
        // parse property ranges that contain the selected property and
        // ignore lines that contain other properties. Most unicode data files contain
        // only one property but some, such as emoji-data.txt contain multiple properties
        // and one code point may appear multiple times
        vector<property_range> result;
        property_range prop{};
        while (tok.type != ttEof) {
            prop = parseOptionalPropertyRange(prop_names);
            if (prop.prop != static_cast<uint8_t>(-1)) {
                result.push_back(prop);
            }
            nextTok();
        }
        return result;
    }
};
template <typename T>
void output_literals(ostream& os, const vector<T>& values) {
    using namespace ranges::views;
    if (values.size() == 0) {
        return;
    }
    char hex_value_buffer[64] = {'0', 'x', 0};
    os << "{";
    for (uint32_t i : values | take(values.size() - 1)) {
        auto [end, ec] = to_chars(hex_value_buffer + 2, std::end(hex_value_buffer) - 1, i, 16);
        assert(ec == errc{});
        os.write(hex_value_buffer, end - hex_value_buffer);
        os << ",";
    }
    auto [end, ec] = to_chars(hex_value_buffer + 2, std::end(hex_value_buffer) - 1, values.back(), 16);
    assert(ec == errc{});
    os.write(hex_value_buffer, end - hex_value_buffer);
    os << "};\n";
}

void output_property_enum(ostream& os, string_view enum_name, const char* const* prop_value_names) {
    os << "enum class " << enum_name << " {";
    if (*prop_value_names != nullptr) {
        // output the first name without a leading comma
        os << *prop_value_names;
    }
    ++prop_value_names;
    for (; *prop_value_names != nullptr; ++prop_value_names) {
        os << ", " << *prop_value_names;
    }
    os << "};\n";
}

#ifdef _WIN32
int wmain(int, wchar_t** argv)
#else
int main(int char** argv)
#endif
{
    path props_path(argv[0]);
    props_path.replace_filename("GraphemeBreakProperty.txt");
    ifstream file(props_path, std::ios::binary | std::ios::in);
    parser parse(file);
    auto gbrprops = parse.parseAll(Grapheme_Cluster_Break_Names);
    ranges::sort(gbrprops, {}, &property_range::first);
    auto gbrtable = convert_to_properties_table(gbrprops);
    ostringstream str;
    output_property_enum(str, "_Grapheme_Break_Property_Values", Grapheme_Cluster_Break_Names);
    output_property_enum(str, "_Extended_Pictographic_Property_Values", Extended_Pictographic_Names);
    str << "static constexpr uint32_t _Grapheme_Break_Property_Codepoint_Lower_Bounds[] = ";
    output_literals(str, gbrtable.code_point_lower_bounds);
    str << "static constexpr uint16_t _Grapheme_Break_Property_Props_And_Range[] = ";
    output_literals(str, gbrtable.props_and_range);

    path emoji_path(argv[0]);
    emoji_path.replace_filename("emoji-data.txt");
    file  = ifstream(emoji_path, std::ios::binary | std::ios::in);
    parse = parser(file);

    auto extended_pictographic_property = parse.parseSelectedProp(Extended_Pictographic_Names);
    ranges::sort(extended_pictographic_property, {}, &property_range::first);
    auto extended_pictographic_table = convert_to_properties_table(extended_pictographic_property);

    str << "static constexpr uint32_t _Extended_Pictographic_Codepoint_Lower_Bounds[] = ";
    output_literals(str, extended_pictographic_table.code_point_lower_bounds);
    str << "static constexpr uint16_t _Extended_Pictographic_Props_And_Range[] = ";
    output_literals(str, extended_pictographic_table.props_and_range);
    fputs(str.str().c_str(), stdout);
}
