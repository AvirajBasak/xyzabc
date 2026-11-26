/*
 * ==========================================================================
 *  PRACTICAL 4: BELLMAN-FORD ALGORITHM
 *  Shortest paths from ONE source vertex to ALL other vertices.
 *  Works with NEGATIVE edge weights, and detects NEGATIVE WEIGHT CYCLES.
 *
 *  BellmanFord(G, V, E, S):
 *    Step 1: dist[v] = INFINITY for every vertex, dist[S] = 0
 *    Step 2: repeat V-1 times:
 *              for each edge (u, v): Relax(u, v, w)
 *    Step 3: for each edge (u, v):
 *              if dist[u] + w(u,v) < dist[v]
 *                  -> "Graph contains a negative weight cycle"
 *            otherwise return dist[]
 *
 *  RELAX(u, v, w): "Is going to v THROUGH u cheaper than what we know?"
 *    if dist[u] + w < dist[v]  then  dist[v] = dist[u] + w
 *
 *  WHY V-1 TIMES?
 *    A shortest path visits each vertex at most once, so it has at most
 *    V-1 edges. Each round fixes at least one more edge of every shortest
 *    path, so after V-1 rounds all distances are final.
 *
 *  WHY STEP 3 FINDS A NEGATIVE CYCLE?
 *    After V-1 rounds nothing should improve any more. If an edge can
 *    STILL make a distance smaller, there is a loop whose total weight is
 *    negative: going round it again and again keeps lowering the cost,
 *    so no shortest path exists.
 *
 *  Time      : O(V * E)
 *  Reference : Kleinberg & Tardos, Ch 6, 6.8
 * ==========================================================================
 */

#include<iostream>          // cin and cout
using namespace std;

const int MAXV = 100;       // largest number of vertices allowed
const int MAXE = 100;       // largest number of edges allowed
const int INF  = 1000000000;// stands for "infinity" (not reachable yet)

// --------------------------------------------------------------------------
// struct Edge: one DIRECTED edge u -> v with weight w
// (Bellman-Ford works on directed graphs: A -> B is not the same as B -> A)
// Example: "A -> B with weight 10" with A=0, B=1 is stored as {0, 1, 10}
// --------------------------------------------------------------------------
struct Edge{
    int u, v, w;
};

// --------------------------------------------------------------------------
// relax: tries to improve dist[v] by going through u.
// Returns true if dist[v] got smaller, false otherwise.
// --------------------------------------------------------------------------
bool relax(int dist[], Edge e){

    // if u itself is not reachable yet (INF), we cannot go through it
    if(dist[e.u]==INF){
        return false;
    }

    // is "reach u, then take edge u->v" cheaper than the current dist[v]?
    if(dist[e.u]+e.w < dist[e.v]){
        dist[e.v]=dist[e.u]+e.w;    // yes: remember the cheaper cost
        return true;
    }

    return false;                   // no improvement
}

// --------------------------------------------------------------------------
// bellmanFord: fills dist[] with the shortest distance from source S.
// Returns false if the graph has a negative weight cycle, true otherwise.
//   V = number of vertices, m = number of edges, E[] = the edges
// --------------------------------------------------------------------------
bool bellmanFord(int V,int m,Edge E[],int S,int dist[]){

    // ---------------- STEP 1 ----------------
    // Nothing is reached yet: every distance is infinity...
    for(int v=0;v<V;v++){
        dist[v]=INF;
    }
    // ...except the source, which is 0 away from itself.
    dist[S]=0;

    // ---------------- STEP 2 ----------------
    // Relax EVERY edge, and do that V-1 times.
    for(int i=1;i<=V-1;i++){
        for(int j=0;j<m;j++){
            relax(dist,E[j]);
        }
    }

    // ---------------- STEP 3 ----------------
    // Check every edge ONE more time. If any distance can still be
    // lowered, a negative weight cycle exists.
    for(int j=0;j<m;j++){
        int u=E[j].u, v=E[j].v, w=E[j].w;

        if(dist[u]!=INF && dist[u]+w < dist[v]){
            return false;           // negative weight cycle found
        }
    }

    return true;                    // dist[] holds the shortest distances
}

// --------------------------------------------------------------------------
// main: reads the graph and the source, runs Bellman-Ford, prints result
// --------------------------------------------------------------------------
int main(){

    Edge E[MAXE];           // all the edges
    int dist[MAXV];         // dist[v] = shortest distance from source to v
    int V,m,S;              // vertices, edges, source

    cout<<"Enter number of vertices and edges:-";
    cin>>V>>m;

    if(V<=0 || V>MAXV || m<0 || m>MAXE){
        cout<<"Invalid input (max "<<MAXV<<" vertices, "<<MAXE<<" edges)\n";
        return 0;
    }

    // vertices are numbered 0 to V-1; each edge goes FROM u TO v
    cout<<"Enter each edge as: u v weight (directed u -> v, vertices 0 to "
        <<V-1<<")\n";

    for(int i=0;i<m;i++){
        cin>>E[i].u>>E[i].v>>E[i].w;

        if(E[i].u<0 || E[i].u>=V || E[i].v<0 || E[i].v>=V){
            cout<<"Invalid vertex in edge "<<i+1<<endl;
            return 0;
        }
    }

    cout<<"Enter source vertex:-";
    cin>>S;

    if(S<0 || S>=V){
        cout<<"Invalid source vertex\n";
        return 0;
    }

    // run the algorithm
    if(!bellmanFord(V,m,E,S,dist)){
        cout<<"\nGraph contains a negative weight cycle\n";
        return 0;
    }

    // print the shortest distance to every vertex
    cout<<"\nVertex\tDistance from source "<<S<<"\n";
    for(int v=0;v<V;v++){
        cout<<v<<"\t";
        if(dist[v]==INF){
            cout<<"INF (not reachable)";
        }
        else{
            cout<<dist[v];
        }
        cout<<"\n";
    }

    return 0;
}
