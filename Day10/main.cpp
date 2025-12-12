#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <queue>

int main() {
    std::ifstream in("input.txt");
    if (!in) {
        std::cerr << "Error: could not open input.txt\n";
        return 1;
    }

    long long total_presses = 0; // sum over all machines
    std::string line;

    while (std::getline(in, line)) {
        // Skip empty lines
        if (line.empty()) continue;

        // Step 1 Extract the indicator pattern inside [ ... ]
        std::size_t lb = line.find('[');
        std::size_t rb = line.find(']', lb + 1);
        if (lb == std::string::npos || rb == std::string::npos || rb <= lb + 1) {
            // malformed line, skip
            continue;
        }

        std::string pattern = line.substr(lb + 1, rb - lb - 1);
        int nLights = static_cast<int>(pattern.size());

        if (nLights == 0) {
            // No lights? Then no presses needed.
            continue;
        }

        // Build target mask: bit i corresponds to light i (0-based from the left)
        // '#' -> 1, '.' -> 0
        int targetMask = 0;
        for (int i = 0; i < nLights; ++i) {
            if (pattern[i] == '#') {
                targetMask |= (1 << i);
            }
        }

        // If target is all off, no presses needed for this machine
        if (targetMask == 0) {
            continue;
        }

        // Step 2 Extract button masks from ( ... ) BEFORE the { ... } block
        std::vector<int> buttonMasks;

        // Only parse up to '{' (ignore joltage block)
        std::size_t bracePos = line.find('{', rb + 1);
        std::size_t searchEnd = (bracePos == std::string::npos) ? line.size() : bracePos;

        std::size_t pos = rb + 1;
        while (true) {
            std::size_t lp = line.find('(', pos);
            if (lp == std::string::npos || lp >= searchEnd) break;
            std::size_t rp = line.find(')', lp + 1);
            if (rp == std::string::npos || rp > searchEnd) break;

            std::string inside = line.substr(lp + 1, rp - lp - 1);

            // Replace commas with spaces to simplify parsing "0,1,2" etc.
            for (char &ch : inside) {
                if (ch == ',') ch = ' ';
            }

            std::stringstream ss(inside);
            int mask = 0;
            int idx;
            while (ss >> idx) {
                if (idx >= 0 && idx < nLights) {
                    mask |= (1 << idx);
                }
            }

            buttonMasks.push_back(mask);
            pos = rp + 1;
        }

        if (buttonMasks.empty()) {
            // No buttons to press, but target is not zero => impossible, skip or handle as you like
            continue;
        }

        // Step 3 BFS over state space [0 .. (1<<nLights)-1]
        int maxState = 1 << nLights;
        std::vector<int> dist(maxState, -1);
        std::queue<int> q;

        dist[0] = 0;
        q.push(0);

        while (!q.empty() && dist[targetMask] == -1) {
            int s = q.front();
            q.pop();
            int d = dist[s];

            // Try pressing each button once
            for (int bm : buttonMasks) {
                int ns = s ^ bm; // toggle bits
                if (dist[ns] == -1) {
                    dist[ns] = d + 1;
                    q.push(ns);
                }
            }
        }

        if (dist[targetMask] == -1) {
            // No way to reach target with any combination of button presses.
            // The puzzle likely guarantees this won't happen.
            // We'll just skip adding anything.
            continue;
        }

        total_presses += dist[targetMask];
    }

    std::cout << total_presses << '\n';
    return 0;
}