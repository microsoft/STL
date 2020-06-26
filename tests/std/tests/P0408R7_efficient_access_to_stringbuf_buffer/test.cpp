#include <cassert>
#include <memory_resource>
#include <sstream>
#include <string>

using namespace std;

template <typename Stream>
void test_rvalue_util(const string& init_value) {
    Stream stream{string{init_value}};
    assert(stream.view() == init_value);
    assert(stream.str() == init_value);
    assert(stream.view() == init_value);
    // Move out the buffer, the underlying buffer should be empty.
    assert(move(stream).str() == init_value);
    assert(stream.view().empty());
    assert(stream.str().empty());
    // Move in the buffer string
    stream.str(string{init_value});
    assert(stream.view() == init_value);
    assert(stream.str() == init_value);
}

template <typename Stream>
void test_rvalue() {
    test_rvalue_util<Stream>("a");
    test_rvalue_util<Stream>(
        "This is a long long long long long long long long string to avoid small string optimization.");
}

template <typename Stream>
void test_allocator_util(const pmr::string& init_value) {
    Stream stream{init_value};
    assert(stream.view() == init_value);
    assert(stream.str<polymorphic_allocator<char>>() == init_value);
    // Clear the stream
    stream.str("");
    stream.str(init_value);
    assert(stream.view() == init_value);
    assert(stream.str<polymorphic_allocator<char>>() == init_value);
}

template <typename Stream>
void test_allocator() {
    test_allocator_util<Stream>("a");
    test_allocator_util<Stream>(
        "This is a long long long long long long long long string to avoid small string optimization.");
}

int main() {
    test_rvalue<stringstream>();
    test_rvalue<istringstream>();
    test_rvalue<ostringstream>();

    test_allocator<stringstream>();
    test_allocator<istringstream>();
    test_allocator<ostringstream>();
}
