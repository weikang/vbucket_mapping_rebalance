#include "Header.h"

int main(int argc, char * argv[]) {
    srand(1);
    int n, l, m, s;
    cin >> n >> l >> m >> s;
    Mapping map0(n, l, m, s);
    map0.InitMapping(cin);

    int newS, newL, tagNumber;
    cin >> newS >> newL >> tagNumber;

	vector<int> TagPrice(tagNumber,0);
    int i, j;
    for (i = 0; i < tagNumber; ++i) {
        if (i + 1 < argc)
            TagPrice[i] = String2Int(string(argv[i + 1]));
        else
            TagPrice[i] = 10;  // default price is set to 10
    }

    string nodeName;
    string nodeTag;
    vector<string> keepNodeNameList;
    vector<string> keepNodeTagList;
    keepNodeNameList.reserve(100);
    keepNodeTagList.reserve(100);
    keepNodeNameList.push_back(_NULL_NODE_NAME);
    keepNodeTagList.push_back("");
    cin >> nodeName;
    while (!cin.eof() ) {
        if (nodeName.size() > 0 && !IsInRow(keepNodeNameList, nodeName) ) {
            keepNodeNameList.push_back(nodeName);
            for (i = 0; i < tagNumber; ++i) {
                cin >> nodeTag;
                keepNodeTagList.push_back(nodeTag);
            }
        }
        cin >> nodeName;
    }

    int newM = keepNodeNameList.size() - 1;
    stack<int> addedNode;
    for (j = 1; j <= newM; ++j ) {
        if (!IsInRow(map0.nodeNameList, keepNodeNameList[j])) {
            addedNode.push(j);
        }
    }
    // Deleted nodes map to zero, used for computing lower bound
    vector<int> oldNode2NewZero(map0.M + 1, 0);
    // Deleted nodes map to added nodes if possible
    // Used for initialize a search for RI
    vector<int> oldNode2NewMatch(map0.M + 1, 0);
    string currentNode;
    bool nodeFound;
    for (i = 1; i <= map0.M; ++i) {
        currentNode = map0.nodeNameList[i];
        nodeFound = 0;
        for (j = 0; j <= newM; ++j ) {
            if (!currentNode.compare(keepNodeNameList[j])) {
                nodeFound = 1;
                break;
            }
        }
        if (nodeFound) {
            oldNode2NewMatch[i] = j;
            oldNode2NewZero[i] = j;
        } else {
            if (!addedNode.empty() ) {
                oldNode2NewMatch[i] = addedNode.top();
                addedNode.pop();
            }
        }
    }

    Mapping map1(map0.N, newL, newM, newS);
    map1.nodeNameList.swap(keepNodeNameList);
    map1.nodeTagList.swap(keepNodeTagList);
    map1.Rebalance(map0, oldNode2NewMatch, TagPrice);
    map1.nodeTagNumber = tagNumber;
    int LowerBound = map1.RebalanceLowerBound(map0, oldNode2NewZero, TagPrice);
#ifdef _PRINT_DIAGNOSTIC_INFO
    if (map1.imbalance)
        _PRINT_DIAGNOSTIC_STREAM << "WARNING : Output mapping has imbalance of " << map1.imbalance << "\n\n";

    _PRINT_DIAGNOSTIC_STREAM << "\nRebalance completed.\nNode number : " << map0.M << " ---> " << map1.M
         << "\nMove count (index/non-index) : " << map1.MoveCount(map0, 1) << " / "
         << map1.MoveCount(map0, 0) << "\tLower bound : " << LowerBound
         << "\n\nSame tags replication :\nPrice : \t";
    vector<int> sameTagCount;
    map1.CheckTags(sameTagCount);
    for (i = 0; i < map1.nodeTagNumber; i++) {
        _PRINT_DIAGNOSTIC_STREAM << TagPrice[i] << '\t';
    }
    _PRINT_DIAGNOSTIC_STREAM << "\nCount : \t";
    for (i = 0; i < map1.nodeTagNumber; i++) {
        _PRINT_DIAGNOSTIC_STREAM << sameTagCount[i] << '\t';
    }
    _PRINT_DIAGNOSTIC_STREAM << endl;

    for (i = 0; i < 20; i++)
        _PRINT_DIAGNOSTIC_STREAM << '-';
    _PRINT_DIAGNOSTIC_STREAM << endl;
    map1.PrintAname(_PRINT_DIAGNOSTIC_STREAM);
#else
/*    if (map0.M > map1.M) {
        LowerBound = map1.N * map1.L / map0.M * (map0.M - map1.M);
    } else {
        LowerBound = map1.N * map1.L / map1.M * (map1.M - map0.M);
    } */
    cout << map0.M << '\t' << map1.M << '\t' << map1.MoveCount(map0, 1) << '\t'
         << map1.MoveCount(map0, 0) << '\t' << LowerBound << endl
         << "same tags count : " << map1.CheckTags() << endl;
    //map1.PrintCheck(cout);
#endif
    return 0;
}
