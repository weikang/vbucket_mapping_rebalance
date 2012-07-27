#include "Header.h"

// caseSize: test cases to generate

int main(int argc, char * argv[]) {
    int caseSize(9);
    if (argc > 1)
        caseSize = String2Int(string(argv[1]));
    ofstream writeMapping;
    ifstream readMapping;
    int i, j, k;
    int N, L, M, S;
    vector<int> index;
    vector<int> index2;
    string filename;
    int addnode;
    int percentNodekept;
    for (i = 0; i <= caseSize; i++) {
        N = 512 * randomUnif2(4);
        L = randomUnif2(3) + 1;
        M = randomUnif2(30) + 5;
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
        for (j = 0; j < map1.N; j++) {
            for (k = 0; k< map1.L; k++) {
                if (randomUnif2(100) < 96) // 95 percent true
                    writeMapping << map1.Aname[index[j] * map1.L + k] << '\t';
                else  // 5 percent garbage
                    writeMapping << map1.Aname[index2[j] * map1.L + k] << '\t';
            }
            writeMapping << endl;
        }
        vector<string> nodeNameList(map1.nodeNameList);
        addnode = randomUnif2(10);
        for (j = 0; j < addnode; j++) {
            writeMapping << "newnode" + Int2String(j) << endl;
        }
        percentNodekept = randomUnif2(50) + 45;
        for (j = 1; j < (int)nodeNameList.size(); j++) {
            if (randomUnif2(100) <= percentNodekept)
                writeMapping << nodeNameList[j] << endl;
        }
        writeMapping.close();
    }
    return 0;
}
