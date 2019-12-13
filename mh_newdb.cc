#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cassert>
#include <ctime>

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

    void add(const Player& p) {
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
    }

    bool isComplete() {
        return int(POR.name != "") + DEF.size() + ATK.size() + MID.size() == 11;
    }

    Player getPlayer(int i) {
        if (i == 0) return POR;
        if (i <= nDEF) return DEF[i-1];
        if (i <= nMID+nDEF) return MID[i-(nDEF+1)];
        if (i <= nATK+nMID+nDEF) return ATK[i-(nMID+nDEF+1)];
        assert(false);
    }

    void exchangePlayer(int i, Player newP) {
        if (i == 0) POR = newP;
        if (i < nDEF) DEF[i-1] = newP;
        if (i < nMID+nDEF) MID[i-nDEF] = newP;
        if (i < nATK+nMID+nDEF) ATK[i-nMID] = newP;
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

//FALTA PASSAR DB AMB CONST
Player randPlayer(const string& pos, const DB& db){
    if(pos == "por") return db.POR[randInt(db.POR.size())];
    if(pos == "def") return db.DEF[randInt(db.DEF.size())];
    if(pos == "mig") return db.MID[randInt(db.MID.size())];
    if(pos == "dav") return db.ATK[randInt(db.ATK.size())];
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

void write(const Alignment& solution){
    const float time = float(clock() - begin_time) / CLOCKS_PER_SEC;

    ofstream out(file_name);
    out.setf(ios::fixed);
    out.precision(1);
    out << time << endl;
    out << solution << endl;
    out.close();

    cout << time << endl;
    cout << solution << endl;
}


Alignment generateInitialAlignment(const DB& players, const Input& input) {
    Alignment sol(input.N1, input.N2, input.N3);

    for (uint i = 0; i < players.size(); i++) {
        const Player& p = players[i];
        if (sol.total_price + p.price <= input.T) {
                if ((p.pos == "por" and sol.POR.name == "") or
                    (p.pos == "def" and int(sol.DEF.size()) < sol.nDEF) or
                    (p.pos == "mig" and int(sol.MID.size()) < sol.nMID) or
                    (p.pos == "dav" and int(sol.ATK.size()) < sol.nATK)) sol.add(p);
        }
        if (sol.isComplete()) return sol;
    }
    assert(false); //As there are fake players, we always can make a team
}

Alignment pickRandomNeighbour(Alignment a, const Input& input, const DB& players) {
    int rp = randInt(10); //Random player from original alignment
    const Player& p = a.getPlayer(rp);
    Player ri = randPlayer(p.pos, players); //Random player from DB
    bool selected = false;
    do {
        if (a.total_price - a.getPlayer(rp).price + ri.price < input.T) {
                selected = true;
                a.exchangePlayer(rp, ri);
            }
        else ri = randPlayer(p.pos, players);
    } while (not selected);
    return a;
}

const double T0 = 50;

double updateT(double oldT){
    return oldT - 0.1;
}

bool randomChosen(double T) {
    return randInt(100) < T;
}

void metaheuristic(const DB& players, const Input& input) {
    Alignment sol = generateInitialAlignment(players, input);
    double T = T0;
    int i = 0;
    while (i++ < 1000) {
        Alignment a = pickRandomNeighbour(sol, input, players);
        if (a.total_score > sol.total_score) {
            sol = a;
            write(sol);
        }
        else if (randomChosen(T)) sol = a;
        T = updateT(T);
        cerr << "Punts: " << sol.total_score << endl;
    }
    write(sol);
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
