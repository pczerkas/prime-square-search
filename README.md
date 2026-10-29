# Prime-square triangular search: `0 <= n <= 100,000,000,000`

This directory contains a reproducible four-process search for

\[
S(n)=\sum_{k=1}^n p_k^2=\frac{m(m+1)}2.
\]

The tested equivalent is `8*S(n)+1=(2m+1)^2`. Every index is visited;
quadratic-residue filtering only rejects values that cannot be squares.
Survivors receive an exact unsigned-`__int128` square test.

## Build

```sh
g++ -std=c++17 -O3 -march=native -o prime_square_scan prime_square_scan.cpp
g++ -std=c++17 -O3 -march=native -o prefix_checkpoint prefix_checkpoint.cpp
```

## Prepare and run

`prepare_starts.py` does not contain precomputed large primes. It uses the
standalone Min_25 program and binary search to find an `X` with
`pi(X)=first-1` for each of four contiguous lanes:

```sh
python3 prepare_starts.py --prefix ./prefix_checkpoint --out starts.csv
python3 run_0_to_100b.py --workers 4
python3 verify_0_to_100b.py --prefix ./prefix_checkpoint
```

The scanner covers `n=2..100,000,000,000`. The runner explicitly checks and
records `n=0` (`S=0`, the trivial `m=0` solution) and `n=1` (`S=4`, no).

`prefix_checkpoint.cpp` is a separately compiled Min_25 implementation used
for independent start/checkpoint verification. It recomputes `pi(X)` and the
prime-square prefix; it does not trust values emitted by the scanner.

The output is a finite-range computational verification, not a proof about
indices outside the stated range. For publication, preserve `starts.csv`,
all logs, compiler command lines, machine details, and SHA-256 hashes.