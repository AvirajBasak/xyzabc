/*
 * ==========================================================================
 *  PRACTICAL 5: B-TREE (INSERT, SEARCH, DISPLAY)
 *
 *  A B-tree is a balanced search tree made for data on disk: each node
 *  holds MANY keys, so the tree is short and few disk reads are needed.
 *
 *  This program uses the MINIMUM DEGREE t (t >= 2):
 *    - every node holds at most 2t-1 keys (so at most 2t children)
 *    - every node except the root holds at least t-1 keys
 *    - keys in a node are sorted; a node with k keys has k+1 children
 *    - all leaves are on the same level
 *  (In Drozdek's terms this is a B-tree of order m = 2t.)
 *
 *  INSERT (top-down): walk from the root to the correct leaf. Before
 *  stepping into a FULL child (2t-1 keys), SPLIT it: its middle key goes
 *  up into the parent, the rest becomes two nodes of t-1 keys each.
 *  If the root itself is full, split it first: that is the only way the
 *  tree grows taller.
 *
 *  SEARCH: in each node find the first key >= k; if equal -> found,
 *  otherwise go down the child between the neighbouring keys.
 *
 *  Time: O(t log_t n) for search and insert; height O(log_t n)
 *  Reference: Drozdek, Ch 7, 7.1.1
 * ==========================================================================
 */

#include<iostream>          // cin and cout
using namespace std;

const int MAXT = 10;        // largest minimum degree allowed

int t;                      // minimum degree, read from the user

// --------------------------------------------------------------------------
// struct Node: one B-tree node
// --------------------------------------------------------------------------
struct Node{
    int keys[2*MAXT-1];         // the keys, kept in sorted order
    Node* child[2*MAXT];        // child[i] holds keys between keys[i-1]
                                // and keys[i]
    int n;                      // how many keys are in use
    bool leaf;                  // true if this node has no children
};

// --------------------------------------------------------------------------
// newNode: an empty node (no keys, no children)
// --------------------------------------------------------------------------
Node* newNode(bool leaf){
    Node* x=new Node;
    x->n=0;
    x->leaf=leaf;
    for(int i=0;i<2*MAXT;i++){
        x->child[i]=NULL;
    }
    return x;
}

// --------------------------------------------------------------------------
// search: returns true if key k is in the subtree of x
// --------------------------------------------------------------------------
bool search(Node* x,int k){
    int i=0;

    // skip all keys smaller than k
    while(i<x->n && k>x->keys[i]){
        i++;
    }

    if(i<x->n && k==x->keys[i]){    // found it in this node
        return true;
    }
    if(x->leaf){                    // nowhere further to go
        return false;
    }
    return search(x->child[i],k);   // go down between keys[i-1] and keys[i]
}

// --------------------------------------------------------------------------
// splitChild: x->child[i] is FULL (2t-1 keys). Split it in two:
//   y = the full child, z = new node for y's upper half.
//   y keeps keys[0..t-2], z gets keys[t..2t-2],
//   the middle key y->keys[t-1] moves UP into x at position i.
//
//   before:   x: [ ... | ... ]            after:  x: [ ... | M | ... ]
//                    |                                   /     \   (split)
//              y: [ A B M C D ]                   y: [A B]   z: [C D]
// --------------------------------------------------------------------------
void splitChild(Node* x,int i){
    Node* y=x->child[i];
    Node* z=newNode(y->leaf);

    z->n=t-1;                               // z gets the upper t-1 keys
    for(int j=0;j<t-1;j++){
        z->keys[j]=y->keys[j+t];
    }
    if(!y->leaf){                           // and the upper t children
        for(int j=0;j<t;j++){
            z->child[j]=y->child[j+t];
        }
    }
    y->n=t-1;                               // y keeps the lower t-1 keys

    // make room in x for the new child z (right of y)
    for(int j=x->n;j>=i+1;j--){
        x->child[j+1]=x->child[j];
    }
    x->child[i+1]=z;

    // make room in x for the middle key and move it up
    for(int j=x->n-1;j>=i;j--){
        x->keys[j+1]=x->keys[j];
    }
    x->keys[i]=y->keys[t-1];
    x->n++;
}

// --------------------------------------------------------------------------
// insertNonFull: inserts k into the subtree of x, where x is NOT full
// --------------------------------------------------------------------------
void insertNonFull(Node* x,int k){
    int i=x->n-1;

    if(x->leaf){
        // shift bigger keys right (like insertion sort) and drop k in
        while(i>=0 && x->keys[i]>k){
            x->keys[i+1]=x->keys[i];
            i--;
        }
        x->keys[i+1]=k;
        x->n++;
        return;
    }

    // find the child that should receive k
    while(i>=0 && x->keys[i]>k){
        i--;
    }
    i++;

    // split that child first if it is full, so there is room below
    if(x->child[i]->n==2*t-1){
        splitChild(x,i);
        if(k>x->keys[i]){               // the middle key moved up into x:
            i++;                        // k may now belong right of it
        }
    }
    insertNonFull(x->child[i],k);
}

// --------------------------------------------------------------------------
// insert: inserts k into the tree; returns the (possibly new) root
// --------------------------------------------------------------------------
Node* insert(Node* root,int k){
    if(root->n==2*t-1){                 // root is full: tree grows taller
        Node* s=newNode(false);         // new root above the old one
        s->child[0]=root;
        splitChild(s,0);
        insertNonFull(s,k);
        return s;
    }
    insertNonFull(root,k);
    return root;
}

// --------------------------------------------------------------------------
// traverse: prints all keys in sorted order (in-order walk)
// --------------------------------------------------------------------------
void traverse(Node* x){
    for(int i=0;i<x->n;i++){
        if(!x->leaf){
            traverse(x->child[i]);      // keys smaller than keys[i]
        }
        cout<<x->keys[i]<<" ";
    }
    if(!x->leaf){
        traverse(x->child[x->n]);       // keys bigger than the last key
    }
}

// --------------------------------------------------------------------------
// display: prints the tree level by level, one node as [k1 k2 ...]
// --------------------------------------------------------------------------
void displayLevel(Node* x,int level,int target){
    if(level==target){
        cout<<"[";
        for(int i=0;i<x->n;i++){
            cout<<x->keys[i]<<(i<x->n-1?" ":"");
        }
        cout<<"] ";
        return;
    }
    if(!x->leaf){
        for(int i=0;i<=x->n;i++){
            displayLevel(x->child[i],level+1,target);
        }
    }
}

void display(Node* root){
    int h=0;                            // height: follow the leftmost path
    for(Node* x=root;!x->leaf;x=x->child[0]){
        h++;
    }
    for(int level=0;level<=h;level++){
        cout<<"Level "<<level<<": ";
        displayLevel(root,0,level);
        cout<<"\n";
    }
}

// --------------------------------------------------------------------------
// freeTree: deletes every node
// --------------------------------------------------------------------------
void freeTree(Node* x){
    if(!x->leaf){
        for(int i=0;i<=x->n;i++){
            freeTree(x->child[i]);
        }
    }
    delete x;
}

// --------------------------------------------------------------------------
// main: menu to insert, search and display
// --------------------------------------------------------------------------
int main(){
    cout<<"Enter minimum degree t (2 to "<<MAXT<<"):-";
    cin>>t;
    if(t<2 || t>MAXT){
        cout<<"Invalid degree\n";
        return 0;
    }

    Node* root=newNode(true);           // empty tree: one empty leaf
    int choice,k;

    while(true){
        cout<<"\n1. Insert\n2. Search\n3. Display\n4. Exit\nEnter choice:-";
        cin>>choice;

        if(choice==1){
            cout<<"Enter key:-";
            cin>>k;
            if(search(root,k)){         // keep keys unique
                cout<<k<<" is already in the tree\n";
            }
            else{
                root=insert(root,k);
                cout<<k<<" inserted\n";
            }
        }
        else if(choice==2){
            cout<<"Enter key:-";
            cin>>k;
            if(search(root,k)){
                cout<<k<<" found\n";
            }
            else{
                cout<<k<<" not found\n";
            }
        }
        else if(choice==3){
            if(root->n==0){
                cout<<"Tree is empty\n";
            }
            else{
                display(root);
                cout<<"Sorted: ";
                traverse(root);
                cout<<"\n";
            }
        }
        else if(choice==4){
            break;
        }
        else{
            cout<<"Invalid choice\n";
        }
    }

    freeTree(root);
    return 0;
}
