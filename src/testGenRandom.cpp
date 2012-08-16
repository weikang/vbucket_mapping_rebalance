#include "Header.h"

// caseSize: test cases to generate

int main(int argc, char * argv[]) {
    int caseSize(9);
    if (argc > 1)
        caseSize = String2Int(string(argv[1]));
    ofstream writeMapping;
    ifstream readMapping;
    int seed(1);
    if (argc > 2)
        seed = String2Int(string(argv[2]));
    srand(seed);
    int i, j, k;
    int N, L, M, S;
    vector<int> index;
    vector<int> index2;
    string filename;

    int pertNonGarbage;
    int tagNumber;
    int addnode;
    int percentNodekept;
    for (i = 0; i <= caseSize; i++) {
        tagNumber = 0; //randomUnif2(4) - 1;  // <--------- modify here
        N = 1024;
        L = 4; //randomUnif2(3) + 1;
        M = randomUnif2(20) + 15;  // <--------- modify here
        S = 10;
        Mapping map1(N, L, M, S);
        map1.GenerateNodeName();
        map1.InitRI();
        map1.Generate();

        index.clear();
        index2.clear();
        for (j = 0; j < map1.N; j++) {
            index.push_back(j);
            index2.push_back(j);
        }
        Shuffle(index);
        Shuffle(index2);
        filename = "testCases/input" + Int2String(i);
        writeMapping.open(filename.c_str() );
        writeMapping << map1.N << endl
                     << map1.L << endl
                     << map1.M << endl
                     << map1.S << endl;
        pertNonGarbage = randomUnif2(5) + 95;  // <--------- modify here
        for (j = 0; j < map1.N; j++) {
            for (k = 0; k< map1.L; k++) {
                if (randomUnif2(100) < pertNonGarbage) // 95 percent true
                    writeMapping << map1.Aname[index[j] * map1.L + k] << '\t';
                else  // 5 percent pertNonGarbage
                    writeMapping << map1.Aname[index2[j] * map1.L + k] << '\t';
            }
            writeMapping << endl;
        }

        writeMapping << 10 << endl;
        if (map1.L > 1 && randomUnif2(4) < 4)
            writeMapping << map1.L << endl;
        else
            writeMapping << randomUnif2(3) + 1 << endl;

        writeMapping << tagNumber << endl;
        bool hasKeptNodes(0);
        vector<string> keptNodes;
        vector<string> nodeNameList(map1.nodeNameList);
        addnode = 0;  // randomUnif2(10) - 1;  // <----------modeify here
        for (j = 0; j < addnode; j++) {
            keptNodes.push_back("newnode" + Int2String(j));
            hasKeptNodes = 1;
        }
        percentNodekept = randomUnif2(5) + 95; // <----------modeify here
        for (j = 1; j < (int)nodeNameList.size(); ++j) {
            if (randomUnif2(100) <= percentNodekept) {
                hasKeptNodes = 1;
                keptNodes.push_back(nodeNameList[j]);
            }
        }
        if (!hasKeptNodes) {
            j = randomUnif2(nodeNameList.size() - 1);
            keptNodes.push_back(nodeNameList[j]);
        }

        int nodesPerTag = randomUnif2(5) + 5;
        int totalTags = keptNodes.size() / nodesPerTag + 1;
        vector<string> tags;
        for (j = 0; j < totalTags; j++) {
            tags.push_back("TAG" + Int2String(j));
        }

        for (j = 1; j < (int)keptNodes.size(); ++j) {
            writeMapping << keptNodes[j] << '\t';
            for (int k = 0; k < tagNumber; ++k) {
                writeMapping << tags[randomUnif2(totalTags) - 1] << '\t';
            }
            writeMapping << endl;
        }
        writeMapping.close();
    }
    return 0;
}
