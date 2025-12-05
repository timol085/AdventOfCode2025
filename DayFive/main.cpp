#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using ll = long long;

// Merge overlapping/touching intervals and return merged vector
std::vector<std::pair<ll, ll>> merge_ranges(std::vector<std::pair<ll, ll>> ranges) {
    if (ranges.empty()) {
        return {};
    }

    std::sort(ranges.begin(), ranges.end(),
              [](const auto& a, const auto& b) {
                  if (a.first != b.first) return a.first < b.first;
                  return a.second < b.second;
              });

    std::vector<std::pair<ll, ll>> merged;
    ll cur_start = ranges[0].first;
    ll cur_end   = ranges[0].second;

    for (std::size_t i = 1; i < ranges.size(); ++i) {
        ll s = ranges[i].first;
        ll e = ranges[i].second;

        if (s <= cur_end + 1) {
            // Overlapping or touching; extend current range
            if (e > cur_end) {
                cur_end = e;
            }
        } else {
            // Disjoint; push current and start a new one
            merged.emplace_back(cur_start, cur_end);
            cur_start = s;
            cur_end   = e;
        }
    }

    merged.emplace_back(cur_start, cur_end);
    return merged;
}

int main() {
    std::ifstream in("input.txt");
    if (!in) {
        std::cerr << "Error: could not open input.txt\n";
        return 1;
    }

    std::vector<std::pair<ll, ll>> ranges;
    std::vector<ll> ids;

    std::string line;
    bool reading_ranges = true;

    // Read ranges until blank line, then read IDs
    while (std::getline(in, line)) {
        if (line.empty()) {
            reading_ranges = false;
            continue;
        }

        if (reading_ranges) {
            std::size_t dash_pos = line.find('-');
            if (dash_pos == std::string::npos) {
                continue;
            }

            std::string start_str = line.substr(0, dash_pos);
            std::string end_str   = line.substr(dash_pos + 1);

            if (start_str.empty() || end_str.empty()) {
                continue;
            }

            try {
                ll start = std::stoll(start_str);
                ll end   = std::stoll(end_str);
                ranges.emplace_back(start, end);
            } catch (const std::exception&) {
                continue;
            }
        } else {
            try {
                ll id = std::stoll(line);
                ids.push_back(id);
            } catch (const std::exception&) {
                continue;
            }
        }
    }

    // Merge ranges once, use for both parts
    auto merged = merge_ranges(ranges);

    // Part 1: how many available IDs are fresh
    ll part1_fresh = 0;
    for (ll id : ids) {
        auto it = std::upper_bound(
            merged.begin(), merged.end(), id,
            [](ll value, const std::pair<ll, ll>& interval) {
                return value < interval.first;
            });

        bool is_fresh = false;
        if (it != merged.begin()) {
            --it;
            const auto& [start, end] = *it;
            if (start <= id && id <= end) {
                is_fresh = true;
            }
        }

        if (is_fresh) {
            ++part1_fresh;
        }
    }

    // Part 2: how many distinct IDs are covered by the ranges
    ll part2_total_ids = 0;
    for (const auto& [start, end] : merged) {
        part2_total_ids += (end - start + 1);
    }

    std::cout << "Part 1 (fresh available IDs): " << part1_fresh << '\n';
    std::cout << "Part 2 (total fresh IDs in ranges): " << part2_total_ids << '\n';

    return 0;
}