//Author : Ritik Malaviya
//Date created : 14/6/21
//File name : S.cpp
//File type : cpp

#include<bits/stdc++.h>
using namespace std;

int t1 = 0; // Global variable denoting time.


struct dat{
	int T,V;
	string K;
	dat(string x, int y, int z) : K(x), T(y), V(z){

	}
	bool operator < (dat const&  other) const {
		return (K < other.K);
	}
};

struct cmp{
		bool operator ()(dat const& fi, dat const& se) const{
		if(fi.K < se.K)
			return true;
		else if(fi.K > se.K)
			return false;
		else if(se.K > fi.K)
			return true;
		else if(se.K < fi.K)
			return false;
		else if(fi.K == se.K){
			if(fi.T < se.T)
				return true;
			else
				return false;
		}
		else
			return true;
	}
};

vector<dat> merge(vector<dat> d1, vector<dat> d2){
	vector<dat> ret;
	for(int i=0;i<(int)d2.size();++i){
		d1.push_back(d2[i]);
	}
	sort(d1.begin(),d1.end(),cmp());
	for(int i=0;i<(int)d1.size();++i){
		string now = d1[i].K;
		int mx = 0;
		dat bundle = dat("GG",0,0);
		while(i < (int)d1.size() && d1[i].K == now){
			if(mx < d1[i].T){
				bundle = dat(now,d1[i].T,d1[i].V);
				mx = d1[i].T;
			}
			++i;
		}
		--i;
		ret.push_back(bundle);
	}
	return ret;
}
void compress(vector<vector<dat>> &segments, int L, map<string,map<int,int>> &h, int sz){
	vector<dat> now = segments[1];
	segments[1].clear();
	for(int i = 2; i <= L; ++i){
		now = merge(now, segments[i]);
		segments[i].clear();
	}
	segments[1].assign(now.begin(),now.end());
	int cnt = 0;
	bool ok = false;
	string last;
	for(auto [str, mp] : h){
		for(auto [v, t] : h[str]){
			if(cnt<sz/2){
				segments[2].push_back(dat(str,t,v));
				++cnt;
			}
			else{
				last = str;
				ok=true;
				break;
			}
		}
		if(ok)
			break;
	}
	int ind = -1;
	for(int i=0;i<(int)segments[2].size();++i){
		if(segments[2][i].K != last)
			h.erase(segments[2][i].K);
		else{
			ind = i;
			break;
		}
	}
	for(int i=ind;i<(int)segments[2].size();++i){
		h[last].erase(segments[2][i].V);
	}
	if((int)h[last].size() == 0)
		h.erase(last);
	
}
void update(vector<vector<dat>> &segments, int L, map<string,map<int,int>> &h,int sz,int ava){
	int cnt = 0;
	string last;
	bool ok = false;
	for(auto [str,mp] : h){
		for(auto [v, t] : h[str]){
			if(cnt<sz/2){
				segments[ava].push_back(dat(str,t,v));
				++cnt;
			}
			else{
				last = str;
				ok = true;
				break;
			}
		}
		if(ok)
			break;
	}
	int ind = -1;
	for(int i=0;i<(int)segments[ava].size();++i){
		if(segments[ava][i].K != last)
			h.erase(segments[ava][i].K);
		else{
			ind = i;
			break;
		}
	}
	for(int i=ind;i<(int)segments[ava].size();++i){
		h[last].erase(segments[ava][i].V);
	}
	if((int)h[last].size() == 0)
		h.erase(last);
}
dat search_m(map<string,map<int,int>> &h, string k){
	if(!h.count(k))
		return dat(k,-1,-1);
	dat ret = dat(k,-1,-1);
	int mx = 0;
	for(auto [v,t] : h[k]){
		if(mx < t){
			mx = t;
			ret = dat(k,t,v);
		}
	}
	return ret;
}
dat search_d(vector<vector<dat>> &segments, string k, int ava){
	int mx = 0;
	dat ret = dat(k,-1,-1);
	//Search by latest insertion order.
	//segments[1] -> <k,v>.....
	//segments[2] -> <k,v>....(sz/2)
	//segments[3] -> <k,v>....
	for(int i=ava-1;i>=1;--i){
		int p1 = lower_bound(segments[i].begin(),segments[i].end(),dat(k,-1,-1)) - segments[i].begin();
		for(int j=p1;j<(int)segments[i].size();++j){
			if(segments[i][j].K!=k)
				break;
			if(mx < segments[i][j].T){
				mx = segments[i][j].T;
				ret = segments[i][j];
			}
		}
	}
	return ret;
}
void delete_m(map<string,map<int,int>> &h, string k){
	if(!h.count(k))
		return;
	h.erase(k);
}
void delete_d(vector<vector<dat>> &segments, string k, int ava){
	for(int i=ava-1;i>=1;--i){
		int p1 = lower_bound(segments[i].begin(),segments[i].end(),dat(k,-1,-1)) - segments[i].begin();
		int p2 = upper_bound(segments[i].begin(),segments[i].end(),dat(k,-1,-1)) - segments[i].begin();
		if(p1>=(int)segments[i].size() || segments[i][p1].K!=k)
			continue;
		segments[i].erase(segments[i].begin() + p1, segments[i].begin() + p2);
	}
}
int main(){
	int n,v,i,L,op;
	
	int ava = 1;

	int sz = 0;

	string k;

	//n -> Total number of queries of insert,find,delete. Typically of order 10^5.
	//L -> Total number of <k,v> pairs our memory can hold. To be specified at runtime.

	cin >> L >> n;
	
	map<string,map<int,int>> h; //Emulates memory.

	vector<vector<dat>> segments(2*L + 1); //Emulates Disk of contiguous blocks(HDD).
	 
	//1 -> insert key,value
	//2 -> find key (returns latest value and -1 if not found)
	//3 -> deletes key (and all values associated with it).

	for(i = 0; i < n ; ++i){

		cin >> op;

		if(op==1){

			cin >> k >> v;

			if(sz<L){

				if(h[k].count(v))
					continue;

				h[k][v] = ++t1;

				++sz;

			}
			else{
				//Memory overflow!
				//Migrate to segments
				//By default we migrate the first contiguous sz/2 amount of stuff to a new segment in segments.
				if(ava>L){
					//compress/merge all L segments to 1 segment and put it into node - 1.
					//And put the pending sz/2 segments on node - 2.
					//Set ava to 3.
					//Set sz to sz - sz/2
					compress(segments,L,h,sz);
					ava = 3;
					sz = sz - sz/2;

					//Now put current k,v in memory.
					if(h[k].count(v))
						continue;
					h[k][v] = ++t1;
					++sz;
				}
				else{
					//We have space in segments.
					//Just put the first sz/2 stuff into segments[ava].
					//Set ava to ava + 1
					//Set sz to sz - sz/2
					//No need for merging.
					update(segments,L,h,sz,ava);
					++ava;
					sz = sz - sz/2;
					
					//Now put current k,v in memory.
					if(h[k].count(v))
						continue;
					h[k][v] = ++t1;
					++sz;
				}
			}
		}
		if(op==2){
			cin >> k;
			//Search for key=k in both.
			//Get time values from both and return latest of them.
			//If not found return -1.
			//d1.K = d2.K = k
			dat d1 = search_m(h,k);
			dat d2 = search_d(segments,k,ava);
			cout<<d1.T<<" "<<d2.T<<endl;
			if(d1.T==-1 && d2.T==-1)
				cout<<-1<<"\n";
			else if(d1.T > d2.T)
				cout<<d1.K<<" "<<d1.V<<"\n";
			else
				cout<<d2.K<<" "<<d2.V<<"\n";

		}
		if(op==3){
			cin >> k;
			//Delete every instance of key = k from memory and disk.
			delete_m(h,k);
			delete_d(segments,k,ava);
		}
	}



}
