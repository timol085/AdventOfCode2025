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

    std::vector<std::string> grid;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty()) {
            grid.push_back(line);
        }
    }

    if (grid.empty()) {
        std::cout << "Part 1: 0\n";
        std::cout << "Part 2: 0\n";
        return 0;
    }

    int rows = static_cast<int>(grid.size());
    int cols = static_cast<int>(grid[0].size());

    // 8 directions (dr[k], dc[k])
    int dr[8] = {-1, -1, -1,  0, 0, 1, 1, 1};
    int dc[8] = {-1,  0,  1, -1, 1,-1, 0, 1};

    // Part 1: count initially accessible rolls
    // For each roll '@', count how many adjacent cells (8 directions)
    // also contain '@'. If that count is < 4, the roll is accessible
    // by a forklift.
    int accessible_initial = 0;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (grid[r][c] != '@') {
                continue;
            }

            int neighbors = 0;
            for (int k = 0; k < 8; ++k) {
                int nr = r + dr[k];
                int nc = c + dc[k];

                if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
                    continue;
                }
                if (grid[nr][nc] == '@') {
                    ++neighbors;
                }
            }

            if (neighbors < 4) {
                ++accessible_initial;
            }
        }
    }

    // Part 2: iterative removal
    // Repeatedly remove all currently accessible rolls:
    //  - In each step, find all '@' with < 4 adjacent '@' neighbors.
    //  - Remove all of them at once (set to '.').
    //  - Keep a running total of how many rolls have been removed.
    // Stop when a step removes nothing more.
    std::vector<std::string> work = grid; // copy to modify
    long long total_removed = 0;

    while (true) {
        std::vector<std::pair<int,int>> to_remove;

        for (int r = 0; r < rows; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (work[r][c] != '@') {
                    continue;
                }

                int neighbors = 0;
                for (int k = 0; k < 8; ++k) {
                    int nr = r + dr[k];
                    int nc = c + dc[k];

                    if (nr < 0 || nr >= rows || nc < 0 || nc >= cols) {
                        continue;
                    }
                    if (work[nr][nc] == '@') {
                        ++neighbors;
                    }
                }

                if (neighbors < 4) {
                    to_remove.emplace_back(r, c);
                }
            }
        }

        if (to_remove.empty()) {
            break; // no more accessible rolls
        }

        total_removed += static_cast<long long>(to_remove.size());

        // Remove them all at once
        for (auto [r, c] : to_remove) {
            work[r][c] = '.'; // now empty
        }
    }

    std::cout << "Part 1 (initial accessible rolls): " << accessible_initial << '\n';
    std::cout << "Part 2 (total removable rolls):   " << total_removed << '\n';

    return 0;
}