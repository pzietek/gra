#include<iostream>
#include<vector>
#include<memory>
#include<string>
#include<cstdlib>

using namespace std;

void cls()
{
    system("cls||clear");
    return;
}

enum Wynik_strzalu {NIETRAFIONY, TRAFIONY, ZATOPIONY};
enum Kierunek {POZIOMO, PIONOWO};
using Pozycja  = pair<int, int>;

char symbol_strzalu(Wynik_strzalu wynik) {
    if(wynik == NIETRAFIONY)
        return 'X';
    else if(wynik == TRAFIONY)
        return 'T';
    else return 'Z';
}

char symbol_statku(Wynik_strzalu wynik) {
    if(wynik == NIETRAFIONY)
        return 'O';
    else if(wynik == TRAFIONY)
        return 'T';
    else return 'Z';
}

char symbol_wody(bool bylo_strzelane) {
    if(bylo_strzelane)
        return 'X';
    else
        return ' ';
}

class Statek {
private :
    string nazwa;
    int rozmiar;
    int zycie;
public:
    Statek(int rozm, string nazw);
    void trafiony();
    int rozm() const;
    string nazw() const;
    int zycie_statku() const;
    Wynik_strzalu stan_statku() const;
};

class Pole {
public:
    virtual char symbol() const = 0;
    virtual Wynik_strzalu wynik_strzalu() = 0;
    virtual string opis() const = 0;
};

class Pole_wody : public Pole {
private:
    bool bylo_strzelane;
public:
    Pole_wody();
    char symbol() const override;
    Wynik_strzalu wynik_strzalu() override;
    string opis() const override;
};

class Pole_statku : public Pole {
private:
    shared_ptr<Statek> statek;
public:
    Pole_statku(shared_ptr<Statek> statek_);
    char symbol() const override;
    Wynik_strzalu wynik_strzalu() override;
    string opis() const override;
};

class Plansza {
private:
    vector<vector<shared_ptr<Pole>>> plansza;
    vector<vector<char>> strzaly;
    vector<shared_ptr<Statek>> statki;
public:
    Plansza();
    void czysc();
    void dodaj_strzal(Pozycja poz, Wynik_strzalu wynik);
    shared_ptr<Pole> get_pole(Pozycja poz);
    char get_pole_strzalu(Pozycja poz);
    bool pole_istnieje(Pozycja poz) const;
    bool tu_juz_bylo_strzelane(Pozycja poz) const;
    vector<shared_ptr<Statek>> moje_statki() const;
    void ustaw_statek(shared_ptr<Statek> statek, Kierunek kierunek, Pozycja poz);
};

class UI;

class Gracz {
private:
    int num;
    Plansza plansza;
    UI &ui;
public:
    Gracz(int numer, UI &ui_);
    int numer() const;
    void ustaw_flote();
    int zycie() const;
    Plansza & get_plansza();
    bool strzal(Gracz g);
    void wygral();
};

class UI {
public:
    Pozycja zapytaj_o_strzal() const;
    Pozycja zapytaj_o_poczatek_statku() const;
    Kierunek zapytaj_o_kierunek() const;
    void poczatek_ustawiania(shared_ptr<Statek> statek) const ;
    void koniec_ustawiania() const;
    void wynik_strzalu(Wynik_strzalu wynik) const;
    void wcisnij_enter() const;
    void podaj_zwyciezce(Gracz g) const;
    void pokaz(Gracz g) const;
    void takie_pole_nie_istnieje() const;
    void tu_juz_strzelales() const;
};

class Gra {
private:
    vector<Gracz> gracze;
public:
    Gra(UI &ui);
    bool czy_koniec() const;
    void nowa_gra();
    Gracz zwyciezca() const;
};

////////////////////STATEK/////////////////////////////////////////////////////////////////////////////

Statek::Statek(int rozm, string nazw)
    : rozmiar(rozm)
    , zycie(rozm)
    , nazwa(nazw)
    {}

void Statek::trafiony() {
    zycie--;
}

int Statek::rozm() const {
    return rozmiar;
}

string Statek::nazw() const {
    return nazwa;
}

int Statek::zycie_statku() const {
    return zycie;
}

Wynik_strzalu Statek::stan_statku() const {
    if (zycie == rozmiar)
        return NIETRAFIONY;
    else if (zycie == 0)
        return ZATOPIONY;
    else
        return TRAFIONY;
}

/////////////////////POLE WODY///////////////////////////////////////////////////////////////

Pole_wody::Pole_wody(): bylo_strzelane(false) {}
    char Pole_wody::symbol() const  {
        return symbol_wody(bylo_strzelane);
}

Wynik_strzalu Pole_wody::wynik_strzalu() {
    bylo_strzelane = true;
    return NIETRAFIONY;
}

string Pole_wody::opis() const {
    return "Woda";
}

///////////////////POLE STATKU///////////////////////////////////////////////////////////////

Pole_statku::Pole_statku(shared_ptr<Statek> statek_)
    : statek(statek_)
    {}

char Pole_statku::symbol() const {
    switch(statek->stan_statku()) {
        case NIETRAFIONY:
            return symbol_statku(NIETRAFIONY);
        case TRAFIONY:
            return symbol_statku(TRAFIONY);
        case ZATOPIONY:
            return symbol_statku(ZATOPIONY);
    }
}

Wynik_strzalu Pole_statku::wynik_strzalu() {
    statek->trafiony();
    return statek->stan_statku();
}

string Pole_statku::opis() const {
    return "Statek";
}

///////////////////PLANSZA////////////////////////////////////////////////////////////////////////////

Plansza::Plansza()
    : plansza(10, vector<shared_ptr<Pole>>(10))
    , strzaly(10, vector<char>(10,' '))
    {
        for(int i=0; i<10; i++) {
            for(int j=0; j<10; j++) {
                plansza[i][j] = make_shared<Pole_wody>();
            }
        }

//        for(int i=0; i<4; i++)
//            statki.push_back(make_shared<Statek>(1, "Jednomasztowiec"));
//        for(int i=0; i<3; i++)
//            statki.push_back(make_shared<Statek>(2, "Dwumasztowiec"));
        for(int i=0; i<2; i++)
            statki.push_back(make_shared<Statek>(3, "Trzymasztowiec"));
        statki.push_back(make_shared<Statek>(4, "Czteromasztowiec"));
}

void Plansza::czysc() {
    Plansza();
}

void Plansza::dodaj_strzal(Pozycja poz, Wynik_strzalu wynik) {
    strzaly[poz.first][poz.second] = symbol_strzalu(wynik);
}

shared_ptr<Pole> Plansza::get_pole(Pozycja poz) {
    return plansza[poz.first][poz.second];
}

char Plansza::get_pole_strzalu(Pozycja poz) {
    return strzaly[poz.first][poz.second];
}

bool Plansza::pole_istnieje(Pozycja poz) const {
    if(poz.first < 0 || poz.first > 9)
        return false;
    if(poz.second < 0 || poz.second > 9)
        return false;
    return true;
}

bool Plansza::tu_juz_bylo_strzelane(Pozycja poz) const {
    if(strzaly[poz.first][poz.second] == symbol_strzalu(NIETRAFIONY) || strzaly[poz.first][poz.second] == symbol_wody(NIETRAFIONY))
        return false;
    return true;
}

vector<shared_ptr<Statek>> Plansza::moje_statki() const {
    return statki;
}

void Plansza::ustaw_statek(shared_ptr<Statek> statek, Kierunek kierunek, Pozycja poz) {
    if(kierunek == POZIOMO) {
        for(int i=0; i<statek->rozm(); i++) {
            plansza[poz.first][poz.second+i] = make_shared<Pole_statku>(statek);
        }
    } else {
        for(int i=0; i<statek->rozm(); i++) {
            plansza[poz.first+i][poz.second] = make_shared<Pole_statku>(statek);
        }
    }
}

////////////////GRACZ//////////////////////////////////////////////////////////////////////////////

Gracz::Gracz(int numer, UI &ui_)
    : plansza(Plansza())
    , num(numer), ui(ui_)
    {}

void Gracz::ustaw_flote() {
    plansza.czysc();
    ui.pokaz(*this);
    for(shared_ptr<Statek> statek : plansza.moje_statki()) {
        ui.poczatek_ustawiania(statek);
        plansza.ustaw_statek(statek, ui.zapytaj_o_kierunek(), ui.zapytaj_o_poczatek_statku());
        ui.pokaz(*this);
    }
    ui.koniec_ustawiania();
    ui.wcisnij_enter();
}

int Gracz::numer() const {
    return num;
}

int Gracz::zycie() const {
    int w = 0;
    for(shared_ptr<Statek> statek : plansza.moje_statki())
        w += statek->zycie_statku();
    return w;
}

Plansza & Gracz::get_plansza() {
    return plansza;
}

bool Gracz::strzal(Gracz g) {
    bool kolejny_ruch = false;;
    ui.pokaz(*this);
    Pozycja poz = ui.zapytaj_o_strzal();
    if (!plansza.pole_istnieje(poz)) {
        ui.takie_pole_nie_istnieje();
        ui.wcisnij_enter();
        return true;
    }
    if (plansza.tu_juz_bylo_strzelane(poz)) {
        ui.tu_juz_strzelales();
        ui.wcisnij_enter();
        return true;
    }
    Wynik_strzalu wynik = g.get_plansza().get_pole(poz)->wynik_strzalu();
    plansza.dodaj_strzal(poz, wynik);
    ui.pokaz(*this);
    ui.wynik_strzalu(wynik);
    ui.wcisnij_enter();
    if (wynik == TRAFIONY) {
        return true;
    }
    else if (wynik == ZATOPIONY) {
        return true;
    }
    else return false;
}

void Gracz::wygral() {
    ui.podaj_zwyciezce(*this);
}

////////////////////UI////////////////////////////////////////////////////////////////////////

Pozycja UI::zapytaj_o_strzal() const {
    char wiersz;
    int kolumna;
    cout << "Strzelam w pole: ";
    cin >> wiersz >> kolumna;
    return {int(wiersz-'A'),kolumna-1};
}

Pozycja UI::zapytaj_o_poczatek_statku() const {
    char wiersz;
    int kolumna;
    cout << "Podaj pole poczatku statku: ";
    cin >> wiersz >> kolumna;
    return {int(wiersz-'A'),kolumna-1};
}

Kierunek UI::zapytaj_o_kierunek() const {
    char kierunek;
    cout << "Podaj kierunek statku (poziomo '-', pionowo '|'): ";
    cin >> kierunek;
    if(kierunek == '-')
        return POZIOMO;
    return PIONOWO;
}

void UI::poczatek_ustawiania(shared_ptr<Statek> statek) const {
    cout << "Ustaw " << statek->nazw() << endl;
}

void UI::koniec_ustawiania() const {
    cout << "BRAWO! USTAWILES SWOJA FLOTE." << endl;
}

void UI::wynik_strzalu(Wynik_strzalu wynik) const {
    if (wynik == TRAFIONY) {
        cout << "TRAFIONY!" << endl;
    }
    else if (wynik == ZATOPIONY) {
        cout << "ZATOPIONY!" << endl;
    }
    else cout << "PUDLO!" << endl;
}

void UI::wcisnij_enter() const {
    cout << endl << "Nacisnij Enter by kontynuowac" << endl;
    cin.get();
    cin.get();
}

void UI::podaj_zwyciezce(Gracz g) const {
    cout << endl << "WYGRAL GRACZ " << g.numer() << "!!!" << endl;
}

void UI::pokaz(Gracz g) const {
    cls();
    string odstep = "          ";
    cout << "                                               GRACZ " << g.numer() << "                                        " << endl;
    cout << "                 Twoje statki               " << odstep << "                 Twoje strzaly              " << endl << endl;
    cout << "     1   2   3   4   5   6   7   8   9   10 " << odstep << "     1   2   3   4   5   6   7   8   9   10 " << endl;
    cout << "   +---+---+---+---+---+---+---+---+---+---+" << odstep << "   +---+---+---+---+---+---+---+---+---+---+" << endl;
    for(int i=0; i<10; i++) {
        cout << " " << char(i + 'A') << " | ";
        for(int j=0; j<10; j++) {
            cout << g.get_plansza().get_pole({i,j})->symbol() << " | ";
        }
        cout << odstep;
        cout << char(i + 'A') << " | ";
        for(int j=0; j<10; j++) {
            cout << g.get_plansza().get_pole_strzalu({i,j}) << " | ";
        }
        cout << endl << "   +---+---+---+---+---+---+---+---+---+---+" << odstep << "   +---+---+---+---+---+---+---+---+---+---+" << endl;
    }
    cout << endl;
}

void UI::takie_pole_nie_istnieje() const {
    cout << "Takie pole nie istnieje, sprobuj ponownie!" << endl;
}

void UI::tu_juz_strzelales() const {
    cout << "Tu juz bylo strzelane, sprobuj gdzie indziej!" << endl;
}

/////////////GRA///////////////////////////////////////////////////////////////////////

Gra::Gra(UI &ui)
    : gracze({Gracz(1, ui)
    , Gracz(2, ui)})
    {}

bool Gra::czy_koniec() const {
    if(gracze[0].zycie() == 0 || gracze[1].zycie() == 0)
        return true;
    return false;
}

void Gra::nowa_gra() {
    gracze[0].ustaw_flote();
    gracze[1].ustaw_flote();
    int obecny_gracz = 0;
    int przeciwnik = 1;
    while(!czy_koniec()) {
        if(!gracze[obecny_gracz].strzal(gracze[przeciwnik])) {
            obecny_gracz = (obecny_gracz + 1) % 2;
            przeciwnik = (przeciwnik + 1) % 2;
        }
    }
    zwyciezca().wygral();
}

Gracz Gra::zwyciezca() const {
    if(gracze[0].zycie() == 0)
        return gracze[1];
    return gracze[0];
}

///////MAIN//////////////////////////////////////////////////////////////////////////////

int main() {
    UI ui;
    Gra g(ui);
    g.nowa_gra();
}
