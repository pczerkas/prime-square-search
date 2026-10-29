#include <bits/stdc++.h>
using namespace std;
using u64 = uint64_t;
using U = unsigned __int128;
using Clock = chrono::steady_clock;

static string dec(U x) {
    if (!x) return "0";
    string s;
    while (x) { s.push_back(char('0' + x % 10)); x /= 10; }
    reverse(s.begin(), s.end());
    return s;
}

struct Prefix { u64 count; U sumsq; };

// Min_25-style prime-counting and prime-square prefix.
static Prefix min25(u64 N) {
    u64 r = sqrtl((long double)N);
    vector<u64> v, small(r + 2), large(r + 2);
    for (u64 l = 1; l <= N; ) {
        u64 x = N / l, h = N / x, id = v.size();
        v.push_back(x);
        if (x <= r) small[x] = id; else large[N / x] = id;
        l = h + 1;
    }
    vector<u64> cnt(v.size());
    vector<U> ss(v.size());
    for (size_t i = 0; i < v.size(); ++i) {
        U x = v[i]; cnt[i] = v[i] - 1;
        ss[i] = x * (x + 1) * (2 * x + 1) / 6 - 1;
    }
    auto at = [&](u64 x) -> size_t { return x <= r ? small[x] : large[N / x]; };
    vector<uint8_t> composite(r + 1);
    vector<u64> ps;
    for (u64 p = 2; p <= r; ++p) {
        if (composite[p]) continue;
        ps.push_back(p);
        if (p * p <= r)
            for (u64 q = p * p; q <= r; q += p) composite[q] = 1;
    }
    u64 pc = 0; U sp = 0;
    for (u64 p : ps) {
        if (p * p > N) break;
        u64 p2 = p * p;
        for (size_t i = 0; i < v.size() && v[i] >= p2; ++i) {
            size_t j = at(v[i] / p);
            cnt[i] -= cnt[j] - pc;
            ss[i] -= U(p2) * (ss[j] - sp);
        }
        ++pc; sp += p2;
    }
    return {cnt[0], ss[0]};
}

static vector<uint32_t> base_primes(u64 limit) {
    vector<bool> c(limit + 1);
    vector<uint32_t> p;
    for (u64 x = 2; x <= limit; ++x) if (!c[x]) {
        p.push_back((uint32_t)x);
        if (x * x <= limit)
            for (u64 y = x * x; y <= limit; y += x) c[y] = true;
    }
    return p;
}

static bool exact_square(U x) {
    U r = (U)sqrtl((long double)x);
    while (r * r > x) --r;
    while ((r + 1) * (r + 1) <= x) ++r;
    return r * r == x;
}

struct ResidueFilter {
    vector<uint32_t> mod;
    vector<vector<uint8_t>> qr;
    explicit ResidueFilter(u64 cap) {
        for (uint32_t p : base_primes(cap)) if (p != 2) {
            mod.push_back(p); qr.emplace_back(p, 0);
            for (u64 x = 0; x < p; ++x) qr.back()[x * x % p] = 1;
        }
    }
};

int main(int argc, char **argv) {
    if (argc < 4) {
        cerr << "usage: scan X first_index last_index [qr_bound=101] [block=8388608]\n";
        return 2;
    }
    try {
        u64 X = stoull(argv[1]), first = stoull(argv[2]), last = stoull(argv[3]);
        u64 qr_bound = argc > 4 ? stoull(argv[4]) : 101;
        u64 block = argc > 5 ? stoull(argv[5]) : 8388608;
        auto t0 = Clock::now();
        Prefix pre = min25(X);
        cerr << "BASE X=" << X << " PI=" << pre.count << " SUMSQ=" << dec(pre.sumsq)
             << "\n" << flush;
        if (pre.count >= first || X < 2) {
            cerr << "ERROR: require X>=2 and pi(X)<first_index\n";
            return 3;
        }
        u64 root_limit = (u64)sqrtl((long double)(X + 60 * (last - pre.count) + 100000000)) + 100;
        vector<uint32_t> base = base_primes(root_limit);
        ResidueFilter filter(qr_bound);
        constexpr u64 RES_MOD = 3234846615ULL;
        u64 residue_sum = (u64)(pre.sumsq % RES_MOD);
        u64 idx = pre.count, tested = 0, survivors = 0, hits = 0, witness_max = 0;
        U sum = pre.sumsq;
        u64 last_prime = X;
        for (u64 lo = X + 1; idx < last; lo += block) {
            u64 hi = lo + block - 1, oddlo = lo | 1ULL;
            vector<uint8_t> composite((hi - oddlo) / 2 + 1, 0);
            for (u64 p : base) {
                if (p == 2) continue;
                if (p * p > hi) break;
                u64 first_multiple = max(p * p, ((oddlo + p - 1) / p) * p);
                if (!(first_multiple & 1)) first_multiple += p;
                for (u64 y = first_multiple; y <= hi; y += 2 * p)
                    composite[(y - oddlo) / 2] = 1;
            }
            for (size_t j = 0; j < composite.size() && idx < last; ++j) {
                if (composite[j]) continue;
                u64 z = oddlo + 2 * j;
                if (z < 3) continue;
                ++idx; sum += U(z) * z; last_prime = z;
                u64 zr = z % RES_MOD;
                residue_sum = (residue_sum + zr * zr % RES_MOD) % RES_MOD;
                if (idx < first) continue;
                ++tested;
                bool pass = true; u64 witness = 0;
                u64 compact = (8 * residue_sum + 1) % RES_MOD;
                for (size_t k = 0; k < filter.mod.size(); ++k) {
                    u64 r = filter.mod[k] <= 29 ? compact % filter.mod[k]
                                                 : (u64)((8 * sum + 1) % filter.mod[k]);
                    if (!filter.qr[k][r]) { pass = false; witness = filter.mod[k]; break; }
                }
                if (!pass) { witness_max = max(witness_max, witness); continue; }
                ++survivors;
                U n = 8 * sum + 1;
                if (exact_square(n)) {
                    U r = (U)sqrtl((long double)n);
                    while (r * r > n) --r;
                    while ((r + 1) * (r + 1) <= n) ++r;
                    cout << "NEW_SOL idx=" << idx << " prime=" << z
                         << " m=" << dec((r - 1) / 2) << " sum=" << dec(sum) << "\n" << flush;
                    ++hits;
                }
            }
        }
        cout << "RESULT first=" << first << " last=" << last << " tested=" << tested
             << " mod_pass=" << survivors << " true_hits=" << hits
             << " largest_witness=" << witness_max << " lastprime=" << last_prime
             << " end_sumsq=" << dec(sum)
             << " seconds=" << chrono::duration<double>(Clock::now() - t0).count() << "\n";
    } catch (const exception &e) { cerr << "ERROR: " << e.what() << "\n"; return 4; }
}