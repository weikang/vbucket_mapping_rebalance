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
void Mapping::OptimalTopology (vector<int>& Roriginal) {
    vector<bool> RIproposed ((M + 1) * (M + 1), 0);
    double currentEnergy;
    bool acceptanceProb;
    // Initial search with a balanced RI
    InitRI();
    if (S == M - 1)
        return;
    currentEnergy = AssessEnergy(Roriginal, RI);
    double MinEnergy = currentEnergy;
    double EnergyDifference;  // current minus proposed
    vector<bool> MinTopology(RI);

    int countMax = _INITIAL_COUNT_MAX;
    int count(0);  // number of proposals made
    while (count < countMax) {
        ++count;
        RIproposed = RI;
        MakeRIProposal(RIproposed, Roriginal, EnergyDifference);
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
    // cout << MinEnergy << '\t';
    RI.swap(MinTopology);
    return;
}

double Mapping::AssessEnergy(vector<int>& Roriginal, vector<bool>& RIproposed) {
    int i, j;
    double energy(0);
    const double unitValue = double(N) / M * (L - 1) / S;
    for (i = 1; i <= M; ++i) {
        for (j = 1; j <= M; ++j) {
            energy += abs(unitValue * RIproposed[i * (M + 1) + j] - Roriginal[i * (M + 1) + j]);
        }
    }
    return energy;
}

// A naive accpetance/rejection sampling
// Pick two rows and two columns for proposing a switch
void Mapping::MakeRIProposal(vector<bool>& RIproposed,
                             vector<int>& Roriginal,
                             double& EnergyDifference) {
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
