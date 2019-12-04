#include <iostream>
#include <algorithm>
#include <fstream>
#include <vector>
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
};


/*****
* DB
* Contains the players of the database
* (added if their price is lower than the maximum price per player)
*****/
using DB = vector<Player>;

DB readDB(string file, const Input& input) {
    ifstream in(file);
    DB playerList;
    while (not in.eof()) {
        Player player;
        if (not player.read(in)) break;
        if (player.price < input.J) playerList.push_back(player);
    }
    in.close();
    return playerList;
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
        POR.name = "";
    }

    Alignment(const DB& db, vector<bool>& selected, int price, int score){
        total_price = price; total_score = score;
        nDEF = nMID = nATK = 0;
        for (uint i = 0; i < selected.size(); i++) {
            if (selected[i]) {
                Player p = db[i];
                if (p.pos == "por") POR = p;
                else if (p.pos == "def") { DEF.push_back(p); nDEF++; }
                else if (p.pos == "mig") { MID.push_back(p); nMID++; }
                else if (p.pos == "dav") { ATK.push_back(p); nATK++; }
            }
        }
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
};

// Esto es muy extra, para poder hacer cout << Alignment;
// Si no te gusta, se puede hacer un print normal;
ostream & operator << (ostream &out, const Alignment &a) {
    out << "POR: " << a.POR.name;
    out << endl << "DEF: ";
    for (int i = 0; i < a.nDEF; i++)
        out << (i == 0 ? "" : ";") << a.DEF[i].name;
    out << endl << "MID: ";
    for (int i = 0; i < a.nMID; i++)
        out << (i == 0 ? "" : ";") << a.MID[i].name;
    out << endl << "DAV: ";
    for (int i = 0; i < a.nATK; i++)
        out << (i == 0 ? "" : ";") << a.ATK[i].name;
    out << endl << "Punts: " << a.total_score << endl;
    out << "Preu: " << a.total_price << endl;
    return out;
}

double a = 3.2;
double b = 0.6;

bool comp(const Player& p1, const Player& p2) {
    //if(a.score == b.score) return a.price > b.price;
    //return a.score > b.score;
    if (p1.price == 0) return false;
    if (p2.price == 0) return true;
    return (pow(p1.score, a))*pow(p2.price, b) > pow(p2.score, a)*pow(p1.price, b);
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


Alignment greedy(DB& players, const Input& input) {
    Alignment solution(input.N1, input.N2, input.N3);
    sort(players.begin(), players.end(), comp);
    cerr << "sorted" << endl;
    for (uint i = 0; i < players.size(); i++) {
        Player& p = players[i];
        if (solution.total_price + p.price <= input.T) {
            if (p.pos == "por") {
                if (solution.POR.name == "")
                    solution.add(p);
            } else if (p.pos == "def") {
                if (solution.DEF.size() < solution.nDEF)
                    solution.add(p);
            } else if (p.pos == "mig") {
                if (solution.MID.size() < solution.nMID)
                    solution.add(p);
            } else if (p.pos == "dav") {
                if (solution.ATK.size() < solution.nATK)
                    solution.add(p);
            }
            else assert(false);

            if (solution.isComplete()) return solution;
        }
    }
}


int main(int argc, char** argv) {
    assert(argc == 4);

    // Read all input
    Input input;
    input.read(argv[2]);
    DB players = readDB(argv[1], input);

    // Aqui empieza la magia :)
    begin_time = clock();
    file_name = argv[3];
    Alignment solution = greedy(players, input);
    //cout << float( clock () - begin_time ) / CLOCKS_PER_SEC << endl;
    write(solution);
}
