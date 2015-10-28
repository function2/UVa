// Michael Seyfert <michael@codesand.org>
// Mon Aug 17 22:08:53 UTC 2015

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <map>
#include <set>
using namespace std;

//============================================================
// FordFulkerson
//============================================================
struct edge{
  int to;
  int capacity;
};

struct flow_edge{
  int flow;
  int c_f;
  flow_edge()
      : flow(0), c_f(0){}
};

class FordFulkerson
{
public:
  FordFulkerson(const vector<vector<edge> > &graph)
      : _graph(graph)
  {}

  /*
    INPUT: source and sink indices.
    OUTPUT: flow in/out for each vertex.
    returns max flow.
    flows should be empty. It will return the flows for each edge.
  */
  int MaxFlow(
    int source_idx, int sink_idx,
    map<int, map<int, flow_edge> >  &flows)
  {
    int max_flow = 0;

    // keep track of "backwards" edges for the residual graph.
    vector<vector<edge> > back_graph(_graph.size());

    // initialize flow
    for(int from = 0;from < _graph.size(); ++from) {
      for(int to_idx = 0; to_idx < _graph[from].size(); ++to_idx){

        int to = _graph[from][to_idx].to;

        // push an edge going backwards, with zero capacity.
        edge t = {from,0};
        back_graph[to].push_back(t);

        // residual capacity = c_f(u,v) = cap(u,v) - flow(u,v)
        flows[from][to].c_f = _graph[from][to_idx].capacity;
        //        flows[to][from].c_f = 0;
      }
    }

    vector<int> mins;
    vector<int> lasts;

    for(;;) { // while a path has been found...
      // ===== Find a path in the residual graph. =====

      // Minimum number of moves to reach this vertex.
      mins.assign(_graph.size(), 999999999);
      lasts.assign(_graph.size(), -1);

      // Make this a stack for depth-first
      // Breadth-first search
      queue<pair<int, int> > s; // <idx, last>
      s.push(make_pair(source_idx, -1));
      mins[source_idx] = 0;

      int idx, last;
      while(!s.empty()){
        pair<int,int> p = s.front();
        s.pop();

        idx = p.first;
        last = p.second;
        int cmin = mins[idx];
        lasts[idx] = last;

        if(idx == sink_idx){
          // We found a path.
          break;
        }

        // try all edges
        for(const edge &e : _graph[idx]) {
          // Check the residual capacity.
          // It must be positive.
          // Lookup the flow.
          if(mins[e.to] > cmin + 1 && flows[idx][e.to].c_f > 0){
            // Try this edge.
            s.push(make_pair(e.to, idx));
            mins[e.to] = cmin + 1;
          }
        }
        // try "backwards" edges.
        // the residual path can go places the normal graph can't.
        for(const edge &e : back_graph[idx]) {

          if(mins[e.to] > cmin + 1 && flows[idx][e.to].c_f > 0){
            // Try this edge.
            s.push(make_pair(e.to, idx));
            mins[e.to] = cmin + 1;
          }
        }
      }

      if(idx != sink_idx)
        break; // no path was found.

      // Got a path.

      // Find c_f_p (min of all c_f in this path)
      int c_f_p = 999999999; // min of all c_f
      idx = sink_idx;
      for(;;){
        last = lasts[idx];
        if(last < 0) break;
        c_f_p = min(c_f_p, flows[last][idx].c_f);
        idx = last;
      }

      max_flow += c_f_p;

      // send flow through
      idx = sink_idx;
      for(;;){
        last = lasts[idx];
        if(last < 0) break;
        flows[last][idx].flow += c_f_p;
        flows[idx][last].flow -= c_f_p;

        flows[last][idx].c_f -= c_f_p;
        flows[idx][last].c_f += c_f_p;
        idx = last;
      }

      // loop and try to find another path...
    }

    return max_flow;
  }
  // if you only want the maximum flow.
  int MaxFlow(int source_idx, int sink_idx) {
    map<int, map<int, flow_edge> > flows;
    return MaxFlow(source_idx, sink_idx, flows);
  }

private:
  const vector<vector<edge> > &_graph;
};
//============================================================

void Solve()
{
  // Read input.
  int num_receptacles;
  cin >> num_receptacles;
  vector<string> receptacles(num_receptacles);
  for(int k = 0;k < num_receptacles;++k)
    cin >> receptacles[k];

  int num_devices;
  cin >> num_devices;
  vector<pair<string, string> > devs(num_devices);
  for(int k = 0;k < num_devices; ++k){
    string d, r;
    cin >> d >> r;
    devs[k] = make_pair(d,r);
  }

  int num_adapter_avail;
  cin >> num_adapter_avail;
  vector<pair<string, string> > adapter;
  map<string,string> have;
  for(int k = 0;k < num_adapter_avail; ++k) {
    string a,b;
    cin >> a >> b;
    if(a == b) continue;
    if(have[b] == a){
      // This will create
      // a loop with two edges, two vertices.
      // This is when you have an adapter with
      // A B
      //  and
      // B A
      // Fix this by making a third vertex.
      string bprime = "__" + a + b;
      // A  __B
      adapter.push_back(make_pair(a, bprime));
      // __B  B
      adapter.push_back(make_pair(bprime, b));
    }else{
      have[a] = b;
      adapter.push_back(make_pair(a,b));
    }
  }
  num_adapter_avail = adapter.size();

  if(!cin)
    cerr << "problem reading input\n";

  // // Done reading input.
  // cerr << "num_receptacles = " << num_receptacles << '\n';
  // cerr << "num_devices = " << num_devices << '\n';
  // cerr << "num_adapter = " << num_adapter_avail << '\n';

  // Setup the graph.
  /*
    0 source
    1 sink
    [2, num_devices)
    [2+num_devices, 2+num_devices+num_receptacles)
    remaining is adapters.
  */
  const int NUM_VERT = 2 + num_devices + num_receptacles + num_adapter_avail;
  vector<vector<edge> > graph(NUM_VERT);
  const int SOURCE = 0;
  const int SINK = 1;
  // Source should point to all of our devices. 1 capacity.
  for(size_t k = 0; k < devs.size(); ++k){
    edge e = {int(2 + k), 1};
    graph[SOURCE].push_back(e);
  }
  // Receptacles point to sink.
  for(size_t k = 0;k < receptacles.size(); ++k){
    edge e = {SINK,1};
    graph[2 + num_devices + k].push_back(e);
  }
  // Devices should point to receptacles / adapter in.
  for(size_t k = 0;k < devs.size(); ++k){
    string type = devs[k].second;

    for(size_t j = 0;j < receptacles.size(); ++j){
      if(type == receptacles[j]){
        edge e = {int(2 + num_devices + j), 1};
        graph[2 + k].push_back(e);
      }
    }

    for(size_t j = 0;j < adapter.size(); ++j){
      if(type == adapter[j].first){
        edge e = {int(2 + num_devices + num_receptacles + j), 1};
        graph[2 + k].push_back(e);
      }
    }
  }

  // Adapter out should point to receptacles.
  for(size_t k = 0;k < adapter.size(); ++k){
    string type = adapter[k].second;
    for(size_t j = 0;j < receptacles.size(); ++j) {
      if(type == receptacles[j]){
        edge e = {int(2 + num_devices + j), 1};
        graph[2 + num_devices + num_receptacles + k].push_back(e);
      }
    }
  }

  // Adapter out should point to adapter in.
  for(size_t k = 0;k < adapter.size(); ++k){
    string type = adapter[k].second;
    for(size_t j = 0;j < adapter.size(); ++j){
      if(type == adapter[j].first){
        //
        edge e = {int(2 + num_devices + num_receptacles + j), 9999999};
        graph[2 + num_devices + num_receptacles + k].push_back(e);
      }
    }
  }

  // // Debug, output graph.
  // for(size_t k = 0;k < graph.size(); ++k){
  //   cerr << k << ":";
  //   for(size_t j = 0;j < graph[k].size(); ++j)
  //     cerr << ' ' << "(" << graph[k][j].to << "/" << graph[k][j].capacity << ")";
  //   cerr << '\n';
  // }

  FordFulkerson f(graph);
  cout << num_devices - f.MaxFlow(SOURCE, SINK) << "\n\n";
}

int main()
{
  // This problem description changed while working on it???
  // apparently there are multiple test cases, when it
  // said it before there was one.
  int num_tests;
  cin >> num_tests;
  for(int k = 0;k < num_tests;++k)
    Solve();
}
