#pragma once

// Convert UTF-8 string to UTF-16LE wstring
inline std::wstring utf8_to_utf16le(const std::string& input) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(input);
}

// Convert UTF-16LE wstring to UTF-8 string
inline std::string utf16le_to_utf8(const std::wstring& input) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(input);
}




template <>
class fmt::formatter<std::filesystem::path> : public fmt::formatter<std::string_view> {
public:
    auto format(const std::filesystem::path& wstr, format_context& ctx) const->format_context::iterator {
        return fmt::formatter<std::string_view>::format(utf16le_to_utf8(wstr), ctx);
    }
};


template <typename CharT, typename Traits>
auto split_string_view(std::basic_string_view<CharT, Traits> input, std::basic_string_view<CharT, Traits> delimiter) -> std::vector<std::basic_string_view<CharT, Traits>> {
    std::vector<std::basic_string_view<CharT, Traits>> result;
    size_t start = 0;
    size_t end = 0;

    while ((end = input.find(delimiter, start)) != std::basic_string_view<CharT, Traits>::npos) {
        result.emplace_back(input.data() + start, end - start);
        start = end + delimiter.size();
    }

    result.emplace_back(input.data() + start, input.size() - start);

    return result;
}

template <typename TyChar, typename TyTrait, typename TyOutput>
void replace_into(const std::basic_string_view<TyChar, TyTrait>& input, const std::basic_string_view<TyChar, TyTrait>& search, const std::basic_string_view<TyChar, TyTrait>& replacement, TyOutput& output) {
    throw std::exception("not implemented yet");
}



inline void plain_replace_into(const std::string& input, const std::string& search, const std::string& replacement, std::string& result) {
    if (search.empty()) return;  // Handle empty search case.


    result.reserve(result.size() + input.length());  // Reserve space to avoid multiple allocations.

    size_t pos = 0;
    size_t search_len = search.length();
    size_t found_pos;

    // Traverse the input string once.
    while ((found_pos = input.find(search, pos)) != std::string::npos) {
        // Append the part before the found position and the replacement.
        result.append(input, pos, found_pos - pos);
        result.append(replacement);
        // Move the position ahead.
        pos = found_pos + search_len;
    }

    // Append the remaining part of the string after the last occurrence.
    result.append(input, pos);
}

inline auto plain_replace(const std::string& input, const std::string& search, const std::string& replacement) -> std::string {
    std::string res;
    plain_replace_into(input, search, replacement, res);
    return res;
}

template <typename CharT, typename Traits, typename Predicate>
auto trim_string_view(std::basic_string_view<CharT, Traits> input, Predicate pred) -> std::basic_string_view<CharT, Traits> {
    size_t start = 0;
    size_t end = input.size();

    // Find the first non-matching character from the start
    while (start < end && pred(input[start])) {
        ++start;
    }

    // Find the first non-matching character from the end
    while (end > start && pred(input[end - 1])) {
        --end;
    }

    return input.substr(start, end - start);
}


inline auto read_from_file(const std::filesystem::path& path) -> std::string {
    std::ifstream fs;
    fs.exceptions(fs.failbit | fs.badbit);
    fs.open(path, fs.in);
    if (!fs.is_open()) {
        throw std::runtime_error(fmt::format("Failed to open file: {}", path));
    }
    std::string str;
    std::getline(fs, str, '\0');
    fs.close();
    return str;
}


inline auto write_to_file(const std::filesystem::path& path, std::string& content) -> void {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream outFile(path);
    // Check if the file stream is open
    if (!outFile) {
        throw std::runtime_error("Failed to write to file");
    }

    // Write the string to the file
    outFile << content;

    // Close the file
    outFile.close();

}







