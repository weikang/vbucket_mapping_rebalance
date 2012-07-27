/********************************
functions associated with R --> A step
********************************/

#include "Header.h"

// Make sure that first row, first colmn and diagonal of taget R are zero
bool Mapping::ExamineDiagonalR() {
    int i;
    bool flag = 0;
    for (i = 0; i <= M; i++) {
        if (R[i] != 0) {
            flag = 1;
            cout << "ERROR : target R nonezero at row 0 column " << i << endl << endl;
        }
        if (R[i * (M + 1)] != 0) {
            flag = 1;
            cout << "ERROR : target R nonezero at row " << i << " column 0\n\n";
        }
        if (R[i * (M + 1) + i] != 0) {
            flag = 1;
            cout << "ERROR : target R nonezero at row " << i << " column " << i << "\n\n";
        }
    }
    return flag;
}

// Adjust A to conform to mapTarget.R
void Mapping::ConformToTargetR(Mapping& mapTarget) {
    if (mapTarget.ExamineDiagonalR() )
        return;
    GetRRI();
    ConformToTargetRAdjustActive(mapTarget);
    GetRRI();
    ConformToTargetRAdjustReplica(mapTarget);
}

// Returns the "similarity score" of departing RowNumberA of A
// The better the result conforms to vecR, the lower the cost
int Mapping::EvaluateDepart(int RowNumberA, vector<int>& vecR, int active) {
    int i;
    int entry;
    int cost = 0;
    for (i = 1; i < L; i++) {
        entry = A[RowNumberA * L + i];
        if (R[active * (M + 1) + entry] > vecR[active * (M + 1) + entry])
            cost--;
    }
    return cost;
}

// Returns the "similarity score" of receiving RowNumberA of A at active
// The better the result conforms to vecR, the lower the cost
int Mapping::EvaluateReceive(int RowNumberA, vector<int>& vecR, int active) {
    int i;
    int cost = 0;
    int entry;
    for (i = 1; i < L; i++) {
        entry = A[RowNumberA * L + i];
        if (R[active * (M + 1) + entry] < vecR[active * (M + 1) + entry])
            cost--;
    }
    return cost;
}

// Inter row adjustment
// First make Rrsum correct
void Mapping::ConformToTargetRAdjustActive(Mapping& mapTarget) {
    int i, j, k;
    vector<int> receiveActive;  // nodes to have more active positions
    vector<int> departActive;  // nodes to have less active positions
    for (i = 0; i <= M; i++){
        if (Rrsum[i] < mapTarget.Rrsum[i]) {
            receiveActive.push_back(i);
        } else if (Rrsum[i] > mapTarget.Rrsum[i]) {
            departActive.push_back(i);
        }
    }

    vector<int> activeList;  // Rows of A having i at active
    int numberToDepart;  // Number of active nodes to depart
    int indexNewActive(0);
    int cost, costmin;
    int index;
    int departRow(0), receiveNode;
    for (index = 0; index < (int)departActive.size() ; index++){
        i = departActive[index];  // We want to depart node i
        numberToDepart = (Rrsum[i] - mapTarget.Rrsum[i]) / (L-1);
        while (numberToDepart > 0) {
            FindARowsActive(activeList, i);
            // Find which row of activeList j to depart
            // Find what to change row activeList[j] to : indexNewActive
            costmin = 1;
            for (j = 0; j < (int)activeList.size(); j++) {
                for (k = 0; k < (int)receiveActive.size() ; k++) {
                    receiveNode = receiveActive[k];
                    if (Rrsum[receiveNode] == mapTarget.Rrsum[receiveNode]) {
                        // already full, no longer can receive
                        receiveActive.erase(receiveActive.begin() + k);
                        k--;
                        continue;
                    }
                    cost = EvaluateDepart(activeList[j], mapTarget.R, i);
                    cost += EvaluateReceive(activeList[j], mapTarget.R, receiveNode);
                    if (cost < costmin) {
                        departRow = j;
                        indexNewActive = receiveNode;
                        costmin = cost;
                    }
                    if (costmin == -(L - 1) * 2)
                        break;  // Stop searching, lowest cost possible
                }
            }
            A[activeList[departRow] * L] = indexNewActive;
            numberToDepart--;
            GetRRI();  // optimize here: only update row i of R
        }  // while: departure for index completed
    }
}

// Intra row adjustment
// Change replica to make R correct within each row of R
void Mapping::ConformToTargetRAdjustReplica(Mapping& mapTarget) {
    int i, j, k, jj, kk, jk;
    vector<int> activeList;  // Rows of A having i at active
    vector<int> Rdiff (M + 1, 0);  // Difference between current R and target
    bool flag;
    int RowHammingDiff = 0;  // The Hamming distance of rows between current R and target
    vector<int> indexRtoFill;  // Candidates to fill in
    vector<int> RowOfA (L, 0);
    int ReplicaIndex;
    int additional(0);
    for (i = 0; i <= M ; i++) {
        RowHammingDiff = 0;
        for (j = 0; j <= M; j++) {
            Rdiff[j] = R[i * (M + 1) + j] - mapTarget.R[i * (M + 1) + j];
            RowHammingDiff += abs(Rdiff[j]);
        }
        if (RowHammingDiff == 0)
            continue;
        FindARowsActive(activeList, i);
        GetNegativeIndex(indexRtoFill, Rdiff);
        // We sweep A for possible substitutes
        // If an adjustment is allowed, we make it
        for (j = 0; j < (int) activeList.size(); j++) {
            //consider row activeList[j] of A
            for (k = 1; k < L; k++) { // sweep this row of A
                ReplicaIndex = A[activeList[j] * L + k];
                // We have a (i,ReplicaIndex) pair here
                if (Rdiff[ReplicaIndex] > 0) {  // If this pair has too many
                    RowOfA.assign(A.begin() + activeList[j] * L,
                                  A.begin() + activeList[j] * L + L);
                    for (kk = 0; kk < (int) indexRtoFill.size(); kk++){
                        // Sweep candidates to fill in
                        if (!IsInRow(RowOfA, indexRtoFill[kk])) {
                            // Not appear in this row of A
                            A[activeList[j] * L + k] = indexRtoFill[kk];  // Make change
                            RowHammingDiff -= 2;
                            R[i * (M + 1) + ReplicaIndex]--;
                            R[i * (M + 1) + indexRtoFill[kk]]++;
                            Rdiff[ReplicaIndex]--;
                            Rdiff[indexRtoFill[kk]]++;
                            if (Rdiff[indexRtoFill[kk]] == 0) {
                                indexRtoFill.erase(indexRtoFill.begin() + kk);
                            }
                            break;
                        }
                    }
                }
            }
        }
        additional += RowHammingDiff/2;
        // Use a second row to make adjustment if RowHammingDiff persists
        while (RowHammingDiff > 0) {
            flag = 0;
            for (j = 0; j < (int) activeList.size(); j++) {
                for (k = 1; k < L; k++) {  // Sweep this row of A
                    ReplicaIndex = A[activeList[j] * L + k];
                    // We have a (i,ReplicaIndex) pair here
                    if (Rdiff[ReplicaIndex] > 0) {
                        kk = indexRtoFill[0];  // We want to change ReplicaIndex to kk
                        // Find a row where kk does not appear
                        for(jj = 0; jj < (int) activeList.size(); jj++) {
                            RowOfA.assign(A.begin() + activeList[jj] * L,
                                          A.begin() + activeList[jj] * L + L);
                            if (jj != j && !IsInRow(RowOfA, kk)) {
                                // We found kk does not appear in row activeList[jj] of A
                                RowOfA.assign(A.begin() + activeList[j] * L,
                                              A.begin() + activeList[j] * L + L);
                                for (jk = 1; jk < L; jk++) {
                                    if (!IsInRow(RowOfA, A[activeList[jj] * L + jk])) {
                                    // jk of row [jj] does not appear in row [j]
                                    A[activeList[j] * L + k] = A[activeList[jj] * L + jk];
                                    A[activeList[jj] * L + jk] = kk;
                                    RowHammingDiff -= 2;
                                    R[i * (M + 1) + ReplicaIndex]--;
                                    R[i * (M + 1) + kk]++;
                                    Rdiff[ReplicaIndex]--;
                                    Rdiff[kk]++;
                                    flag = 1;
                                    if (Rdiff[kk] == 0) {
                                        indexRtoFill.erase(indexRtoFill.begin() );
                                    }
                                    break;
                                    }
                                }
                                if (flag)  break;
                            }
                        }
                    }
                    if (flag)  break;
                }
                if (flag)  break;
            }
        }
    }
    // cout << additional << '\t'; // Additonal move
    return;
}

// Find all rows of A having active in the first column
// Record the row numbers in list
void Mapping::FindARowsActive(vector<int>& list, int active) {
    int i;
    list.clear();
    for (i = 0; i < N; i++){
        if (A[i * L] == active) {
            list.push_back(i);
        }
    }
}

