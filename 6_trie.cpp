/*
 * ==========================================================================
 *  PRACTICAL 6: TRIE DATA STRUCTURE (INSERT and SEARCH)
 *
 *  A trie stores words letter by letter, as paths from the root.
 *  Words that start the same way SHARE the same path:
 *
 *      insert "sand", "sam":       root
 *                                   └─ s
 *                                       └─ a
 *                                           ├─ n
 *                                           │   └─ d*
 *                                           └─ m*      (* = a word ends here)
 *
 *  Each node has:
 *    children[26] -> one slot per letter a..z
 *                    slot index = letter - 'a'   ('a'->0, 'm'->12, 'z'->25)
 *                    NULL = no path with that letter yet
 *    isEnd        -> true if a word ENDS at this node
 *
 *  INSERT: follow the letters; create a node where the path is missing;
 *          mark the last node isEnd = true.
 *  SEARCH: follow the letters; if the path breaks -> not found;
 *          at the end, found only if isEnd is true.
 *
 *  Time: O(L) for both, where L = length of the word
 *        (one step per letter, no matter how many words are stored)
 *  Reference: Goodrich, Tamassia & Mount, Ch 12, 12.5
 * ==========================================================================
 */

#include<iostream>          // cin and cout
#include<string>            // string
using namespace std;

const int ALPHABET = 26;    // lowercase letters a..z -> 26 slots per node

// --------------------------------------------------------------------------
// struct Node: one node of the trie
// --------------------------------------------------------------------------
struct Node{
    Node* children[ALPHABET];   // children[i] = node for letter ('a' + i),
                                // or NULL if that path doesn't exist yet
    bool isEnd;                 // true if some word ends exactly here
};

// --------------------------------------------------------------------------
// newNode: creates an empty node and returns a pointer to it.
// "Empty" = all 26 slots NULL (no children) and isEnd false (no word ends
// here yet). Created with 'new', so it lives on the heap until deleted.
// --------------------------------------------------------------------------
Node* newNode(){
    Node* n=new Node;

    for(int i=0;i<ALPHABET;i++){
        n->children[i]=NULL;    // no path for any letter yet
    }
    n->isEnd=false;             // no word ends here yet

    return n;
}

// --------------------------------------------------------------------------
// cleanWord: makes the word safe for the 26-slot array.
//   - turns uppercase into lowercase ('A' -> 'a'), so "Sam" == "sam"
//   - returns false if any character is not a letter (digit, space, '-')
// Without this, 'A' - 'a' = -32 would be an index OUTSIDE children[0..25]
// and the program would crash.
// --------------------------------------------------------------------------
bool cleanWord(string &word){
    if(word.length()==0){                   // empty word is not allowed
        return false;
    }

    for(int i=0;i<(int)word.length();i++){
        if(word[i]>='A' && word[i]<='Z'){
            word[i]=word[i]-'A'+'a';        // 'A'..'Z' -> 'a'..'z'
        }
        if(word[i]<'a' || word[i]>'z'){     // still not a..z -> reject
            return false;
        }
    }
    return true;
}

// --------------------------------------------------------------------------
// insert: adds a word to the trie.
// Walk down one letter at a time; where the path is missing, create a node.
// At the last letter, mark isEnd = true.
// --------------------------------------------------------------------------
void insert(Node* root,string word){

    Node* cur=root;                         // start at the root

    for(int i=0;i<(int)word.length();i++){

        int index=word[i]-'a';              // which slot: 'a'->0 ... 'z'->25

        if(cur->children[index]==NULL){     // no path for this letter yet
            cur->children[index]=newNode(); // create it
        }
        // else: the path already exists (shared prefix), just reuse it

        cur=cur->children[index];           // move down to that child
    }

    // We are at the node of the LAST letter: a word ends here.
    // (Needed even if no new node was created, e.g. inserting "ca"
    //  after "cat": the path exists, only isEnd changes.)
    cur->isEnd=true;
}

// --------------------------------------------------------------------------
// search: returns true if the word was inserted, false otherwise.
// Walk down one letter at a time:
//   - at a fork (e.g. after "sa": n or m) the letter itself picks the slot,
//     so we never try both branches
//   - if a slot is NULL, the path breaks -> word not in the trie
// At the end, the word exists only if isEnd is true
// ("san" has a path inside "sand" but isEnd is false -> not found).
// --------------------------------------------------------------------------
bool search(Node* root,string word){

    Node* cur=root;                         // start at the root

    for(int i=0;i<(int)word.length();i++){

        int index=word[i]-'a';              // which slot to follow

        if(cur->children[index]==NULL){     // path breaks here
            return false;                   // -> not found
        }

        cur=cur->children[index];           // move down
    }

    return cur->isEnd;                      // found only if a word ENDS here
}

// --------------------------------------------------------------------------
// freeTrie: deletes every node (memory made with 'new' must be freed).
// It frees the children first, then the node itself.
// --------------------------------------------------------------------------
void freeTrie(Node* node){
    if(node==NULL){
        return;
    }
    for(int i=0;i<ALPHABET;i++){
        freeTrie(node->children[i]);        // free each child's subtree
    }
    delete node;                            // then this node
}

// --------------------------------------------------------------------------
// main: menu to insert and search words
// --------------------------------------------------------------------------
int main(){

    Node* root=newNode();       // the root: empty, holds no letter itself
    int choice;
    string word;

    while(true){
        cout<<"\n1. Insert\n2. Search\n3. Exit\nEnter choice:-";
        cin>>choice;

        if(choice==1){
            cout<<"Enter word to insert:-";
            cin>>word;

            if(!cleanWord(word)){           // only letters a..z allowed
                cout<<"Invalid word: use letters only\n";
                continue;
            }
            insert(root,word);
            cout<<"\""<<word<<"\" inserted\n";
        }
        else if(choice==2){
            cout<<"Enter word to search:-";
            cin>>word;

            if(!cleanWord(word)){
                cout<<"Invalid word: use letters only\n";
                continue;
            }
            if(search(root,word)){
                cout<<"\""<<word<<"\" found\n";
            }
            else{
                cout<<"\""<<word<<"\" not found\n";
            }
        }
        else if(choice==3){
            break;                          // leave the menu loop
        }
        else{
            cout<<"Invalid choice\n";
        }
    }

    freeTrie(root);             // free all nodes before exiting
    return 0;
}
