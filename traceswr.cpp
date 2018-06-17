#include "traceswr.hpp"

#define For(i,n) for(int i = 0; i < n; i++)

std::vector<std::vector<bool> > canonical(std::vector<std::vector<bool> >& adjmatrix) {
    DYNALLSTAT(int,lab1,lab1_sz);
    DYNALLSTAT(int,ptn,ptn_sz);
    DYNALLSTAT(int,orbits,orbits_sz);
    DYNALLSTAT(int,map,map_sz);
    static DEFAULTOPTIONS_TRACES(options);
    TracesStats stats;
 /* Declare and initialize sparse graph structures */
    SG_DECL(sg1);
    SG_DECL(cg1);

    options.getcanon=TRUE;

    int n = adjmatrix.size(), m=0;
    For(i, n) {
        For(j, n) if (adjmatrix [i][j]) m++;
    }
    SG_ALLOC(sg1, n, m, "Failed to allocate space for graph");


    DYNALLOC1(int,lab1,lab1_sz,n,"malloc");
    DYNALLOC1(int,ptn,ptn_sz,n,"malloc");
    DYNALLOC1(int,orbits,orbits_sz,n,"malloc");
    DYNALLOC1(int,map,map_sz,n,"malloc");

    sg1.nv = n;
    sg1.nde = m;
    int cur = 0;
    For(i, n) {
        sg1.v[i] = cur;
        int curedges = 0;
        For(j, n) if (adjmatrix [i][j]) {
            sg1.e[cur] = j;
            cur ++;
            curedges ++;
        }
        sg1.d[i] = curedges;
    }
    Traces(&sg1,lab1,ptn,orbits,&options,&stats,&cg1);
    
    std::vector<std::vector<bool> > result(n, std::vector<bool>(n, false));

    For(i, n) {
        for(int j = cg1.v[i]; j < cg1.v[i] + cg1.d[i]; j++) {
            result [i][cg1.e[j]] = true;
        }
    }
    SG_FREE(sg1);
    SG_FREE(cg1);
    DYNFREE(lab1, lab1_sz);
    DYNFREE(ptn, ptn_sz);
    DYNFREE(orbits, orbits_sz);
    DYNFREE(map, map_sz);

    return result;

}

