/*
 * ==========================================================================
 *  PRACTICAL 8: SUFFIX TRIE (COMPRESSED, WITH (j, k) EDGE LABELS)
 *
 *  A suffix trie of text X stores EVERY suffix of X in a COMPRESSED trie.
 *  Every substring of X is a prefix of some suffix, so:
 *      "P occurs in X"  <=>  P can be spelled from the root down.
 *
 *  Example: X = banana  ->  banana$   (index: b0 a1 n2 a3 n4 a5 $6)
 *
 *      root
 *      ├─ (0,6) "banana$"        [0]
 *      ├─ (1,1) "a"
 *      │   ├─ (6,6) "$"          [5]
 *      │   └─ (2,3) "na"
 *      │       ├─ (6,6) "$"      [3]
 *      │       └─ (4,6) "na$"    [1]
 *      ├─ (2,3) "na"
 *      │   ├─ (6,6) "$"          [4]
 *      │   └─ (4,6) "na$"        [2]
 *      └─ (6,6) "$"              [6]
 *
 *  COMPACT STORAGE: an edge label is NOT stored as characters. It is stored
 *  as two numbers (j, k) meaning X[j..k]. So every node takes O(1) space
 *  and the whole trie takes O(n) space (instead of O(n^2)).
 *
 *  '$' is added at the end so no suffix is a prefix of another:
 *  every suffix then ends at its OWN leaf. A leaf remembers where its
 *  suffix starts (the number in [ ]).
 *
 *  BUILD : insert suffixes one by one; when a suffix leaves an edge
 *          halfway, SPLIT that edge.            -> O(n^2) for this method
 *  SEARCH: walk down comparing characters.      -> O(m), m = pattern length
 *          The leaves below the end point = the positions of P in X.
 *
 *  Reference: Goodrich, Tamassia & Mount, Ch 12, 12.5.3
 * ==========================================================================
 */

#include<iostream>          // cin and cout
#include<string>            // string
using namespace std;

const int SLOTS = 27;       // 26 letters a..z + 1 slot for '$'
const int MAXN  = 1000;     // longest text allowed

// --------------------------------------------------------------------------
// struct Node: one node of the suffix trie.
// The EDGE coming INTO this node is stored here as (start, end):
// its label is X[start..end].
// --------------------------------------------------------------------------
struct Node{
    int start, end;             // edge label into this node = X[start..end]
    int suffixIndex;            // for a LEAF: where its suffix starts in X
                                // for an internal node: -1
    Node* children[SLOTS];      // one child per possible FIRST character
                                // of the next edge (a..z and $)
};

string X;                       // the text, with '$' added at the end
int n;                          // length of X (including '$')

// --------------------------------------------------------------------------
// slot: which children[] slot a character uses.
//   'a'..'z' -> 0..25,   '$' -> 26
// Two edges leaving the same node always start with DIFFERENT characters
// (otherwise they would have been merged), so one slot per character works.
// --------------------------------------------------------------------------
int slot(char c){
    if(c=='$'){
        return 26;
    }
    return c-'a';
}

// --------------------------------------------------------------------------
// newNode: creates a node whose incoming edge is X[start..end].
// suffixIndex = -1 for internal nodes, or the suffix start for a leaf.
// --------------------------------------------------------------------------
Node* newNode(int start,int end,int suffixIndex){
    Node* nd=new Node;

    nd->start=start;
    nd->end=end;
    nd->suffixIndex=suffixIndex;

    for(int i=0;i<SLOTS;i++){
        nd->children[i]=NULL;       // no children yet
    }
    return nd;
}

// --------------------------------------------------------------------------
// insertSuffix: adds the suffix X[i..n-1] to the trie.
//
// Walk down from the root, comparing the suffix with the edge labels.
// Three things can happen:
//   1. No edge starts with the next character
//        -> hang a NEW LEAF here with label X[pos..n-1].
//   2. The whole edge matches
//        -> move down to that child and keep going.
//   3. The suffix stops matching HALFWAY along an edge
//        -> SPLIT the edge: a new internal node at the mismatch point,
//           the old part of the edge below it, and a new leaf beside it.
// --------------------------------------------------------------------------
void insertSuffix(Node* root,int i){

    Node* cur=root;     // node we are at
    int pos=i;          // next character of the suffix to match: X[pos]

    while(true){

        int c=slot(X[pos]);             // slot of the next character

        // ---- case 1: no edge starts with this character ----
        if(cur->children[c]==NULL){
            cur->children[c]=newNode(pos,n-1,i);    // leaf: rest of suffix
            return;
        }

        // ---- otherwise follow that edge and compare character by character
        Node* child=cur->children[c];
        int j=child->start;             // position inside the edge label

        while(j<=child->end && X[pos]==X[j]){
            pos++;                      // matched one more character
            j++;
        }

        // ---- case 2: the WHOLE edge matched -> go down and continue ----
        if(j>child->end){
            cur=child;
            continue;
            // (pos never runs off the end: the '$' appears only once, so a
            //  suffix can't completely match a path and still have more
            //  characters left -- it always leaves through a new leaf)
        }

        // ---- case 3: mismatch at X[j] (edge) vs X[pos] (suffix) ----
        // Split the edge X[child->start .. child->end] at j:
        //   upper part X[child->start .. j-1] -> new internal node "mid"
        //   lower part X[j .. child->end]     -> the old child, now below mid
        Node* mid=newNode(child->start,j-1,-1);

        cur->children[c]=mid;               // mid takes the child's place
        child->start=j;                     // the old child keeps the rest
        mid->children[slot(X[j])]=child;    // old part hangs under mid

        // the rest of OUR suffix becomes a new leaf under mid
        mid->children[slot(X[pos])]=newNode(pos,n-1,i);
        return;
    }
}

// --------------------------------------------------------------------------
// buildSuffixTrie: inserts every suffix X[0..], X[1..], ..., X[n-1..] ("$")
// --------------------------------------------------------------------------
Node* buildSuffixTrie(){
    Node* root=newNode(-1,-1,-1);   // the root has no incoming edge

    for(int i=0;i<n;i++){
        insertSuffix(root,i);
    }
    return root;
}

// --------------------------------------------------------------------------
// collectLeaves: puts the suffixIndex of every leaf below 'node' into pos[].
// These are exactly the places where the searched pattern starts in X.
// --------------------------------------------------------------------------
void collectLeaves(Node* node,int pos[],int &count){
    if(node->suffixIndex!=-1){          // a leaf: one occurrence
        pos[count]=node->suffixIndex;
        count++;
        return;
    }
    for(int c=0;c<SLOTS;c++){           // internal node: visit all children
        if(node->children[c]!=NULL){
            collectLeaves(node->children[c],pos,count);
        }
    }
}

// --------------------------------------------------------------------------
// search: finds pattern P in the text.
// Walks down from the root comparing P with the edge labels.
//   - if a character doesn't match -> P is not a substring -> return 0
//   - if P ends (even in the middle of an edge) -> found:
//     every leaf below that point is one occurrence
// Returns the number of occurrences; their start positions go into pos[].
// --------------------------------------------------------------------------
int search(Node* root,string P,int pos[]){

    Node* cur=root;
    int m=P.length();
    int p=0;                            // next character of P to match

    while(p<m){

        Node* child=cur->children[slot(P[p])];
        if(child==NULL){                // no edge starts with P[p]
            return 0;                   // -> not found
        }

        // compare P with this edge's label X[child->start .. child->end]
        int j=child->start;
        while(p<m && j<=child->end){
            if(P[p]!=X[j]){             // mismatch inside the edge
                return 0;               // -> not found
            }
            p++;
            j++;
        }

        cur=child;                      // P matched (part of) this edge
    }

    // P is fully matched: all leaves below cur are occurrences
    int count=0;
    collectLeaves(cur,pos,count);
    return count;
}

// --------------------------------------------------------------------------
// sortPositions: insertion sort, so positions print in increasing order
// --------------------------------------------------------------------------
void sortPositions(int A[],int count){
    for(int i=1;i<count;i++){
        int key=A[i];
        int j=i-1;
        while(j>=0 && A[j]>key){
            A[j+1]=A[j];
            j--;
        }
        A[j+1]=key;
    }
}

// --------------------------------------------------------------------------
// display: prints the trie sideways, one edge per line, as
//      (j,k) "label"   [suffix start]   <- the number only for leaves
// 'depth' controls the indentation.
// --------------------------------------------------------------------------
void display(Node* node,int depth){
    for(int c=0;c<SLOTS;c++){
        Node* child=node->children[c];
        if(child==NULL){
            continue;
        }

        for(int s=0;s<depth;s++){
            cout<<"    ";                               // indentation
        }
        cout<<"("<<child->start<<","<<child->end<<") \""
            <<X.substr(child->start,child->end-child->start+1)<<"\"";

        if(child->suffixIndex!=-1){                     // leaf
            cout<<"   ["<<child->suffixIndex<<"]";
        }
        cout<<"\n";

        display(child,depth+1);                         // its children
    }
}

// --------------------------------------------------------------------------
// freeTrie: deletes every node (they were made with 'new')
// --------------------------------------------------------------------------
void freeTrie(Node* node){
    for(int c=0;c<SLOTS;c++){
        if(node->children[c]!=NULL){
            freeTrie(node->children[c]);
        }
    }
    delete node;
}

// --------------------------------------------------------------------------
// isLowerWord: true if s is non-empty and only has letters a..z
// (so every character has a slot, and '$' can't appear inside the text)
// --------------------------------------------------------------------------
bool isLowerWord(string s){
    if(s.length()==0){
        return false;
    }
    for(int i=0;i<(int)s.length();i++){
        if(s[i]<'a' || s[i]>'z'){
            return false;
        }
    }
    return true;
}

// --------------------------------------------------------------------------
// main: reads the text, builds the trie, then a menu to display/search
// --------------------------------------------------------------------------
int main(){

    string text;
    cout<<"Enter text (lowercase letters only):-";
    cin>>text;

    if(!isLowerWord(text) || (int)text.length()>MAXN){
        cout<<"Invalid text\n";
        return 0;
    }

    X=text+"$";                 // add the end marker
    n=X.length();

    Node* root=buildSuffixTrie();

    int pos[MAXN+1];            // start positions of a pattern's occurrences
    int choice;
    string P;

    while(true){
        cout<<"\n1. Display suffix trie\n2. Search pattern\n3. Exit\n"
            <<"Enter choice:-";
        cin>>choice;

        if(choice==1){
            cout<<"\nText: "<<X<<"   (index 0 to "<<n-1<<")\n";
            cout<<"root\n";
            display(root,1);
        }
        else if(choice==2){
            cout<<"Enter pattern:-";
            cin>>P;

            if(!isLowerWord(P)){
                cout<<"Invalid pattern: lowercase letters only\n";
                continue;
            }

            int count=search(root,P,pos);

            if(count==0){
                cout<<"\""<<P<<"\" not found\n";
            }
            else{
                sortPositions(pos,count);
                cout<<"\""<<P<<"\" found "<<count<<" time(s) at position(s): ";
                for(int i=0;i<count;i++){
                    cout<<pos[i]<<" ";
                }
                cout<<"\n";
            }
        }
        else if(choice==3){
            break;
        }
        else{
            cout<<"Invalid choice\n";
        }
    }

    freeTrie(root);
    return 0;
}
