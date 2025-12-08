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

    // Read the grid
    std::vector<std::string> grid;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back(); // handle CRLF if needed
        }
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

    // Find the starting point 'S'
    int start_row = -1;
    int start_col = -1;
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (grid[r][c] == 'S') {
                start_row = r;
                start_col = c;
                break;
            }
        }
        if (start_row != -1) break;
    }

    if (start_row == -1) {
        std::cerr << "Error: no 'S' found in grid\n";
        return 1;
    }

    // Part 1: classical beams, count splitter hits
    //  - We track which columns currently have a beam in this row.
    //  - Beams only move downward.
    //  - On '.', beam continues straight.
    //  - On '^', beam stops and two new beams spawn left/right.
    //  - We count how many times any beam hits '^'.
    long long split_count = 0;

    std::vector<bool> active(cols, false);
    std::vector<bool> next_active(cols, false);

    // First beams start just below S, if that row exists.
    if (start_row + 1 < rows) {
        active[start_col] = true;
    }

    for (int r = start_row + 1; r < rows; ++r) {
        std::fill(next_active.begin(), next_active.end(), false);

        for (int c = 0; c < cols; ++c) {
            if (!active[c]) continue; // no beam in this column at this row

            char cell = grid[r][c];

            if (cell == '.') {
                // Beam passes straight down
                next_active[c] = true;
            } else if (cell == '^') {
                // Beam hits splitter: count split, spawn left/right beams
                ++split_count;

                if (c - 1 >= 0) {
                    next_active[c - 1] = true;
                }
                if (c + 1 < cols) {
                    next_active[c + 1] = true;
                }
                // Beam in this column stops here
            } else {
                // Treat any other char as empty (S shouldn't appear below start anyway)
                next_active[c] = true;
            }
        }

        active.swap(next_active);
    }

    // Part 2: quantum many-worlds timelines
    //  - Only one particle is sent in.
    //  - At each splitter '^', the timeline splits in two:
    //      left and right branches both exist.
    //  - We want to count how many distinct timelines exist
    //    after the particle has completed all possible journeys
    //    (i.e., after all branches exit the manifold).
    //
    // We do DP over the grid:
    //  ways[r][c] = number of distinct histories that place
    //               the particle at cell (r, c) moving downwards.
    //
    // Transitions:
    //   '.' : ways[r+1][c] += ways[r][c]  (or exits at bottom)
    //   '^' : split into two:
    //         left  -> (r+1, c-1)
    //         right -> (r+1, c+1)
    //         if a branch goes out-of-bounds (left/right or bottom),
    //         that contributes directly to final timeline count.
    long long timeline_count = 0;
    std::vector<std::vector<long long>> ways(rows, std::vector<long long>(cols, 0));

    // The particle first appears in the cell just below 'S'
    if (start_row + 1 < rows) {
        ways[start_row + 1][start_col] = 1;
    }

    for (int r = start_row + 1; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            long long w = ways[r][c];
            if (w == 0) continue;

            char cell = grid[r][c];
            bool is_splitter = (cell == '^');

            if (!is_splitter) {
                // Behaves like empty space: particle goes straight down
                int nr = r + 1;
                int nc = c;
                if (nr >= rows) {
                    // Exits the bottom of the manifold
                    timeline_count += w;
                } else {
                    ways[nr][nc] += w;
                }
            } else {
                // Splitter: timeline splits into left and right branches
                int nr = r + 1;

                // Left branch
                int lc = c - 1;
                if (lc < 0 || nr >= rows) {
                    // Goes out of bounds (left or bottom) -> contributes a timeline
                    timeline_count += w;
                } else {
                    ways[nr][lc] += w;
                }

                // Right branch
                int rc = c + 1;
                if (rc >= cols || nr >= rows) {
                    // Goes out of bounds (right or bottom) -> contributes a timeline
                    timeline_count += w;
                } else {
                    ways[nr][rc] += w;
                }
            }
        }
    }

    // It's possible that the last row contains particles that haven't "stepped"
    // out yet only if we didn't treat their move; but in this formulation,
    // every move either goes to row+1 or exits, so all exits are already counted.

    std::cout << "Part 1 (total splits):    " << split_count << '\n';
    std::cout << "Part 2 (total timelines): " << timeline_count << '\n';

    return 0;
}