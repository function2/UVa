// Michael Seyfert <michael@codesand.org>
// Thu Aug 27 05:32:08 UTC 2015
#include<iostream>

using namespace std;

bool Solve()
{
  int N;
  cin >> N;
  if(N == 0) return false;

  //  vector<pair<int, int> > endpoints(N+2);
  for(int k = 0;k < N+2;++k){
    int x,y;
    cin >> x >> y;
    //    endpoints[k] = make_pair(x,y);
  }

  for(int k = 0; k < N; ++k){
    int c,w,s;
    cin >> c >> w >> s;
  }

  return true;
}

int main()
{
  while(Solve());
}
