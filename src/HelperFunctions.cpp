//some petty helper functions

#include "Header.h"

// Greatest common divisor
int GCD(int a, int b) {
    if (a == 0 || b==0)
        return 1;
    while (1) {
        a = a % b;
		if( a == 0 )
            return b;
		b = b % a;
        if( b == 0 )
            return a;
    }
}

int randomUnif2(int Max) {
    int value = rand();
    int multiple = RAND_MAX / Max;
    if (value >= multiple * Max) {
        value = rand();
    }
    return (value % Max) + 1;
}

// Find (max - min) for array
int Width(vector<int>& array) {
    if (array.empty())
        return 0;
    int max(array[0]), min(array[0]);
    for (int i = 1; i < (int) array.size(); i++) {
        if (max < array[i])
            max = array[i];
        if (min > array[i])
            min = array[i];
    }
    return max - min;
}

// Find (max - min) for array with non-negative entries
// But disregard 0 when finding min
int WidthNotZero(vector<int>& array) {
    if (array.empty())
        return 0;
    int max(array[0]), min(INT_MAX);
    for (int i = 0; i < (int) array.size(); i++) {
        if (max < array[i])
            max = array[i];
        // Zeros will not be considered
        if (min > array[i] && array[i] != 0)
            min = array[i];
    }
    return max-min;
}

string Int2String(int Number) {
	stringstream ss;
	ss << Number;
	return ss.str();
}

int String2Int(const string& Text) {
	stringstream ss(Text);
	int result;
	return ss >> result ? result : 0;
}

// We want num of val1 and (M-num) of val2, (num <= M/2)
// So that they will sum to N
// M must be positive
void EvenDivide(int N, int M, int& num, int& val1, int& val2) {
    int q,r;
    q = N / M;
    r = N % M;  // N = M * q + r
    if (2*r<M) {
        num = r;
        val1 = q + 1;
        val2 = q;
    } else {
        num = M - r;
        val1 = q;
        val2 = q + 1;
    }
    return;
}

// Input list should be a vector of all zeros
// This function evenly distribute list with num 1s among 0s
// Returns the most frequent distance between 1s
int EvenDistribute(vector<int>& list, int num) {
    int N = list.size();
    double len = (double) N / (double) num;
    int pos;
    for (int i=0; i<num; i++) {
        pos = (int)(floor(len * i + 0.5));
        list[pos] = 1;
    }
    int freq_dist;
    int q, r;
    q = N / num;
    r = N % num;
    if (2 * r <= num)
        freq_dist = q;
    else
        freq_dist = q + 1;
    return freq_dist;
}

// Whether or now target is in vec
bool IsInRow(vector<int>& vec, int target) {
    for (int i=0; i<(int)vec.size(); i++)
        if (vec[i] == target)
            return 1;  // Found one in vec
    return 0;
}

// Whether or now target is in vec
bool IsInRow(vector<string>& vec, string target) {
    for (int i = 0; i < (int)vec.size(); i++)
        if (!vec[i].compare(target) )
            return 1;  // Found one in vec
    return 0;
}

// Input: list is a vector of non-negative integers
// Functions sorts it in strictly increasing order
// Function deletes duplicates and ignores negative integers in List
void IncreaseSort(vector<int>& List) {
    vector<int> Listnew;
    int N = List.size();
    Listnew.reserve(N);
    int min(INT_MAX);
    int i;
    while (1) {
        for (i=0; i<N; i++) {  // Find min of non-negative elements in List
            if (List[i] > -1 && List[i] < min)
                min = List[i];
        }
        if (min == INT_MAX) break;
        Listnew.push_back(min);
        for (i=0; i<N; i++)
            if (List[i] == min)
                List[i] = -1;
        min = INT_MAX;
    }
    List.swap(Listnew);
    return;
}

// Returns index that records positions of all the negative entries of array
void GetNegativeIndex(vector<int>& index, vector<int>& array) {
    index.clear();
    for (int i = 1; i < (int) array.size(); i++) {
        if ( array[i] < 0 )
            index.push_back(i);  // i has fewer than needed
    }
    if ( array[0] < 0 )
            index.push_back(0);  // i has fewer than needed
}

// A PrintCheck wrapper
void PrintDiag(Mapping& A, int active) {
    A.PrintTopology(cerr);
    A.PrintCheck(cerr);
/*    if (active == 0)
        return;
    int i, j;
    int widthA = (int)log10((double)(A.M)) + 2;  // output width for elements in A
        for (i = 0; i < A.N; i++) {
            if (A.A[i * A.L] == active) {
                for (j = 0;j < A.L;j++) {
                    cerr.width(widthA);
                    cerr << A.A[i * A.L + j];
                }
                cerr << endl;
            }
        } */
}

// Fisher–Yates shuffle
void Shuffle(vector<int>& samples) {
    int n = samples.size();
    vector<int> a (samples.size(), 0);
    int i,j;
    a[0] = samples[0];
    for (i = 1; i < n; i++) {
        j = randomUnif2(i + 1) - 1;
        a[i] = a[j];
        a[j] = samples[i];
    }
    samples = a;
}

// newNodeNameList should be initialized to be the same as nodeNameList
// This function takes in changeNodeNameList and nodeNameList
// It mark deleted nodes 0 and outputs the old2new index mapping
// It also updates the new NodeNameList to be used in the new mapping
void UpdateNodeNameList(vector<string>& nodeNameList,
                        vector<string>& newNodeNameList,
                        vector<string>& changeNodeNameList,
                        vector<int>& old2new) {
    int i, j;
    bool flag(1);  // 0 if currentNodeName is to be deleted
    string currentNodeName;
    for (i = 0; i < (int) changeNodeNameList.size(); i++) {
        currentNodeName = changeNodeNameList[i];
#ifdef _PRINT_DIAGNOSTIC_INFO
        if (!currentNodeName.compare(_NULL_NODE_NAME)) {
            _PRINT_DIAGNOSTIC_STREAM << "WARNING : Update node with undefined node name !\n\n";
            continue;
        }
#endif
        flag = 1;
        for (j = 1; j < (int) newNodeNameList.size(); j++) {
            if (!currentNodeName.compare(newNodeNameList[j]) ) {
                // We delete newNodeNameList[j]
                newNodeNameList.erase(newNodeNameList.begin() + j);
                flag = 0;
                break;
            }
        }
        if (!flag)
            continue;
        newNodeNameList.push_back(currentNodeName);  // currentNodeName to be added
    }
    old2new.clear();
    old2new.resize(nodeNameList.size(), 0);
    // 0 always --> 0, old2new starts at node 0
    for (i = 1; i < (int) nodeNameList.size(); i++) {
        for (j = 1; j < (int) newNodeNameList.size(); j++) {
            flag = 0;  // not found
            if (!nodeNameList[i].compare(newNodeNameList[j]) ) {
                // We found old node in new
                old2new[i] = j;
                flag = 1;
                break;
            }
        }
        if (!flag) {
            old2new[i] = 0;
        }
    }
}
