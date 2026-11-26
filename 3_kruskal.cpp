/*
 * ==========================================================================
 *  PRACTICAL 3: MINIMUM SPANNING TREE (MST) USING KRUSKAL'S ALGORITHM
 *  (plain arrays only: no vector, no library sort)
 *
 *  KRUSKAL:
 *    1. Sort all edges by weight, cheapest first.
 *    2. For each edge u-v (cheapest first):
 *         - if u and v are on DIFFERENT teams -> PICK the edge, join teams
 *         - if u and v are on the SAME team   -> SKIP it (it makes a loop)
 *    3. Stop when V-1 edges are picked.
 *
 *  TEAMS (UNION-FIND):
 *    A "team" = vertices already connected by the picked edges.
 *    Every team has one CAPTAIN.
 *      parent[x]   -> "who x asks" to reach the captain.
 *                     A captain points to itself: parent[c] == c
 *      setSize[c]  -> how many vertices are on captain c's team
 *      find(x)     -> keep asking until we reach the captain; return it
 *      unite(a,b)  -> one captain points to the other: teams merge
 *    Same captain  <=>  already connected  <=>  new edge would make a loop
 *
 *  REFERENCE: Kleinberg & Tardos, Ch 4, 4.6
 * ==========================================================================
 */

#include<iostream>          // cin and cout
using namespace std;        // write cout instead of std::cout

const int MAXV = 100;       // largest number of vertices allowed
const int MAXE = 100;       // largest number of edges allowed
                            // (const: array sizes must be fixed numbers)

// --------------------------------------------------------------------------
// struct Edge: one edge of the graph
//   u, v   -> the two vertices it joins
//   weight -> its cost
// Example: edge "2 - 3 with weight 2" is stored as {2, 3, 2}
// --------------------------------------------------------------------------
struct Edge{
    int u, v, weight;
};

// --------------------------------------------------------------------------
// TEAM ARRAYS (global so find() and unite() can both use them)
//
//   parent[x]  : who x asks to reach its captain.
//                parent[x] == x  means x IS the captain.
//
//   setSize[c] : number of vertices on captain c's team.
//                Only the captain's entry is used. It lets unite() put the
//                SMALLER team under the BIGGER one, which keeps the
//                "who to ask" chains short (so find() is fast).
//
// Example after picking 2-3 and 1-2 in the graph from the notes:
//   x         : 0  1  2  3  4
//   parent[x] : 0  2  2  2  4    -> 1 and 3 ask 2; 2 is a captain
//   teams     : {0}  {1,2,3}  {4}    captains 0, 2, 4
//   setSize[2] = 3
// --------------------------------------------------------------------------
int parent[MAXV];
int setSize[MAXV];

// --------------------------------------------------------------------------
// sortEdges: INSERTION SORT of the edges by weight (cheapest first).
// Same as insertion sort on numbers, but each item is a whole Edge
// and we compare the .weight field.
// --------------------------------------------------------------------------
void sortEdges(Edge E[], int m){

    // E[0..i-1] is already sorted; insert E[i] into its correct place
    for(int i=1;i<m;i++){

        Edge key=E[i];      // the edge we are inserting
        int j=i-1;          // compare with the edge just before it

        // shift every HEAVIER edge one place to the right
        // (j>=0 is checked first so we never read E[-1])
        while(j>=0 && E[j].weight>key.weight){
            E[j+1]=E[j];
            j--;
        }

        E[j+1]=key;         // put the edge into the gap
    }
}

// --------------------------------------------------------------------------
// makeSet: before any edge is picked, nobody is connected.
// So every vertex is its OWN team and its OWN captain.
//   parent  = [0, 1, 2, ..., V-1]   (everyone points to themselves)
//   setSize = [1, 1, 1, ..., 1]     (every team has 1 vertex)
// --------------------------------------------------------------------------
void makeSet(int V){
    for(int x=0;x<V;x++){
        parent[x]=x;        // x points to itself -> x is a captain
        setSize[x]=1;       // x's team has just x
    }
}

// --------------------------------------------------------------------------
// find: returns the CAPTAIN of x's team.
// Keep asking parent[] until we reach someone who points to themselves.
//
// Example: parent[1]=2, parent[2]=2
//   find(1): 1 asks 2 -> 2 points to itself -> captain is 2
//
// PATH COMPRESSION (the assignment below): on the way back, x is pointed
// DIRECTLY at the captain, so next time find(x) takes only one step.
// Example: chain 0 -> 1 -> 2 becomes 0 -> 2 after find(0).
// --------------------------------------------------------------------------
int find(int x){
    if(parent[x]!=x){                   // x is NOT a captain
        parent[x]=find(parent[x]);      // ask x's parent for the captain,
                                        // then point x straight at it
    }
    return parent[x];                   // parent[x] is now the captain
}

// --------------------------------------------------------------------------
// unite: merges the team of a and the team of b into ONE team.
// We only change ONE entry: one captain now points to the other captain.
// Everyone on that team follows automatically (they ask their old
// captain, who now asks the new captain).
//
// UNION BY SIZE (setSize): the SMALLER team's captain points to the
// BIGGER team's captain, so the chains stay short.
// --------------------------------------------------------------------------
void unite(int a,int b){
    int rootA=find(a);                  // captain of a's team
    int rootB=find(b);                  // captain of b's team

    if(rootA==rootB){                   // same captain: already one team
        return;
    }

    if(setSize[rootA]<setSize[rootB]){  // a's team is smaller
        parent[rootA]=rootB;            // a's captain now points to b's captain
        setSize[rootB]+=setSize[rootA]; // b's team grows by a's team size
    }
    else{                               // b's team is smaller (or equal)
        parent[rootB]=rootA;            // b's captain now points to a's captain
        setSize[rootA]+=setSize[rootB]; // a's team grows by b's team size
    }
}

// --------------------------------------------------------------------------
// kruskal: builds the MST and prints it
//   V = number of vertices, m = number of edges, E[] = the edges
// --------------------------------------------------------------------------
void kruskal(int V,int m,Edge E[]){

    Edge mst[MAXV];         // the PICKED edges (an MST has V-1 of them)
    int count=0;            // how many edges picked so far
    int totalWeight=0;      // total cost of the picked edges

    // STEP 1: cheapest edge first
    sortEdges(E,m);

    // STEP 2: every vertex starts as its own team
    makeSet(V);

    // STEP 3: try each edge, cheapest first.
    // Stop early once we have V-1 edges (the tree is complete).
    for(int i=0;i<m && count<V-1;i++){

        // LOOP CHECK:
        //   different captains -> not connected yet -> no loop -> PICK
        //   same captain       -> already connected -> loop    -> SKIP
        if(find(E[i].u)!=find(E[i].v)){

            mst[count]=E[i];            // store the picked edge
            count++;                    // one more edge in the tree
            totalWeight+=E[i].weight;   // add its cost

            unite(E[i].u,E[i].v);       // the two teams are now connected:
                                        // merge them into one team
        }
        // else: skip the edge (it would close a loop)
    }

    // STEP 4: fewer than V-1 edges means some vertex could never be
    // reached -> the graph is disconnected -> no spanning tree
    if(count!=V-1){
        cout<<"\nThe graph is not connected, so no spanning tree exists.\n";
        return;
    }

    // print the picked edges and the total cost
    cout<<"\nEdges in the Minimum Spanning Tree:\n";
    for(int i=0;i<count;i++){
        cout<<mst[i].u<<" - "<<mst[i].v<<"  (weight "<<mst[i].weight<<")\n";
    }
    cout<<"\nTotal weight of MST = "<<totalWeight<<endl;
}

// --------------------------------------------------------------------------
// main: reads the graph and calls kruskal()
// --------------------------------------------------------------------------
int main(){

    Edge E[MAXE];           // all the edges
    int V,m;                // V = vertices, m = edges

    cout<<"Enter number of vertices and edges:-";
    cin>>V>>m;

    // the input must fit in the fixed-size arrays
    if(V<=0 || V>MAXV || m<0 || m>MAXE){
        cout<<"Invalid input (max "<<MAXV<<" vertices, "<<MAXE<<" edges)\n";
        return 0;
    }

    // vertices are numbered 0, 1, ..., V-1
    cout<<"Enter each edge as: u v weight (vertices 0 to "<<V-1<<")\n";

    for(int i=0;i<m;i++){
        cin>>E[i].u>>E[i].v>>E[i].weight;   // read one edge

        // a vertex outside 0..V-1 would go outside parent[] and setSize[]
        if(E[i].u<0 || E[i].u>=V || E[i].v<0 || E[i].v>=V){
            cout<<"Invalid vertex in edge "<<i+1<<endl;
            return 0;
        }
    }

    kruskal(V,m,E);         // build and print the MST

    return 0;
}
