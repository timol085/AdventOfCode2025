#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <cctype>

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::ifstream in("input.txt");
    if (!in) {
        std::cerr << "Error: could not open input.txt\n";
        return 1;
    }

    // First pass: parse lines into raw adjacency with names
    std::vector<std::pair<std::string, std::vector<std::string>>> raw;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;

        // Find device name before ':'
        std::size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        std::string src = line.substr(0, colonPos);
        // trim spaces on src
        auto trim = [](std::string &s) {
            while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) {
                s.pop_back();
            }
            std::size_t i = 0;
            while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) {
                ++i;
            }
            if (i > 0) {
                s.erase(0, i);
            }
        };
        trim(src);

        std::string rhs = line.substr(colonPos + 1);
        // split rhs on spaces
        std::stringstream ss(rhs);
        std::string token;
        std::vector<std::string> outs;
        while (ss >> token) {
            outs.push_back(token);
        }

        raw.push_back({src, outs});
    }

    // Map device name -> index
    std::unordered_map<std::string, int> id;
    id.reserve(raw.size() * 2);

    auto get_id = [&](const std::string &name) -> int {
        auto it = id.find(name);
        if (it != id.end()) return it->second;
        int newId = static_cast<int>(id.size());
        id[name] = newId;
        return newId;
    };

    // Ensure all nodes have indices
    for (auto &p : raw) {
        get_id(p.first);
        for (auto &to : p.second) {
            get_id(to);
        }
    }

    int n = static_cast<int>(id.size());
    std::vector<std::vector<int>> adj(n);
    std::vector<std::vector<int>> rev(n);

    // Build adjacency lists (forward and reverse)
    for (auto &p : raw) {
        int u = get_id(p.first);
        for (auto &to : p.second) {
            int v = get_id(to);
            adj[u].push_back(v);
            rev[v].push_back(u);
        }
    }

    // Look up important nodes
    auto get_optional = [&](const char* name) -> int {
        auto it = id.find(name);
        if (it == id.end()) return -1;
        return it->second;
    };

    int you = get_optional("you");
    int svr = get_optional("svr");
    int out = get_optional("out");
    int dac = get_optional("dac");
    int fft = get_optional("fft");

    if (out == -1) {
        std::cerr << "Error: no 'out' node in input\n";
        return 1;
    }

    // Common helper: paths_to_out(u)
    std::vector<long long> memo_to_out(n, -1);

    std::function<long long(int)> paths_to_out = [&](int u) -> long long {
        if (u == out) return 1;  // exactly one path: already at 'out'
        long long &memo_val = memo_to_out[u];
        if (memo_val != -1) return memo_val;

        long long total = 0;
        for (int v : adj[u]) {
            total += paths_to_out(v);
        }
        memo_val = total;
        return total;
    };

    // Part 1: paths from "you" to "out"
    if (you != -1) {
        long long part1 = paths_to_out(you);
        std::cout << "Part 1 (paths from you to out): " << part1 << "\n";
    } else {
        std::cout << "Part 1 (paths from you to out): (no 'you' node in this input)\n";
    }

    // Helper: paths_from_svr(u) via reverse graph
    std::vector<long long> memo_from_svr(n, -1);

    std::function<long long(int)> paths_from_svr = [&](int u) -> long long {
        if (svr == -1) return 0;    // no svr in input
        if (u == svr) return 1;     // exactly one path: svr to itself
        long long &memo_val = memo_from_svr[u];
        if (memo_val != -1) return memo_val;

        long long total = 0;
        // In reversed graph, edges go "backwards".
        // Paths from svr -> u (forward) == paths from u -> svr (in rev graph).
        for (int p : rev[u]) {
            total += paths_from_svr(p);
        }
        memo_val = total;
        return total;
    };

    // Helper: count paths between two nodes (source -> target)
    auto count_paths_between = [&](int source, int target) -> long long {
        if (source == -1 || target == -1) return 0;
        std::vector<long long> memo(n, -1);

        std::function<long long(int)> dfs_between = [&](int u) -> long long {
            if (u == target) return 1;
            long long &mv = memo[u];
            if (mv != -1) return mv;

            long long total = 0;
            for (int v : adj[u]) {
                total += dfs_between(v);
            }
            mv = total;
            return total;
        };

        return dfs_between(source);
    };

    // Part 2: paths from svr to out that visit both dac and fft
    long long part2 = 0;

    if (svr != -1 && dac != -1 && fft != -1) {
        // Count pieces:

        // svr -> dac, svr -> fft  (using reverse graph)
        long long svr_to_dac = paths_from_svr(dac);
        long long svr_to_fft = paths_from_svr(fft);

        // dac -> fft and fft -> dac
        long long dac_to_fft = count_paths_between(dac, fft);
        long long fft_to_dac = count_paths_between(fft, dac);

        // dac -> out and fft -> out (we already have paths_to_out)
        long long dac_to_out = paths_to_out(dac);  // used via formula
        long long fft_to_out = paths_to_out(fft);

        // Total paths S -> A-> B -> O + S -> B -> A -> O
        // = svr_to_dac * dac_to_fft * fft_to_out
        // + svr_to_fft * fft_to_dac * dac_to_out
        part2 = svr_to_dac * dac_to_fft * fft_to_out
              + svr_to_fft * fft_to_dac * dac_to_out;

        std::cout << "Part 2 (paths from svr to out visiting dac and fft): "
                  << part2 << "\n";
    } else {
        std::cout << "Part 2: missing svr/dac/fft in this input, cannot compute.\n";
    }

    return 0;
}