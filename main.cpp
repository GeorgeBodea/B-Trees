# include <iostream>
#include <algorithm>
#include <fstream>
using namespace std;

#define MIN 2

ifstream f("abce.in");
/// Putem avea doar numere naturale dar si numerele fractionare (doar cu +0.5).
/// Restrictii: MIN >=2. Motiv: Daca avem MIN = 1, respectiv 1.5, atunci maximul de chei pe nod e 2 si daca facem split nodului radacina si mai adaugam un nod
/// trebuie sa avem minim 3 chei la dispozitie pentru a le amplasa in cele noduri, altfel am putea avea un nod vid, ceea ce nu este acceptabbil intrucat fiecare nod trebuie sa aiba minim MIN/2 chei

struct nod {
    int *chei;
    nod **adrese;
    int nr_chei = 0;
    bool este_frunza = 0;

    nod() {
        chei = new int[int(2 * MIN) - 1]; /// Numar maxim de chei
        adrese = new nod *[int(2 * MIN)]; /// Numar maxim de adrese
        for (int i=0; i<2*MIN; i++)
            adrese[i] = nullptr;
    }

    ~nod(){
        for(int i=0; i<nr_chei; i++)
            delete adrese[i];
        delete adrese;
    }

    // Observatie foarte importanta: cum aceasta este metoda proactiva vom verifica de fiecare daca cand introducem o cheie de la tata la nodul fiu
    // daca nodul fiu este plin. Daca este plin avem dreptul de a face split ( intrucat avem t-1 chei de adaugat in nodul nou din split) si nu vom face vreodata overflow de noduri,
    // deoarece vom avea nodul subarborelui plin exact dupa ce vom avea maximul de chei pe nivelul umrator.

    void insereaza_in_nod(int valoare) {
        int i = 0;
            if (este_frunza == 0){
                // Cautam unde trebuie sa punem fiul
                while (valoare>chei[i] && i<nr_chei)
                    i++;
                // Aceasta verificare ne asigura ca nu facem overflow de chei, atat pentru noduri interne, cat si pentru frunze ( verificarea e inainte de intrare in frunza )
                if (adrese[i]->nr_chei == 2*MIN-1){
                    this->split_nod(adrese[i]);
                    /// Tre sa mai inseram valoarea
                    this->insereaza_in_nod(valoare);
                }
                else adrese[i]->insereaza_in_nod(valoare);
            }
            else{
                /// Mutam cheile mai mari ca valoarea in dreapta
                /// Tinem cont de faptul ca nu avem nodul plin de chei.
                int i=0;
                while(valoare>chei[i] && i<nr_chei)
                    i++;
                int j = 2*MIN-2;
                while(j>i){
                    chei[j]=chei[j-1];
                    j--;
                }
                chei[i]=valoare;
                /// Nu ne itereseaza adresele nodului curent intrucat daca suntem pe acest caz valorile nu conteaza
                nr_chei+=1;
            }
    }

    void split_nod(nod* adresa_nod){
        /// Cand intram pe functia aceasta suntem in nodul la adresa this si facem split nodului de la adresa_nod (fiul al lui this)
        nod* nod_geaman = new nod();
        if (adresa_nod->este_frunza==1){
            nod_geaman->este_frunza = 1;
        }
        else nod_geaman->este_frunza = 0;

        /// Primul lucru care trebuie sa il facem e sa ne asiguram ca adresa nodului geaman este in nodul curent
        /// Nodul geaman se va creea la dreapta nodului adresa_nod (deoarece punem jumatate din elementele mai mari ale nodului adresa_nod)
        /// Deci facem loc pentru adresa nodului geaman in nodul tata
        nod* adresa_aux = adrese[0];
        int contor=0;
        while (adresa_aux!=adresa_nod){
            contor+=1;
            adresa_aux = adrese[contor];
        }
        /// Putem garanta ca vectorul de adrese nu e plin, intrucat e metoda proactiva (s-ar fi facut splitul nodului in care suntem inainte de acest pas si deci avem loc)
        for (int k= MIN*2-1; k>contor; k--){
            adrese[k] = adrese[k-1];
        }
        adrese[contor+1] = nod_geaman; /// Am facut conexiunea nodului tata cu nodul nou.
        /// Acum mutam cheile dupa cum urmeaza: trecem t-1 chei in nodul geaman din nodul adresa_nod si o cheie o punem la tata

        /// Cum arborele nostru e left biased lasam ce e mai mult in partea stanga cand avem nr par de chei
        contor = 0;
            while (contor<int(MIN-1)) {
                nod_geaman->chei[contor] = adresa_nod->chei[contor + int(MIN)];
                contor++;
            }
        /// Trebuie sa adaugam un pointer la adresele fiu in plus la nodul nostru nou.
        contor = 0;
            while (contor< int(MIN)) {
           nod_geaman->adrese[contor] = adresa_nod->adrese[contor+ int(MIN)];
           contor++;
       }
        /// Stim ca trebuie sa mai mutam o cheie in nodul curent (mijlocul). Dupa pozitionare adresele din stanga si dreapta acestei chei vor fi pozitionate corect.
        contor = 0;
        int element_mijloc = adresa_nod->chei[int(MIN-1)];
        if (nr_chei==0){
            chei[0]=element_mijloc; // Pentru primul split
        }
        else{
            while (element_mijloc>chei[contor] && contor<nr_chei) {
                contor++;
            }
            for (int k=nr_chei;k>contor; k--){
                chei[k]=chei[k-1];
            }
            chei[contor]= element_mijloc;
        }
        nr_chei+=1;
        nod_geaman->nr_chei = MIN-1;
        adresa_nod->nr_chei = adresa_nod->nr_chei-MIN;

        /// Punem cheia din mijloc in tata ( in nodul in care ne aflam ) dar trebuie sa o pozitionam corect deci trebuie sa mutam si cheiele din nodul curent in partea dreapta
    }

    int cautare_succesor_pe_nod(int valoare){
        int minim_bun = 2<<29, i=0;
        while(valoare > chei[i] && i<nr_chei)
            i++;
        if (i<nr_chei)
            minim_bun = chei[i];
        if (este_frunza==1)
            return minim_bun;
        else return min(minim_bun, adrese[i]->cautare_succesor_pe_nod(valoare));
    }

    int cautare_predecesor_pe_nod(int valoare){
        int maxim_bun = -(2<<29), i=0;
        while(valoare >= chei[i] && i<nr_chei)
            i++;
        if (i>0)
            maxim_bun = chei[i-1];
        if (este_frunza==1)
            return maxim_bun;
        else return max(maxim_bun, adrese[i]->cautare_predecesor_pe_nod(valoare));
    }
};

struct b_arbore {
    nod *radacina = nullptr;

    void inserare(int valoare) {
        if (radacina == nullptr) {

            nod *primul_nod = new nod(); /// Primul nod este radacina si frunza la prima inserare
            primul_nod->este_frunza = 1;
            primul_nod->chei[0] = valoare;
            primul_nod->nr_chei = 1;
            radacina = primul_nod;
        } else {
            if (radacina->nr_chei != 2 * MIN - 1) /// In cazul cand nodul radacina NU e plin
                radacina->insereaza_in_nod(valoare);

                                    /* Singurul lucru de care trebuie sa tinem cont in acest caz este
                                    pozitia valorii de inserat
                                   ( fie ca e local (in acelasi nod) sau extinst (pe alta cheie) )*/

            else {
                /* Intodeauna inseram in frunze, iar daca frunza e plina se adauga la tatal frunzei,
                 iar daca si tatal e plin continuam pana ajungem in radacina.
                  Deci, daca radacina e plina e momentul sa adaugam doua noduri pentru ca nu mai avem locuri potrivite pentru valoarea noastra. */
                nod *nod_nou = new nod();
                nod_nou->este_frunza = 0;
/// Noul nod va deveni radacina si va lua
/// elementul din mijloc al radacinii vechi
/// si mai adaugam un nod in care ii punem o cheie.
/// Radacina noua va contine adresele spre radacina
/// veche si un nod nou
                nod_nou->adrese[0] = radacina;
                nod_nou->split_nod(radacina);
                radacina = nod_nou;
/// Acum trebuie sa vedem unde putem amplasa cheia noua (in primul fiu sau al doilea fiu al radacinii) in functie de fostul nod din mijloc
                int i = 0;
                if (nod_nou->chei[0] < valoare) /// Daca prima cheie din radacina noua e mai mica ca si valoarea mea pun valoarea in fiul drept
                    i += 1;
                nod_nou->adrese[i]->insereaza_in_nod(valoare); /// Acelasi principiu ca mai sus si la adrese

            }

        }

    }

    int cautare_valoare_in_arbore(int valoare){
        if (radacina== nullptr){
            return 0;
        }
        return cautare_valoare(radacina, valoare);
    }

    int cautare_valoare(nod* adresa_nod, int valoare){
        int i=0;
        while (valoare>adresa_nod->chei[i] && i<adresa_nod->nr_chei) /// Cautam poztita unde ar putea fi nodul
            i++;
        if (adresa_nod->chei[i] == valoare)
            return 1;
        else if( adresa_nod->este_frunza == 0)
            return cautare_valoare(adresa_nod->adrese[i], valoare); /// Avansam in noduri, daca nu gasim elementul, pana ajungem la frunzq
        else return 0; /// Daca nodul nu e nici in frunza inseamna ca nu exista
    }

    int succesor(int valoare){
        if (radacina == nullptr)
            return -1;
        else {
            int val_ret = radacina->cautare_succesor_pe_nod(valoare);
            if (val_ret == 2 << 29)
                return -1;
            else return val_ret;
        }
    }

    int predecesor(int valoare){
        if (radacina == nullptr)
            return -1;
        else {
            int val_ret = radacina->cautare_predecesor_pe_nod(valoare);
            if (val_ret == -(2 << 29))
                return -1;
            else return val_ret;
        }
    }

    void afisare_interval(nod* nod_curent, int x, int y){
        if (radacina == nullptr)
            cout<<"Nu exista radacina ";
        else {
            int i = 0, j = nod_curent->nr_chei;
            while (nod_curent->chei[i] < x && nod_curent->nr_chei > i)
                i++;
            while (nod_curent->chei[j-1] >y && 0 < j)
                j--;
            for (int trop_trop = i; trop_trop < j; trop_trop++)
                cout << nod_curent->chei[trop_trop] << ' ';
            if (nod_curent->este_frunza == 0){
            for (int fiecare_adresa = i; fiecare_adresa <= j; fiecare_adresa++)
                afisare_interval(nod_curent->adrese[fiecare_adresa], x, y);
            }
        }
    }

    void afisare_interval_in_arbore(int x,int y){
        return afisare_interval(radacina,x,y);
    }

};

/// Functie de verificat cheile din nod.
void valoare(nod* nod_curent){
    int i=0;
    while (i<nod_curent->nr_chei) {
        cout << nod_curent->chei[i] << ' ';
        i++;
    }
    cout<<endl;
}



int main() {

   /*  Test vechi:
    * b_arbore ob1;
    ob1.inserare(4);
    ob1.inserare(5);
    ob1.inserare(8);
    cout<<"Singurul nod e plin"<<ob1.radacina->chei[0]<<' '<<ob1.radacina->chei[1]<<' '<< ob1.radacina->chei[2];
    cout<<"Adresa radacinii este: "<<ob1.radacina<<endl;
    cout<<endl;
    // cout<<"S-a terminat de citit"<<ob1.radacina->adrese[0]->chei[0]<<' '<<ob1.radacina->adrese[0]->chei[1]<<endl;
    // cout<<"S-a terminat de citit"<<ob1.radacina->adrese[1]->chei[0]<<' '<<ob1.radacina->adrese[1]->chei[1]<<endl;
    ob1.inserare(65);
    ob1.inserare(2);
    ob1.inserare(1);
    ob1.inserare(7);
    cout<<"Radacina "<<ob1.radacina->chei[0]<<' '<<ob1.radacina->chei[1]<<' '<< ob1.radacina->chei[2]<<" cu "<<ob1.radacina->nr_chei<< " chei \n";
    cout<<"Nod STANGA "<<ob1.radacina->adrese[0]->chei[0]<<' '<<ob1.radacina->adrese[0]->chei[1]<<' '<< ob1.radacina->adrese[0]->chei[2]<<" cu "<<ob1.radacina->adrese[0]->nr_chei<< " chei \n";
    cout<<"Nod DREAPTA "<<ob1.radacina->adrese[1]->chei[0]<<' '<<ob1.radacina->adrese[1]->chei[1]<<' '<< ob1.radacina->adrese[1]->chei[2]<<" cu "<<ob1.radacina->adrese[1]->nr_chei<< " chei \n";
    cout<<"Adresa radacinii noi si a radacinii stanga dreapta este: "<<ob1.radacina<<' '<<ob1.radacina->adrese[0]<<' '<<ob1.radacina->adrese[1]<<endl;
    cout<<endl;
    ob1.inserare(23);
    cout<<"Radacina "<<ob1.radacina->chei[0]<<' '<<ob1.radacina->chei[1]<<' '<< ob1.radacina->chei[2]<<" cu "<<ob1.radacina->nr_chei<< " chei \n";
    cout<<"Nod STANGA "<<ob1.radacina->adrese[0]->chei[0]<<' '<<ob1.radacina->adrese[0]->chei[1]<<' '<< ob1.radacina->adrese[0]->chei[2]<<" cu "<<ob1.radacina->adrese[0]->nr_chei<< " chei \n";
    /// cout<<"Nod DREAPTA "<<ob1.radacina->adrese[1]->chei[0]<<' '<<ob1.radacina->adrese[1]->chei[1]<<' '<< ob1.radacina->adrese[1]->chei[2]<<" cu "<<ob1.radacina->adrese[1]->nr_chei<< " chei \n";
    /// cout<<"Nod FAR DREAPTA "<<ob1.radacina->adrese[2]->chei[0]<<' '<<ob1.radacina->adrese[2]->chei[1]<<' '<< ob1.radacina->adrese[2]->chei[2]<<" cu "<<ob1.radacina->adrese[2]->nr_chei<< " chei \n";
    cout<<"Adresa radacinii noi si a radacinii stanga dreapta FAR drapta este: "<<ob1.radacina<<' '<<ob1.radacina->adrese[0]<<' '<<ob1.radacina->adrese[1]<<" "<<ob1.radacina->adrese[2]<<endl;
    cout<<endl;
    ob1.inserare(63);
    ob1.inserare(66);
    cout<<"Radacina "<<ob1.radacina->chei[0]<<' '<<ob1.radacina->chei[1]<<' '<< ob1.radacina->chei[2]<<" cu "<<ob1.radacina->nr_chei<< " chei \n";
    cout<<"Nod STANGA "<<ob1.radacina->adrese[0]->chei[0]<<' '<<ob1.radacina->adrese[0]->chei[1]<<' '<< ob1.radacina->adrese[0]->chei[2]<<" cu "<<ob1.radacina->adrese[0]->nr_chei<< " chei \n";
    cout<<"Nod DREAPTA "<<ob1.radacina->adrese[1]->chei[0]<<' '<<ob1.radacina->adrese[1]->chei[1]<<' '<< ob1.radacina->adrese[1]->chei[2]<<" cu "<<ob1.radacina->adrese[1]->nr_chei<< " chei \n";
    /// cout<<"Nod FAR DREAPTA "<<ob1.radacina->adrese[2]->chei[0]<<' '<<ob1.radacina->adrese[2]->chei[1]<<' '<< ob1.radacina->adrese[2]->chei[2]<<" cu "<<ob1.radacina->adrese[2]->nr_chei<< " chei \n";
    /// cout<<"Nod FAR FAR DREAPTA "<<ob1.radacina->adrese[3]->chei[0]<<' '<<ob1.radacina->adrese[3]->chei[1]<<' '<< ob1.radacina->adrese[3]->chei[2]<<" cu "<<ob1.radacina->adrese[3]->nr_chei<< " chei \n";
    cout<<endl;
    ob1.inserare(0);

    valoare(ob1.radacina);
    valoare(ob1.radacina->adrese[0]);
    /// valoare(ob1.radacina->adrese[0]->adrese[0]);
    /// valoare(ob1.radacina->adrese[0]->adrese[1]);
    /// valoare(ob1.radacina->adrese[0]->adrese[2]);
    /// valoare(ob1.radacina->adrese[1]);
    /// valoare(ob1.radacina->adrese[1]->adrese[0]);
    /// valoare(ob1.radacina->adrese[1]->adrese[1]);

    cout<<ob1.predecesor(-3);
    cout<<ob1.predecesor(64);
    /// ob1.insereaza_in_arbore(69);
    /// cout<<"Radacina "<<ob1.radacina->chei[0]<<' '<<ob1.radacina->chei[1]<<' '<< ob1.radacina->chei[2]<<" cu "<<ob1.radacina->nr_chei<< " chei \n";
    /// cout<<"Nod STANGA "<<ob1.radacina->adrese[0]->chei[0]<<' '<<ob1.radacina->adrese[0]->chei[1]<<' '<< ob1.radacina->adrese[0]->chei[2]<<" cu "<<ob1.radacina->adrese[0]->nr_chei<< " chei \n";
    /// cout<<"Nod DREAPTA "<<ob1.radacina->adrese[1]->chei[0]<<' '<<ob1.radacina->adrese[1]->chei[1]<<' '<< ob1.radacina->adrese[1]->chei[2]<<" cu "<<ob1.radacina->adrese[1]->nr_chei<< " chei \n";
    // cout<<"Nod FAR DREAPTA "<<ob1.radacina->adrese[2]->chei[0]<<' '<<ob1.radacina->adrese[2]->chei[1]<<' '<< ob1.radacina->adrese[2]->chei[2]<<" cu "<<ob1.radacina->adrese[2]->nr_chei<< " chei \n";
    // cout<<"Adresa radacinii noi si a radacinii stanga dreapta FAR drapta este: "<<ob1.radacina<<' '<<ob1.radacina->adrese[0]<<' '<<ob1.radacina->adrese[1]<<" "<<ob1.radacina->adrese[2]<<endl;
    */
    int q, optiune, aux1, aux2;
    b_arbore arbore;
    f>>q;
    cout<<"1) Insereaza"<<' '<<"2) Sterge (Inexistenta)"<<' '<<"3) Cauta Element"<<"\n";
    cout<<"4) Cauta Predecesor"<<' '<<"5) Cauta Succesor"<<' '<<"6) Cauta din interval"<<"\n";
    while(q>0){
       f>>optiune;
       switch (optiune){
           case 1:
               f>>aux1;
               arbore.inserare(aux1);
               break;
           case 2:
               f>>aux1;
               cout<<"Nu exista functie de stergere :( "<<'\n';
               break;
           case 3:
               f>>aux1;
               cout<<arbore.cautare_valoare_in_arbore(aux1)<<'\n';
               break;
           case 4:
               f>>aux1;
               cout<<arbore.succesor(aux1)<<' '<<'\n';
               break;
           case 5:
               f>>aux1;
               cout<<arbore.predecesor(aux1)<<' '<<'\n';
               break;
           case 6:
               f>>aux1>>aux2;
               arbore.afisare_interval(arbore.radacina,aux1, aux2);
               cout<<'\n';
               break;
       }
        q--;
    }

    f.close();
    return 0;
}