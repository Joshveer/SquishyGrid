#include<bits/stdc++.h>
using namespace std;
typedef long long ll;
const int dx[]={1,-1,0,0},dy[]={0,0,1,-1};

class Rand{
    public:
    virtual double ra()=0;
};

class PolygonWeights: public Rand{
    public:
    vector<vector<double>> weights_h, weights_v;
    int grid_radius;
    int center_x, center_y;
    
    PolygonWeights(string filename, int radius=80){
        grid_radius = radius;
        center_x = center_y = grid_radius;
        load_weights(filename);
    }
    
    void load_weights(string filename){
        ifstream file(filename);
        if(!file.is_open()){
            cerr << "Error: Could not open " << filename << endl;
            exit(1);
        }
        
        string line;
        // Skip header lines
        while(getline(file, line) && line[0] == '#') {}
        
        // Read horizontal weights
        weights_h.resize(2*grid_radius+1, vector<double>(2*grid_radius));
        for(int i = 0; i < 2*grid_radius+1; i++){
            for(int j = 0; j < 2*grid_radius; j++){
                file >> weights_h[i][j];
            }
        }
        
        // Skip empty line
        getline(file, line);
        while(getline(file, line) && line[0] == '#') {}
        
        // Read vertical weights
        weights_v.resize(2*grid_radius, vector<double>(2*grid_radius+1));
        for(int i = 0; i < 2*grid_radius; i++){
            for(int j = 0; j < 2*grid_radius+1; j++){
                file >> weights_v[i][j];
            }
        }
        
        file.close();
        cout << "Loaded polygon weights from " << filename << endl;
    }
    
    double ra() override{
        // This shouldn't be called for polygon weights
        return 1.0;
    }
    
    double get_weight(int x1, int y1, int x2, int y2){
        // Convert to array indices
        int i1 = y1 + center_y;
        int j1 = x1 + center_x;
        int i2 = y2 + center_y;
        int j2 = x2 + center_x;
        
        // Check bounds
        if(i1 < 0 || i1 >= 2*grid_radius+1 || j1 < 0 || j1 >= 2*grid_radius+1) return 1.0;
        if(i2 < 0 || i2 >= 2*grid_radius+1 || j2 < 0 || j2 >= 2*grid_radius+1) return 1.0;
        
        // Determine edge type and return weight
        if(y2 == y1 && x2 == x1 + 1){ // horizontal edge (right)
            if(j1 < 2*grid_radius) return weights_h[i1][j1];
        }
        else if(y2 == y1 && x2 == x1 - 1){ // horizontal edge (left)
            if(j2 < 2*grid_radius) return weights_h[i2][j2];
        }
        else if(x2 == x1 && y2 == y1 + 1){ // vertical edge (up)
            if(i1 < 2*grid_radius) return weights_v[i1][j1];
        }
        else if(x2 == x1 && y2 == y1 - 1){ // vertical edge (down)
            if(i2 < 2*grid_radius) return weights_v[i2][j2];
        }
        
        return 1.0; // default weight
    }
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
ll n=30000,width=1000;
pair<ll,ll> global_range;
ll gid_gl(ll i,ll j){ return 1ll*(i+j)*(i+j+1)/2+j; }
int gid_ar(ll i,ll j){ return int(gid_gl(i,j)%(2*width*n)); }
double a[N],e[2][N];

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

void shift_polygon(PolygonWeights* poly_weights){
    global_range.first+=2*n;
    global_range.second+=2*n;
    for(ll gl_id=global_range.second-2*n;gl_id<global_range.second;++gl_id){
        ll ar_id=gl_id%(2*width*n);
        a[ar_id]=1e18;
        // Use polygon weights instead of random
        pair<int,int> pos = gl_to_xy(gl_id);
        e[0][ar_id] = poly_weights->get_weight(pos.first, pos.second, pos.first+1, pos.second);
        e[1][ar_id] = poly_weights->get_weight(pos.first, pos.second, pos.first, pos.second+1);
    }
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
};

MyQueue q;

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
    FILE* f=fopen("output_ball.txt","w");
    while(global_range.second<=2ll*n*n){
        std::cerr<<"{New global Range:}"<<global_range.first<<","<<global_range.second<<"{End Point:}"<<2ll*n*n<<std::endl;
        while(!q.empty()){
            pair<int,int> idx=gl_to_xy(ar_to_gl(q.extract_min())); 
            int i=idx.first,j=idx.second; 
            
            double dist=sqrtl(1ll*(i-width)*(i-width)+1ll*(j-width)*(j-width)),angle=1.0*j/i;
            if(fabs(dist-(n-width))<=1&&i>=width&&j>=width){
            	ans.push_back(make_pair(make_pair(i,j),a[gid_ar(i,j)]/dist));
            }
            for(int asp=0;asp<4;++asp){
                int x=i+dx[asp],y=j+dy[asp];
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
                    }
                }
            }
        }
        shift(myrand);
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

vector<pair<pair<int,int>,double>> attempt_polygon(PolygonWeights* poly_weights){
    global_range=make_pair(0,2*n*width);
    for(ll i=0;i<2*n*width;++i){
        a[i]=1e18; 
        pair<int,int> pos = gl_to_xy(i);
        e[0][i] = poly_weights->get_weight(pos.first, pos.second, pos.first+1, pos.second);
        e[1][i] = poly_weights->get_weight(pos.first, pos.second, pos.first, pos.second+1);
    }
    q.init(); assert(q.empty());
    a[gid_ar(width,width)]=0; q.push(gid_ar(width,width));
    vector<pair<pair<int,int>,double>> ans;
    FILE* f=fopen("output_ball_polygon.txt","w");
    while(global_range.second<=2ll*n*n){
        std::cerr<<"{New global Range:}"<<global_range.first<<","<<global_range.second<<"{End Point:}"<<2ll*n*n<<std::endl;
        while(!q.empty()){
            pair<int,int> idx=gl_to_xy(ar_to_gl(q.extract_min())); 
            int i=idx.first,j=idx.second; 
            
            double dist=sqrtl(1ll*(i-width)*(i-width)+1ll*(j-width)*(j-width)),angle=1.0*j/i;
            if(fabs(dist-(n-width))<=1&&i>=width&&j>=width){
            	ans.push_back(make_pair(make_pair(i,j),a[gid_ar(i,j)]/dist));
            }
            for(int asp=0;asp<4;++asp){
                int x=i+dx[asp],y=j+dy[asp];
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
                    }
                }
            }
        }
        shift_polygon(poly_weights);
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

int main(int argc, char* argv[]){
    if(argc < 2){
        cout << "Usage: " << argv[0] << " <polygon_type>" << endl;
        cout << "Available types: triangle, trapezoid, square" << endl;
        return 1;
    }
    
    string polygon_type = argv[1];
    string weight_file;
    
    if(polygon_type == "triangle"){
        weight_file = "triangle_weights.txt";
    } else if(polygon_type == "trapezoid"){
        weight_file = "trapezoid_weights.txt";
    } else if(polygon_type == "square"){
        weight_file = "square_weights.txt";
    } else {
        cout << "Unknown polygon type: " << polygon_type << endl;
        return 1;
    }
    
    cout << "Running FPP with " << polygon_type << " polygon weights..." << endl;
    
    PolygonWeights poly_weights(weight_file, 80);
    vector<pair<pair<int,int>,double>> ans = attempt_polygon(&poly_weights);
    
    FILE* f=fopen("output_ball_polygon.txt","w");
    for(int i=0;i<ans.size();++i){
        double len=ans[i].second/ans[0].second;
    	double angle=atan2(ans[i].first.second-width,ans[i].first.first-width);
        fprintf(f,"(%.6lf,%.6lf),",len*cos(angle),len*sin(angle));
    }
    fclose(f);
    
    cout << "Results written to output_ball_polygon.txt" << endl;
    return 0;
}
