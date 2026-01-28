#include<bits/stdc++.h>
using namespace std;
typedef long long ll;
const int dx[]={1,-1,0,0},dy[]={0,0,1,-1};
class Rand{
    public:
    virtual double ra()=0;
};
class Plan: public Rand{
    public:
    std::discrete_distribution<> distribution;      
    std::mt19937 gen;                                
    vector<double> p,v;
    double ans;
    Plan(){ans=1e18;}
    Plan(vector<double> _p,vector<double> _v):p(_p),v(_v)
    { 
        assert(p.size()==v.size()); 
        double sum_p=0;
        for(int i=0;i<p.size();++i) sum_p+=p[i],assert(p[i]>=0&&p[i]<=1);
        for(int i=0;i<p.size();++i) p[i]/=sum_p;
        ans=1e18; 
    }
    void init(){
        distribution=discrete_distribution<>(p.begin(), p.end());
        std::random_device rd;
        gen = std::mt19937(rd());
    }
    void normalize(double std){
        for(int i=0;i<v.size();++i) v[i]/=std;
    }
    void print(FILE* f=stderr){
        for(int i=0;i<p.size();++i)
            fprintf(f,"(%.10lf,%.10lf)",p[i],v[i]);
        fprintf(f,"\n");
        //printf("{%.5lf}\n",ans);
    }
    double ra() override{
        int idx = distribution(gen);
        return v[idx];
    }
};
double a[1005][2005],e[2][1005][2005];
pair<int,int> pre[1005][2005];
void attempt(Rand* myrand){
    int n=1000;
    for(ll i=0;i<=n;++i){
        for(int j=0;j<=n+n;++j){
            a[i][j]=1e18;
            e[0][i][j]=myrand->ra();
            e[1][i][j]=myrand->ra();
        }
    }
    a[0][n]=0; 
    priority_queue<pair<double,pair<int,int>>> q;
    q.push(make_pair(0,make_pair(0,n)));
    while(!q.empty()){
        pair<double,pair<int,int>> k=q.top(); q.pop();
        int x=k.second.first,y=k.second.second;
        if(x==n&&y==n+n/10) break;
        //if(y!=n)fprintf(stderr,"<(%d,%d),%.5lf>",x,y,a[x][y]);
        if(a[x][y]!=-k.first) continue;
        for(int asp=0;asp<4;++asp){
            int x1=x+dx[asp],y1=y+dy[asp];
            if(x1<0||x1>n||y1<0||y1>n+n) continue;
            double d=a[x][y]+e[int(x!=x1)][min(x,x1)][min(y,y1)];
            if(d<a[x1][y1]){
                pre[x1][y1]=make_pair(x,y);
                a[x1][y1]=d;
                q.push(make_pair(-d,make_pair(x1,y1)));
            }
        }
    }
    FILE* f=fopen("output_path.txt","w");
    int x=n,y=n+n/10;
    while(!(x==0&&y==n)){
        //fprintf(stderr,"{(%d,%d),%.5lf,%.5lf,%.5lf}",x,y,a[x][y],a[x][y-1],e[int(x!=x)][x][y-1]);
        fprintf(f,"(%d,%d),",x,y-n);
        pair<int,int> k=pre[x][y];
        x=k.first,y=k.second;
    }
    fprintf(f,"(%d,%d)",0,0);
}
void set_range(double& x,double left,double right){
    if(x<left) x=left;
    if(x>right) x=right;
}

int main(){
        srand(time(0));
    Plan pl=Plan({0.44273,0.55727},{0.41223,4.73269}); pl.init();
    attempt(static_cast<Rand*>(&pl));
    return 0;
}
