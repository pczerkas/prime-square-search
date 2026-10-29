#!/usr/bin/env python3
"""Find exact safe scan starts using the standalone Min_25 checkpoint binary."""
import argparse, subprocess

ap = argparse.ArgumentParser()
ap.add_argument("--prefix", default="./prefix_checkpoint")
ap.add_argument("--out", default="starts.csv")
ap.add_argument("--max-n", type=int, default=100_000_000_000)
ap.add_argument("--lanes", type=int, default=4)
args = ap.parse_args()

def pi(x):
    out = subprocess.check_output([args.prefix, str(x)], text=True)
    return int(dict(z.split('=') for z in out.split()).get('PI', -1))

rows=[]
for lane in range(args.lanes):
    first = max(2, args.max_n * lane // args.lanes + (1 if lane else 0))
    last = args.max_n * (lane + 1) // args.lanes
    target = first - 1
    if target == 1: x = 2
    else:
        # Dusart-style estimate, then enlarge the bracket until it contains target.
        import math
        if target < 6: guess = 15
        else: guess = int(target * (math.log(target) + math.log(math.log(target))))
        lo, hi = max(2, guess // 2), max(3, guess * 2)
        while pi(lo) > target: lo //= 2
        while pi(hi) <= target: hi *= 2
        while lo + 1 < hi:
            mid = (lo + hi) // 2
            if pi(mid) <= target: lo = mid
            else: hi = mid
        x = lo
    got = pi(x)
    if got != target: raise SystemExit(f"cannot prepare lane {lane}: pi({x})={got}, target={target}")
    rows.append((lane, first, last, x, got))
with open(args.out, 'w') as f:
    f.write('lane,first,last,X,pi_X\n')
    for row in rows: f.write(','.join(map(str,row))+'\n')
print(f"wrote {args.out}")