#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <vector>
#include <ctime>
using namespace std;

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

    // A player is "lower" than another if it has lower score
    bool operator < (const Player& p) {
        return score < p.score;
    }
};

/*****
* DB
* Contains all the players of the database
*****/
using DB = vector<Player>;

// Load all the database in a vector of players
DB readDB(string file) {
    ifstream in(file);
    vector<Player> players;
    while (not in.eof()) {
        Player player;
        if (not player.read(in)) break;
        players.push_back(player);
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

Alignment exh(const DB &db, const Input &input){
    
}

// Algorisme: agafa els primers jugadors de la DB sense mirar preus ni score
Alignment exh(DB db, Input input) {
    Alignment solution(input.N1, input.N2, input.N3);
    int i = 0;
    while (db[i].pos != "por") i++;
    solution.POR = db[i];
    for (int j = 0; j < solution.nDEF; j++) {
        while (db[i].pos != "def") i++;
        solution.add(db[i]);
        i++;
    }
    for (int j = 0; j < solution.nMID; j++) {
        while (db[i].pos != "mig") i++;
        solution.add(db[i]);
        i++;
    }
    for (int j = 0; j < solution.nATK; j++) {
        while (db[i].pos != "dav") i++;
        solution.add(db[i]);
        i++;
    }
    return solution;
}

/*
*  Example of use: ./exh.out data_base.txt public_benchs/easy-1.txt
*/
int main(int argc, char** argv) {
    assert(argc == 3);

    // Read all input
    DB players = readDB(argv[1]);
    Input input;
    input.read(argv[2]);

    // Aqui empieza la magia :)
    const clock_t begin_time = clock();
    Alignment solution = exh(players, input);
    cout << float( clock () - begin_time ) / CLOCKS_PER_SEC << endl;
    cout << solution;
}
