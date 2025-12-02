#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

// Part 1: invalid if decimal representation is repeated exactly twice
bool is_invalid_part1(long long n) {
    std::string s = std::to_string(n);
    int len = static_cast<int>(s.size());

    if (len % 2 != 0) {
        return false;
    }

    int half = len / 2;
    std::string left  = s.substr(0, half);
    std::string right = s.substr(half, half);

    return left == right;
}

// Part 2: invalid if decimal representation is repeated at least twice
bool is_invalid_part2(long long n) {
    std::string s = std::to_string(n);
    int len = static_cast<int>(s.size());

    for (int block_len = 1; block_len <= len / 2; ++block_len) {
        if (len % block_len != 0) {
            continue;
        }

        int repeats = len / block_len;
        if (repeats < 2) {
            continue;
        }

        const std::string pattern = s.substr(0, block_len);
        bool ok = true;

        for (int i = block_len; i < len; i += block_len) {
            if (s.compare(i, block_len, pattern) != 0) {
                ok = false;
                break;
            }
        }

        if (ok) {
            return true;
        }
    }

    return false;
}

int main() {
    std::ifstream in("input.txt");
    if (!in) {
        std::cerr << "Error: could not open input.txt\n";
        return 1;
    }

    std::string line;
    if (!std::getline(in, line)) {
        std::cerr << "Error: input.txt is empty or unreadable\n";
        return 1;
    }

    long long part1_sum = 0;
    long long part2_sum = 0;

    std::stringstream ss(line);
    std::string range;

    // Each 'range' is something like "11-22"
    while (std::getline(ss, range, ',')) {
        if (range.empty()) {
            continue;
        }

        std::size_t dash_pos = range.find('-');
        if (dash_pos == std::string::npos) {
            continue;
        }

        std::string start_str = range.substr(0, dash_pos);
        std::string end_str   = range.substr(dash_pos + 1);

        if (start_str.empty() || end_str.empty()) {
            continue;
        }

        long long start = 0;
        long long end   = 0;
        try {
            start = std::stoll(start_str);
            end   = std::stoll(end_str);
        } catch (const std::exception&) {
            continue;
        }

        if (end < start) {
            continue;
        }

        for (long long id = start; id <= end; ++id) {
            if (is_invalid_part1(id)) {
                part1_sum += id;
            }
            if (is_invalid_part2(id)) {
                part2_sum += id;
            }
        }
    }

    std::cout << "Part 1 sum: " << part1_sum << '\n';
    std::cout << "Part 2 sum: " << part2_sum << '\n';
    return 0;
}