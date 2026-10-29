#!/usr/bin/env python3
"""Run the complete n=0..100,000,000,000 search on four worker processes."""
import argparse, csv, pathlib, subprocess, concurrent.futures

ap=argparse.ArgumentParser();ap.add_argument('--exe',default='./prime_square_scan');ap.add_argument('--starts',default='starts.csv');ap.add_argument('--out',default='results');ap.add_argument('--workers',type=int,default=4);args=ap.parse_args()
out=pathlib.Path(args.out);out.mkdir(exist_ok=True)
rows=list(csv.DictReader(open(args.starts)))
if len(rows)!=4: raise SystemExit('starts.csv must contain four lanes')
def run(r):
    log=out/f"{r['first']}_{r['last']}.log"; prog=out/f"{r['first']}_{r['last']}.progress.log"
    with log.open('w') as a, prog.open('w') as e:
        rc=subprocess.run([args.exe,r['X'],r['first'],r['last'],'101','8388608'],stdout=a,stderr=e).returncode
    if rc: raise RuntimeError(f"lane failed: {r['lane']}")
    lines=[x for x in log.read_text().splitlines() if x.startswith('RESULT ')]
    if len(lines)!=1: raise RuntimeError(f"missing RESULT in {log}")
    return int(r['lane']),lines[0]
with concurrent.futures.ThreadPoolExecutor(max_workers=args.workers) as pool:
    results=list(pool.map(run,rows))
with open(out/'summary.txt','w') as f:
    f.write('n=0 S=0 m=0 (trivial solution)\n')
    f.write('n=1 S=4: 8*S+1=33, not a square\n')
    for lane,line in sorted(results): f.write(line+'\n')
print(f'completed {len(results)} lanes; bootstrap n=0,1 recorded in {out}/summary.txt')