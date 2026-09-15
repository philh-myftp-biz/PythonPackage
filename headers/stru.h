#pragma once

#include <string>
#include <vector>
#include <iterator>

#include "ww898/utf_converters.hpp"

using str = std::string;
using wstr = std::wstring;

namespace stru {

    //===============================================================================

    wstr to_wstr(const str& _str) {
        if (_str.empty()) return wstr();
        return ww898::utf::conv<wchar_t>(_str);
    }

    str to_str(const wstr& _wstr) {
        if (_wstr.empty()) return str();
        return ww898::utf::conv<char>(_wstr);
    }

    //===============================================================================

    bool has_str(str mainstr, str substr) {
        return mainstr.find(substr) != str::npos;
    }

    bool has_str(wstr mainstr, wstr substr) {
        return mainstr.find(substr) != wstr::npos;
    }

    //===============================================================================

    bool match_str(str a, str b) {
        return has_str(a, b) || has_str(b, a);
    }

    bool match_str(wstr a, wstr b) {
        return has_str(a, b) || has_str(b, a);
    }

    //===============================================================================

}
