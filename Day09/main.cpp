#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

using ll = long long;

struct Point {
    ll x, y;
};

// Check if point p lies on segment a-b (axis-aligned)
bool point_on_segment(const Point& p, const Point& a, const Point& b) {
    if (a.x == b.x) {
        // vertical segment
        if (p.x != a.x) return false;
        ll ymin = std::min(a.y, b.y);
        ll ymax = std::max(a.y, b.y);
        return (p.y >= ymin && p.y <= ymax);
    } else if (a.y == b.y) {
        // horizontal segment
        if (p.y != a.y) return false;
        ll xmin = std::min(a.x, b.x);
        ll xmax = std::max(a.x, b.x);
        return (p.x >= xmin && p.x <= xmax);
    } else {
        return false;
    }
}

// Ray-casting: is p inside or on boundary of orthogonal polygon poly?
bool point_inside_or_on(const Point& p, const std::vector<Point>& poly) {
    int n = static_cast<int>(poly.size());

    // First: check if on any edge -> treat as inside
    for (int i = 0; i < n; ++i) {
        const Point& a = poly[i];
        const Point& b = poly[(i + 1) % n];
        if (point_on_segment(p, a, b)) {
            return true;
        }
    }

    // Standard even/odd rule, cast ray to +x, only vertical edges matter
    int crossings = 0;
    for (int i = 0; i < n; ++i) {
        const Point& a = poly[i];
        const Point& b = poly[(i + 1) % n];

        if (a.x == b.x) {
            // Vertical edge at x = a.x, from y1 to y2
            ll xE = a.x;
            ll y1 = a.y;
            ll y2 = b.y;
            if (y1 > y2) std::swap(y1, y2);

            // Check if ray from p to +x crosses this edge.
            // We use [y1, y2) to avoid double-counting vertices.
            if (p.y >= y1 && p.y < y2 && p.x < xE) {
                crossings++;
            }
        } else {
            // Horizontal edge: doesn't affect vertical ray crossing in this scheme
            continue;
        }
    }

    return (crossings % 2) == 1;
}

// Check if the rectangle [X1,X2]x[Y1,Y2] has any edge that properly crosses polygon boundary
bool rectangle_crosses_polygon(ll X1, ll Y1, ll X2, ll Y2, const std::vector<Point>& poly) {
    int n = static_cast<int>(poly.size());

    for (int i = 0; i < n; ++i) {
        Point a = poly[i];
        Point b = poly[(i + 1) % n];

        if (a.x == b.x) {
            // Polygon edge vertical at x = a.x, y in [yA, yB]
            ll xP = a.x;
            ll yA = a.y;
            ll yB = b.y;
            if (yA > yB) std::swap(yA, yB);

            // Check intersection with top edge y = Y1, x in [X1, X2]
            // Want a "proper" intersection, not just touching at endpoints.
            if (Y1 > yA && Y1 < yB && X1 < xP && xP < X2) {
                return true;
            }
            // Bottom edge y = Y2
            if (Y2 > yA && Y2 < yB && X1 < xP && xP < X2) {
                return true;
            }
        } else if (a.y == b.y) {
            // Polygon edge horizontal at y = a.y, x in [xA, xB]
            ll yP = a.y;
            ll xA = a.x;
            ll xB = b.x;
            if (xA > xB) std::swap(xA, xB);

            // Check intersection with left edge x = X1, y in [Y1, Y2]
            if (X1 > xA && X1 < xB && Y1 < yP && yP < Y2) {
                return true;
            }
            // Right edge x = X2
            if (X2 > xA && X2 < xB && Y1 < yP && yP < Y2) {
                return true;
            }
        }
    }

    return false;
}

// Check if rectangle with corners (X1,Y1) and (X2,Y2) is fully inside polygon (including boundary)
bool rectangle_inside_polygon(ll X1, ll Y1, ll X2, ll Y2, const std::vector<Point>& poly) {
    // Normalize ordering
    if (X1 > X2) std::swap(X1, X2);
    if (Y1 > Y2) std::swap(Y1, Y2);

    // Corner points
    Point p1{X1, Y1};
    Point p2{X1, Y2};
    Point p3{X2, Y1};
    Point p4{X2, Y2};

    // 1) All corners must be inside or on boundary
    if (!point_inside_or_on(p1, poly)) return false;
    if (!point_inside_or_on(p2, poly)) return false;
    if (!point_inside_or_on(p3, poly)) return false;
    if (!point_inside_or_on(p4, poly)) return false;

    // 2) No rectangle edge may properly cross the polygon boundary
    if (rectangle_crosses_polygon(X1, Y1, X2, Y2, poly)) return false;

    return true;
}

int main() {
    std::ifstream in("input.txt");
    if (!in) {
        std::cerr << "Error: could not open input.txt\n";
        return 1;
    }

    std::vector<Point> reds;
    std::string line;

    // Input lines "x,y" in polygon order
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        ll x, y;
        char comma;
        if (!(ss >> x >> comma >> y)) {
            continue;
        }
        reds.push_back({x, y});
    }

    int n = static_cast<int>(reds.size());
    if (n < 2) {
        std::cout << "Part 1: 0\n";
        std::cout << "Part 2: 0\n";
        return 0;
    }

    // Part 1: largest rectangle using any two red tiles as opposite corners,
    // ignoring interior content.
    ll best_part1 = 0;

    // Part 2: largest rectangle that is fully within the red+green region,
    // i.e., fully inside or on the polygon formed by reds in input order.
    ll best_part2 = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            ll x1 = reds[i].x;
            ll y1 = reds[i].y;
            ll x2 = reds[j].x;
            ll y2 = reds[j].y;

            ll dx = std::llabs(x2 - x1);
            ll dy = std::llabs(y2 - y1);

            ll width  = dx + 1;
            ll height = dy + 1;
            ll area   = width * height;

            // Part 1
            if (area > best_part1) {
                best_part1 = area;
            }

            // Part 2: only if rectangle is fully inside/on polygon
            if (area > best_part2) {
                if (rectangle_inside_polygon(x1, y1, x2, y2, reds)) {
                    best_part2 = area;
                }
            }
        }
    }

    std::cout << "Part 1 (any tiles inside):     " << best_part1 << '\n';
    std::cout << "Part 2 (only red/green tiles): " << best_part2 << '\n';

    return 0;
}