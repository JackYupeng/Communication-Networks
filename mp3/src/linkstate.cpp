#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <map>
#include <vector>
#include <fstream>
using namespace std;

ofstream fout("output.txt");

struct structure{
    int a,b,c;
    structure(){}
    structure(int aa, int bb, int cc){a=aa;b=bb;c=cc;}
};


class graph{
    private:
    int vertice_count, edge_count; //number of vertices and edges

    struct edge_str
    {
        int end;
        int weight;
         edge_str*next;
         edge_str(int e,int w, edge_str*n=NULL){end=e;weight=w;next=n;}
    };

    struct vertice_str //create linked list
    {
        int ver;
         edge_str*head;
        vertice_str( edge_str*h=NULL){head=h;}
    };
    vertice_str*vertice_list; //pointer

    

    public:


    graph(int vsize,int d[]) //constructor
    {
        vertice_count=vsize;
         edge_count=0;
        vertice_list=new vertice_str[vsize]; //allocate new memory
        for(int i=0;i<vertice_count;++i)
        {
            vertice_list[i].ver=d[i];
        }
    }

    ~graph() //destructor
    {
         edge_str*p;
        for(int i=0;i<vertice_count;++i) //delete the whole linked list
        {
            while(vertice_list[i].head!=NULL)
            {
                p=vertice_list[i].head;
                vertice_list[i].head=p->next;
                delete p;
            }
        }
        delete []vertice_list;
    }

    bool insert_edge(int u,int v,int w)
    {
        vertice_list[u].head=new  edge_str(v,w,vertice_list[u].head);
        ++ edge_count;
        return true;
    }

    bool remove_edge(int u,int v) //remove_edge an edge in the graph
    {
        edge_str*p=vertice_list[u].head;
        edge_str*q;
        if(p==NULL)	{//if the linked list does not exist_or_not, return false
        	return false;
        }
        if(p->end==v) {
            vertice_list[u].head=p->next;
            delete p;
            -- edge_count;
            return true;
        }
        if(p->next!=NULL && p->next->end!=v) {
        	p=p->next;
        }
        if(p->next==NULL) {
        	return false;
        }
        if(p->next->end==v)
        {
            q=p->next;
            p->next=q->next;
            delete q;
            --edge_count;
            return true;
        }
        return true;
    }

    bool exist_or_not(int u,int v)
    {
         edge_str*p=vertice_list[u].head;
        while(p!=NULL && p->end!=v) { //loop through the linked list
        	p=p->next;
        }
        if(p==NULL) {
        	return false;
        }
        else {
        	return true;
        }
    }

    void print_path(int start, int end, int prev[], vector<int>&v2){

        if(start==end) {
            v2.push_back(vertice_list[start].ver);
            return;
        }
        print_path(start, prev[end], prev, v2);
        v2.push_back(vertice_list[end].ver);
    }

    static bool compare2(const structure &aa, const structure &bb){ //compare function
        return aa.a<bb.a;
    }

    void Dijkstra(int start)
    {
        bool *known=new bool[vertice_count];
        int *distance=new int[vertice_count];
        int *prev=new int[vertice_count];

         edge_str*p;
        int node;
        int i,j;
        for(i=0;i<vertice_count;++i)
        {
            known[i]=false;
            distance[i]=999999999;
        }
        int nnodee;
        for(nnodee=0;nnodee<vertice_count;++nnodee)
        {
            if(vertice_list[nnodee].ver==start) {
            	break;
            }
        }
        distance[nnodee]=0;
        prev[nnodee]=nnodee;

        for(i=1;i<vertice_count;++i)   
        {
            int min=999999999;
            for(j=0;j<vertice_count;++j){
                if(!known[j] && distance[j]<min) {
                    min=distance[j];
                    node=j;
                }
            }
            known[node]=true;
            for(p=vertice_list[node].head;p!=NULL;p=p->next){
                if(!known[p->end] && distance[p->end]>=min+p->weight) {
                    if(distance[p->end]>min+p->weight) {
                    	prev[p->end]=node;
                    }
                    if(distance[p->end]!=999999999 && distance[p->end]==min+p->weight && prev[p->end]>node) {
                    	prev[p->end]=node;
                    }
                    distance[p->end]=min+p->weight;
                }
            }
        }
        vector<int>v2;
        vector<structure>ans;
        structure tempp;
        for(i=0;i<vertice_count;++i){
            tempp.a = vertice_list[i].ver;
            v2.clear();
            if(distance[i]!=999999999) {
            	print_path(nnodee, i, prev, v2);
            }
            else {
            	continue;
            }

            if(v2.size()==0) {
            	continue;
            }
            else if(v2.size()==1) {
            	tempp.b = v2[0];
            }
            else {
            	tempp.b = v2[1];
            }

            if(distance[i]!=999999999) {
            	tempp.c = distance[i];
            }
            else {
            	tempp.c = -999;
            }
            ans.push_back(tempp);
        }
        sort(ans.begin(),ans.end(),compare2);   
        for(int i=0;i<ans.size();++i){
            if(ans[i].c != -999) {
            	fout<<ans[i].a<<' '<<ans[i].b<<' '<<ans[i].c<<endl;
            }
        }
    }

    void Dijkstra2(int start, int final) //basically do the same thing
    {
        bool*known=new bool[vertice_count];
        int*distance=new int[vertice_count];
        int *prev=new int[vertice_count];

         edge_str*p;
        int node;
        int i,j;
        for(i=0;i<vertice_count;++i)
        {
            known[i]=false;
            distance[i]=999999999;
        }
        int nnodee=start;

        distance[nnodee]=0;
        prev[nnodee]=nnodee;

        for(i=1;i<vertice_count;++i)       
        {
            int min=999999999;
            for(j=0;j<vertice_count;++j)
            {
                if(!known[j] && distance[j]<min)
                {
                    min=distance[j];
                    node=j;
                }
            }
            known[node]=true;
            for(p=vertice_list[node].head;p!=NULL;p=p->next)
            {
                if(!known[p->end] && distance[p->end]>=min+p->weight)
                {
                    if(distance[p->end]>min+p->weight) {
                    	prev[p->end]=node;
                    }
                    if(distance[p->end]!=999999999 && distance[p->end]==min+p->weight && prev[p->end]>node) {
                    	prev[p->end]=node;
                    }
                    distance[p->end]=min+p->weight;
                }
            }
        }
        if(distance[final]!=999999999) {
            fout<<distance[final]<<" "<<"hops"<<" ";
        }
        else{
            fout<<"infinite hops unreachable"<<" ";
            return;
        }
        vector<int>v2;
        print_path(nnodee, final, prev, v2);
        for(int i=0;i<v2.size()-1;++i) {
        	fout<<v2[i]<<' '; //print all the hops needed to reach
        }
    }
};

/////////////////////////end of class graph

struct message{
    int from;
    int to;
    string sentence;
};


static bool compare(const pair<int,int>&a, const pair<int,int>&b){
    return a.first<b.first;
}




#include <unordered_map>



int main(int argc, char** argv) //do not need argc here in this program
{

	vector<structure>vc;
    unordered_map<int,int>mp;
	int find1;
    int find2;
    int find3;

    ifstream inFile(argv[1]);
    ifstream inFile2(argv[3]);
    ifstream inFile3(argv[2]);

    
    int num=0;
    while(inFile>>find1>>find2>>find3){
        structure tmp(find1,find2,find3);
        vc.push_back(tmp);
        if(mp.find(find1)==mp.end()) {
        	mp[find1]=num;
        	num++;
        }
        if(mp.find(find2)==mp.end()) {
        	mp[find2]=num;
        	num++;
        }
    }
    int num_of=num;
    int*array=new int[num_of];
    for(int i=0;i<num_of;++i){
        int tmp=0;
        for(auto iter=mp.begin();iter!=mp.end();++iter){
            if(iter->second==i) {
            	tmp=iter->first;
            	break;
            }
        }
        array[i]=tmp;
    }
    graph g(num_of,array);
    for(int i=0;i<vc.size();++i){
        if(vc[i].c == -999) {
        	continue;
        }
        if(!g.exist_or_not(mp[vc[i].a],mp[vc[i].b])) {
        	g.insert_edge(mp[vc[i].a],mp[vc[i].b],vc[i].c);
        }
        if(!g.exist_or_not(mp[vc[i].b],mp[vc[i].a])) {
        	g.insert_edge(mp[vc[i].b],mp[vc[i].a],vc[i].c);
        }
    }

    vector<pair<int,int>>vp;
    pair<int,int> tmppair;
    for(auto iter=mp.begin();iter!=mp.end();++iter){
        tmppair.first=iter->first;
        tmppair.second=iter->second;
        vp.push_back(tmppair);
    }
    sort(vp.begin(),vp.end(),compare);
    for(int i=0;i<num_of;++i){
        g.Dijkstra(vp[i].first);
    }

    vector<message>message_vc;
    message m;
    int l1,l2;
    char ww;
    string s;
    while(inFile3>>l1>>l2) {
        inFile3.get(ww);
        getline(inFile3,s);
        m.from=l1;
        m.to=l2;
        m.sentence=s;
        message_vc.push_back(m);
    }
    for(int i=0;i<message_vc.size();++i) {
        fout<<"from"<<" "<<message_vc[i].from<<" "<<"to"<<" "<<message_vc[i].to<<" "<<"cost"<<" ";
        g.Dijkstra2(mp[message_vc[i].from],mp[message_vc[i].to]);
        fout<<"message"<<" "<<message_vc[i].sentence<<endl;
    }

    int mm,nn,weightt;
    while(inFile2>>mm>>nn>>weightt){
        g.remove_edge(mp[mm],mp[nn]);
        g.remove_edge(mp[nn],mp[mm]);
        if(weightt != -999){
            if(!g.exist_or_not(mp[mm],mp[nn])) {
            	g.insert_edge(mp[mm],mp[nn],weightt);
            }
            if(!g.exist_or_not(mp[nn],mp[mm])) {
            	g.insert_edge(mp[nn],mp[mm],weightt);
            }
        }
        for(int i=0;i<num_of;++i){
            g.Dijkstra(vp[i].first);
        }
        for(int i=0;i<message_vc.size();++i){
            fout<<"from"<<" "<<message_vc[i].from<<" "<<"to"<<" "<<message_vc[i].to<<" "<<"cost"<<" ";
            g.Dijkstra2(mp[message_vc[i].from],mp[message_vc[i].to]); 
            fout<<"message"<<" "<<message_vc[i].sentence<<endl;
        }
    }

    return 0;
}


















/*
int main(int argc, char** argv) {
    //printf("Number of arguments: %d", argc);
    if (argc != 4) {
        printf("Usage: ./linkstate topofile messagefile changesfile\n");
        return -1;
    }

    FILE *fpOut;
    fpOut = fopen("output.txt", "w");
    fclose(fpOut);
    

    return 0;
}
*/
