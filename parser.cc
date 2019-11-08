#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
using namespace std;

string outputFile;

int main(int argc, char** argv) {
  if (argc != 2) {
    cout << "Syntax: " << argv[0] << " data_base.txt" << endl;
    exit(1);
  }

  ifstream in(argv[1]);
  while (not in.eof()) {
    string nom, posicio, club;
    int punts, preu;
    getline(in,nom,';');    if (nom == "") break;
    getline(in,posicio,';');
    in >> preu;
    char aux; in >> aux;
    getline(in,club,';');
    in >> punts;
    string aux2;
    getline(in,aux2);
    cout << "Nom: " << nom << endl;
    cout << "Posició: " << posicio << endl;
    cout << "Preu: " << preu << endl;
    cout << "Club: " << club << endl;
    cout << "Punts: " << punts << endl;
    cout << endl;
  }
  in.close(); 
}
