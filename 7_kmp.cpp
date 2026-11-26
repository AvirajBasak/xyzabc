/*
 * ==========================================================================
 *  PRACTICAL 7: PATTERN SEARCH USING THE KMP ALGORITHM
 *
 *  Find every position where pattern P occurs in text T.
 *
 *  BRUTE FORCE: on a mismatch, shift P by one and start comparing again
 *  from P[0]. It re-checks characters it already matched -> O(n*m).
 *
 *  KMP: first build the FAILURE FUNCTION f[] of the pattern:
 *      f[j] = length of the longest proper prefix of P[0..j]
 *             that is also a suffix of P[0..j]
 *  On a mismatch after matching j characters, P can be shifted so that
 *  its first f[j-1] characters line up with what was just matched.
 *  The text index i NEVER moves backwards -> O(n + m).
 *
 *  Example: P = ababaca
 *      j    : 0 1 2 3 4 5 6
 *      P[j] : a b a b a c a
 *      f[j] : 0 0 1 2 3 0 1
 *
 *  Reference: Goodrich, Tamassia & Mount, Ch 12, 12.3.1 and 12.3.3
 * ==========================================================================
 */

#include<iostream>          // cin and cout
#include<string>            // string
using namespace std;

const int MAXM = 1000;      // longest pattern allowed

// --------------------------------------------------------------------------
// failureFunction: fills f[0..m-1] for pattern P.
//   i = position in P we are computing f for
//   j = length of the current matching prefix
// --------------------------------------------------------------------------
void failureFunction(string P,int f[]){
    int m=P.length();

    f[0]=0;                 // one character: no proper prefix
    int i=1;
    int j=0;

    while(i<m){
        if(P[i]==P[j]){             // prefix of length j grows by one
            f[i]=j+1;
            i++;
            j++;
        }
        else if(j>0){               // fall back to a shorter prefix
            j=f[j-1];               // (i stays where it is)
        }
        else{                       // no prefix matches at all
            f[i]=0;
            i++;
        }
    }
}

// --------------------------------------------------------------------------
// kmpSearch: prints every position where P occurs in T.
//   i = position in the text      (only moves forward)
//   j = number of pattern characters matched so far
// Returns how many matches were found.
// --------------------------------------------------------------------------
int kmpSearch(string T,string P){
    int n=T.length();
    int m=P.length();
    int f[MAXM];
    int count=0;

    failureFunction(P,f);

    int i=0;
    int j=0;

    while(i<n){
        if(T[i]==P[j]){             // characters match: move both forward
            i++;
            j++;

            if(j==m){               // the WHOLE pattern matched
                cout<<"Pattern found at position "<<i-m<<endl;
                count++;
                j=f[j-1];           // keep going for more (overlapping) matches
            }
        }
        else if(j>0){               // mismatch after some matches:
            j=f[j-1];               // reuse what was matched; i does NOT move
        }
        else{                       // mismatch on the first character:
            i++;                    // just move to the next text character
        }
    }
    return count;
}

// --------------------------------------------------------------------------
// main: reads text and pattern (whole lines, spaces allowed)
// --------------------------------------------------------------------------
int main(){
    string T,P;

    cout<<"Enter text:-";
    getline(cin,T);
    cout<<"Enter pattern:-";
    getline(cin,P);

    if(P.length()==0 || (int)P.length()>MAXM){
        cout<<"Invalid pattern\n";
        return 0;
    }

    // print the failure function (useful to check by hand)
    int f[MAXM];
    failureFunction(P,f);
    cout<<"\nFailure function:\n";
    for(int k=0;k<(int)P.length();k++){
        cout<<P[k]<<" ";
    }
    cout<<"\n";
    for(int k=0;k<(int)P.length();k++){
        cout<<f[k]<<" ";
    }
    cout<<"\n\n";

    if(kmpSearch(T,P)==0){
        cout<<"Pattern not found\n";
    }
    return 0;
}
