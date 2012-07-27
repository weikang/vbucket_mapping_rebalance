/****************************************
Step RI --> R,
including functions to balance R in column sum
****************************************/

#include "Header.h"

// We try to switch row sums of R at most _SWITCH_ROWS_TRIAL_MAX steps
// And each step, we attempt a weak switch up to _WEAK_SWITCH_TRIAL_MAX times
#define _SWITCH_ROWS_TRIAL_MAX 100
#define _WEAK_SWITCH_TRIAL_MAX 100


void Mapping::DisperseR(vector<int>& Roriginal) {
    int i, j;
    int numRrsum, valRrsum[2];
    EvenDivide(N, M, numRrsum, valRrsum[1], valRrsum[0]);
    // We want numRrsum of valRrsum[1] and (M-numRrsum) of valRrsum[0] for M row sums of R
    // For two different types of row sums of R,
    // we want numR of valRentry1 and (S-numR) of valRentry2 in that row,
    // so that they sum to valRrsum*(L-1)
    int valRentry[2][2];
    int numR[2];
    EvenDivide(valRrsum[0]*(L - 1), S, numR[0], valRentry[0][1], valRentry[0][0]);
    EvenDivide(valRrsum[1]*(L - 1), S, numR[1], valRentry[1][1], valRentry[1][0]);
    int typeOfRow,typeOfEntry;
    for (i = 0; i <= M; i++) {
        R[i] = 0;
    }
    vector<int> RoriginalRsum(M + 1, 0);
    for (i = 1; i <= M; i++) {
        for (j = 0; j <= M; j++) {
            RoriginalRsum[i] += Roriginal[i * (M + 1) + j];
        }
    }
    int numRval0, numRval1;
    for (j = 0; j <= M; j++) {
            R[j] = 0;
    }
    int numRrsum1 = numRrsum;
    int numRrsum0 = M - numRrsum;
    for (i = 1; i <= M; i++) {
        // Which type of row sum is current row
        if (numRrsum1 == 0) {
            typeOfRow = 0;
            numRrsum0--;
        } else if (numRrsum0 == 0) {
            typeOfRow = 1;
            numRrsum1--;
        } else {
            if (abs(RoriginalRsum[i] - valRrsum[1]*(L - 1)) >
                abs(RoriginalRsum[i] - valRrsum[0]*(L - 1)) ) {
                    typeOfRow = 0;
                    numRrsum0--;
                } else {
                    typeOfRow = 1;
                    numRrsum1--;
                }
        }

        // We want the row sums to valRrsum[typeOfRow]*(L-1)
        // i.e. we want numRval0 of valRentry[typeOfRow][0]
        // and numRval1 of valRentry[typeOfRow][1]
        numRval0 = S - numR[typeOfRow];
        numRval1 = numR[typeOfRow];
        for (j = 0; j <= M; j++) {
            if (!RI[i * (M + 1) + j]) {
                R[i * (M + 1) + j] = 0;
            } else {
                if (numRval0 == 0) {
                    typeOfEntry = 1;
                    numRval1--;
                } else if (numRval1 == 0) {
                    typeOfEntry = 0;
                    numRval0--;
                } else {
                    if (abs(Roriginal[i * (M + 1) + j] - valRentry[typeOfRow][1]) >
                        abs(Roriginal[i * (M + 1) + j] - valRentry[typeOfRow][0]) ) {
                            typeOfEntry = 0;
                            numRval0--;
                        } else {
                            typeOfEntry = 1;
                            numRval1--;
                        }
                }
                R[i * (M + 1) + j] = valRentry[typeOfRow][typeOfEntry];
            }
        }
    }
}

// After Dispersion: Rrsum and RIrsum are balanced
// Disperse R from blank
void Mapping::DisperseR() {
    int i, j, k;
    int numRrsum, valRrsum[2];
    int distRows(0);
    EvenDivide(N, M, numRrsum, valRrsum[1], valRrsum[0]);
    // We want numRrsum of valRrsum[1] and (M-numRrsum) of valRrsum[0] for M row sums of R
    vector<int> RowOfR (M,0);  // 0s and 1s: representing type of row
    if (numRrsum>0) {
        // we want these numRrsum of valRrsum[1]'s dispersed:
        distRows = EvenDistribute(RowOfR, numRrsum);
    }

    // For two different types of row sums of R,
    // we want numR of valRentry1 and (S-numR) of valRentry2 in that row,
    // so that they sum to valRrsum*(L-1)
    int valRentry[2][2];
    int numR1, numR2;
    int distRentry1(0), distRentry2(0);
    EvenDivide(valRrsum[0]*(L - 1), S, numR1, valRentry[0][1], valRentry[0][0]);
    EvenDivide(valRrsum[1]*(L - 1), S, numR2, valRentry[1][1], valRentry[1][0]);
    // 0s and 1s of length S: representing type of nonzero entry
    vector<vector<int> > EntryOfR(2, vector<int> (S, 0));
    // We want these numR of valRentry's dispersed:
    if (numR1 > 0)
        distRentry1 = EvenDistribute(EntryOfR[0], numR1);
    if (numR2 > 0)
        distRentry2 = EvenDistribute(EntryOfR[1], numR2);

    int offset[2], gcdDist[2];
    offset[0] = 0;
    offset[1] = 0;
    gcdDist[0] = GCD(distRows, distRentry1);
    gcdDist[1] = GCD(distRows, distRentry2);
    int typeOfRow,typeOfEntry;
    for (i = 0; i <= M; i++) {
        R[i] = 0;
    }
    for (i = 1; i <= M; i++) {
        typeOfRow = RowOfR[i - 1];  // Which type of row sum is current row
        // We want the row sums to valRrsum[typeOfRow]*(L-1)
        // i.e. we want numR(typeOfRow) of valRentry[typeOfRow][1]
        // Set different starting offset to avoid patterns
        // offset increases by gcdDist-1
        offset[typeOfRow] = (offset[typeOfRow] + gcdDist[typeOfRow] + S-1) % S;
        k = offset[typeOfRow];
        for (j = 0; j <= M; j++) {
            if (!RI[i * (M + 1) + j]) {
                R[i * (M + 1) + j] = 0;
            } else {
                typeOfEntry = EntryOfR[typeOfRow][k % S];
                R[i * (M + 1) + j] = valRentry[typeOfRow][typeOfEntry];
                k++;
            }
        }
    }
}

// This function finds a local switch at column
// conditionLevel == 1  strong condition
// conditionLevel == 0  not so strong condition for swtiching
bool Mapping::BalanceRLocalSearch(int column, int valRcsum, int d, bool conditionLevel) {
    // high = 1 if current column sum is too high
    bool high;
    if (Rcsum[column] > valRcsum + d)
        high = 1;
    else if (Rcsum[column] < valRcsum)
        high = 0;
    else
        return (1);
    int j, k;
    bool condition;
    // sweep row j of this column
    for (j = 1; j <= M; j++) {
        if (R[j * (M + 1) + column] == 0)
            continue;
        // sweep the columns of row j
        for (k = 1; k <= M; k++) {
            if (conditionLevel) { //strong condition
                condition = (high && R[j * (M + 1) + k] < R[j * (M + 1) + column] && Rcsum[k] < valRcsum + d) ||
                         (!high && R[j * (M + 1) + k] > R[j * (M + 1) + column] && Rcsum[k] > valRcsum);
            } else {
                condition = (high && R[j * (M + 1) + k] < R[j * (M + 1) + column] && Rcsum[k] < Rcsum[column] - 1) ||
                         (!high && R[j * (M + 1) + k] > R[j * (M + 1) + column] && Rcsum[k] > Rcsum[column] + 1);
            }
            if (R[j * (M + 1) + k] > 0 && condition) {
                    ExchangeEntriesOfR(j, k, column);
                    return 1;
            }
        } //for k
    }// for j
    return 0;
}

// This function finds a local switch at column WITH WEAK CONDITION
bool Mapping::BalanceRLocalSearchWeak(int column, int valRcsum, int d) {
    bool flag = 0;  // 1 if a valid change has been found
    // high = 1 if current column sum is too high
    bool high;
    if (Rcsum[column] > valRcsum + d)
        high = 1;
    else if (Rcsum[column] < valRcsum)
        high = 0;
    else
        return 1;
    int j, k;
    vector<int> potentialRow, potentialCol;
    // sweep row j of this column
    for (j = 1; j <= M; j++) {
        if (R[j * (M + 1) + column] == 0)
            continue;
        // sweep the columns of row j
        for (k = 1; k <= M; k++) {
            if (R[j * (M + 1) + k] > 0 &&
                ((high && R[j * (M + 1) + k] < R[j * (M + 1) + column] && Rcsum[k] < Rcsum[column]) ||
                 (!high && R[j * (M + 1) + k] > R[j * (M + 1) + column] && Rcsum[k] > Rcsum[column])) ) {
                    potentialCol.push_back(k);
                    potentialRow.push_back(j);
                    //ExchangeEntriesOfR(j, k, column);
                    flag = 1;
            }
        } //for k
    }// for j
    if (!flag)
        return 0;
    int i = RNG() % ((int) potentialCol.size());
    ExchangeEntriesOfR(potentialRow[i], potentialCol[i], column);
    return 1;
}

// This function finds a switch at column via a thrid column
bool Mapping::BalanceRThirdColumnSearch(int column, int valRcsum, int d) {
    bool high;
    if (Rcsum[column] > valRcsum + d)
        high = 1;
    else if (Rcsum[column] < valRcsum)
        high = 0;
    else
        return (1);
    int j, k, l, i;
    // sweep row j of this column
    for (j = 1; j <= M; j++) {
        if (R[j * (M + 1) + column] == 0)
            continue;
        // sweep column k of row j
        for (k = 1; k <= M ; k++) {
            if (R[j * (M + 1) + k] > 0 &&
                ((high && R[j * (M + 1) + k] == R[j * (M + 1) + column] - 1) ||
                 (!high && R[j * (M + 1) + k] == R[j * (M + 1) + column] + 1)) ) {
                    // sweep row l of column k
                    for (l = 1; l <= M; l++) {
                        if (R[l * (M + 1) + k] == 0)
                            continue;
                        // sweep column i of row l
                        for (i = 1; i <= M; i++) {
                            if (R[l * (M + 1) + i] > 0 &&
                                ((high && R[l * (M + 1) + i] == R[l * (M + 1) + k] - 1 && Rcsum[i] < valRcsum + d) ||
                                 (!high && R[l * (M + 1) + i] == R[l * (M + 1) + k] + 1 && Rcsum[i] > valRcsum) )) {
                                    ExchangeEntriesOfR(j, k, column);
                                    ExchangeEntriesOfR(l, k, i);
                                    return 1;
                            }
                        }
                    }
                 }
        }
    }
    return 0;
}

void Mapping::UpdateChangeIndex(vector<int>& changeIndex, int valRcsum, bool d) {
    int i;
    changeIndex.clear();
    for (i = 1; i < (int) Rcsum.size(); i++) {
        if (Rcsum[i] > valRcsum + d || Rcsum[i] < valRcsum) {
            changeIndex.push_back(i);
        }
    }
}

// We complete balancing Rcsum by globally adjust R to balance
// If a global switch is not found, we do a most economic switch a Rrsum
void Mapping::BalanceR() {
    // N * (L - 1) = valRrsum * M + reminder
    int valRcsum = (N * (L - 1)) / M;
    int d(0);
    if ((N * (L - 1)) % M)
        d = 1;
    // First, check whether valid local change exists
    int j;
    for (j = 1; j <= M; j++) {
        BalanceRLocalSearch(j, valRcsum, d, 1);
    }
    bool flag(0);  // 1 if a valid change is found
    vector<int> changeIndex;
    UpdateChangeIndex(changeIndex, valRcsum, d);
    // Only valRrsum and (valRrsum + d) are allowed for column sums for R
    // (If reminder == 0, only one value is allowed.)

    // Then try to balance R with local search or use a third column
    // If not found, switch Rrsum
    // After TrialsMax of switch Rrsum, quit with current imbalance
    int switchRcsumTrials(0);
    int weakSwitchTrials(0);
    while (!changeIndex.empty() ) {
        flag = 0;
        for (j = 0; j < (int) changeIndex.size(); j++) {
            flag = BalanceRLocalSearch(changeIndex[j], valRcsum, d, 1);
            if (flag)
                break;
        }
        if (flag) {
            UpdateChangeIndex(changeIndex, valRcsum, d);
            continue;
        }
        for (j = 0; j < (int) changeIndex.size(); j++) {
            flag = BalanceRThirdColumnSearch(changeIndex[j], valRcsum, d);
            if (flag)
                break;
        }
        if (flag) {
            UpdateChangeIndex(changeIndex, valRcsum, d);
            continue;
        }
        for (j = 0; j < (int) changeIndex.size(); j++) {
            flag = BalanceRLocalSearch(changeIndex[j], valRcsum, d, 0);
            if (flag)
                break;
        }
        if (flag) {
            UpdateChangeIndex(changeIndex, valRcsum, d);
            continue;
        }
        if (weakSwitchTrials < _WEAK_SWITCH_TRIAL_MAX) {
            for (j = 0; j < (int) changeIndex.size(); j++) {
                flag = BalanceRLocalSearchWeak(changeIndex[j], valRcsum, d);
                if (flag)
                    break;
            }
        }
        if (flag) {
            weakSwitchTrials++;
            UpdateChangeIndex(changeIndex, valRcsum, d);
            continue;
        }
        if (switchRcsumTrials > _SWITCH_ROWS_TRIAL_MAX)
                return;
        SwitchRcsum();
        switchRcsumTrials++;
        weakSwitchTrials = 0;
        UpdateChangeIndex(changeIndex, valRcsum, d);
    }
}

// This function takes the row of R with lowest Rcsums and
// another row of R with highest and exchagne (L-1) between them
void Mapping::SwitchRcsum() {
    // First find lowest (L-1) positions sums of all rows and add 1 to all of them
    // Then find highest (L-1) position sums of all rows and minus 1 to all of them
    int i;
    int smallestRowExtremeSum(N * L * 2), smallestRowExtremeSumIndex(0);
    int highestRowExtremeSum(0), highestRowExtremeSumIndex(0);
    int sum;
    vector<int> smallestRowIndex (L - 1, 0);
    vector<int> highestRowIndex (L - 1, 0);
    vector<int> tempRowIndex (L - 1, 0);
    int valRsum = N / M;
    if (N % M == 0)
        return;
    for (i = 1; i <= M; i++) {
        sum = ExtremeSum(i, 1, tempRowIndex);
        if (Rrsum[i] > valRsum * (L - 1) &&
            sum > highestRowExtremeSum) {
                highestRowExtremeSum = sum;
                highestRowExtremeSumIndex = i;
                tempRowIndex.swap(highestRowIndex);
        }
        sum = ExtremeSum(i, 0, tempRowIndex);
        if (Rrsum[i] == valRsum * (L - 1) &&
            sum < smallestRowExtremeSum) {
                smallestRowExtremeSum = sum;
                smallestRowExtremeSumIndex = i;
                tempRowIndex.swap(smallestRowIndex);
        }
    }
    if (highestRowExtremeSum == smallestRowExtremeSum) {
        return;
    }
    SwitchRcsumRow(smallestRowExtremeSumIndex, 0, smallestRowIndex);
    SwitchRcsumRow(highestRowExtremeSumIndex, 1, highestRowIndex);
    GetRI();
}

// add/minus 1 to the (L-1) extreme elements of row in R
// when highestFlag == 1, we subtract, otherwise add
void Mapping::SwitchRcsumRow(int row, bool highestFlag, vector<int>& rowIndex) {
    int i;
    if (highestFlag) {
        for (i = 0; i < L - 1; i++) {
            R[row * (M + 1) + rowIndex[i] ]--;
        }
    } else {
        for (i = 0; i < L - 1; i++) {
            R[row * (M + 1) + rowIndex[i] ]++;
        }
    }
}

// sums of Rcsums of one row of R of (L-1) most extreme values
// Look at the S nonzero values of row of R
// find the (L-1) most extreme Rcsum value of the columns of these entries
// Sum them up and return the sum
// (If two column have the same Rcsum, we pick the first)
int Mapping::ExtremeSum(int row, bool highestFlag, vector<int>& rowIndex) {
    int largeNumber = N * N;
    int length = L - 1;
    vector<int> RcsumCopy;
    int i, k, sum(0);
    for (i = 0; i <= M; i++) {
        if (R[row *  (M + 1) + i]) {
            RcsumCopy.push_back(Rcsum[i] + largeNumber * R[row * (M + 1) + i]);
        }
    }
    rowIndex.clear();
    sort(RcsumCopy.begin(), RcsumCopy.end() );
    vector<int> loopIndex;
    if (highestFlag) {  // highest (L-1) elements
        for (i = S - 1; i >= S - length; i--)
            loopIndex.push_back(i);
    } else {
        for (i = 0; i < length; i++)
            loopIndex.push_back(i);
    }
    for (i = 0; i < (int)loopIndex.size() ; i++) {
        for (k = 0; k <= M; k++) {
            if (!IsInRow(rowIndex, k) &&
                RcsumCopy[loopIndex[i] ] == Rcsum[k] + largeNumber * R[row * (M + 1) + k])
                    break;
        }
        rowIndex.push_back(k);
        sum += Rcsum[k];
    }
    return sum;
}

// Exchange R(RowNum, ColNum1) with R(RowNum, ColNum2) on the same row
// and also update Rcsum
void Mapping::ExchangeEntriesOfR(int RowNum, int ColNum1, int ColNum2) {
    int diff;
    diff = R[RowNum * (M + 1) + ColNum1] - R[RowNum * (M + 1) + ColNum2];
    if (abs(diff) > 1)
        cout << "ERROR : Attempting to modify RI when balancing R !" << endl << endl;
    R[RowNum * (M + 1) + ColNum1] -= diff;
    Rcsum[ColNum1] -= diff;
    R[RowNum * (M + 1) + ColNum2] += diff;
    Rcsum[ColNum2] += diff;
}

