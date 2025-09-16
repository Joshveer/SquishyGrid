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
const int N=30000*2010;
constexpr ll get_arr_sz(ll n,ll width){
    ll ret=1;
    while(ret<2ll*width*n) ret<<=1;
    return ret;
}
ll n=20000,width=1000;
pair<ll,ll> global_range;
ll gid_gl(ll i,ll j){ return 1ll*(i+j)*(i+j+1)/2+j; }
int gid_ar(ll i,ll j){ return int(gid_gl(i,j)%(2*width*n)); }
double a[N],e[2][N];
bool vis[N];
void shift(Rand* myrand){
    global_range.first+=2*n;
    global_range.second+=2*n;
    for(ll gl_id=global_range.second-2*n;gl_id<global_range.second;++gl_id){
        ll ar_id=gl_id%(2*width*n);
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
vector<pair<pair<int,int>,double>> attempt(Rand* myrand){
    global_range=make_pair(0,2*n*width);
    for(ll i=0;i<2*n*width;++i){
        a[i]=1e18; vis[i]=0;
        e[0][i]=myrand->ra();
        e[1][i]=myrand->ra();
    }
    q.init();
    a[gid_ar(width,width)]=0; q.push(make_pair(width,width));
    vis[gid_ar(width,width)]=1;
    vector<pair<pair<int,int>,double>> ans;
    FILE* f=fopen("output_ball.txt","w");
    while(global_range.second<=2ll*n*n){
        std::cerr<<"{New global Range:}"<<global_range.first<<","<<global_range.second<<"{End Point:}"<<2ll*n*n<<std::endl;
        while(!q.empty()){
            pair<int,int> idx=q.extract_front(); 
            int i=idx.first,j=idx.second; vis[gid_ar(i,j)]=0;
            
            double dist=sqrtl(1ll*(i-width)*(i-width)+1ll*(j-width)*(j-width)),angle=1.0*j/i;
            //printf("(%d %d)[%.5lf,%.5lf]\n",i,j,a[gid_ar(i,j)],a[gid_ar(i,j)]/dist);
            if(fabs(dist-(n-width))<=1&&i>=width&&j>=width){
            	ans.push_back(make_pair(make_pair(i,j),a[gid_ar(i,j)]/dist));
            }
            for(int asp=0;asp<4;++asp){
                int x=i+dx[asp],y=j+dy[asp];
                if(x>=0&&y>=0&&gid_gl(x,y)>=global_range.first
                    &&gid_gl(x,y)<global_range.second){
                    double d=a[gid_ar(i,j)]
                        +e[int(i!=x)][gid_ar(min(i,x),min(j,y))];
                    if(a[gid_ar(x,y)]>d){
                        a[gid_ar(x,y)]=d;
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
        ll x=ll(sqrt(2ll*(global_range.second-2*n)))-2,y=0;
        for(ll gl_id=global_range.second-2*n;gl_id<global_range.second;++gl_id){
            while(gl_id>=(x+y+1)*(x+y+2)/2) x=x+y+1,y=0;
            ll shift_xy=gl_id-(x+y)*(x+y+1)/2-y;
            y+=shift_xy,x-=shift_xy;
            for(int asp=0;asp<2;++asp){
                ll x1=x+dx[asp*2+1],y1=y+dy[asp*2+1];
                if(x1>=0&&y1>=0
                   &&gid_gl(x1,y1)>=global_range.first
                   &&gid_gl(x1,y1)<global_range.second){
                      a[gid_ar(x,y)]=min(a[gid_ar(x,y)]
                        ,a[gid_ar(x1,y1)]+e[int(x!=x1)][gid_ar(x1,y1)]);
                }
            }
            //printf("[%lld,%lld]\n",x,y);
            //assert(a[gid_ar(x,y)]<1e9);
            q.push(make_pair(x,y)); vis[gid_ar(x,y)]=1;
        }
    }
    sort(ans.begin(),ans.end());
    int as_j=0;
    for(int as_i=0;as_i<ans.size();++as_i){
    	if(ans[as_i].first==ans[as_i-1].first);
    	else ans[as_j++]=ans[as_i];
    }
    ans.erase(ans.begin()+as_j,ans.end());
    return ans;
}
vector<pair<pair<int,int>,double>> estimate(Plan& r1){
    vector<pair<pair<int,int>,double>> sum_ans;
    int num=20; r1.init();
    for(int i=0;i<num;++i){
    	vector<pair<pair<int,int>,double> > ans=attempt(static_cast<Rand*>(&r1));
    	if(i==0) sum_ans=ans;
    	else{
    	    for(int j=0;j<ans.size();++j){
    	    	assert(ans[j].first==sum_ans[j].first);
    	    	sum_ans[j].second+=ans[j].second;
    	    }
	}
    }
    for(int i=0;i<sum_ans.size();++i) sum_ans[i].second/=num;
    return sum_ans;
}

void set_range(double& x,double left,double right){
    if(x<left) x=left;
    if(x>right) x=right;
}

int main(){
    FILE* f=fopen("output_ball.txt","w");
        srand(time(0));
    Plan pl=Plan({0.23575,0.24575,0.26821,0.25029},{0.0313151204,0.7925472392,14.7165582951,31.0208181649});
    vector<pair<pair<int,int>,double>> ans=estimate(pl);
    for(int i=0;i<ans.size();++i){
    	//printf("{(%d,%d),%.5lf}",ans[i].first.first,ans[i].first.second,ans[i].second);
        double len=ans[i].second/ans[0].second;
    	double angle=atan2(ans[i].first.second-width,ans[i].first.first-width);
    	//printf("(%.5lf,%.5lf)",len,angle);
        fprintf(f,"(%.6lf,%.6lf),",len*cos(angle),len*sin(angle));

    }
    return 0;
}
