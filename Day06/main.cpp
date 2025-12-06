#include <fstream>
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::ifstream in("input.txt");
    if (!in) {
        std::cerr << "Error: could not open input.txt\n";
        return 1;
    }

    // Read all lines into a grid
    std::vector<std::string> grid;
    std::string line;
    std::size_t max_width = 0;

    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back(); // handle CRLF if present
        }
        max_width = std::max(max_width, line.size());
        grid.push_back(line);
    }

    if (grid.empty()) {
        std::cout << "Part 1: 0\n";
        std::cout << "Part 2: 0\n";
        return 0;
    }

    // Pad all lines to same width with spaces so we can index any column
    for (auto& row : grid) {
        if (row.size() < max_width) {
            row.append(max_width - row.size(), ' ');
        }
    }

    int rows = static_cast<int>(grid.size());
    int cols = static_cast<int>(max_width);

    // Helper: is column c completely empty (all spaces)?
    auto is_empty_column = [&](int c) {
        for (int r = 0; r < rows; ++r) {
            if (grid[r][c] != ' ') {
                return false;
            }
        }
        return true;
    };

    long long grand_total_part1 = 0; // original interpretation (row-wise numbers)
    long long grand_total_part2 = 0; // cephalopod interpretation (column-wise numbers)

    int c = 0;
    while (c < cols) {
        // Skip separator columns (all empty spaces)
        while (c < cols && is_empty_column(c)) {
            ++c;
        }
        if (c >= cols) {
            break;
        }

        int start_col = c;
        // Advance until we hit an empty column or end -> defines one problem block [start_col, end_col]
        while (c < cols && !is_empty_column(c)) {
            ++c;
        }
        int end_col = c - 1;

        // We now have one problem block in columns [start_col, end_col].

        // 1) Find the operator in the last row (same for both parts).
        int op_row = rows - 1;
        char op = 0;
        for (int cc = start_col; cc <= end_col; ++cc) {
            char ch = grid[op_row][cc];
            if (ch == '+' || ch == '*') {
                op = ch;
                break;
            }
        }
        if (op == 0) {
            // No operator found; skip this block
            continue;
        }

        // Part 1: row-wise numbers
        // For each row above operator row, collect all digits in [start_col, end_col]
        {
            std::vector<long long> nums;
            for (int r = 0; r < op_row; ++r) {
                std::string num_str;
                for (int cc = start_col; cc <= end_col; ++cc) {
                    char ch = grid[r][cc];
                    if (std::isdigit(static_cast<unsigned char>(ch))) {
                        num_str.push_back(ch);
                    }
                }
                if (!num_str.empty()) {
                    long long val = std::stoll(num_str);
                    nums.push_back(val);
                }
            }

            if (!nums.empty()) {
                long long value;
                if (op == '+') {
                    value = 0;
                    for (long long x : nums) {
                        value += x;
                    }
                } else { // op == '*'
                    value = 1;
                    for (long long x : nums) {
                        value *= x;
                    }
                }
                grand_total_part1 += value;
            }
        }

        // Part 2: column-wise numbers
        // Cephalopod math: each column in the block is one number.
        // We read columns from right to left, digits from top to just above operator row.
        {
            std::vector<long long> nums2;

            for (int cc = end_col; cc >= start_col; --cc) {
                std::string num_str;
                for (int r = 0; r < op_row; ++r) {
                    char ch = grid[r][cc];
                    if (std::isdigit(static_cast<unsigned char>(ch))) {
                        num_str.push_back(ch);
                    }
                }
                if (!num_str.empty()) {
                    long long val = std::stoll(num_str);
                    nums2.push_back(val);
                }
            }

            if (!nums2.empty()) {
                long long value2;
                if (op == '+') {
                    value2 = 0;
                    for (long long x : nums2) {
                        value2 += x;
                    }
                } else { // op == '*'
                    value2 = 1;
                    for (long long x : nums2) {
                        value2 *= x;
                    }
                }
                grand_total_part2 += value2;
            }
        }
    }

    std::cout << "Part 1 grand total: " << grand_total_part1 << '\n';
    std::cout << "Part 2 grand total: " << grand_total_part2 << '\n';

    return 0;
}