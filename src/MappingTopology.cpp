/********************************
Search for optimal RI
********************************/

#include "Header.h"

// Initial maximum step size is _INITIAL_COUNT_MAX
// And if an improvement is found when less than _THRESHOLD_COUNT_MAX steps are left
// The maximum step size is then augmented by _ADD_COUNT_MAX
#define _INITIAL_COUNT_MAX 500000
#define _ADD_COUNT_MAX 200000
#define _THRESHOLD_COUNT_MAX 200000

// multiple runs and burn-in stages
#define _TOTAL_TRIALS 5
#define _BURN_IN 1000

// Choose uniform randomly from 1,2,...,Max
static int randomUnif(int Max) {
    int value = rand();
    int multiple = RAND_MAX / Max;
    if (value >= multiple * Max) {
        value = rand();
    }
    return (value % Max) + 1;
}

// Input Roriginal: R from previous mapping
// The function finds a good new topology that is closest to the original
void Mapping::OptimalTopology (vector<int>& Roriginal, vector<int>& TagPrice) {
    vector<bool> RIproposed ((M + 1) * (M + 1), 0);
    double currentEnergy;
    bool acceptanceProb;
    // Initial search with a balanced RI
    InitRI();
    if (S == M - 1)
        return;
    currentEnergy = 0;
    double MinEnergy = currentEnergy;
    double EnergyDifference;  // current minus proposed
    vector<bool> MinTopology(RI);
    for (int trials = 1; trials <= _TOTAL_TRIALS; trials++) {
        int countMax = _INITIAL_COUNT_MAX;
        int count(0);  // number of proposals made
        while (count < countMax) {
            ++count;
            if (count < _BURN_IN) {
                RIproposed = RI;
                MakeRIProposal(RIproposed, Roriginal, EnergyDifference, TagPrice);
                RI.swap(RIproposed);
                currentEnergy -= EnergyDifference;
            }
            RIproposed = RI;
            MakeRIProposal(RIproposed, Roriginal, EnergyDifference, TagPrice);
            if (EnergyDifference < 0)
                continue;
            else if (EnergyDifference > 0)
                acceptanceProb = 1;
            else
                acceptanceProb = 0;
            if (acceptanceProb || randomUnif(2) == 1) {  // accept with 1/2 prob. if equal
                // We accpet here
                RI.swap(RIproposed);
                currentEnergy -= EnergyDifference;
                if (currentEnergy < MinEnergy) {
                    MinEnergy = currentEnergy;
                    MinTopology = RI;
                    if (countMax - count < _THRESHOLD_COUNT_MAX) {
                        // Dynamically augment countMax
                        countMax += _ADD_COUNT_MAX;
                    }
                }
            }
        }
    }
    RI.swap(MinTopology);
    return;
}

// A naive accpetance/rejection sampling
// Pick two rows and two columns for proposing a switch
// difference: current minus proposed
void Mapping::MakeRIProposal(vector<bool>& RIproposed,
                             vector<int>& Roriginal,
                             double& EnergyDifference,
                             vector<int>& TagPrice) {
    int row1, row2, col1, col2;
    while (1) {
        row1 = randomUnif(M);
        row2 = randomUnif(M);
        if (row2 == row1)
            continue;
        col1 = randomUnif(M);
        if (col1 == row1 || col1 == row2)
            continue;
        col2 = randomUnif(M);
        if (col2 == row1 || col2 == row2 || col2 == col1)
            continue;
        if (RIproposed[row1 * (M + 1) + col1] == RIproposed[row1 * (M + 1) + col2] ||
            RIproposed[row2 * (M + 1) + col1] == RIproposed[row2 * (M + 1) + col2] ||
            RIproposed[row1 * (M + 1) + col1] == RIproposed[row2 * (M + 1) + col1] ||
            RIproposed[row1 * (M + 1) + col2] == RIproposed[row2 * (M + 1) + col2]) {
                continue;
            }
        // Do the switch as our proposal
        RIproposed[row1 * (M + 1) + col1] = !RIproposed[row1 * (M + 1) + col1];
        RIproposed[row1 * (M + 1) + col2] = !RIproposed[row1 * (M + 1) + col2];
        RIproposed[row2 * (M + 1) + col1] = !RIproposed[row2 * (M + 1) + col1];
        RIproposed[row2 * (M + 1) + col2] = !RIproposed[row2 * (M + 1) + col2];
        break;
    }
    const double unitValue = double(N) / M * (L - 1) / S;
    EnergyDifference = abs(unitValue * !RIproposed[row1 * (M + 1) + col1] - Roriginal[row1 * (M + 1) + col1]);
    EnergyDifference += abs(unitValue * !RIproposed[row1 * (M + 1) + col2] - Roriginal[row1 * (M + 1) + col2]);
    EnergyDifference += abs(unitValue * !RIproposed[row2 * (M + 1) + col1] - Roriginal[row2 * (M + 1) + col1]);
    EnergyDifference += abs(unitValue * !RIproposed[row2 * (M + 1) + col2] - Roriginal[row2 * (M + 1) + col2]);
    EnergyDifference -= abs(unitValue * RIproposed[row1 * (M + 1) + col1] - Roriginal[row1 * (M + 1) + col1]);
    EnergyDifference -= abs(unitValue * RIproposed[row1 * (M + 1) + col2] - Roriginal[row1 * (M + 1) + col2]);
    EnergyDifference -= abs(unitValue * RIproposed[row2 * (M + 1) + col1] - Roriginal[row2 * (M + 1) + col1]);
    EnergyDifference -= abs(unitValue * RIproposed[row2 * (M + 1) + col2] - Roriginal[row2 * (M + 1) + col2]);
    int tagNumber = TagPrice.size();
    for (int i = 0; i < tagNumber; i++) {
        double tagMatch[4];
        tagMatch[0] = double(!nodeTagList[i * tagNumber + row1].compare(nodeTagList[i * tagNumber + col1]));
        tagMatch[1] = double(!nodeTagList[i * tagNumber + row2].compare(nodeTagList[i * tagNumber + col2]));
        tagMatch[2] = double(!nodeTagList[i * tagNumber + row1].compare(nodeTagList[i * tagNumber + col2]));
        tagMatch[3] = double(!nodeTagList[i * tagNumber + row2].compare(nodeTagList[i * tagNumber + col1]));
        if(RIproposed[row1 * (M + 1) + col1]) {
            EnergyDifference += TagPrice[i] * (tagMatch[2] + tagMatch[3] - tagMatch[0] - tagMatch[1]);
        } else {
            EnergyDifference += TagPrice[i] * (tagMatch[0] + tagMatch[1] - tagMatch[2] - tagMatch[3]);
        }
    }
}

// from RI to a balanced R
void Mapping::RfromTopology() {
    if (M > 1) {
        DisperseR();
        GetRI();
        BalanceR();
    }
    return;
}

// from RI to a balanced R
void Mapping::RfromTopology(vector<int>& Roriginal) {
    if (M > 1) {
        DisperseR(Roriginal);
        GetRI();
        BalanceR();
    }
    return;
}
