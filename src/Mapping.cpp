// Defines some basic Mapping class member functions

#include "Header.h"

// Initialize an empty mapping
Mapping::Mapping(int n, int l, int m, int s) :
    N(n), L(l), M(m), S(s),
    empty(1), imbalance(),
    nodeNameList(M + 1), Aname(N * L),
    nodeTagList(M + 1),
    A(N * L, 1), R((M+1) * (M+1), 0),
    RI((M+1) * (M+1), 0),
    RIrsum(M + 1, 0), Rrsum(M + 1, 0), Rcsum(M + 1, 0) {
    nodeNameList[0] = _NULL_NODE_NAME;
    // Degenerate cases: need to override input S, L
    if (S >= M) {
        S = M - 1;
#ifdef _PRINT_DIAGNOSTIC_INFO
        _PRINT_DIAGNOSTIC_STREAM << "WARNING: Slave number should be less then node number !" << endl;
        _PRINT_DIAGNOSTIC_STREAM << "         Slave number is overridden to be " << S << endl << endl;
#endif
    }
    if (S * M > N * (L - 1) ) {
        S = (N / M) * (L - 1);  // S too large!
#ifdef _PRINT_DIAGNOSTIC_INFO
        _PRINT_DIAGNOSTIC_STREAM << "WARNING: Slave number is too large s.t. S * M > N * (L - 1) !" << endl;
        _PRINT_DIAGNOSTIC_STREAM << "         Slave number is overridden to be " << S << endl << endl;
#endif
    }
    if (S == 0 && M > 1) {
        S = M - 1;
        if (S > 10)
            S = 10;
#ifdef _PRINT_DIAGNOSTIC_INFO
        _PRINT_DIAGNOSTIC_STREAM << "WARNING: Slave number cannot be zero unless M == 0 !" << endl;
        _PRINT_DIAGNOSTIC_STREAM << "         Slave number is overridden to be " << S << endl << endl;
#endif
    }
    if (L > S + 1) {
        L = S + 1;
#ifdef _PRINT_DIAGNOSTIC_INFO
        _PRINT_DIAGNOSTIC_STREAM << "WARNING: Number of copies L cannot be larger than (S + 1) !" << endl;
        _PRINT_DIAGNOSTIC_STREAM << "         L is overridden to be " << L << endl << endl;
#endif
        A.resize(N * L, 0);
        Aname.resize(N * L);
    }
}

Mapping::Mapping(const Mapping& mapSource) :
    N(mapSource.N), L(mapSource.L),
    M(mapSource.M), S(mapSource.S),
    empty(mapSource.empty), imbalance(mapSource.imbalance),
    nodeNameList(mapSource.nodeNameList),
    Aname(mapSource.Aname), nodeTagList(mapSource.nodeTagList),
    A(mapSource.A), R(mapSource.R),
    RI(mapSource.RI), RIrsum(mapSource.RIrsum),
    Rrsum(mapSource.Rrsum), Rcsum(mapSource.Rcsum) {
}

// Input stream:
// N lines, each line represents a vbucket
// each line will have L node names corresponding to L nodes
// It is the CALLER's responsibility to make sure that
// the input stream is VALID, i.e.
// (0) Every line will have L entries, separated only by space
// (1) No two nodes on the same line are the same, except _NULL_NODE_NAME
// (2) the number of total nodes is M, except _NULL_NODE_NAME
// (3) Active node is never _NULL_NODE_NAME, only replicas can be _NULL_NODE_NAME
// (4) Exactly N lines is entered
// Function will initialize Mapping members
void Mapping::InitMapping(istream& fin) {
    int i, j, k;
    string s;
    getline(fin, s);
    for (i = 0; i < N; ++i) {
        getline(fin, s);
        istringstream iss(s);
        for (j = 0; j < L; j++)
            iss >> Aname[i * L + j];
    }
    bool flag; // 0: current node name has NOT appeared before
    int nodeNum(1);
    nodeNameList.clear();
    nodeNameList.push_back(_NULL_NODE_NAME);
    for (i= 0; i < N; i++) {
        for (j = 0; j < L; j++) {  // Sweep through the table Aname
            flag = 0;
            for (k = 0; k < (int) nodeNameList.size(); k++) {
                if (nodeNameList[k].compare(Aname[i * L + j]) == 0) {
                    flag = 1;  // current node has ID k in the nodeNameList
                    A[i * L + j] = k;
                    break;
                }
            }
            if (!flag) {  // New node here
                nodeNameList.push_back(Aname[i * L + j]);
                A[i * L + j] = nodeNum;
                nodeNum++;
            }
        }
    }
    if (M != nodeNum - 1) {
#ifdef _PRINT_DIAGNOSTIC_INFO
        _PRINT_DIAGNOSTIC_STREAM << "WARNING: Input mapping has node number unequal to specified !\n";
        _PRINT_DIAGNOSTIC_STREAM << "         Node number is overridden to be " << nodeNum - 1 << "\n\n";
#endif
        ResizeM(nodeNum - 1);
    }
    empty = 0;
    GetRRI();
    CheckBalance();
    }

// Get RI... from R
void Mapping::GetRI() {
    int i, j;
    for (i = 0; i <= M; i++) {
        RIrsum[i] = 0;
        Rrsum[i] = 0;
        Rcsum[i] = 0;
    }
    for (i = 0; i < (M + 1) * (M + 1); i++)
        RI[i] = 0;
    for (i = 0 ; i < (M + 1) * (M + 1) ; i++) {
        if (R[i] > 0)
            RI[i] = 1;
    }
    for (i = 0; i <= M; i++) {
        for (j = 0; j <= M; j++) {
            RIrsum[i] += RI[i * (M + 1) + j];
            Rrsum[i] += R[i * (M + 1) + j];
            Rcsum[j] += R[i * (M + 1) + j];
        }
    }
}

// Get R... from A
void Mapping::GetRRI() {
    int i, j;
    for (i = 0; i < (M + 1) * (M + 1); i++)
        R[i] = 0;
    for (j = 1; j < L; j++) {
        for (i = 0; i < N; i++)
            R[A[i * L] * (M + 1) + A[i * L + j]]++;  // pair A(i,0) -> A(i,j)
    }
    GetRI();
}

// Get Aname from A and nodeNameList
void Mapping::FillAname() {
    Aname.clear();
    Aname.resize(N * L);
    int i, j;
    for (i = 0; i < N; i++) {
        for (j = 0; j < L; j++)
            Aname[i * L + j] = nodeNameList[A[i * L + j]];
    }
}

void Mapping::InitRI() {
    int i, j;
    for (i = 0; i <= M; i++) {
        for (j = 0; j <= M; j++) {
            if (i == 0 || j == 0 || (j - i + M - 1) % M >= S)
                RI[i * (M + 1) + j] = 0;
            else
                RI[i * (M + 1) + j] = 1;
        }
    }
}

void Mapping::GenerateNodeName() {
    int i;
    for (i = 1; i <= M; i++) {
        nodeNameList[i] = "node" + Int2String(i);
    }
}

// Generate a balanced mapping
void Mapping::Generate() {
    if (M > 1) {
        RfromTopology();
        FillA();
        FillAname();
        empty = 0;
        CheckBalance();
    } else if (M == 1) {
        FillAname();
        empty = 0;
        imbalance = 0;
    } else {
#ifdef _PRINT_DIAGNOSTIC_INFO
        _PRINT_DIAGNOSTIC_STREAM << "ERROR : M must be positive.\n\n";
#endif
    }
    return;
}


// Construct A from R by vertically fill in nodes
// This construction is always feasible given S >= L-1
void Mapping::FillA () {
    int i, j, k;
    int NumRowAi;
    int fillCount=0, indexRowAbase=0;
    int q,r;  // Quotient and reminder
    for (i = 1; i <= M; i++) {
        NumRowAi = Rrsum[i] / (L-1);  // numRrsumer of rows with i active
        for (j = 0; j < NumRowAi; j++)
            A[(indexRowAbase + j) * L] = i;  // Fill active position
        fillCount = 0;
        for (j = 1; j <= M; j++) {  // Fill (i -> j) pair in A
            for (k = 0; k < R[i * (M + 1) + j]; k++) {  // R[i * M + j] such pairs to fill
                // fillCount = NumRowAi * q + r
                q = fillCount / NumRowAi;
                r = fillCount % NumRowAi;
                // Replicas positions: vertical fill
                A[(indexRowAbase + r) * L + (q + 1)] = j;
                fillCount++;
            }
        }
        indexRowAbase += NumRowAi;
    }
}

void Mapping::CheckBalance() {
    if (empty) {
        imbalance = -1;
        return;
    }
    if (M == 1) {
        imbalance = 0;
        return;
    }
    imbalance = 0;
    int i, j, k;
    int val = N / M;
    int d = ((N % M) > 0);
    for (i = 1; i <= M; i++) {
        if (Rrsum[i] / (L-1) < val) {
            imbalance += val - Rrsum[i] / (L-1);
        } else if (Rrsum[i] / (L-1) > val + d) {
            imbalance += Rrsum[i] / (L-1) - val - d;
        }
    }
    val = N * (L - 1) / M;
    d = ((N * (L - 1) % M) > 0);
    for (i = 1; i <= M; i++) {
        if (Rcsum[i] > val + d) {
            imbalance += Rcsum[i] - val - d;
        } else if (Rcsum[i] < val) {
            imbalance += val - Rcsum[i];
        }
    }
    vector<int> a(M, 0);
    for (i = 1; i <= M; i++) {
        for (j = 1; j <= M; j++)
            a[j - 1] = R[i * (M + 1) + j];
        k = WidthNotZero(a);
        if (k > 1)
            imbalance += k - 1;
    }
    for (i = 1; i <= M; i++) {
        imbalance += abs(RIrsum[i] - S);
    }
}

// Print in Aname format
// N, L, M, S
// then N rows of Aname
void Mapping::PrintAname(ostream& out) {
    int i,j;
    out << N << '\n' << L << '\n' << M << '\n' << S << endl;
    for (i = 0; i < N; i++) {
        for (j = 0; j < L; j++)
            out << Aname[i * L + j]<<'\t';
        out << endl;
    }
    out << endl;
    return;
}

int Mapping::CheckTags(){
    int i, j;
    int sameTagCount(0);
    for (i = 1; i <= M; i++) {
        for (j = 1; j <= M; j++) {
            if (RI[i * (M + 1) + j] && !nodeTagList[i].compare(nodeTagList[j])) {
                sameTagCount++;
            }
        }
    }
    return sameTagCount;
}

// printA==1 : also print matrix A
void Mapping::PrintCheck(ostream& fout, bool printA) {
    fout << "N = " << N << "\t(Number of vBuckets)\n";
    fout << "L = " << L << "\t(Number of total copies)\n";
    fout << "M = " << M << "\t(Number of Nodes\n";
    fout << "S = " << S << "\t(Number of slaves)\n\n";
    fout << "imbalance = " << imbalance << "\n\n";
    int i, j;
    int maxEntry = (N / M + 1) * (L - 1);
    int width = (int)log10((double)maxEntry ) + 2;  //output width
    fout.width(width);
    fout << 'R';
    for (i = 0; i <= M; i++) {
        fout.width(width);
        fout << Rcsum[i];
    }
    fout << endl;
    for (i = 0; i <= M; i++) {
        fout.width(width);
        fout << Rrsum[i];
        for (j = 0; j <= M; j++) {
            fout.width(width);
            fout << R[i * (M + 1) + j];
        }
        fout << endl;
    }
    fout << endl << "nodeNameList : " << endl;
    for (i = 0; i <= M; i++) {
        fout << i << " : " << nodeNameList[i] << '\t' << nodeTagList[i] << endl;
    }
    if (printA) {  // also print matrix A
        int widthA = (int)log10((double)M) + 2;  // output width for elements in A
        fout << "\nA\n";
        for (i = 0; i < N; i++) {
            for (j = 0;j < L;j++) {
                fout.width(widthA);
                fout << A[i * L + j];
            }
            fout << endl;
        }
    }
    fout << endl;
}

void Mapping::PrintTopology(ostream & fout) {
    int i, j;
    fout << "RI : " << endl;
    for (i = 0; i <= M; i++) {
        for (j = 0; j <= M; j++) {
            fout.width(2);
            fout << RI[i * (M + 1) + j];
        }
        fout << endl;
    }
    fout << endl;
}
