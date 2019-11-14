#include <iostream>
#include <algorithm>
#include <fstream>
#include <cassert>
#include <string>
#include <vector>
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

    // A player is "lower" than another if it has lower score
    bool operator < (const Player& p) {
        if(score == p.score) return price < p.price;
        return score < p.score;
    }
};

/*****
* DB
* Contains all the players of the database
*****/
struct DB {
    vector<Player> players;
    int minPrice;
    int maxScore;

    DB (string file, const Input& input) {
        readDB(file, input);

        minPrice = 1e9; maxScore = 0;
        for (Player p : players) {
            if (p.price != 0 and p.price < minPrice) minPrice = p.price;
            if (p.score > maxScore) maxScore = p.score;
        }

    }

    void readDB(string file, const Input& input) {
        ifstream in(file);
        vector<Player> playerList;
        while (not in.eof()) {
            Player player;
            if (not player.read(in)) break;
            if (player.price < input.J) playerList.push_back(player);
        }
        in.close();
        players = playerList;
    }
};


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

    Alignment(const DB& db, vector<bool>& selected, int price, int score){
        total_price = price; total_score = score;
        nDEF = nMID = nATK = 0;
        for (uint i = 0; i < selected.size(); i++) {
            if (selected[i]) {
                Player p = db.players[i];
                if (p.pos == "por") POR = p;
                else if (p.pos == "def") { DEF.push_back(p); nDEF++; }
                else if (p.pos == "mig") { MID.push_back(p); nMID++; }
                else if (p.pos == "dav") { ATK.push_back(p); nATK++; }
            }
        }
    }
};

// Para poder hacer cout << Alignment;
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

    cerr << time << endl;
    cerr << solution << endl;
}

void search(uint i, vector<bool>& used, int price, int score, int por, int n1, int n2, int n3, const DB &db, const Input &input, Alignment& solution){
    if (n1+n2+n3+por == 11){
        if(score > solution.total_score){ solution = Alignment(db, used, price, score); write(solution); }
        return;
    }
    if (i >= used.size()) return;
    else {

        Player p = db.players[i];
        used[i] = true;
        if (price + p.price + (10-por-n1-n2-n3)*db.minPrice <= input.T and
            score + p.score + (10-por-n1-n2-n3)*db.maxScore > solution.total_score) {
                 if (p.pos == "por") { if (por < 1)       search(i+1, used, price+p.price, score+p.score, por+1, n1, n2, n3, db, input, solution); }
            else if (p.pos == "def") { if (n1 < input.N1) search(i+1, used, price+p.price, score+p.score, por, n1+1, n2, n3, db, input, solution); }
            else if (p.pos == "mig") { if (n2 < input.N2) search(i+1, used, price+p.price, score+p.score, por, n1, n2+1, n3, db, input, solution); }
            else if (p.pos == "dav") { if (n3 < input.N3) search(i+1, used, price+p.price, score+p.score, por, n1, n2, n3+1, db, input, solution); }
        }

        used[i] = false;
        search(i+1, used, price, score, por, n1, n2, n3, db, input, solution);
    }
}

Alignment exh(DB &db, const Input &input){
    sort(db.players.begin(), db.players.end());
    vector<bool> used (db.players.size(), false);
    Alignment solution = Alignment();
    search(0, used, 0, 0, 0, 0, 0, 0, db, input, solution);
    return solution;
}

/*
*  Example of use: ./a.out data_base.txt public_benchs/easy-1.txt solutions.txt
*/
int main(int argc, char** argv) {
    assert(argc == 4);

    cerr.setf(ios::fixed);
    cerr.precision(1);

    begin_time = clock();
    file_name = argv[3];

    // Read all input
    Input input;
    input.read(argv[2]);
    DB players(argv[1], input);

    Alignment solution = exh(players, input);
    write(solution);

}
