// string for the name given to the undefined node
#define _NULL_NODE_NAME "undefined"

// Print out diagnostic information
#ifndef _PRINT_DIAGNOSTIC_INFO
#define _PRINT_DIAGNOSTIC_INFO
#define _PRINT_DIAGNOSTIC_STREAM cout
#endif

// Not printing any diagonostic, used for testing
#undef _PRINT_DIAGNOSTIC_INFO

#ifndef _Header_h
#define _Header_h

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <climits>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <stack>
#include <vector>
#include <algorithm>
using namespace std;

class Mapping {
 public:
    int N;  // number of vbuckets
    int L;  // number of copoes (including the active node)
    int M;  // number of nodes
    int S;  // number of slaves for every node
    bool empty;     // 1 if mapping is empty
    int imbalance;
    vector<string> nodeNameList;  // nodeNameList[node ID] = node name, size (M+1)
    vector<string> Aname;  // matrix A with node names, size N * L
    vector<string> nodeTagList;

    Mapping(int n, int l, int m, int s);  // Initialize an empty Mapping
    Mapping(const Mapping& mapSource);  // Copy constructor
    void InitMapping(istream& fin);  // Read input (in Aname format)
    void PrintAname(ostream& out);  // Print in Aname format
    void PrintMapping(ostream &);  // Print Mapping format
    void PrintTopology(ostream & fout);
    void PrintCheck(ostream & fout, bool printA = 0);  // Print Diagnostics
    int CheckTags();
    void Generate();  // generate a new balanced mapping from empty
    void InitRI();  // Initialize a block strip RI with S = tempS
    void Rebalance(Mapping& mapOriginal, vector<int>& old2new, int TagPrice);
    int RebalanceLowerBound(Mapping& mapOriginal, vector<int>& old2new);
    // Count the number of data movements
    // clusterType 1:index  0:nonindex
    int MoveCount(Mapping& mapOriginal, bool clusterType = 1);
    void GenerateNodeName();
private:
    vector<int> A;  // matrix A with node ID, size NxL
    vector<int> R;     // Replication matrix R, size (M+1)x(M+1)
    vector<bool> RI;  // Incidence matrix: R>0, size (M+1)x(M+1)
    vector<int> RIrsum;    // Row sum of RI, size (M+1)
    vector<int> Rrsum;     // Row sum of R, size (M+1)
    vector<int> Rcsum;     // Column sum of R, size (M+1)

    void OptimalTopology (vector<int>& Roriginal, int TagPrice = 5);
    void GetRRI(); // Compute R,RI... from A
    void GetRI(); // Compute RI... from R
    void FillAname();  // fill Astr and serverList from A
    void CheckBalance();  // compute imbalance

    void MakeRIProposal(vector<bool>& RIproposed, vector<int>& Roriginal, double& EnergyDifference, int TagPrice);
    void DisperseR();  // Create R from M,N,S,L
    void DisperseR(vector<int>& Roriginal);
    void BalanceR();  // Make R balanced
    void UpdateChangeIndex(vector<int>& changeIndex, int valRcsum, bool d);
    bool BalanceRLocalSearch(int column, int valRcsum, int d, bool conditionLevel);
    bool BalanceRLocalSearchWeak(int column, int valRcsum, int d);
    bool BalanceRThirdColumnSearch(int column, int valRcsum, int d);
    void SwitchRcsum();
    void SwitchRcsumRow(int row, bool highestFlag, vector<int>& rowIndex);
    int ExtremeSum(int row, bool highestFlag, vector<int>& rowIndex);
    void ExchangeEntriesOfR(int RowNum, int ColNum1, int ColNum2);  // Exchange two entries in R
    void FillA();  // fill in A from R vertically

    void UpdateA(vector<int>& old2new);
    void ResizeM(int newM);
    void RfromTopology(vector<int>& Roriginal);
    void RfromTopology();
    int FindLowerBound();
    void UpdateL(int newL, vector<int>& Rtarget);

    bool ExamineDiagonalR();
    void ConformToTargetR(Mapping& mapTarget);  // Adjust A to targetR
    void ConformToTargetRAdjustReplica(Mapping& mapTarget, vector<int> &active);  // Intra row adjust
    void ConformToTargetRAdjustActive(Mapping& mapTarget);  // Inter row adjust
    void FindARowsActive(vector<int>& list, int active);  // Find rows of A having active
    int EvaluateDepart(int RowNumberA, vector<int>& vecR, int active);
    int EvaluateReceive(int RowNumberA, vector<int>& vecR, int active);
};

// Some samll general-purpose helper functions in small_func.cpp
int WidthNotZero(vector<int>& array);
int Width(vector<int>& array);
int GCD(int a, int b);
int String2Int(const string &Text);
string Int2String(int Number);
void EvenDivide(int N, int M, int& num, int& val1, int& val2);
int EvenDistribute(vector<int>& list, int num);
bool IsInRow(vector<int>& vec, int target);
bool IsInRow(vector<string>& vec, string target);
void IncreaseSort(vector<int>& List);
void GetNegativeIndex(vector<int>& index, vector<int>& array);
void PrintDiag(Mapping& A, int active);  // a wrapper for PrintCheck
void Shuffle(vector<int>& samples);
int randomUnif2(int Max);

#endif

