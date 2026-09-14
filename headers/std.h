#ifndef TO_WSTRING_EXT_H
#define TO_WSTRING_EXT_H

#include <string>
#include <codecvt>
#include <locale>

// Explicitly extending the std namespace
namespace std {

    inline std::wstring to_wstring(const std::string& str) {

        int wideLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
        
        if (wideLen > 0) {
            // 2. Allocate the wide string buffer
            std::wstring wide_str(wideLen - 1, L'\0'); 
            
            // 3. Perform the actual conversion
            MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wide_str[0], wideLen);
            
            return wide_str;
        } else {
            return L"";
        }
        
    }

    std::string to_string(const std::wstring& wstr) {
        if (wstr.empty()) return "";

        // 1. Determine the required size for the destination buffer
        size_t size_needed = std::wcstombs(nullptr, wstr.c_str(), 0);
        
        if (size_needed == static_cast<size_t>(-1)) {
            throw std::runtime_error("Conversion failed: invalid wide character sequence.");
        }

        // 2. Allocate space and convert
        std::string str(size_needed, '\0');
        std::wcstombs(&str[0], wstr.c_str(), size_needed);

        return str;
    }

}

#endif // TO_WSTRING_EXT_H
