/********************************
General rebalance functions
Functions for preparation
********************************/

#include "Header.h"

int Mapping::RebalanceLowerBound(Mapping& mapOriginal, vector<int>& old2new) {
    Mapping map0(mapOriginal);
    map0.ResizeM(M);
    map0.UpdateA(old2new);
    Mapping mapTarget(N, L, M, S);
    if (map0.L < L) {
        map0.UpdateL(L, mapTarget.R);
    } else if (map0.L > L) {
        Mapping mapTemp(map0);
        mapTarget.InitRI();
        mapTarget.Generate();
        mapTemp.UpdateL(L, mapTarget.R);
        mapTarget.OptimalTopology(mapTemp.R);
        mapTarget.RfromTopology();
        map0.UpdateL(L, mapTarget.R);
    }
    map0.GetRRI();
    return map0.FindLowerBound();
}

// Rebalance for potential change of L, S, M
// WE ASSUME N DOES NOT CHANGE
void Mapping::Rebalance(Mapping& mapOriginal, vector<int>& old2new, int TagPrice) {
    if (M == 1 || mapOriginal.M == 1) {
        InitRI();
        Generate();
        return;
    }
    Mapping map0(mapOriginal);
    Mapping mapTarget(N, L, M, S);
    mapTarget.nodeTagList.assign(nodeTagList.begin(), nodeTagList.end());
    map0.ResizeM(M);
    map0.UpdateA(old2new);
    if (map0.L < L) {
        map0.UpdateL(L, mapTarget.R);
    } else if (map0.L > L) {
        Mapping mapTemp(map0);
        mapTarget.InitRI();
        mapTarget.Generate();
        mapTemp.UpdateL(L, mapTarget.R);
        mapTarget.OptimalTopology(mapTemp.R, TagPrice);
        mapTarget.RfromTopology();
        map0.UpdateL(L, mapTarget.R);
    }
    map0.GetRRI();
    A.swap(map0.A);
    mapTarget.OptimalTopology(map0.R, TagPrice);
    mapTarget.RfromTopology(map0.R);
    ConformToTargetR(mapTarget);
    GetRRI();
    FillAname();
    empty = 0;
    CheckBalance();
}

// Find lower bound function
// uses the following lemma (which I think is correct)
// to minimize \sum |a_i - b_i|
// both sequence should be monotone
int Mapping::FindLowerBound() {
    int i;
    if (L == 1) {
        int sum = 0;
        for (i = 0; i < N; ++i) {
            if (!A[i * L])
                sum++;
        }
        return sum;
    }
    // balanced column sums: N(L-1)/M
    // balanced row sums: N/M * (L_1)
    vector<int> Rc(Rcsum);
    vector<int> Rr(Rrsum);
    sort(Rc.begin() + 1, Rc.end() );
    sort(Rr.begin() + 1, Rr.end() );
    vector<int> Rcbalanced;
    vector<int> Rrbalanced;
    i = M;
    int remainSum = N * (L - 1);
    Rcbalanced.push_back(0);
    while (i > 0) {
        Rcbalanced.push_back(remainSum / i);
        remainSum -= remainSum / i;
        i--;
    }
    i = M;
    remainSum = N;
    Rrbalanced.push_back(0);
    while (i > 0) {
        Rrbalanced.push_back(remainSum / i * (L - 1) );
        remainSum -= remainSum / i;
        i--;
    }
    sort(Rcbalanced.begin() + 1, Rcbalanced.end() );
    sort(Rrbalanced.begin() + 1, Rrbalanced.end() );
    int lowerBound(0);
    for (i = 0; i <= M; i++) {
        lowerBound += abs(Rcbalanced[i] - Rc[i])/2;
        lowerBound += abs(Rrbalanced[i] - Rr[i])/2/(L-1);
    }
    return lowerBound;
}

void Mapping::ResizeM(int newM) {
    M = newM;
    R.resize((M + 1) * (M + 1), 0);
    RI.resize((M + 1) * (M + 1), 0);
    RIrsum.resize(M + 1, 0);
    Rrsum.resize(M + 1, 0);
    Rcsum.resize(M + 1, 0);
}

void Mapping::UpdateA(vector<int>& old2new) {
    int i, j;
    for (i = 0; i < N; i++) {
        for (j= 0 ; j < L; j++) {
            A[i * L + j] = old2new[A[i * L + j] ];
        }
    }
    GetRRI();
}

void Mapping::UpdateL(int newL, vector<int>& Rtarget) {
    vector<int> Anew (N * newL, 0);
    int i, j;
    int activeIndex;
    int numberOfNodesToDelete;
    int replicaIndex;
    vector<int> indexNodesToDelete;
    vector<int> RowOfA (L, 0);

    if (L > newL) {
        // L has decreased
        // Delete unwanted nodes first
        for (i = 0; i < N; i++) {
            Anew[i * newL] = A[i * L];
            activeIndex = A[i * L];
            numberOfNodesToDelete = L - newL;
            indexNodesToDelete.clear();
            // First delete unwanted nodes
            // Then delete any additional nodes
            for (j = 1; j < L; j++) {
                if (numberOfNodesToDelete == 0)
                    break;
                replicaIndex = A[i * L + j];
                if (Rtarget[activeIndex * (M + 1) + replicaIndex] <
                    R[activeIndex * (M + 1) + replicaIndex]) {
                         indexNodesToDelete.push_back(j);
                         numberOfNodesToDelete--;
                         R[activeIndex * (M + 1) + replicaIndex]--;
                     }
            }
            while (numberOfNodesToDelete > 0) {
                for (j = 1; j < L; j++) {
                    if (numberOfNodesToDelete == 0)
                        break;
                    if (!IsInRow(indexNodesToDelete, j) ) {
                         replicaIndex = A[i * L + j];
                         indexNodesToDelete.push_back(j);
                         numberOfNodesToDelete--;
                         R[activeIndex * (M + 1) + replicaIndex]--;
                         }
                }
            }
            IncreaseSort(indexNodesToDelete);
            RowOfA.assign(A.begin() + i * L,
                          A.begin() + i * L + L);
            for (j = 0; j < (int)indexNodesToDelete.size(); j++) {
                RowOfA.erase(RowOfA.begin() + indexNodesToDelete[j] - j);
            }
            for (j = 0; j < newL; j++) {
                Anew[i * newL + j] = RowOfA[j];
            }
        }
        L = newL;
        A.swap(Anew);
    } else if (L < newL) {
        // L has increased
        // Fill in _NULL_NODE_NAME make L == newL
        for (i = 0; i < N; i++) {
            for (j = 0; j < L; j++) {
                Anew[i * newL + j] = A[i * L + j];
            }
        }
        L = newL;
        A.swap(Anew);
    }
    return;
}

// clusterType:1 (default) non-indexed version
// clusterType: 0  indexed version
int Mapping::MoveCount(Mapping& map0, bool clusterType) {
    int i, j;
    int move(0);
    vector<string> RowOfA;
    RowOfA.reserve(L + map0.L);
    if (clusterType) {  // indexed version
        for (i = 0; i < N; i++) {
            if (map0.Aname[i * map0.L].compare(Aname[i * L]) )
                move++;
            RowOfA.clear();
            RowOfA.assign(map0.Aname.begin() + i * map0.L + 1,
                          map0.Aname.begin() + i * map0.L + map0.L);
            for (j = 1; j < L; j++) {
                if (!IsInRow(RowOfA, Aname[i * L + j]))
                    move++;
            }
        }
    } else {  // non-indexed version
        for (i = 0; i < N; ++i) {
            RowOfA.clear();
            RowOfA.assign(map0.Aname.begin() + i * map0.L,
                          map0.Aname.begin() + i * map0.L + map0.L);
            for (j = 0; j < L; j++) {
                if (!IsInRow(RowOfA, Aname[i * L + j]))
                    move++;
            }
        }
    }
    return move;
}
