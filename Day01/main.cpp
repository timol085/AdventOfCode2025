#include <fstream>
#include <iostream>
#include <string>
#include <cctype>

int main() {
    std::ifstream in("input.txt");
    if (!in) {
        std::cerr << "Error: could not open input.txt\n";
        return 1;
    }

    // Dial starts at 50
    int position = 50;

    // Part 1: number of times we end a rotation at 0
    long long zero_end_count = 0;

    // Part 2: number of times any click (during or at the end of a rotation) lands on 0
    long long zero_click_count = 0;

    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }

        std::size_t i = 0;
        while (i < line.size() && std::isspace(static_cast<unsigned char>(line[i]))) {
            ++i;
        }
        if (i >= line.size()) {
            continue;
        }

        char direction = line[i];
        if (direction != 'L' && direction != 'R') {
            continue;
        }

        // After direction comes delta steps
        std::string number_part = line.substr(i + 1);

        std::size_t j = 0;
        while (j < number_part.size() && std::isspace(static_cast<unsigned char>(number_part[j]))) {
            ++j;
        }
        number_part = number_part.substr(j);

        if (number_part.empty()) {
            continue;
        }

        int distance = 0;
        try {
            distance = std::stoi(number_part);
        } catch (const std::exception&) {
            continue;
        }

        if (distance < 0) {
            continue;
        }

        // Part 2: count how many times we hit 0 during THIS rotation

        long long d = distance;  // Widen to 64-bit for safety
        if (d > 0) {
            long long first_step;

            if (direction == 'R') {
                // S + k ≡ 0 (mod 100) -> k ≡ (100 - S) (mod 100)
                if (position == 0) {
                    first_step = 100; // 0 + 100 ≡ 0
                } else {
                    first_step = 100 - position;
                }
            } else { // direction == 'L'
                // S - k ≡ 0 (mod 100) -> k ≡ S (mod 100)
                if (position == 0) {
                    first_step = 100; // 0 - 100 ≡ 0
                } else {
                    first_step = position;
                }
            }

            if (first_step <= d) {
                // First hit at first_step, then every +100 steps
                long long hits = 1 + (d - first_step) / 100;
                zero_click_count += hits;
            }
        }

        // Update position as in Part 1
        int distance_mod = distance % 100; // Reduce because dial has 100 positions
        int delta = (direction == 'R') ? distance_mod : -distance_mod;

        int temp = (position + delta) % 100;
        if (temp < 0) {
            temp += 100;
        }
        position = temp;

        // Part 1 counting: dial ends at 0 after this rotation
        if (position == 0) {
            ++zero_end_count;
        }
    }

    std::cout << "Part 1 (end-of-rotation zeros): " << zero_end_count << '\n';
    std::cout << "Part 2 (all clicks hitting 0):  " << zero_click_count << '\n';

    return 0;
}