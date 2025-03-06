// mygrep.cpp
// Ege Özkan

// Kirjastot:
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>
using namespace std;

// Funktio, joka etsii hakumerkkijonon sijainnin kohdemerkkijonossa
int etsiMerkkijono(const string& kohde, const string& haku, bool caseInsensitive) {
    string kohdeCopy = kohde;
    string hakuCopy = haku;
    if (caseInsensitive) {
        transform(kohdeCopy.begin(), kohdeCopy.end(), kohdeCopy.begin(), ::tolower);
        transform(hakuCopy.begin(), hakuCopy.end(), hakuCopy.begin(), ::tolower);
    }
    size_t sijainti = kohdeCopy.find(hakuCopy);
    if (sijainti != string::npos) {
        return static_cast<int>(sijainti);
    }
    else {
        return -1;
    }
}

// Funktio, joka tarkistaa, sisältääkö rivi kaikki hakusanat (AND-logiikka)
bool sisaltaaKaikki(const string& rivi, const vector<string>& hakusanat, bool caseInsensitive) {
    for (const string& haku : hakusanat) {
        if (etsiMerkkijono(rivi, haku, caseInsensitive) == -1) {
            return false;
        }
    }
    return true;
}

// Funktio, joka tarkistaa, sisältääkö rivi jonkin hakusanoista (OR-logiikka)
bool sisaltaaJonkin(const string& rivi, const vector<string>& hakusanat, bool caseInsensitive) {
    for (const string& haku : hakusanat) {
        if (etsiMerkkijono(rivi, haku, caseInsensitive) != -1) {
            return true;
        }
    }
    return false;
}

// Pääohjelma:
int main(int argc, char* argv[]) {
    // Tarkistetaan, että komentoriviargumentteja on riittävästi
    if (argc < 4) {
        cout << "Käyttö: " << argv[0] << " [-valinnat] <AND/OR> <hakumerkkijono1> [<hakumerkkijono2> ...] <tiedostonimi>" << endl;
        cout << "Valinnat:" << endl;
        cout << "  -l  Tulosta rivinumerot" << endl;
        cout << "  -o  Tulosta löytyneiden rivien lukumäärä" << endl;
        cout << "  -r  Käänteinen haku (etsi rivit, jotka eivät sisällä hakusanaa)" << endl;
        cout << "  -i  Ei eroa isoilla ja pienillä kirjaimilla" << endl;
        return EXIT_FAILURE;
    }

    // Muuttujat valintojen tallentamiseen
    bool tulostaRivinumerot = false;
    bool tulostaLukumaarat = false;
    bool käänteinenHaku = false;
    bool caseInsensitive = false;
    int hakuArgIndeksi = 1;

    // Tarkistetaan, onko valintoja annettu
    if (argv[1][0] == '-') {
        string valinnat = argv[1];
        for (char valinta : valinnat) {
            if (valinta == 'l') {
                tulostaRivinumerot = true;
            }
            else if (valinta == 'o') {
                tulostaLukumaarat = true;
            }
            else if (valinta == 'r') {
                käänteinenHaku = true;
            }
            else if (valinta == 'i') {
                caseInsensitive = true;
            }
        }
        hakuArgIndeksi = 2;
    }

    // Varmistetaan, että komentoriviargumentteja on riittävästi hakusanoille ja tiedostonimelle
    if (argc < hakuArgIndeksi + 3) {
        cout << "Käyttö: " << argv[0] << " [-valinnat] <AND/OR> <hakumerkkijono1> [<hakumerkkijono2> ...] <tiedostonimi>" << endl;
        return EXIT_FAILURE;
    }

    string logiikka = argv[hakuArgIndeksi];
    vector<string> hakusanat;
    for (int i = hakuArgIndeksi + 1; i < argc - 1; ++i) {
        hakusanat.push_back(argv[i]);
    }
    string tiedostonNimi = argv[argc - 1];

    ifstream tiedosto;
    try {
        tiedosto.open(tiedostonNimi);
        if (!tiedosto.is_open()) {
            throw runtime_error("Tiedostoa ei voitu avata");
        }
    }
    catch (const exception& e) {
        cout << "Poikkeus tapahtui: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    string rivi;
    int rivinumero = 0;
    int loydot = 0;

    while (getline(tiedosto, rivi)) {
        rivinumero++;
        bool loytyy = (logiikka == "AND") ? sisaltaaKaikki(rivi, hakusanat, caseInsensitive) : sisaltaaJonkin(rivi, hakusanat, caseInsensitive);
        if (käänteinenHaku) {
            loytyy = !loytyy;
        }
        if (loytyy) {
            if (tulostaRivinumerot) {
                cout << rivinumero << ": ";
            }
            cout << rivi << endl;
            loydot++;
        }
    }

    if (tulostaLukumaarat) {
        cout << "Rivit, jotka " << (käänteinenHaku ? "eivät " : "") << "sisältävät ";
        for (const string& haku : hakusanat) {
            cout << "\"" << haku << "\" ";
        }
        cout << ": " << loydot << endl;
    }

    tiedosto.close();
    return EXIT_SUCCESS;
}