#include "Header.h"

int main() {
    int n, l, m, s;
    cin >> n >> l >> m >> s;
    Mapping map0(n, l, m, s);
    map0.InitMapping(cin);

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
    UpdateNodeNameList(map0.nodeNameList, newNodeNameList,
                       changeNodeNameList, old2new);

    Mapping map1(map0.N, map0.L, newM, map0.S);
    map1.nodeNameList.swap(newNodeNameList);
    int LowerBound = map1.RebalanceLowerBound(map0, old2new);
    map1.Rebalance(map0, old2new);
    if (map1.imbalance)
        cout << "WARNING : Output mapping has imbalance of " << map1.imbalance << endl << endl;

    cout << "Rebalance completed.\nNode number : " << map0.M << " ---> " << map1.M
         << "\tMove count : " << map1.MoveCount(map0) << "\tLower bound : "
         << LowerBound << endl;
    for (i = 0; i < 20; i++)
        cout << '-';
    cout << endl;
    map1.PrintAname(cout);
    return 0;
}
