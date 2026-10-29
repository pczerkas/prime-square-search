#include <bits/stdc++.h>
using namespace std; using u64=uint64_t; using U=unsigned __int128;
static string dec(U x){if(!x)return"0";string s;while(x){s+=char('0'+x%10);x/=10;}reverse(s.begin(),s.end());return s;}
struct V{u64 pi;U s;};
static V calc(u64 n){
 u64 r=sqrtl((long double)n); vector<u64>a,lo(r+2),hi(r+2);
 for(u64 x=1;x<=n;){u64 q=n/x,e=n/q,id=a.size();a.push_back(q);if(q<=r)lo[q]=id;else hi[n/q]=id;x=e+1;}
 vector<u64> c(a.size());vector<U>s(a.size());for(size_t i=0;i<a.size();++i){U x=a[i];c[i]=a[i]-1;s[i]=x*(x+1)*(2*x+1)/6-1;}
 auto id=[&](u64 x){return x<=r?lo[x]:hi[n/x];};vector<char>b(r+1);vector<u64>p;
 for(u64 x=2;x<=r;++x)if(!b[x]){p.push_back(x);if(x*x<=r)for(u64 y=x*x;y<=r;y+=x)b[y]=1;}
 u64 pc=0;U ps=0;for(u64 x:p){if(x*x>n)break;u64 x2=x*x;for(size_t i=0;i<a.size()&&a[i]>=x2;++i){auto j=id(a[i]/x);c[i]-=c[j]-pc;s[i]-=U(x2)*(s[j]-ps);}++pc;ps+=x2;}return{c[0],s[0]};
}
int main(int ac,char**av){if(ac!=2&&ac!=3){cerr<<"usage: prefix_checkpoint PRIME [EXPECTED_PI]\n";return 2;}try{u64 p=stoull(av[1]);V v=calc(p);if(ac==3&&v.pi!=stoull(av[2])){cerr<<"PI mismatch: "<<v.pi<<"\n";return 3;}cout<<"PI="<<v.pi<<" SUMSQ="<<dec(v.s)<<"\n";}catch(exception&e){cerr<<e.what()<<"\n";return 4;}}