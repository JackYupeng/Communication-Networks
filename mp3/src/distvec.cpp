#include<stdio.h>
#include<string.h>
#include<stdlib.h>


#include <string>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <queue>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

int nodes_count;
int node_array_count;

vector <vector<int> > edge_cost;


struct DV_message
{
	int source;
	int destination;
	int link;
	int cost;
	vector<int> distance_vector;
};

struct Node
{
	int id;
	unordered_map <int,int> neighbors;
	vector <vector <int> > cost_table;
	vector <int> path;
	vector <int> min_cost;
	bool flag;
	vector <DV_message> msgs;
};

//forwarding table: destination, nexthop, pathcost
struct ForwardTable{
	int destination;
	int nexthop;
	int pathcost;
};


struct Message_structure{
	int source;
	int destination;
	string message;
};

//declare for future use
queue<Message_structure*> allmessages;
vector<vector<ForwardTable>>  forward_table;
vector <Node> every_node;
vector <vector <DV_message> >  NodeMessages;
string topo_filename;
string changes_filename;
string msg_filename;
ofstream outfile;
map <int, Node* > cur_nodes;
map<int, Node*> topology; 



void print_forward(int node){
	Node cur=every_node[node];
	forward_table[node].resize(node_array_count);
	for(int i = 1; i < node_array_count; i++){
		outfile << i;
		forward_table[node][i].destination=i;
		outfile << "\t";
		if(node==i){
			outfile<<i;
			forward_table[node][i].nexthop=i;
		}
		else{
			outfile << cur.path[i];
			forward_table[node][i].nexthop=cur.path[i];
		}
		outfile << "\t";
		outfile << cur.cost_table[node][i];
		forward_table[node][i].pathcost=cur.cost_table[node][i];
		outfile << "\n";
	}
}



void sendmessage(){

	while(allmessages.size() > 0){
		int source, dest;
		source = allmessages.front()->source;
		dest = allmessages.front()->destination;
		
		outfile << "from"<<" " << source << " "<< "to" <<" " << dest <<" "<< "cost" <<" ";
		
		queue<int> pathhops; 
		pathhops.push(source);

		int vectorindex = 0;
		vector<ForwardTable> cur_table = forward_table[source];
		bool isreachable = false;
		while(cur_table[vectorindex].destination != dest && vectorindex < cur_table.size()){
			vectorindex++;
		}
		if(cur_table[vectorindex].nexthop != -1){
				isreachable = true;
		}
		if(isreachable){
			outfile << every_node[source].cost_table[source][dest]<<" "<<"hops"<<" ";
			int nexthop = cur_table[vectorindex].nexthop;
			if(nexthop != dest){
				pathhops.push(nexthop);
			}
			while(nexthop != dest){
				cur_table = forward_table[nexthop];
				vectorindex = 0;
				while(cur_table[vectorindex].destination != dest && vectorindex < cur_table.size()){
				vectorindex++;
				}
				nexthop = cur_table[vectorindex].nexthop;
				if(nexthop != dest){
					pathhops.push(nexthop);
				}
			}
			
			//print
			while(pathhops.size() > 0){
				outfile << pathhops.front() << " ";
				pathhops.pop();
			}
			outfile << "message" << allmessages.front()->message << "\n";
			allmessages.pop();
		}
		//unreachable
		else{
			outfile << "infinite hops"<<" ";
			outfile << "unreachable message" << allmessages.front()->message << "\n";
			allmessages.pop();
		}
	}
	outfile << "\n";

}
void prepare_message_to_send(string fname){
	ifstream infile(fname);
	string line, message;

	if (infile.is_open()){
		while(getline(infile,line)){
			int source;
			int dest;
			sscanf(line.c_str(), "%d %d %*s", &source, &dest);
			message = line.substr(line.find(" ")); 
			message = message.substr(line.find(" ") + 1); 
			Message_structure *msg = new Message_structure;
			msg->source = source;
			msg->destination = dest;
			msg->message = message;
			allmessages.push(msg);
		}
		infile.close();
	}
}
int get_topology(string fname){
	ifstream topfile(fname);
	int total_nodes=0;
	int a, b, c;

	while(topfile >> a >> b >> c){
		int aa=0;
		int bb=0;
		if(aa != bb) {
			break;
		}
		
		if(topology.find(a) == topology.end()){
			Node *source_node = new Node;
			topology[a] = source_node;
			topology[a]->id = a; 
			total_nodes++;
		}
		
		if(topology.find(b) == topology.end()){
			Node *dest_node = new Node;
			topology[b]= dest_node;
			topology[b]->id = b;
			total_nodes++;
		}
		
		if(topology[a]->neighbors.find(b) == topology[a]->neighbors.end()) {
			topology[a]->neighbors[b] = c; 
		}
		
		if(topology[b]->neighbors.find(a) == topology[b]->neighbors.end()) {
			topology[b]->neighbors[a] = c;
		}
	}
	
	topfile.close();

	//edge_cost matrix
	edge_cost.resize(total_nodes+1);
	for(int i = 1; i <total_nodes+1; i++)
	{
		edge_cost[i].resize(total_nodes+1);
		for(int j = 1; j< total_nodes+1; j++)
		{
			if(i == j) 
			{
				edge_cost[i][j] = 0;
			}
			else 
			{
				edge_cost[i][j] = 999999999; 
			}
		}
	}

	ifstream topfile2(fname);
		while(topfile2 >> a >> b >> c)
		{
			if(c > 0  && c < 100)
			{
				edge_cost[a][b] = c;
				edge_cost[b][a] = c;
			}

		}

	topfile2.close();

	return total_nodes;
}



void initialization_of_topo()
{
	cout<<"1233"<<endl;
	every_node.resize(node_array_count);
	for(int i = 1 ; i < node_array_count; i++)
	{
		every_node[i].id = i;
		every_node[i].flag = false;
		every_node[i].min_cost.resize(node_array_count);
		every_node[i].path.resize(node_array_count);
		every_node[i].cost_table.resize(node_array_count);
		for(int j = 1; j<node_array_count ; j++)
		{
			every_node[i].cost_table[j].resize(node_array_count);
		}
	}

	for(int i = 1 ; i < node_array_count; i++)//loop through every node
	{
		for(int j = 1; j < node_array_count; j++)
		{
			every_node[i].min_cost[j] = edge_cost[i][j];
			if(edge_cost[i][j]<100 && edge_cost[i][j]>0) {
				every_node[i].path[j]=j;
			}
			else
				if(i==j) {
					every_node[i].path[j]=0;
				}
				else {
					every_node[i].path[j]=-1;//cannot reach
				}
		}
		for(int m=1;m<node_array_count;m++){
			for(int n=1;n<node_array_count;n++){
				if(i==m) {
					every_node[i].cost_table[m][n] = edge_cost[m][n];
				}
				else {
					every_node[i].cost_table[m][n] = 999999999;
				}
				if(m==n) {
					every_node[i].cost_table[m][n] = 0;
				}
			}
		}

		//sending DV messages to neighbors
		for(unordered_map<int, int>::iterator it=topology[i]->neighbors.begin();it!=topology[i]->neighbors.end();it++){
			int dest=it->first;
			DV_message message;
			message.source=i;
			message.destination=dest;
			message.link=i;
			message.distance_vector=every_node[i].cost_table[i];
			every_node[dest].msgs.push_back(message);
		}

	}

}

void recalculate_cost(int cur_node){
	cout<<"123";
	int flag=0;
	for(vector<DV_message>::iterator it=every_node[cur_node].msgs.begin();it!=every_node[cur_node].msgs.end();it++){
		int link=it->link;
		every_node[cur_node].cost_table[link]=it->distance_vector;
			for(int j=1;j<node_array_count;j++){
				if(cur_node!=j&&link!=j){
					int ori_cost=every_node[cur_node].cost_table[cur_node][j];
					int new_cost=edge_cost[cur_node][link]+it->distance_vector[j];
					if(new_cost<ori_cost){
						every_node[cur_node].cost_table[cur_node][j]=new_cost;
						every_node[cur_node].path[j]=link;
						if(every_node[cur_node].path[link]!=link) {
							every_node[cur_node].path[j]=every_node[cur_node].path[link];
						}
						every_node[cur_node].min_cost[j]=new_cost;
						flag=1;
					}
					if(new_cost==ori_cost){
						if(link<every_node[cur_node].path[j]) {
							every_node[cur_node].path[j]=link;
						}
					}
				every_node[cur_node].cost_table[j][cur_node]=every_node[cur_node].cost_table[cur_node][j];
				}
			}
	}

	every_node[cur_node].msgs.clear();

	for(int m=1;m<node_array_count;m++)
		for(int n=1;n<node_array_count;n++)
			if(every_node[cur_node].cost_table[m][n]>every_node[cur_node].cost_table[n][m]) {
				every_node[cur_node].cost_table[m][n]=every_node[cur_node].cost_table[n][m];
			}

	if(flag==1){
		for(unordered_map<int, int>::iterator it2=topology[cur_node]->neighbors.begin();it2!=topology[cur_node]->neighbors.end();it2++){
			int dest=it2->first;
			DV_message message;
			message.source=cur_node;
			message.destination=dest;
			message.link=cur_node;
			message.distance_vector=every_node[cur_node].cost_table[cur_node];
			every_node[dest].msgs.push_back(message);
		}
	}
}

void update(string changefname, string msgfname){
	cout<<"123";
	ifstream changefile(changefname);
	int source;
	int destination;
	int cost;
	int change_time=0;
	while(changefile >> source >> destination >> cost){
		change_time++;
		if(topology[source]->neighbors.find(destination) != topology[source]->neighbors.end()){
			if(cost > 0){
				topology[source]->neighbors[destination] = cost;
				topology[destination]->neighbors[source] = cost;
				edge_cost[source][destination]=cost;
				edge_cost[destination][source]=cost;
			}
			else if(cost == -999){
				topology[source]->neighbors.erase(destination);
				topology[destination]->neighbors.erase(source);
				edge_cost[source][destination]=999999999;
				edge_cost[destination][source]=999999999;
			}
		}
		else{
			if(cost > 0){
				topology[source]->neighbors[destination] = cost;
				topology[destination]->neighbors[source] = cost;
				edge_cost[source][destination]=cost;
				edge_cost[destination][source]=cost;
			}
		}

		for(int i = 1 ; i < node_array_count; i++)
		{
			for(int j = 1; j < node_array_count; j++)
			{
				every_node[i].min_cost[j] = edge_cost[i][j];
				if(edge_cost[i][j]<100 && edge_cost[i][j]>0) {
					every_node[i].path[j]=j;
				}
				else {
					if(i==j) {
						every_node[i].path[j]=0;
					}
					else {
						every_node[i].path[j]=-1;
					}
				}
			}

			for(int a=1;a<node_array_count;a++){
				for(int b=1;b<node_array_count;b++){
					if(i==a) {
						every_node[i].cost_table[a][b] = edge_cost[a][b];
					}
					else {
						every_node[i].cost_table[a][b] = 999999999;
					}
					if(a==b) {
						every_node[i].cost_table[a][b] = 0;
					}
				}
			}

			for(unordered_map<int, int>::iterator it=topology[i]->neighbors.begin();it!=topology[i]->neighbors.end();it++){
				int final_node=it->first;
				DV_message message;
				message.source=i;
				message.destination=final_node;
				message.link=i;
				message.distance_vector=every_node[i].cost_table[i];
				every_node[final_node].msgs.push_back(message);
			}

		}

		for(int node=1;node<node_array_count;node++){
			recalculate_cost(node);
			if(node==node_array_count-1) {
				for(int m=1;m<node_array_count;m++)
					if(!every_node[m].msgs.empty()) {
						node=0;
					}
			}
		}

		for(int node=1;node<node_array_count;node++){
			print_forward(node);
		}

		prepare_message_to_send(msgfname);
		sendmessage();
		cout<<"bbb"<<endl;
	}
	changefile.close();
	cout<<"ccc"<<endl;
}










int main(int argc, char** argv)
{
	cout<<"aaa";
	outfile.open("output.txt"); 

	if(argc != 4)
	{
		fprintf(stderr, "usage: %s topofile messagefile changesfile\n", argv[0]);
		exit(1);
	}
	topo_filename = argv[1];
	msg_filename = argv[2];
	changes_filename = argv[3];

	nodes_count = get_topology(topo_filename); 
	node_array_count = nodes_count + 1;
	forward_table.resize(node_array_count);
	cout<<"aaa";
	initialization_of_topo();
	int iter=0;
	for(int node=1;node<node_array_count;node++){
		recalculate_cost(node);
		iter++;
		if(node==node_array_count-1) {
			for(int m=1;m<node_array_count;m++) {
				if(!every_node[m].msgs.empty()) {
					node=0;
				}
			}
		}
	}

	for(int node=1;node<node_array_count;node++){
		print_forward(node);
	}

	prepare_message_to_send(msg_filename);
	sendmessage();
	update(changes_filename, msg_filename);

	outfile.close();
}

























/*

int main(int argc, char** argv) {
    //printf("Number of arguments: %d", argc);
    if (argc != 4) {
        printf("Usage: ./distvec topofile messagefile changesfile\n");
        return -1;
    }

    FILE *fpOut;
    fpOut = fopen("output.txt", "w");
    fclose(fpOut);

    return 0;
}
*/

