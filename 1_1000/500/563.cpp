// Michael Seyfert <michael@codesand.org>
// Thu Aug 13 04:19:57 UTC 2015

#include<iostream>
#include<cstdlib>
#include<vector>
#include<map>
#include<queue>
using namespace std;

/*
  This is a graph theory (source->sink) problem in disguise.
  You might be able to solve this without graphs, but I couldn't
  find a solution that way.
  We setup the graph, then use the Ford-Fulkerson algorithm to
  determine the max flow.
  The max flow coming in the sink should be equal to the number
  of banks.
*/

// // for testing...
// map<int, string> NAMES;

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
        flows[to][from].c_f = 0;
      }
    }

    vector<int> mins(_graph.size(), 999999999);
    vector<int> lasts(_graph.size(), -1);

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

//============================================================
// Crimewave
//============================================================
const char *POSSIBLE = "possible";
const char *NOT_POSSIBLE = "not possible";

class Crimewave
{
public:
  const char* Solve(int x_max, int y_max,
                    const vector<pair<int, int> > &banks) {

    _x_max = x_max;
    _y_max = y_max;

    // Build a graph.
    // We count starting at zero.

    // For each intersection we need two vertices.
    // We need a source and sink vertex.
    int num_verts = 1 + 1 + x_max * y_max * 2;
    _graph.resize(num_verts);

    // Setup each intersection.
    for(int x = 0;x < _x_max; ++x)
      for(int y = 0;y < _y_max; ++y) {
        // each intersection (or location in grid) has two vertices.
        // the capacity between them is 1, so two robbers cannot go through
        // here (only 1 is allowed).
        int in_idx = GetVertIdx(x,y);
        int out_idx = in_idx + 1;
        _graph[in_idx].push_back(MakeEdge(in_idx, out_idx, 1));
      }

    // Setup each intersection's edges to other intersections.
    for(int x = 0;x < _x_max; ++x)
      for(int y = 0;y < _y_max; ++y){
        // idx out represents the "from" for this (x,y) grid.
        int idx_out = 1 + GetVertIdx(x,y);

        // try up (y+1)
        {
          int idx_in = GetVertIdx(x,y+1);
          if(idx_in >= 0)
            _graph[idx_out].push_back(MakeEdge(idx_out, idx_in, 1));
          else
            // The robber can escape off the grid.
            _graph[idx_out].push_back(MakeEdge(idx_out, SINK_IDX, 1));
        }
        // try down (y-1)
        {
          int idx_in = GetVertIdx(x,y-1);
          if(idx_in >= 0)
            _graph[idx_out].push_back(MakeEdge(idx_out, idx_in, 1));
          else
            _graph[idx_out].push_back(MakeEdge(idx_out, SINK_IDX, 1));
        }
        // try left (x-1)
        {
          int idx_in = GetVertIdx(x-1,y);
          if(idx_in >= 0)
            _graph[idx_out].push_back(MakeEdge(idx_out, idx_in, 1));
          else
            _graph[idx_out].push_back(MakeEdge(idx_out, SINK_IDX, 1));
        }
        // try right (x+1)
        {
          int idx_in = GetVertIdx(x+1,y);
          if(idx_in >= 0)
            _graph[idx_out].push_back(MakeEdge(idx_out, idx_in, 1));
          else
            _graph[idx_out].push_back(MakeEdge(idx_out, SINK_IDX, 1));
        }
      }

    // Setup banks, (source edges to the grid)
    for(size_t k = 0;k < banks.size(); ++k) {
      // zero index these.
      int x = banks[k].first - 1;
      int y = banks[k].second - 1;
      //
      int idx_to = GetVertIdx(x,y);
      // from source to bank intersection.
      _graph[SOURCE_IDX].push_back(MakeEdge(SOURCE_IDX, idx_to, 1));
    }

    // We have made the graph, now run the FordFulkerson to find
    // the flow out.
    int max = FordFulkerson(_graph).MaxFlow(SOURCE_IDX, SINK_IDX);
//    cerr << "max = " << max << endl;
    if(max != banks.size())
      return NOT_POSSIBLE;
    return POSSIBLE;
  }
private:
  static const int SOURCE_IDX = 0;
  static const int SINK_IDX = 1;
  //_graph[vert idx] = vector of edges starting at this index.
  vector<vector<edge> > _graph;
  int _x_max, _y_max;

  // Returns an integer index for a vertex in the grid.
  // -1 on out of bounds.
  int GetVertIdx(int x, int y) const{
    if(x < 0 || x >= _x_max)
      return -1;
    if(y < 0 || y >= _y_max)
      return -1;

    // Skip 2 for source and sink.
    return 2 + y * (_x_max * 2) + x*2;
  }

  edge MakeEdge(int from_idx, int to_idx, int capacity) {
    edge e;
    e.to = to_idx;
    e.capacity = capacity;
    return e;
  }
};
//============================================================

int main()
{
  // Get problem input from cin.
  int num_problems;
  int streets, avenues, banks;
  cin >> num_problems;

  for(size_t p = 0;p < num_problems; ++p){
    cin >> streets >> avenues >> banks;
    vector<pair<int,int> > bank_locs(banks);
    for(size_t k = 0;k < banks;++k)
      // Problem input uses 1 as the start index.
      cin >> bank_locs[k].first >> bank_locs[k].second;

    cout << Crimewave().Solve(streets, avenues, bank_locs) << endl;
  }

  // // For testing...
  // NAMES[-1] = "NONE";
  // NAMES[0] = "x";
  // NAMES[1] = "a";
  // NAMES[2] = "b";
  // NAMES[3] = "c";
  // NAMES[4] = "d";
  // NAMES[5] = "e";
  // NAMES[6] = "y";
  // // Test FordFulkerson
  // vector<vector<edge> > g(7);
  // int x = 0; // source
  // int a = 1;
  // int b = 2;
  // int c = 3;
  // int d = 4;
  // int e = 5;
  // int y = 6; // sink
  // g[x].push_back({b, 1});
  // g[x].push_back({a, 3});
  // g[a].push_back({c, 3});
  // g[b].push_back({c, 5});
  // g[b].push_back({d, 4});
  // g[c].push_back({y, 2});
  // g[d].push_back({e, 2});
  // g[e].push_back({y, 3});
  // FordFulkerson f(g);
  // int max_flow = f.MaxFlow(x, y);
  // cout << "MAX_FLOW = " << max_flow << endl;
}
