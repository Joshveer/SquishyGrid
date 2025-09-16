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
const int N=5+3e7;
constexpr ll arr_size(ll width){ 
    ll len=1;
    while(len<=(2*width+1)*width) len<<=1;
    return len;
}
ll n=50000,width=2000; //,ar_sz=arr_size(width);
double _atan;
pair<ll,ll> global_range;
ll gid_gl(int i,int j){ return i*(2*width+1)+(j-int(i*_atan)+width); }
bool is_valid(int i,int j){ return i>=0&&i<=n&&j-int(i*_atan)>=-width&&j-ll(i*_atan)<=width; }
int gid_ar(ll i,ll j){ return int(gid_gl(i,j)%((2*width+1)*width)); }
double a[N],e[2][N];
struct Path_Record{
    int c0,c1,c2;
};
struct Path_Record2{
    double c0,c1,c2;
};
Path_Record pre[N];
bool vis[N];
void shift(Rand* myrand){
    global_range.first+=2*width+1;
    global_range.second+=2*width+1;
    for(ll gl_id=global_range.second-(2*width+1);gl_id<global_range.second;++gl_id){
        ll ar_id=gl_id%((2ll*width+1)*width);
        a[ar_id]=1e18;
        e[0][ar_id]=myrand->ra();
        e[1][ar_id]=myrand->ra();
    }
}
struct MyQueue{
    pair<int,int> q[N+5];
    int left,right;
    void init(){ left=right=0; }
    MyQueue(){ left=right=0; }
    bool empty(){ return left==right; }
    pair<int,int> extract_front(){
        pair<int,int> ret=q[left++];
        if(left>N) left=0;
        return ret;
    }
    void push(const pair<int,int>& key){
        q[right++]=key; if(right>N) right=0;
    }
};
MyQueue q;
    /* The range of ith two is based on int(i*atan). i.e., [int(i*atan)-width,int(i*atan)+width]
//void trans_path(Path_Record& dest,const Path_Record& source,Plan& plan,double e){
    dest=source;
    if(e==plan.v0) ++dest.c0;
    if(e==plan.v1) ++dest.c1;
    if(e==plan.v2) ++dest.c2;
}*/
double attempt(Rand* myrand,double __atan,Plan& plan){
    _atan=__atan;
    global_range=make_pair(0,width*(2*width+1));
    for(int gl_id=0;gl_id<width*(2*width+1);++gl_id){
   	 a[gl_id]=1e18; vis[gl_id]=0;
   	 e[0][gl_id]=myrand->ra();
   	 e[1][gl_id]=myrand->ra();
    }
    
    q.init();
    a[gid_ar(0,0)]=0; //pre[gid_ar(0,0)]=(Path_Record){0,0,0};
    q.push(make_pair(0,0));
    vis[gid_ar(0,0)]=1;
    double ans=0;
    //FILE* f=fopen("output_ball.txt","w");
    ll end_point=(2*width+1)*(n+5);
    while(global_range.second<=end_point){
        //std::cerr<<"{New global Range:}"<<global_range.first<<","<<global_range.second<<"{End Point:}"<<end_point<<std::endl;
        while(!q.empty()){
            pair<int,int> idx=q.extract_front(); 
            int i=idx.first,j=idx.second; vis[gid_ar(i,j)]=0;
            //printf("(%d,%d)[%.5lf]\n",i,j,a[gid_ar(i,j)]); 
            double dist=sqrtl(1ll*i*i+1ll*j*j),angle=1.0*j/i;
            if(fabs(j-int(i*_atan))<=1&&i==n){
                return a[gid_ar(i,j)]/dist;
                //ans=max({ans,a[gid_ar(i,j)]/dist});
                 
                
                //Path_Record path=pre[gid_ar(i,j)];
                //Path_Record2 ret;
                //ret.c0=1.0*path.c0/dist;
                //ret.c1=1.0*path.c1/dist;
                //ret.c2=1.0*path.c2/dist;
                //return ret;
            }
            for(int asp=0;asp<4;++asp){
                int x=i+dx[asp],y=j+dy[asp];
                if(is_valid(x,y)&&gid_gl(x,y)>=global_range.first
                    &&gid_gl(x,y)<global_range.second){
                    double e_val=e[int(i!=x)][gid_ar(min(i,x),min(j,y))];
                    double d=a[gid_ar(i,j)]+e_val;
                    if(a[gid_ar(x,y)]>d){
                        a[gid_ar(x,y)]=d;
                        //trans_path(pre[gid_ar(x,y)],pre[gid_ar(i,j)],plan,e_val);
                        if(!vis[gid_ar(x,y)]){
                            q.push(make_pair(x,y));
                            vis[gid_ar(x,y)]=1;
                        }
                    }
                }
            }
        }
        shift(myrand);
        //for(int i=0;i<2*width*n;++i) printf("<%d,%.5lf>",i,a[i]);
        ll x=global_range.second/(2*width+1)-1,y=int(x*_atan)-width;
        for(ll gl_id=global_range.second-(2*width+1);gl_id<global_range.second;++gl_id,++y){
            //printf("{%lld,%lld,%d,%.5lf}\n",x,y,gid_ar(x,y),a[gid_ar(x,y)]);
            //assert(y>=0&&x>=0&&gid_gl(x,y)==gl_id);
            for(int asp=0;asp<4;++asp){
                ll x1=x+dx[asp],y1=y+dy[asp];
                if(is_valid(x1,y1)
                   &&gid_gl(x1,y1)>=global_range.first
                   &&gid_gl(x1,y1)<global_range.second){
                      double e_val=e[int(x!=x1)][gid_ar(min(x,x1),min(y,y1))];
                      double d=a[gid_ar(x1,y1)]+e_val;
                      if(a[gid_ar(x,y)]>d){
                        a[gid_ar(x,y)]=d;
                        //trans_path(pre[gid_ar(x,y)],pre[gid_ar(x1,y1)],plan,e_val);
                      }
                }
            }
            q.push(make_pair(x,y)); vis[gid_ar(x,y)]=1;
        }
    }
}
//double calc_ans_from_path(Path_Record2 path,Plan plan){
 //   return plan.v0*path.c0+plan.v1*path.c1+plan.v2*path.c2;
//}
double estimate(Plan& r1){
    r1.init();
    int num=5;
    double ans1=0,ans2=0;
    for(int i=1;i<=num;++i){
        ans1+=attempt(static_cast<Rand*>(&r1),0,r1);
        ans2+=attempt(static_cast<Rand*>(&r1),1,r1);
    }
    ans1/=num; ans2/=num;
    double middle=sqrtl(ans1*ans2);
    r1.print(); fprintf(stderr,"{0d: %.5lf,45d: %.5lf}",ans1,ans2);
    r1.normalize(middle);
    r1.print();
    ans1=ans1/middle;
    ans2=ans2/middle;
    return ans2/ans1;
}

void set_range(double& x,double left,double right){
    if(x<left) x=left;
    if(x>right) x=right;
}
Plan adjust_values(vector<double> p){
    vector<double> v(p.size());
    for(int i=0;i<v.size();++i) v[i]=10.0*rand()/RAND_MAX;
    sort(v.begin(),v.end());
    Plan pl(p,v);
    double temp=0.1,cooling_rate=1.01; n=10000,width=300;
    for(int i=1;i<=100;++i){
        Plan pl1=pl;
        for(int i=0;i<v.size();++i){
            pl1.v[i]=pl1.v[i]+temp*rand()/RAND_MAX-temp/2;
            set_range(pl1.v[i],0,100);
        }
        sort(pl1.v.begin(),pl1.v.end());
        pl1.ans=estimate(pl1);
        if(pl1.ans<pl.ans) pl=pl1;
        fprintf(stderr,"Best: "); pl.print(); fprintf(stderr,"{%.5lf}",pl.ans);
        if(i%5==0) temp/=cooling_rate;
        if(i==50) n=50000,width=800,pl.ans=estimate(pl);
    }
    return pl;
}
double estimate_2(vector<double> p,vector<double> v){
    Plan pl(p,v);
    return estimate(pl);
}
int main() {
    srand(time(0));
    printf("{%.6lf}",estimate_2({0.23575,0.24575,0.26821,0.25029},{0.03135,0.79343,14.73295,31.05537}));
    return 0;
    vector<double> p({0.3,0.3,0.4});
    Plan best; double temp=0.2,cooling_rate=1.01; 
    for(int i=1;i<=200;++i){
        vector<double> p1(p.size());
        double sum_p1=0;
        for(int i=0;i<p1.size();++i){
            p1[i]=p[i]+temp*rand()/RAND_MAX;
            sum_p1+=p1[i];
        }
        for(int i=0;i<p1.size();++i){
            p1[i]/=sum_p1;
            fprintf(stderr,"<%.5lf>",p1[i]);
        }
        fprintf(stderr,"\n");
        Plan pl=adjust_values(p1);
        FILE* f=fopen("record.txt","a+");
        pl.print(f); fprintf(f,"{%.5lf}\n",pl.ans);
        fclose(f);
        if(pl.ans<best.ans) best=pl;
        if(i%10==0) temp/=cooling_rate;
    }
    fprintf(stderr,"Best:");
    best.print();
    return 0;
}
