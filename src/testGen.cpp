#include "Header.h"

// caseSize: test cases to generate

int main(int argc, char * argv[]) {
    ofstream writeMapping;
    ifstream readMapping;
    int seed(1);
    if (argc > 2)
        seed = String2Int(string(argv[2]));
    srand(seed);
    int i, j, k;
    int m0, m1;
    int N(1024), L, M, S(10);
    L = String2Int(string(argv[1]));
    vector<int> index;
    string filename;
    for (m0 = 1; m0 <= 100; ++m0) {
        for (m1 = 1; m1 <= 100; ++m1) {
            if (m0 == m1)
                continue;
            M = m0;
            i = m0 * 1000 + m1;
            filename = "testCases/input" + Int2String(i);
            Mapping map1(N, L, M, S);
            map1.GenerateNodeName();
            map1.InitRI();
            map1.Generate();

            index.clear();
            for (j = 0; j < map1.N; j++)
                index.push_back(j);
            Shuffle(index);
            writeMapping.open(filename.c_str() );
            writeMapping << map1.N << endl
                         << map1.L << endl
                         << map1.M << endl
                         << map1.S << endl;
            for (j = 0; j < map1.N; j++) {
                for (k = 0; k < map1.L; k++) {
                    writeMapping << map1.Aname[index[j] * map1.L + k] << '\t';
                }
                writeMapping << endl;
            }
            writeMapping << "10\n" << L << endl;

            if (m1 > m0) {  // add nodes
                for (j = 0; j < m1 - m0; j++) {
                    writeMapping << "newnode" + Int2String(j) << endl;
                }
                for (j = 1; j < (int) map1.nodeNameList.size(); ++j) {
                    writeMapping << map1.nodeNameList[j] << endl;
                }
            } else if (m1 < m0) {  // delete nodes
                index.clear();
                for (j = 1; j < (int) map1.nodeNameList.size(); ++j)
                    index.push_back(j);
                Shuffle(index);
                for (j = 0; j < m1; ++j) {
                    writeMapping << map1.nodeNameList[index[j]] << endl;
                }
            }
            writeMapping.close();
        }
    }
    return 0;
}
