# DSC13: Algorithms and Advanced Data Structures — Practicals

C++ programs for the eight practicals in the DSC13 syllabus. Each file is commented and compiles with `g++` on its own.

| # | Practical | File | Reference |
|---|---|---|---|
| 1 | Sort an array using randomized quicksort and report the number of comparisons | `1_randomized_quicksort.cpp` | CLRS 7.3 |
| 2 | Find the i-th smallest element using randomized select | `2_randomized_select.cpp` | CLRS 9.2 |
| 3 | Minimum spanning tree using Kruskal's algorithm (union-find) | `3_kruskal.cpp` | Kleinberg-Tardos 4.6 |
| 4 | Shortest paths from a source using Bellman-Ford | `4_bellman_ford.cpp` | Kleinberg-Tardos 6.8 |
| 5 | B-tree (insert, search, display) | `5_btree.cpp` | Drozdek 7.1.1 |
| 6 | Trie supporting insert and search | `6_trie.cpp` | Goodrich 12.5 |
| 7 | Pattern search using KMP | `7_kmp.cpp` | Goodrich 12.3.3 |
| 8 | Suffix trie (compressed, with (j, k) edge labels) | `8_suffix_trie.cpp` | Goodrich 12.5.3 |

## Compile and run

```
g++ 1_randomized_quicksort.cpp -o quicksort
./quicksort
```

## Sample inputs

**3. Kruskal** (5 vertices, 6 edges; MST weight 15)
```
5 6
0 1 4
0 2 8
1 2 3
1 4 6
2 3 2
3 4 10
```

**4. Bellman-Ford** (A=0, B=1, C=2, source 0; prints 0, 10, 30)
```
3 3
0 1 10
1 2 20
2 1 -10
0
```
Change the last edge weight to `-30` to see the negative weight cycle message.

**7. KMP**: text `abababacaba`, pattern `ababaca` (found at position 2).

**8. Suffix trie**: text `banana`, then search `ana` (found at positions 1 and 3).
