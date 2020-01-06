#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <ctime>
#include <cmath>

using namespace std;

clock_t begin_time;
string file_name;

/*****
* INPUT
* Contains the variables needed for the problem
*****/
struct Input {
    int N1, N2, N3, T, J;

    void read(string file) {
        ifstream in(file);
        while (not in.eof())
            in >> N1 >> N2 >> N3 >> T >> J;
        in.close();
    }
};


/*****
* PLAYER
* Contains all information about each player
*****/
struct Player {
    string name;
    string pos;
    int price;
    string club;
    int score;

    bool read(ifstream& in) {
        getline(in,name,';'); if (name == "") return false;
        getline(in,pos,';');
        in >> price;
        char aux; in >> aux;
        getline(in,club,';');
        in >> score;
        string aux2;
        getline(in,aux2);
        return true;
    }
    friend bool operator == (const Player& p1, const Player& p2) {
        return (p1.name == p2.name)
           and (p1.pos == p2.pos)
           and (p1.price == p2.price)
           and (p1.club == p2.club)
           and (p1.score == p2.score);
}

};

/*****
* DB
* Contains the players of the database
* (added if their price is lower than the maximum price per player)
*****/
struct DB{
    vector<Player> POR;
    vector<Player> DEF;
    vector<Player> MID;
    vector<Player> ATK;
};

DB readDB(string file, const Input& input) {
    ifstream in(file);
    DB players;
    while (not in.eof()) {
        Player p;
        if (not p.read(in)) break;
        if (p.price < input.J) {
                 if(p.pos == "por") players.POR.push_back(p);
            else if(p.pos == "def") players.DEF.push_back(p);
            else if(p.pos == "mig") players.MID.push_back(p);
            else if(p.pos == "dav") players.ATK.push_back(p);
        }
    }
    in.close();
    return players;
}


/*****
* ALIGNMENT
* Set of 11 players divided by position
* -> |POR| + |DEF| + |MID| + |ATK| <= 11
* -> |DEF| <= nDEF, |MID| <= nMID, |ATK| <= nATK
*****/
struct Alignment {
    Player POR;
    vector<Player> DEF;
    vector<Player> MID;
    vector<Player> ATK;
    int nDEF, nMID, nATK;
    int total_price, total_score;

    Alignment() {
        total_price = total_score = 0;
    }

    Alignment(int n1, int n2, int n3) {
        nDEF = n1; nMID = n2; nATK = n3;
        total_price = 0;
        total_score = 0;
    }

    bool repeated(const Player& p){
        if (p.pos == "por") return POR == p;
        if (p.pos == "def") {
            for(uint i = 0; i < DEF.size(); ++i){
                if(p == DEF[i]) return true;
            }
        }
        else if (p.pos == "mig") {
            for(uint i = 0; i < MID.size(); ++i){
                if(p == MID[i]) return true;
            }
        }
        else if (p.pos == "dav") {
            for(uint i = 0; i < ATK.size(); ++i){
                if(p == ATK[i]) return true;
            }
        }
        return false;
    }

    bool add(const Player& p) {
        if(repeated(p)) return false;
        if (p.pos == "por") POR = p;
        if (p.pos == "def") {
            assert(int(DEF.size()) < nDEF);
            DEF.push_back(p);
        }
        if (p.pos == "mig") {
            assert(int(MID.size()) < nMID);
            MID.push_back(p);
        }
        if (p.pos == "dav") {
            assert(int(ATK.size()) < nATK);
            ATK.push_back(p);
        }
        total_price += p.price;
        total_score += p.score;
        return true;
    }

    bool isComplete() {
        return int(POR.name != "") + DEF.size() + ATK.size() + MID.size() == 11;
    }

    Player getPlayer(int i) {
        if (i == 0) return POR;
        if (i <= nDEF) return DEF[i-1];
        if (i <= nMID+nDEF) return MID[i-(nDEF+1)];
        if (i <= nATK+nMID+nDEF) return ATK[i-(nMID+nDEF+1)];
        cout << "=> " << i << endl;
        assert(false);
    }

    bool exchangePlayer(int i, Player newP) {

        if(repeated(newP)) return false;
        Player oldP = getPlayer(i);
        total_score += (newP.score - oldP.score);
        total_price += (newP.price - oldP.price);

        if (i == 0) POR = newP;
        else if (i <= nDEF) DEF[i-1] = newP;
        else if (i <= nMID+nDEF) MID[i-(nDEF+1)] = newP;
        else if (i <= nATK+nMID+nDEF) ATK[i-(nMID+nDEF+1)] = newP;
        return true;
    }
};


/*****
* Generates a random integer in [a,b]
*****/
int randInt(int a, int b) {
    assert(b > a);
    return rand() % (b-a+1) + a;
}

int randInt(int b) { // a = 0
    assert(b > 0);
    return rand() % (b+1);
}

/*****
 * Generates a random double between (0,1)
 *****/
double randDouble() { 
    return rand() / RAND_MAX;
}

Player randPlayer(const string& pos, const DB& db){
    if(pos == "por") return db.POR[randInt(db.POR.size()-1)];
    if(pos == "def") return db.DEF[randInt(db.DEF.size()-1)];
    if(pos == "mig") return db.MID[randInt(db.MID.size()-1)];
    if(pos == "dav") return db.ATK[randInt(db.ATK.size()-1)];
    cout << pos << endl;
    assert(false);
}

// In order to be able to do: cout << Alignment;
ostream & operator << (ostream &out, const Alignment &a) {
    out << "POR: " << a.POR.name;
    out << endl << "DEF: ";
    for (int i = 0; i < a.nDEF; i++)
        out << (i == 0 ? "" : ";") << a.DEF[i].name;
    out << endl << "MIG: ";
    for (int i = 0; i < a.nMID; i++)
        out << (i == 0 ? "" : ";") << a.MID[i].name;
    out << endl << "DAV: ";
    for (int i = 0; i < a.nATK; i++)
        out << (i == 0 ? "" : ";") << a.ATK[i].name;
    out << endl << "Punts: " << a.total_score << endl;
    out << "Preu: " << a.total_price << endl;
    return out;
}

void write(const Alignment& solution, bool print){
    const float time = float(clock() - begin_time) / CLOCKS_PER_SEC;

    ofstream out(file_name);
    out.setf(ios::fixed);
    out.precision(1);
    out << time << endl;
    out << solution << endl;
    out.close();

    if(print){
        cout << time << endl;
        cout << solution << endl;
    }
}

Alignment generateInitialAlignment(const Input& input, const DB& players) {
    Alignment sol(input.N1, input.N2, input.N3);
    Player p;

    while(input.N1 != int(sol.DEF.size())){
        p = randPlayer("def", players);
        if(sol.total_price + p.price < input.T) sol.add(p);
    }
    while(input.N2 != int(sol.MID.size())){
        p = randPlayer("mig", players);
        if(sol.total_price + p.price < input.T) sol.add(p);
    }
    while(input.N3 != int(sol.ATK.size())){
        p = randPlayer("dav", players);
        if(sol.total_price + p.price < input.T) sol.add(p);
    }
    while(sol.POR.name == ""){
        p = randPlayer("por", players);
        if(sol.total_price + p.price < input.T) sol.add(p);
    }
    cout << sol;
    return sol; //As there are fake players, we always can make a team
}

Alignment pickRandomNeighbour(Alignment a, const Input& input, const DB& players) {
    int rp = randInt(10); 

    const Player& p = a.getPlayer(rp); //Random player from original alignment

    bool selected = false;
    while (not selected) {
        Player ri = randPlayer(p.pos, players); // Random player from DB
        if (a.total_price - p.price + ri.price < input.T) selected = a.exchangePlayer(rp, ri);
    }
    return a;
}

const double T0 = 1e9; // 40
const double alpha = 0.99999;

double updateT(double oldT){
    oldT *= alpha; //oldT -= 0.1;
    if(oldT < 0.05) return 0.05;
    return oldT;
}

/*bool randomChosen(double T) {
    return randInt(100) <= T;
}*/

bool randomChosen(const Alignment& sol, const Alignment& worse_sol, const double T) {
    return randDouble() < exp((worse_sol.total_score - sol.total_score)/T);
}

void metaheuristic(const DB& players, const Input& input) {
    Alignment best = Alignment();
    Alignment sol = generateInitialAlignment(input, players);
    double T = T0;
    while (true) {
        Alignment a = pickRandomNeighbour(sol, input, players);

        //Actualitzo sol si milloro o amb una probabilitat Pr(T):
        if (a.total_score > sol.total_score or randomChosen(sol, a, T)) sol = a;

        //Nomès imprimeixo una nova solució si és millor que la que ja tenia:
        if (sol.total_score > best.total_score){
            best = sol;
            write(best, true);
            cerr << "T actual: " << T << endl;
        }

        T = updateT(T);

    }
}



int main(int argc, char** argv) {
    if(argc != 4){
        cout << "Sintaxi incorrecta!" << endl;
        cout << "Exemple d'ús: " << argv[0] << " data_base.txt input.txt solutions.txt" << endl;
        return 1;
    }

    cout.setf(ios::fixed);
    cout.precision(1);

    begin_time = clock();
    srand(time(NULL));

    file_name = argv[3];

    // Read all input
    Input input;
    input.read(argv[2]);
    DB players = readDB(argv[1], input);

    metaheuristic(players, input);
}

