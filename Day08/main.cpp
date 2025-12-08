#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

using ll = long long;

struct Point {
    ll x, y, z;
};

struct Edge {
    ll dist2;
    int a, b;
};

// Disjoint Set Union/Union-Find
struct DSU {
    std::vector<int> parent;
    std::vector<int> size;

    explicit DSU(int n) : parent(n), size(n, 1) {
        for (int i = 0; i < n; ++i) {
            parent[i] = i;
        }
    }

    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }

    // Returns true if a merge happened (two different components joined)
    bool unite(int a, int b) {
        int ra = find(a);
        int rb = find(b);
        if (ra == rb) return false;

        // union by size
        if (size[ra] < size[rb]) {
            std::swap(ra, rb);
        }
        parent[rb] = ra;
        size[ra] += size[rb];
        return true;
    }

    int get_size(int x) {
        int r = find(x);
        return size[r];
    }
};

int main() {
    std::ifstream in("input.txt");
    if (!in) {
        std::cerr << "Error: could not open input.txt\n";
        return 1;
    }

    std::vector<Point> points;
    std::string line;

    // Parse lines of form "x,y,z"
    while (std::getline(in, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        ll x, y, z;
        char c1, c2;
        if (!(ss >> x >> c1 >> y >> c2 >> z)) {
            continue; // malformed line
        }
        points.push_back({x, y, z});
    }

    int n = static_cast<int>(points.size());
    if (n == 0) {
        std::cout << "Part 1: 0\n";
        std::cout << "Part 2: 0\n";
        return 0;
    }

    // Build all edges with squared Euclidean distance
    std::vector<Edge> edges;
    edges.reserve(static_cast<std::size_t>(n) * (n - 1) / 2);

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            ll dx = points[i].x - points[j].x;
            ll dy = points[i].y - points[j].y;
            ll dz = points[i].z - points[j].z;
            ll d2 = dx * dx + dy * dy + dz * dz;
            edges.push_back({d2, i, j});
        }
    }

    // Sort edges by distance ascending (using squared distance)
    std::sort(edges.begin(), edges.end(),
              [](const Edge& e1, const Edge& e2) {
                  return e1.dist2 < e2.dist2;
              });

    // Part 1: first 1000 connections
    const int K = 1000;
    std::size_t limit = std::min<std::size_t>(K, edges.size());

    DSU dsu1(n);

    for (std::size_t i = 0; i < limit; ++i) {
        dsu1.unite(edges[i].a, edges[i].b);
        // Even if already connected, we still "used" this pair;
        // that's exactly how the puzzle describes it.
    }

    // Compute component sizes from dsu1
    std::vector<ll> comp_sizes;
    comp_sizes.reserve(n);
    std::vector<bool> seen(n, false);

    for (int i = 0; i < n; ++i) {
        int r = dsu1.find(i);
        if (!seen[r]) {
            seen[r] = true;
            comp_sizes.push_back(dsu1.size[r]);
        }
    }

    std::sort(comp_sizes.begin(), comp_sizes.end(), std::greater<ll>());

    ll part1_answer = 1;
    if (comp_sizes.size() >= 3) {
        part1_answer = comp_sizes[0] * comp_sizes[1] * comp_sizes[2];
    } else {
        // Fallback if somehow fewer than 3 components
        for (ll s : comp_sizes) {
            part1_answer *= s;
        }
    }

    // Part 2: continue until single circuit
    DSU dsu2(n);
    int components = n;

    int last_a = -1;
    int last_b = -1;

    for (const auto& e : edges) {
        bool merged = dsu2.unite(e.a, e.b);
        if (merged) {
            --components;
            if (components == 1) {
                last_a = e.a;
                last_b = e.b;
                break;
            }
        }
    }

    ll part2_answer = 0;
    if (last_a != -1 && last_b != -1) {
        part2_answer = points[last_a].x * points[last_b].x;
    }

    std::cout << "Part 1 (product of 3 largest circuits): " << part1_answer << '\n';
    std::cout << "Part 2 (product of X of last connection): " << part2_answer << '\n';

    return 0;
}