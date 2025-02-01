#include <cassert>
#include <format>
#include <iostream>
using namespace std;

int main() {
    string expected = "[1.e-37]";
    string actual   = format("[{:#6.0g}]", 1.234e-37);
    assert(expected == actual);
    return 0;
}
