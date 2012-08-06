#include "Header.h"

int main() {
    srand(1);
    int n, l, m, s;
    cin >> n >> l >> m >> s;
    Mapping map0(n, l, m, s);
    map0.InitMapping(cin);

    int newS, newL;
    cin >> newS >> newL;

    string nodeName;
    vector<string> keepNodeNameList;
    keepNodeNameList.reserve(100);
    cin >> nodeName;
    while (!cin.eof() ) {
        if (nodeName.size() > 0 && !IsInRow(keepNodeNameList, nodeName) )
            keepNodeNameList.push_back(nodeName);
        cin >> nodeName;
    }

    int newM = keepNodeNameList.size();
    vector<string> changeNodeNameList;
    int i;
    for (i = 0; i < (int) keepNodeNameList.size(); i++) {
        if (!IsInRow(map0.nodeNameList, keepNodeNameList[i]) )
            changeNodeNameList.push_back(keepNodeNameList[i]);
    }
    for (i = 1; i <= map0.M; i++) {
        if (!IsInRow(keepNodeNameList, map0.nodeNameList[i]) )
            changeNodeNameList.push_back(map0.nodeNameList[i]);
    }
    vector<int> old2new;
    vector<string> newNodeNameList(map0.nodeNameList);
    UpdateNodeNameList(map0.nodeNameList, newNodeNameList, changeNodeNameList, old2new);

    Mapping map1(map0.N, newL, newM, newS);
    map1.nodeNameList.swap(newNodeNameList);
    map1.Rebalance(map0, old2new);
#ifdef _PRINT_DIAGNOSTIC_INFO
    int LowerBound = map1.RebalanceLowerBound(map0, old2new);
    if (map1.imbalance)
        _PRINT_DIAGNOSTIC_STREAM << "WARNING : Output mapping has imbalance of " << map1.imbalance << "\n\n";

    _PRINT_DIAGNOSTIC_STREAM << "Rebalance completed.\nNode number : " << map0.M << " ---> " << map1.M
         << "\tMove count : (index/non-index)" << map1.MoveCount(map0, 1) << '\t'
         << map1.MoveCount(map0, 0) << "\tLower bound : "
         << LowerBound << endl;
    for (i = 0; i < 20; i++)
        _PRINT_DIAGNOSTIC_STREAM << '-';
    _PRINT_DIAGNOSTIC_STREAM << endl;
    map1.PrintAname(_PRINT_DIAGNOSTIC_STREAM);
#else
    int LowerBound;
    if (map0.M > map1.M) {
        LowerBound = map1.N * map1.L / map0.M * (map0.M - map1.M);
    } else {
        LowerBound = map1.N * map1.L / map1.M * (map1.M - map0.M);
    }
    cout << map0.M << '\t' << map1.M << '\t' << map1.MoveCount(map0, 1) << '\t'
         << map1.MoveCount(map0, 0) << '\t' << LowerBound << endl;
#endif
    return 0;
}
