import re
import pulp as pl

def parse_line(line: str):
    line = line.strip()
    if not line:
        return None

    # Joltage targets in { ... }
    m_targets = re.search(r"\{([^}]*)\}", line)
    if not m_targets:
        return None
    targets_str = m_targets.group(1)
    targets = [int(x) for x in targets_str.split(",") if x.strip() != ""]
    m = len(targets)

    # Button definitions in ( ... )
    button_parts = re.findall(r"\(([^)]*)\)", line)
    buttons = []
    for part in button_parts:
        part = part.strip()
        if not part:
            continue
        idxs = [int(x) for x in part.split(",") if x.strip() != ""]
        idxs = [i for i in idxs if 0 <= i < m]
        if idxs:
            buttons.append(idxs)

    return buttons, targets

def solve_machine(buttons, targets) -> int:
    m = len(targets)
    k = len(buttons)

    if all(t == 0 for t in targets):
        return 0

    prob = pl.LpProblem("machine", pl.LpMinimize)
    x = [pl.LpVariable(f"x_{j}", lowBound=0, cat="Integer") for j in range(k)]

    # constraints: per counter
    for i in range(m):
        prob += pl.lpSum(x[j] for j in range(k) if i in buttons[j]) == targets[i]

    prob += pl.lpSum(x)

    prob.solve(pl.PULP_CBC_CMD(msg=False))
    status = pl.LpStatus[prob.status]
    if status != "Optimal":
        raise RuntimeError(f"Line infeasible or not solved optimally (status={status})")

    total_presses = pl.value(pl.lpSum(x))
    return int(round(total_presses))

def main():
    total = 0
    with open("input.txt", "r") as f:
        for line_no, line in enumerate(f, start=1):
            parsed = parse_line(line)
            if parsed is None:
                continue
            buttons, targets = parsed
            if not buttons and any(t != 0 for t in targets):
                raise RuntimeError(f"Line {line_no}: no buttons but non-zero targets")
            presses = solve_machine(buttons, targets)
            # print(f"Line {line_no}: {presses}")
            total += presses

    print("Part 2 (joltage) total presses:", total)

if __name__ == "__main__":
    main()