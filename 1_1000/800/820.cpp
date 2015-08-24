// Michael Seyfert <michael@codesand.org>
// Mon Aug 24 04:09:17 UTC 2015
#include <iostream>
#include <vector>
#include <map>
#include <queue>

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

bool Solve(int network)
{
  int num_nodes;
  cin >> num_nodes; // [2 to 100]
  if(num_nodes == 0) return false; // terminated input

  int source, sink, total_conn;
  cin >> source >> sink >> total_conn;

  // indexing into graph will be (node id) - 1. (zero based)
  vector<vector<edge> > graph(num_nodes);

  for(int k = 0 ; k < total_conn; ++k){

    int a, b, bandwidth;
    cin >> a >> b >> bandwidth;

    // Every edge will have an extra two vertices
    // to combine flow between 'a' and 'b'
    // since you can go from a to b, and from b to a.
    graph.push_back(vector<edge>()); // c
    int c = graph.size();
    int d = graph.size()+1;

    edge e = {d-1,bandwidth}; // add edge c -> d
    graph[c-1].push_back(e);
    graph.push_back(vector<edge>()); // d

    // add edge a -> c,  b -> c
    e = {c-1, bandwidth};
    graph[a-1].push_back(e);
    graph[b-1].push_back(e);

    // d -> a
    e = {a-1, bandwidth};
    graph[d-1].push_back(e);
    // d-> b
    e = {b-1, bandwidth};
    graph[d-1].push_back(e);
  }

  // // Debug, output graph.
  // for(size_t k = 0;k < graph.size(); ++k){
  //   cerr << k << ":";
  //   for(size_t j = 0;j < graph[k].size(); ++j)
  //     cerr << ' ' << "(" << graph[k][j].to << "/" << graph[k][j].capacity << ")";
  //   cerr << '\n';
  // }

  // Output max bandwidth, according to the format in
  //  the problem statement.
  cout << "Network " << network << '\n';
  int max_flow = FordFulkerson(graph).MaxFlow(source-1,sink-1);
  cout << "The bandwidth is " << max_flow << ".\n";
  cout << '\n'; // print a blank line after each test case.
  return true;
}

int main()
{
  for(int k = 1; Solve(k) ;++k);
}
