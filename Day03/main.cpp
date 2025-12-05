#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

int main() {
    std::ifstream in("input.txt");
    if (!in) {
        std::cerr << "Error: could not open input.txt\n";
        return 1;
    }

    unsigned long long part1_total = 0; // sum of best 2-digit joltage per line
    unsigned long long part2_total = 0; // sum of best 12-digit joltage per line

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }

        int n = static_cast<int>(line.size());
        if (n < 2) {
            continue; // can't do part 1 either
        }

        // Convert chars to digits
        std::vector<int> digits(n);
        for (int i = 0; i < n; ++i) {
            digits[i] = line[i] - '0'; // '1'..'9' -> 1..9
        }

        // Part 1: best 2-digit number per line
        {
            std::vector<int> suffixMax(n);
            suffixMax[n - 1] = digits[n - 1];
            for (int i = n - 2; i >= 0; --i) {
                suffixMax[i] = std::max(digits[i], suffixMax[i + 1]);
            }

            int best2 = 0;
            for (int i = 0; i < n - 1; ++i) {
                int tens  = digits[i];
                int units = suffixMax[i + 1];
                int val   = tens * 10 + units;
                if (val > best2) {
                    best2 = val;
                }
            }

            part1_total += static_cast<unsigned long long>(best2);
        }

        // Part 2: best 12-digit number per line
        const int K = 12;
        if (n >= K) {
            int to_remove = n - K;
            std::vector<char> st;
            st.reserve(n);

            // Monotonic stack: keep digits as large as possible
            for (char c : line) {
                while (!st.empty() && to_remove > 0 && st.back() < c) {
                    st.pop_back();
                    --to_remove;
                }
                st.push_back(c);
            }

            // If we still have extra digits, drop from the end
            while ((int)st.size() > K) {
                st.pop_back();
            }

            // Convert chosen digits to a 12-digit number
            unsigned long long best12 = 0;
            for (char c : st) {
                int d = c - '0';
                best12 = best12 * 10ULL + static_cast<unsigned long long>(d);
            }

            part2_total += best12;
        }
    }

    std::cout << "Part 1 total joltage: " << part1_total << '\n';
    std::cout << "Part 2 total joltage: " << part2_total << '\n';

    return 0;
}