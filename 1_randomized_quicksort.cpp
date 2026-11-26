/*
 * ==========================================================================
 *  PRACTICAL 1: RANDOMIZED QUICK SORT (pivot = FIRST element)
 *  Sorts an array and reports the number of comparisons made.
 *
 *  Idea (divide and conquer):
 *    1. Pick a RANDOM element and move it to the FRONT of the subarray.
 *    2. Use that front element as the PIVOT and PARTITION the subarray:
 *         [ elements <= pivot ]  pivot  [ elements > pivot ]
 *       After this, the pivot is at its FINAL sorted position.
 *    3. Recursively sort the left part and the right part.
 *
 *  Expected time : O(n log n) on every input (because the pivot is random)
 *  Worst case    : O(n^2) (very unlikely)
 *  Reference     : CLRS Ch 7, 7.3 (Randomized version of Quicksort)
 * ==========================================================================
 */

#include <iostream>   // for cin and cout
#include <cstdlib>    // for rand() and srand()
#include <ctime>      // for time(), used to seed the random generator
using namespace std;  // so we can write cout instead of std::cout

// --------------------------------------------------------------------------
// GLOBAL COUNTER
// Counts how many times an array element is compared with the pivot.
// It is global so that every function can increase it.
// --------------------------------------------------------------------------
int comparisons = 0;

// --------------------------------------------------------------------------
// mySwap: exchanges the values of two integers.
// The '&' means a and b are REFERENCES to the original variables,
// so the change happens in the real array, not in copies.
// (Named mySwap so it does not clash with the library's std::swap.)
// --------------------------------------------------------------------------
void mySwap(int &a, int &b)
{
    int temp = a;   // keep a's value safe
    a = b;          // copy b into a
    b = temp;       // copy the saved value into b
}

// --------------------------------------------------------------------------
// Partition: rearranges the subarray A[p..r] around the pivot A[p].
//
// During the loop the subarray looks like this:
//
//     | pivot | <= pivot  | > pivot  | not checked yet |
//       p      p+1 ... i   i+1 ... j-1  j ........... r
//
// Returns the final index of the pivot.
// --------------------------------------------------------------------------
int Partition(int A[], int p, int r)
{
    int pivot = A[p];   // the first element is the pivot

    int i = p;          // i = last index of the "<= pivot" region.
                        // It starts at p (the pivot itself), so the
                        // "<= pivot" region is empty at the beginning.

    // Look at every element after the pivot, one by one
    for (int j = p + 1; j <= r; j++)
    {
        comparisons++;  // we are about to compare A[j] with the pivot

        if (A[j] <= pivot)      // A[j] belongs on the LEFT side
        {
            i++;                // make room: grow the "<= pivot" region by 1
            mySwap(A[i], A[j]); // move A[j] into that region
        }
        // else: A[j] > pivot, so it stays where it is, which is
        // automatically the end of the "> pivot" region
    }

    // Now A[p+1..i] are all <= pivot and A[i+1..r] are all > pivot.
    // Swap the pivot with A[i] (the last "<= pivot" element) so the
    // pivot sits exactly between the two regions.
    mySwap(A[p], A[i]);

    return i;           // the pivot's final, correct position
}

// --------------------------------------------------------------------------
// rand_Partition: chooses a RANDOM pivot, then calls Partition.
// This is the only thing that makes the algorithm "randomized".
// --------------------------------------------------------------------------
int rand_Partition(int A[], int p, int r)
{
    // rand() % (r - p + 1) gives a number from 0 to (r - p).
    // Adding p shifts it into the range p .. r.
    int i = p + rand() % (r - p + 1);

    // Move the randomly chosen element to the FRONT,
    // because Partition always uses A[p] as the pivot.
    mySwap(A[i], A[p]);

    return Partition(A, p, r);
}

// --------------------------------------------------------------------------
// rand_QuickSort: sorts A[p..r] recursively.
// --------------------------------------------------------------------------
void rand_QuickSort(int A[], int p, int r)
{
    // Base case: if p >= r the subarray has 0 or 1 element,
    // which is already sorted, so do nothing.
    if (p < r)
    {
        // Place a random pivot at its final position q
        int q = rand_Partition(A, p, r);

        // Sort the elements to the LEFT of the pivot
        rand_QuickSort(A, p, q - 1);

        // Sort the elements to the RIGHT of the pivot
        // (q itself is skipped: the pivot is already in place)
        rand_QuickSort(A, q + 1, r);
    }
}

// --------------------------------------------------------------------------
// main: reads the array, sorts it, prints the result and the comparisons.
// --------------------------------------------------------------------------
int main()
{
    // Seed the random number generator with the current time,
    // so each run picks DIFFERENT random pivots.
    srand(time(0));

    int n;   // number of elements

    cout << "Enter number of elements: ";
    cin >> n;

    // Guard against an invalid size (a negative size would crash 'new')
    if (n <= 0)
    {
        cout << "Invalid array size" << endl;
        return 0;
    }

    // Create an array of size n at run time (dynamic memory)
    int *A = new int[n];

    cout << "Enter array elements: ";

    for (int i = 0; i < n; i++)
    {
        cin >> A[i];   // read each element
    }

    // Print the array before sorting
    cout << "\nOriginal Array: ";

    for (int i = 0; i < n; i++)
    {
        cout << A[i] << " ";
    }

    comparisons = 0;               // reset the counter before sorting

    rand_QuickSort(A, 0, n - 1);   // sort the whole array: indexes 0 to n-1

    // Print the array after sorting
    cout << "\n\nSorted Array: ";

    for (int i = 0; i < n; i++)
    {
        cout << A[i] << " ";
    }

    // Report the number of comparisons
    // (it can differ between runs because the pivots are random)
    cout << "\n\nNumber of Comparisons: "
         << comparisons << endl;

    delete[] A;   // free the memory created with 'new'

    return 0;
}
