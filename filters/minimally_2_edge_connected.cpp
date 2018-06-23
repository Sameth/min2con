#include"minimally_2_edge_connected.hpp"
#include<vector>
#include<algorithm>
#include<functional>

using std::vector;
using std::min;
using std::function;

minimally_2_connected_f = [](vector<vector<bool> >& adjmatrix){return minimallyconnected(adjmatrix);};

// Hlavna rekurzivna funkcia pre hladanie mostov. Vrati najmensie cislo
// vrchola po spatnej hrane. Ak objavi most, nastavi bridge na true.
ll dfs(int v, int pred, int& num, vector<int>& renumber, int actn, bool& bridge, vector<vector<bool> >& adjmatrix) {
    renumber [v] = num;
    ll minup = num;
    For(i, actn) if (i != pred && adjmatrix [v][i]) {
        if (renumber [i] == -1) {
            num ++;
            minup = min(dfs(i, v, num, renumber, actn, bridge, adjmatrix), minup);
        }
        else {
            minup = min(minup, renumber [i]);
        }
    }
    if (v != 0 && minup == renumber [v]) bridge = true;
    return minup;
}

// Zisti, ci je graf aktualne ulozeny v adjmatrix minimalne 2-hranovo-suvisly
// Skusi odstranit kazdu hranu a hlada most.
bool minimally_2_connected(vector<vector<bool> >& adjmatrix) {
    int n = adjmatrix.size();
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if (adjmatrix [i][j]) {
                // Sanity check
                if (i == j) {
                    cerr << "Slucka v grafe!!!\n";
                    exit(1);
                }
                // Cisovanie v dfs-strome
                vector<int> renumber(n, -1);
                // Odstranime hranu
                adjmatrix [i][j] = false;
                adjmatrix [j][i] = false;

                // Inicializacia poli a premennych
                ll num = 0;
                bool bridge = false;

                // Pocitanie
                dfs(0, -1, num, renumber, n, bridge, adjmatrix);

                // Vratime hranu
                adjmatrix [i][j] = true;
                adjmatrix [j][i] = true;

                // Ak nie je most, koncime.
                if (!bridge) {
                    return false;
                }
            }
        }
    }
    return true;
}

