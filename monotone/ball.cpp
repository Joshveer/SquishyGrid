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
ll n=3000,width=1000;
pair<ll,ll> global_range;
ll gid_gl(ll i,ll j){ return 1ll*(i+j)*(i+j+1)/2+j; }
int gid_ar(ll i,ll j){ return int(gid_gl(i,j)%(2*width*n)); }
double a[N],e[2][N];
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
inline ll ar_to_gl(int ar_id){
    ll ret=global_range.first-global_range.first%(2*width*n)+ar_id;
    if(ret<global_range.first) ret+=2*width*n;
    if(ret>=global_range.second) ret-=2*width*n;
    return ret;
}
inline pair<int,int> gl_to_xy(ll gl_id){
    ll x=ll(sqrt(2ll*(gl_id)))-2,y=0;
    while(gl_id>=(x+y+1)*(x+y+2)/2) x=x+y+1,y=0;
    ll shift_xy=gl_id-(x+y)*(x+y+1)/2-y;
    y+=shift_xy,x-=shift_xy;
    return make_pair((int)x,(int)y);
}
struct MyQueue{
    int q[N];
    int pos[N],n;
    void init(){ n=0; memset(pos,-1,sizeof(pos)); }
    MyQueue(){ n=0; memset(pos,-1,sizeof(pos)); }
    void node_swap(int x,int y){ swap(q[x],q[y]); swap(pos[q[x]],pos[q[y]]); }
    bool empty(){ return n==0; }
    bool in_queue(int ar_id){ return pos[ar_id]!=-1; }
    void dec_key(int ar_id){
        //printf("(Dec-Key: %.5lf,%d)",a[ar_id],pos[ar_id]);
        int x=pos[ar_id];
        for(;x>1&&a[q[x]]<a[q[x>>1]];x>>=1) node_swap(x,x>>1);
    }
    void push_down(int x){
        for(int x1=x+x;x1<=n;x=x1,x1=x+x){
            if(x1+1<=n&&a[q[x1+1]]<a[q[x1]]) x1+=1;
            if(a[q[x]]>a[q[x1]]) node_swap(x,x1);
            else break;
        }
    }
    int extract_min(){
        int ret=q[1]; 
        node_swap(1,n); n--;
        pos[ret]=-1; 
        push_down(1);
        return ret;
    }
    void push(int ar_id){
        ++n;
        q[n]=ar_id; 
        pos[ar_id]=n;
        dec_key(ar_id);
    }
    void check(){
        for(int i=1;i<=n;++i){
            pair<int,int> id_xy=gl_to_xy(ar_to_gl(q[i]));
            printf("{<%d,%d>,%.5lf}",id_xy.first,id_xy.second,a[q[i]]);
        } puts("");
        for(int i=2;i<=n;++i) assert(a[q[i]]>=a[q[i>>1]]);
    }
};
MyQueue q;
vector<pair<int,int>> attempt_ball(Rand* myrand){
    global_range=make_pair(0,2*n*width);
    for(ll i=0;i<2*n*width;++i){
        a[i]=1e18; 
        e[0][i]=myrand->ra();
        e[1][i]=myrand->ra();
    }
    q.init(); assert(q.empty());
    a[gid_ar(width,width)]=0; q.push(gid_ar(width,width));
    vector<pair<int,int>> ans;
    while(global_range.second<=3ll*n*n){
        std::cerr<<"{New global Range:}"<<global_range.first<<","<<global_range.second<<"{End Point:}"<<3ll*n*n<<std::endl;
        while(!q.empty()){
            pair<int,int> idx=gl_to_xy(ar_to_gl(q.extract_min())); 
            int i=idx.first,j=idx.second; 
            
            double dist=sqrtl(1ll*(i-width)*(i-width)+1ll*(j-width)*(j-width)),angle=1.0*j/i;
            //printf("(%d %d)[%.5lf,%.5lf]\n",i,j,a[gid_ar(i,j)],a[gid_ar(i,j)]/dist);
            if(a[gid_ar(i,j)]>=n-width){
            	if(i>=width&&j>=width)
                    ans.push_back(make_pair(i-width,j-width));
                continue;
            }
            for(int asp=0;asp<2;++asp){
                int x=i+dx[asp*2],y=j+dy[asp*2];
                int ar_id_xy=gid_ar(x,y);
                if(x>=0&&y>=0&&gid_gl(x,y)>=global_range.first
                    &&gid_gl(x,y)<global_range.second){
                    double d=a[gid_ar(i,j)]
                        +e[int(i!=x)][gid_ar(min(i,x),min(j,y))];
                    if(a[ar_id_xy]>d){
                        a[ar_id_xy]=d;
                        if(q.in_queue(ar_id_xy)){
                            q.dec_key(ar_id_xy);
                        }else q.push(ar_id_xy);
                        //q.check();
                    }
                }
            }
        }
        shift(myrand);
        //for(int i=0;i<2*width*n;++i) printf("<%d,%.5lf>",i,a[i]);
        pair<int,int> id_xy=gl_to_xy(global_range.second-2*n);
        ll x=id_xy.first,y=id_xy.second;
        for(ll gl_id=global_range.second-2*n;gl_id<global_range.second;++gl_id){
            while(gl_id>=(x+y+1)*(x+y+2)/2) x=x+y+1,y=0;
            ll shift_xy=gl_id-(x+y)*(x+y+1)/2-y;
            y+=shift_xy,x-=shift_xy;
            for(int asp=0;asp<2;++asp){
                ll x1=x+dx[asp*2+1],y1=y+dy[asp*2+1];
                if(x1>=0&&y1>=0
                   &&gid_gl(x1,y1)>=global_range.first
                   &&gid_gl(x1,y1)<global_range.second){
                    if(a[gid_ar(x1,y1)]<n-width){
                      a[gid_ar(x,y)]=min(a[gid_ar(x,y)]
                        ,a[gid_ar(x1,y1)]+e[int(x!=x1)][gid_ar(x1,y1)]);
                    }
                }
            }
            //printf("[%lld,%lld]\n",x,y);
            //assert(a[gid_ar(x,y)]<1e9);
            if(a[gid_ar(x,y)]<1e18) q.push(gid_ar(x,y));
        }
    }
    sort(ans.begin(),ans.end());
    ans.erase(unique(ans.begin(),ans.end()),ans.end());
    return ans;
}
vector<pair<pair<int,int>,double>> attempt(Rand* myrand){
    global_range=make_pair(0,2*n*width);
    for(ll i=0;i<2*n*width;++i){
        a[i]=1e18;
        e[0][i]=myrand->ra();
        e[1][i]=myrand->ra();
    }
    q.init(); assert(q.empty());
    a[gid_ar(width,width)]=0; q.push(gid_ar(width,width));
    vector<pair<pair<int,int>,double>> ans;
    //FILE* f=fopen("output_ball.txt","w");
    while(global_range.second<=2ll*n*n){
        std::cerr<<"{New global Range:}"<<global_range.first<<","<<global_range.second<<"{End Point:}"<<2ll*n*n<<std::endl;
        while(!q.empty()){
            pair<int,int> idx=gl_to_xy(ar_to_gl(q.extract_min()));
            int i=idx.first,j=idx.second;

            double dist=sqrtl(1ll*(i-width)*(i-width)+1ll*(j-width)*(j-width)),angle=1.0*j/i;
            //printf("(%d %d)[%.5lf,%.5lf]\n",i,j,a[gid_ar(i,j)],a[gid_ar(i,j)]/dist);
            if(fabs(dist-(n-width))<=1&&i>=width&&j>=width){
            	ans.push_back(make_pair(make_pair(i,j),a[gid_ar(i,j)]/dist));
            }
            for(int asp=0;asp<2;++asp){
                int x=i+dx[asp*2],y=j+dy[asp*2];
                int ar_id_xy=gid_ar(x,y);
                if(x>=0&&y>=0&&gid_gl(x,y)>=global_range.first
                    &&gid_gl(x,y)<global_range.second){
                    double d=a[gid_ar(i,j)]
                        +e[int(i!=x)][gid_ar(min(i,x),min(j,y))];
                    if(a[ar_id_xy]>d){
                        a[ar_id_xy]=d;
                        if(q.in_queue(ar_id_xy)){
                            q.dec_key(ar_id_xy);
                        }else q.push(ar_id_xy);
                        //q.check();
                    }
                }
            }
        }
        shift(myrand);
        //for(int i=0;i<2*width*n;++i) printf("<%d,%.5lf>",i,a[i]);
        pair<int,int> id_xy=gl_to_xy(global_range.second-2*n);
        ll x=id_xy.first,y=id_xy.second;
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
            q.push(gid_ar(x,y));
        }
    }
    sort(ans.begin(),ans.end());
    int as_j=0;
    for(int as_i=0;as_i<ans.size();++as_i){
    	if(as_i>0&&ans[as_i].first==ans[as_i-1].first);
    	else ans[as_j++]=ans[as_i];
    }
    ans.erase(ans.begin()+as_j,ans.end());
    return ans;
}
vector<pair<pair<int,int>,double>> estimate(Plan& r1){
    vector<pair<pair<int,int>,double>> sum_ans;
    int num=1; r1.init();
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
    FILE* f_angle_len=fopen("output_angle_len.txt","w");
    FILE* f_ball=fopen("output_ball.txt","w");
        srand(time(0));
    Plan pl=Plan({0.66,0.34},{1.0/sqrt(2),(1-0.66/sqrt(2))/0.34});
    
    if(1){
        pl.init();
        vector<pair<int,int>> ans=attempt_ball(static_cast<Rand*>(&pl));
        
        assert(ans.size()>0);
        double len=sqrtl(1.0*ans[0].first*ans[0].first+1.0*ans[0].second*ans[0].second);
        for(int i=0;i<ans.size();++i){
            fprintf(stderr,"AA");
    	    fprintf(f_ball,"(%.6lf,%.6lf),",1.0*ans[i].first/len,1.0*ans[i].second/len);
        }
        fclose(f_ball);
    }
    //return 0;
    if(1){
        vector<pair<pair<int,int>,double>> ans=estimate(pl);
        for(int i=0;i<ans.size();++i){
            double len=ans[i].second/ans[0].second;
    	    double angle=atan2(ans[i].first.second-width,ans[i].first.first-width);
            fprintf(f_angle_len,"(%.6lf,%.6lf),",angle,len);

            //printf("(%.6lf,%.6lf),",angle,ans[i].second);
        }
        fclose(f_angle_len);
    }

    return 0;
}
