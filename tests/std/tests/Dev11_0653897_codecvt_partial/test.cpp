// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <cassert>
#include <locale>
#include <sstream>
#include <string>

using namespace std;

typedef codecvt<wchar_t, char, mbstate_t> Facet;

void convert(ostringstream& oss, const wchar_t* first, const wchar_t* last, string& output, const Facet& fac) {
    char out_buf[10]{};

    Facet::state_type state{};

    for (;;) {
        oss << "input left: " << last - first << " chars" << endl;

        char* dest     = begin(out_buf);
        char* dest_end = end(out_buf);

        codecvt_base::result res = fac.out(state, first, last, first, dest, dest_end, dest);

        switch (res) {
        case codecvt_base::ok:
            oss << "conversion returned ok, " << last - first << " chars left, " << dest - out_buf
                << " output chars received" << endl;
            output.append(out_buf, dest);
            return;

        case codecvt_base::partial:
            oss << "conversion returned partial, " << last - first << " chars left, " << dest - out_buf
                << " output chars received" << endl;

            if (dest == out_buf) {
                return;
            }

            output.append(out_buf, dest);
            break;

        case codecvt_base::noconv:
            oss << "conversion returned noconv" << endl;
            return;

        case codecvt_base::error:
            oss << "conversion returned error" << endl;
            return;

        default:
            oss << "uh oh" << endl;
            return;
        }
    }
}

int main() {
    wstring input(15, L'x');
    string output;
    locale loc;
    ostringstream oss;

    convert(oss, input.c_str(), input.c_str() + input.size(), output, use_facet<Facet>(loc));

    oss << "output size: " << output.size() << ": " << output << endl;

    assert(oss.str()
           == "input left: 15 chars\n"
              "conversion returned partial, 5 chars left, 10 output chars received\n"
              "input left: 5 chars\n"
              "conversion returned ok, 0 chars left, 5 output chars received\n"
              "output size: 15: xxxxxxxxxxxxxxx\n");
}
