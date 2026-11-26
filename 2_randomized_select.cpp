/*
 * ==========================================================================
 *  PRACTICAL 2: RANDOMIZED SELECT (pivot = FIRST element)
 *  Finds the i-th smallest element of an array WITHOUT sorting it fully.
 *
 *  Idea:
 *    1. Partition the array around a RANDOM pivot (same Partition as
 *       Randomized Quick Sort). The pivot lands at its final position q.
 *    2. k = number of elements in A[p..q] = rank of the pivot.
 *         - if i == k : the pivot IS the answer
 *         - if i <  k : the answer is in the LEFT part
 *         - if i >  k : the answer is in the RIGHT part, and it is the
 *                       (i - k)-th smallest there
 *    3. Recurse on ONLY ONE side (Quick Sort recurses on both sides).
 *
 *  Expected time : O(n)      (because only one side is searched)
 *  Worst case    : O(n^2)    (very unlikely)
 *  Reference     : CLRS Ch 9, 9.2 (Selection in expected linear time)
 * ==========================================================================
 */

#include <iostream>   // for cin and cout
#include <cstdlib>    // for rand() and srand()
#include <ctime>      // for time(), used to seed the random generator
using namespace std;

// --------------------------------------------------------------------------
// mySwap: exchanges two integers (by reference, so the array changes)
// --------------------------------------------------------------------------
void mySwap(int &a, int &b)
{
    int temp = a;
    a = b;
    b = temp;
}

// --------------------------------------------------------------------------
// Partition: rearranges A[p..r] around the pivot A[p].
// Afterwards:  A[p..q-1] <= pivot,  A[q] = pivot,  A[q+1..r] > pivot
// Returns q, the pivot's final position.
// (Exactly the same function as in Practical 1.)
// --------------------------------------------------------------------------
int Partition(int A[], int p, int r)
{
    int pivot = A[p];   // first element is the pivot
    int i = p;          // last index of the "<= pivot" region (empty at start)

    for (int j = p + 1; j <= r; j++)
    {
        if (A[j] <= pivot)      // A[j] belongs on the left side
        {
            i++;
            mySwap(A[i], A[j]);
        }
    }

    mySwap(A[p], A[i]); // put the pivot between the two regions
    return i;
}

// --------------------------------------------------------------------------
// rand_Partition: picks a random index in p..r, moves that element to the
// front, then partitions around it.
// --------------------------------------------------------------------------
int rand_Partition(int A[], int p, int r)
{
    int i = p + rand() % (r - p + 1);   // random index between p and r
    mySwap(A[i], A[p]);                 // random element becomes the pivot
    return Partition(A, p, r);
}

// --------------------------------------------------------------------------
// rand_Select: returns the i-th smallest element of A[p..r].
// i is counted from 1 WITHIN the current subarray
// (i = 1 means the smallest element of A[p..r]).
// --------------------------------------------------------------------------
int rand_Select(int A[], int p, int r, int i)
{
    // Base case: only one element left, so it must be the answer
    if (p == r)
        return A[p];

    // Put a random pivot at its final position q
    int q = rand_Partition(A, p, r);

    // k = how many elements are in A[p..q]
    //   = the rank of the pivot inside this subarray
    // (q - p elements are smaller/equal and to its left, +1 for the pivot)
    int k = q - p + 1;

    if (i == k)
    {
        // The pivot is exactly the i-th smallest: done
        return A[q];
    }
    else if (i < k)
    {
        // The answer is among the elements LEFT of the pivot.
        // Its rank there is still i.
        return rand_Select(A, p, q - 1, i);
    }
    else
    {
        // The answer is among the elements RIGHT of the pivot.
        // We skip k elements (left part + pivot), so in the right part
        // we now want the (i - k)-th smallest.
        return rand_Select(A, q + 1, r, i - k);
    }
}

// --------------------------------------------------------------------------
// main: reads the array and i, then prints the i-th smallest element.
// --------------------------------------------------------------------------
int main()
{
    srand(time(0));   // seed once so the pivots differ between runs

    int n;
    cout << "Enter number of elements: ";
    cin >> n;

    if (n <= 0)       // guard against an invalid size
    {
        cout << "Invalid array size" << endl;
        return 0;
    }

    int *A = new int[n];   // dynamic array of size n

    cout << "Enter array elements: ";
    for (int j = 0; j < n; j++)
    {
        cin >> A[j];
    }

    int i;
    cout << "Enter i (1 to " << n << "): ";
    cin >> i;

    if (i < 1 || i > n)    // i must be a valid rank
    {
        cout << "Invalid value of i" << endl;
        delete[] A;
        return 0;
    }

    int result = rand_Select(A, 0, n - 1, i);   // search the whole array

    cout << "\nThe " << i << "-th smallest element is: " << result << endl;

    delete[] A;   // free the memory
    return 0;
}
