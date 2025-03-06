// mygrep.cpp
// Ege �zkan

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

// Funktio, joka tarkistaa, sis�lt��k� rivi kaikki hakusanat (AND-logiikka)
bool sisaltaaKaikki(const string& rivi, const vector<string>& hakusanat, bool caseInsensitive) {
    for (const string& haku : hakusanat) {
        if (etsiMerkkijono(rivi, haku, caseInsensitive) == -1) {
            return false;
        }
    }
    return true;
}

// Funktio, joka tarkistaa, sis�lt��k� rivi jonkin hakusanoista (OR-logiikka)
bool sisaltaaJonkin(const string& rivi, const vector<string>& hakusanat, bool caseInsensitive) {
    for (const string& haku : hakusanat) {
        if (etsiMerkkijono(rivi, haku, caseInsensitive) != -1) {
            return true;
        }
    }
    return false;
}

// P��ohjelma:
int main(int argc, char* argv[]) {
    // Tarkistetaan, ett� komentoriviargumentteja on riitt�v�sti
    if (argc < 4) {
        cout << "K�ytt�: " << argv[0] << " [-valinnat] <AND/OR> <hakumerkkijono1> [<hakumerkkijono2> ...] <tiedostonimi>" << endl;
        cout << "Valinnat:" << endl;
        cout << "  -l  Tulosta rivinumerot" << endl;
        cout << "  -o  Tulosta l�ytyneiden rivien lukum��r�" << endl;
        cout << "  -r  K��nteinen haku (etsi rivit, jotka eiv�t sis�ll� hakusanaa)" << endl;
        cout << "  -i  Ei eroa isoilla ja pienill� kirjaimilla" << endl;
        return EXIT_FAILURE;
    }

    // Muuttujat valintojen tallentamiseen
    bool tulostaRivinumerot = false;
    bool tulostaLukumaarat = false;
    bool k��nteinenHaku = false;
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
                k��nteinenHaku = true;
            }
            else if (valinta == 'i') {
                caseInsensitive = true;
            }
        }
        hakuArgIndeksi = 2;
    }

    // Varmistetaan, ett� komentoriviargumentteja on riitt�v�sti hakusanoille ja tiedostonimelle
    if (argc < hakuArgIndeksi + 3) {
        cout << "K�ytt�: " << argv[0] << " [-valinnat] <AND/OR> <hakumerkkijono1> [<hakumerkkijono2> ...] <tiedostonimi>" << endl;
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
        if (k��nteinenHaku) {
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
        cout << "Rivit, jotka " << (k��nteinenHaku ? "eiv�t " : "") << "sis�lt�v�t ";
        for (const string& haku : hakusanat) {
            cout << "\"" << haku << "\" ";
        }
        cout << ": " << loydot << endl;
    }

    tiedosto.close();
    return EXIT_SUCCESS;
}