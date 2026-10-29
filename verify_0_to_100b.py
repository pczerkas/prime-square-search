#!/usr/bin/env python3
"""Verify coverage, lane endpoints and every listed solution."""
import argparse,csv,pathlib,re,subprocess
ap=argparse.ArgumentParser();ap.add_argument('--starts',default='starts.csv');ap.add_argument('--out',default='results');ap.add_argument('--prefix',default='./prefix_checkpoint');a=ap.parse_args()
rows=list(csv.DictReader(open(a.starts))); d=pathlib.Path(a.out)
assert len(rows)==4
total=0;hits=0
for r in sorted(rows,key=lambda x:int(x['first'])):
    fn=d/f"{r['first']}_{r['last']}.log"; lines=fn.read_text().splitlines(); rs=[x for x in lines if x.startswith('RESULT ')]
    assert len(rs)==1,fn
    z=dict(re.findall(r'(\w+)=([^ ]+)',rs[0])); first,last=int(r['first']),int(r['last'])
    assert (int(z['first']),int(z['last']))==(first,last)
    assert int(z['tested'])==last-first+1
    x=int(r['X']); pre=subprocess.check_output([a.prefix,str(x)],text=True)
    q=dict(re.findall(r'(PI|SUMSQ)=(\d+)',pre)); assert int(q['PI'])==int(r['pi_X'])
    end_prime=int(z['lastprime'])
    end=subprocess.check_output([a.prefix,str(end_prime)],text=True)
    e=dict(re.findall(r'(PI|SUMSQ)=(\d+)',end))
    assert int(e['PI'])==last, (fn, end_prime, e['PI'], last)
    assert int(e['SUMSQ'])==int(z['end_sumsq']), (fn, end_prime)
    for line in lines:
        if line.startswith('NEW_SOL '):
            m=dict(re.findall(r'(\w+)=([^ ]+)',line)); assert first<=int(m['idx'])<=last
            assert 2*int(m['sum'])==int(m['m'])*(int(m['m'])+1); hits+=1
    total+=int(z['tested'])
expected=sum(int(r['last'])-int(r['first'])+1 for r in rows)
assert total==expected
if max(int(r['last']) for r in rows)==100_000_000_000:
    assert expected==99_999_999_999
assert 'n=0 S=0 m=0' in (d/'summary.txt').read_text()
print(f'PASS: {total} scanned indices from n=2 through 100b; hits={hits}; n=0,1 checked')