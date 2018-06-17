#include "traceswr.hpp"

using std::min;
using std::max;
using std::cin;
using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::to_string;

int n;
std::vector<std::vector<bool> > adjmatrix;
ll cyclepp, bestother = -1;

std::set<std::vector<std::vector<bool> > > canongs;
std::string path;

ll grafov = 0;

// Hlavna rekurzivna funkcia pre hladanie mostov. Vrati najmensie cislo
// vrchola po spatnej hrane. Ak objavi most, nastavi bridge na true.
ll dfs(int v, int pred, ll& num, vi& renumber, int actn, bool& bridge) {
    renumber [v] = num;
    ll minup = num;
    For(i, actn) if (i != pred && adjmatrix [v][i]) {
        if (renumber [i] == -1) {
            num ++;
            minup = min(dfs(i, v, num, renumber, actn, bridge), minup);
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
bool minimallyconnected(int actn) {
    For(i, actn) For(j, actn) if (adjmatrix [i][j]) {
        // Sanity check
        if (i == j) {
            cerr << "Slucka v grafe!!!\n";
            exit(1);
        }
        // Cisovanie v dfs-strome
        vi renumber(actn, -1);
        // Odstranime hranu
        adjmatrix [i][j] = false;
        adjmatrix [j][i] = false;

        // Inicializacia poli a premennych
        ll num = 0;
        bool bridge = false;

        // Pocitanie
        dfs(0, -1, num, renumber, actn, bridge);

        // Vratime hranu
        adjmatrix [i][j] = true;
        adjmatrix [j][i] = true;

        // Ak nie je most, koncime.
        if (!bridge) {
            return false;
        }
    }
    return true;
}

int recursive_maxdist(int v, int t, ll mask, vvi& memo, int actn) {
/*    if (path == "{5,(3,0,0),") {
        cerr << v << ' ' << t << ' ' << mask << ": ";
        For(i, actn) {
            cerr << ' ' << ((mask & (1 << i)) != 0);
        }
        cerr << endl;
    }*/
    if (memo [v][mask] != -1000000) return memo [v][mask];
    if (v == t) return 0;

    int res = -10000;
    For(i, actn) {
        if (adjmatrix [v][i] && (mask & (1 << i)) == 0) {
            res = max(res, 1 + recursive_maxdist(i, t, mask | (1<<i), memo, actn));
        }
    }
//    if (path == "{5,(3,0,0),") cerr << "RES: " << v << ' ' << mask << ' ' << res << endl;
    memo [v][mask] = res;
    return res;
}

void compute_maxdists(vvi& result) {
/*    if (path == "{5,(3,0,0),") {
        For(i, n) {
            For(j,n) cout << adjmatrix [i][j];
            cout << endl;
        }
    }*/
    int actn = result.size();
    For(i, actn) {
//        if (path == "{5,(3,0,0),") cerr << "ITER " << i << "!\n";
        vvi memo(actn, vi(1<<actn, -1000000));
        For(j, i) {
            result [i][j] = recursive_maxdist(j, i, 1<<j, memo, actn);
//            if (path == "{5,(3,0,0),") cerr << "MAXRES " << i << ' ' << j << ' ' << result [i][j] << "!\n";
            result [j][i] = result [i][j];
        }

    }
}


void generate(int next, int maxcycle, pii minadd) {
//    cout << path << endl;
    // Koncova podmienka
    if (next == n) {
        // Grafy, ktore nie su minimalne 2-hranovo suvisle, nema zmysel kontrolovat
        if (!minimallyconnected(next)) return;

        // Navysime pocitadlo grafov ;-)
        grafov ++;

        // Sanity check
        For(i, n) For(j, n) if (adjmatrix [i][j] != adjmatrix [j][i]) {
            cerr << "Nesymetricka matica susednosti!\n";
            exit(0);
        }
    
        auto canon = canonical(adjmatrix);
        if (canongs.find(canon) ==canongs.end()) {
            canongs.insert(canon);
        }

        return;
    }


    // Zaciname od nicoho
    if (next == 0) {
        // Musime si zvolit velkost prveho cyklu
        for(int i = 3; i <= n; i++) {
            // Nastavime cyklus
            For(j, i) {
                adjmatrix [j][(j+1)%i] = true;
                adjmatrix [(j+1)%i][j] = true;
            }

            path = std::string("{") + std::to_string(i) + std::string(",");

            generate(i, i, minadd);

            // Odstranime cyklus
            For(j, i) {
                adjmatrix [j][(j+1)%i] = false;
                adjmatrix [(j+1)%i][j] = false;
            }
        }
        return;
    }

    // Ak graf nie je minimalne 2-hranovo suvisly, uz nikdy nebude
    if (!minimallyconnected(next)) return;

    vvi dists(next, vi(next, -1));

    if (n - next > 3) compute_maxdists(dists);
    else {
        For(i, next) For(j, next) dists [i][j] = 1023456789LL;
        For(i, next) For(j, next) if (adjmatrix [i][j]) dists [i][j] = 1;
        For(i, next) dists [i][i] = 0;
        For(k, next) For(i, next) For(j, next) dists [i][j] = min(dists [i][j], dists [i][k] + dists [k][j]);
    }


    // Najprv vyskusame cykly do seba
    if (n - next > 1) {
        // Dlzka cyklu - pocet novych vrcholov
        for (int i = 2; i <= min(n - next, maxcycle - 1); i++) {
            // Spojime nove vrcholy v cykle
            For(j, i - 1) {
                adjmatrix [next + j][next + j + 1] = true;
                adjmatrix [next + j + 1][next + j] = true;
            }

            // Vrchol, kde pripojime cyklus
            For(j, next) {
                // Zakazeme pridanie na skorsiu dvojicu pozicii
                if (mp(j, j) >= minadd && (minadd != mp(-1, -1) || j == 0)) {
                    // Pripojime zaciatok a koniec
                    adjmatrix [next][j] = true;
                    adjmatrix [j][next] = true;
                    adjmatrix [next + i - 1][j] = true;
                    adjmatrix [j][next + i - 1] = true;


                    string added = string("(") + to_string(i) + string(",") + to_string(j) + string(",") + to_string(j) + string("),");
                    path = path + added;

                    // Rekurzivne volanie
                    generate (next + i, maxcycle, {j,j});

                    for (char c : added) path.pop_back();

                    // Odpojime zaciatok a koniec
                    adjmatrix [next][j] = false;
                    adjmatrix [j][next] = false;
                    adjmatrix [next + i - 1][j] = false;
                    adjmatrix [j][next + i - 1] = false;
                }
            }

            // Rozpojime nove vrcholy v cykle
            For(j, i - 1) {
                adjmatrix [next + j][next + j + 1] = false;
                adjmatrix [next + j + 1][next + j] = false;
            }
        }
    }

    // Vyskusame pridanie ucha
    // Cyklus cez dlzku ucha (pocet vrcholov)
    for (int i = 1; i <= n - next; i++) {
        // Spojime vrcholy v uchu
        For(j, i - 1) {
            adjmatrix [next + j][next + j + 1] = true;
            adjmatrix [next + j + 1][next + j] = true;
        }

        // Cykly cez konce ucha
        // Koniec nemoze byt ten isty a konce nemozu susedit (=> trivialne odstranitelna hrana)
        For(j, next) For(k, j) if (mp(j, k) >= minadd && !adjmatrix [j][k] && (minadd != mp(-1, -1) || k == 0)) {
            // Ak by sme pridanim ucha trivialne zvacsili maximalny cyklus,
            // koncime. Tuto moznost preskumame, ked zacneme vacsim cyklom.
            if (dists [j][k] + i + 1 > maxcycle) continue;

            // Spojime zaciatok a koniec ucha
            adjmatrix [j][next] = true;
            adjmatrix [next][j] = true;
            adjmatrix [k][next + i - 1] = true;
            adjmatrix [next + i - 1][k] = true;

            string added = string("(") + to_string(i) + string(",") + to_string(j) + string(",") + to_string(k) + string("),");
            path = path + added;

            generate(next + i, maxcycle, {j, k});

            for (char c : added) path.pop_back();

            // Odpojime zaciatok a koniec ucha
            adjmatrix [j][next] = false;
            adjmatrix [next][j] = false;
            adjmatrix [k][next + i - 1] = false;
            adjmatrix [next + i - 1][k] = false;
        }


        // Rozpojime vrcholy v uchu
        For(j, i - 1) {
            adjmatrix [next + j][next + j + 1] = false;
            adjmatrix [next + j + 1][next + j] = false;
        }
        
    }
}


int main () {
    cin >> n;

    vvi edges(n);
    adjmatrix.resize(n, std::vector<bool>(n, false));

    For(i, n) {
        edges [i].push_back((i+1)%n);
        edges [(i+1)%n].push_back(i);
    }
    cyclepp = count_pp(edges);

    generate(0, -1, {-1,-1});
    cout << "Mame " << grafov << " grafov.\n";
    cout << "Mame " << canongs.size() << " neizomorfnych grafov\n";

    std::string gfilename;
    if (!(cin >> gfilename)) return 0;
    std::ifstream graphfile(gfilename, std::ifstream::in);
    std::string graphdesc;
    while (graphfile >> graphdesc) {
        std::vector<std::vector<bool> > nadjmatrix(n, std::vector<bool>(n, false));
        for(int i = 1; i <graphdesc.size(); i++) {
            if (graphdesc [i] == '{') {
                i++;
                int x = 0, y = 0;
                while (std::isdigit(graphdesc [i])) {
                    x *= 10;
                    x += graphdesc [i] - '0';
                    i++;
                }
                if (graphdesc [i] != ',') {
                    cerr << "CHYBA NACITANIA!\n";
                    exit(1);
                }
                i++;
                while (std::isdigit(graphdesc [i])) {
                    y *= 10;
                    y += graphdesc [i] - '0';
                    i++;
                }
                nadjmatrix [x][y] = true;
                nadjmatrix [y][x] = true;
            }
        }
        auto canon = canonical(nadjmatrix);
        if (canongs.find(canon) == canongs.end()) {
            cout << graphdesc << endl;
        }
    }
}
