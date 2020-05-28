#include <fstream>
#include <string>
#include <queue>
#include <set>
#include <map>
#include <iostream>

using namespace std;

void citire(istream& f, int& n, char st[], char& start, set<string> p[], bool el[])
{
    int m, i;
    string prd;

    f >> n;
    for(i = 0; i < n; i++)
    {
        f >> st[i] >> m;
        while(m)
        {
            f >> prd;
            p[int(st[i])].insert(prd);
            m--;
        }
        el[i] = false;
    }
    start = st[0];
}

void eliminare_lambda_productii(const int& n, const char st[], char& start, set<string> p[], bool el[])
{
    int i, j;
    bool prod_vide;
    string aux;
    set<string> copie;
    set<string>::iterator it;

    prod_vide = true;
    while(prod_vide)
    {
        prod_vide = false;
        for(i = 0; i < n; i++)
            if(p[int(st[i])].find("$") != p[int(st[i])].end())  /// daca starea i are productia $
            {
                prod_vide = true;
                if(p[int(st[i])].size() == 1)   /// daca nu mai are alte productii
                {
                    el[i] = true;   /// starea i este eliminata
                    for(j = 0; j < n; j++)  ///     pentru toate productiile care nu au fost eliminate, verific daca starea i
                        if(!el[j])          /// apare intr-una dintre productiile starii j
                        {
                            copie = p[int(st[j])];
                            p[int(st[j])].clear();
                            for(it = copie.begin(); it != copie.end(); it++)
                            {
                                aux = *it;
                                while(aux.find(st[i]) != -1)    /// cat timp starea i apare intr-o productie a starii j
                                    if(aux.size() == 1)                    /// daca productia contine doar starea i, ea devine $
                                        aux = "$";
                                    else    /// altfel eliminam starea i din productie
                                        aux.erase(aux.find(st[i]), 1);
                                p[int(st[j])].insert(aux);  /// noua productie este adaugata in multimea productiilor
                            }
                        }
                }
                else    /// daca starea i mai are si alte productii
                {
                    p[int(st[i])].erase("$");   /// elimin productia $
                    for(j = 0; j < n; j++)
                        if(!el[j])
                        {
                            copie = p[int(st[j])];
                            for(it = copie.begin(); it != copie.end(); it++)
                            {
                                aux = *it;  /// cat timp starea i apare intr-o productie de minim 2 stari a lui j, adaug noua productie la j
                                while(aux.size() >= 2 && aux.find(st[i]) != -1)
                                {
                                    aux.erase(aux.find(st[i]), 1);
                                    p[int(st[j])].insert(aux);
                                }
                            }
                        }
                }
            }
    }
}

int stare(const string& A, const int& n, const char v[], const bool el[])
{
    int i;

    for(i = 0; i < n; i++)
        if(!el[i] && A == string(1, v[i]))
            return i;
    return -1;
}

void eliminare_redenumiri(const int& n, const char st[], const char& start, set<string> p[], const bool el[])
{
    int i, s;
    bool redenumiri;
    set<string> copie;
    set<string>::iterator it1, it2;

    for(i = 0; i < n; i++)
        if(!el[i] && p[int(st[i])].find(string(1, st[i])) != p[int(st[i])].end())
            p[int(st[i])].erase(string(1, st[i]));  /// daca am productii de tipul X -> X, le elimin
    redenumiri = true;  /// redenumire = X -> Y, Y -> a => X -> a
    while(redenumiri)   /// cat timp avem redenumiri, le inlocuim
    {
        redenumiri = false;
        for(i = 0; i < n; i++)
            if(!el[i])  /// st[i] este pe post de X
            {
                copie = p[int(st[i])];
                p[int(st[i])].clear();
                for(it1 = copie.begin(); it1 != copie.end(); it1++)
                {
                    if((*it1).size() == 1)
                        s = stare(*it1, n, st, el);
                    else
                        s = -1;
                    if(s != -1 && !el[s]) /// inseamna ca *it1 este de forma Y
                    {
                        for(it2 = p[int(st[s])].begin(); it2 != p[int(st[s])].end(); it2++)
                            p[int(st[i])].insert(*it2);
                        redenumiri = true;
                    }
                    else
                        p[int(st[i])].insert(*it1);
                }
            }
    }
}

bool trebuie_eliminat(const int& x, bool t[], const set<string> p[], const char st[], const int& n)
{
    int i, j, k;
    set<string>::iterator it;
    bool viz[260];
    queue<int> q;

    while(!q.empty())
        q.pop();
    q.push(x);
    while(!q.empty())
    {
        i = q.front();
        q.pop();
        viz[i] = true;
        for(it = p[int(st[i])].begin(); it != p[int(st[i])].end(); it++)
            for(j = 0; j < (*it).size(); j++)
                for(k = 0; k < n; k++)
                    if(st[k] == (*it)[i])
                    {
                        if(t[k])
                        {
                            t[i] = true;
                            return false;
                        }
                        else
                        {
                            if(!viz[k])
                                q.push(k);
                            break;
                        }
                    }
    }

    return true;
}

void eliminare_productii_inutile(int& n, char st[], const char& start, set<string> p[], bool el[])
{
    int x, i, j, s;
    bool viz[260], terminal[260];
    queue<int> q;
    set<string> copie;
    set<string>::iterator it;

    q.push(0);  /// eliminare stari inaccesibile din starea de start
    while(!q.empty())
    {
        x = q.front();
        q.pop();
        viz[x] = true;
        for(it = p[int(st[x])].begin(); it != p[int(st[x])].end(); it++)
            for(i = 0; i < (*it).size(); i++)
            {
                for(s = 0; s < n; s++)
                    if(st[s] == (*it)[i])
                        break;
                if(!viz[s])
                    q.push(s);
            }
    }
    for(i = 0; i < n; i++)
        if(!viz[i])
            el[i] = true;
    for(i = 0; i < n; i++)  /// eliminare stari care nu se termina
        if(!el[i])
        {
            terminal[i] = false;
            for(it = p[int(st[i])].begin(); it != p[int(st[i])].end(); it++)
                if((*it).size() == 1 && 'a' <= (*it)[0] && (*it)[0] <= 'z')
                {
                    terminal[i] = true; /// daca starea i are o productie care merge in terminal, o marcam cu true
                    break;
                }
                else
                    if((*it).size() > 1)
                    {
                        for(j = 0; j < (*it).size(); j++)
                            if(!('a' <= (*it)[j] && (*it)[j] <= 'z'))
                                break;
                        if(j >= (*it).size())
                            terminal[i] = true;
                    }
        }
    for(i = 0; i < n; i++)
        if(!el[i] && !terminal[i])    /// daca starea i nu are productii care merg in terminale, verificam daca neterminalele
            el[i] = trebuie_eliminat(i, terminal, p, st, n);    /// ei au aceasta proprietate
    for(s = 0; s < n; s++)  /// stergem aparitiile neterminalelor eliminate din productiile ramase
        if(!el[s])
        {
            copie = p[int(st[s])];
            p[int(st[s])].clear();
            for(it = copie.begin(); it != copie.end(); it++)
            {
                for(i = 0; i < (*it).size(); i++)
                {
                    for(j = 0; j < n; j++)
                        if(el[j] && st[j] == (*it)[i])
                            break;
                    if(j < n)
                        break;
                }
                if(i >= (*it).size())
                    p[int(st[s])].insert(*it);
            }
        }
    for(i = 0; i < n; i++)  /// eliminam din memorie starile marcate ca eliminate
        if(el[i])
        {
            for(j = i + 1; j < n; j++)
            {
                st[j] = st[j + 1];
                p[j] = p[j + 1];
            }
            i--;
            n--;
        }
}

void adaugare_neterminale_noi(int& n, char st[], const char& start, set<string> p[])
{
    int i, j;
    char lit;
    bool ok;
    set<char> disponibile;
    string aux;
    set<string> copie;
    set<string>::iterator it;
    map<char, char> neterminal; /// neterminal[i] = neterminalul corespunzator terminalului i

    for(lit = 'A'; lit <= 'Z'; lit++)
        disponibile.insert(lit);    /// multimea numelor de neterminale disponibile contine toate literele mari
    for(i = 0; i < n; i++)
        disponibile.erase(st[i]);   /// eliminam literele existente
    for(i = 0; i < n; i++)
    {
        copie.clear();
        for(it = p[int(st[i])].begin(); it != p[int(st[i])].end(); it++)
        {
            ok = false;
            for(j = 0; j < (*it).size(); j++)
                if('A' <= (*it)[j] && (*it)[j] <= 'Z')
                {
                    ok = true;  /// productia curenta contine terminale
                    break;
                }
            if(ok)  /// verific daca productia contine si neterminale
            {
                aux = "";
                for(j = 0; j < (*it).size(); j++)
                {
                    if('a' <= (*it)[j] && (*it)[j] <= 'z')  /// daca am si terminale, le inlocuiesc
                    {
                        if(neterminal.find((*it)[j]) != neterminal.end())   /// daca terminalul gasit are asociat un neterminal, il inlocuiesc
                            aux += neterminal[(*it)[j]];
                        else
                        {
                            st[n++] = *disponibile.begin();
                            disponibile.erase(st[n - 1]);
                            neterminal[(*it)[j]] = st[n - 1];
                            aux += st[n - 1];
                            p[int(st[n - 1])].insert(string(1, (*it)[j]));
                        }
                    }
                    else
                        aux += (*it)[j];
                }
                copie.insert(aux);
            }
            else
                copie.insert(*it);
        }
        p[int(st[i])] = copie;
    }
}

void adaugare_neterminale_noi_2(int& n, char st[], const char& start, set<string> p[])
{
    int i, j;
    bool productii_lungi;
    char l;
    string aux, aux2;
    set<char> disponibile;
    set<string> copie, copie2;
    set<string>::iterator it, itr;

    for(l = 'A'; l <= 'Z'; l++)
        disponibile.insert(l);
    for(i = 0; i < n; i++)
        disponibile.erase(st[i]);   /// numele disponibile pentru neterminale
    productii_lungi = true;
    while(productii_lungi)  /// cat timp avem productii formate din mai mult de 2 neterminale, adaugam productii noi
    {
        productii_lungi = false;
        for(i = 0; i < n; i++)
        {
            copie = {};
            for(it = p[int(st[i])].begin(); it != p[int(st[i])].end(); it++)
                if(it->size() > 2 && 'A' <= (*it)[0] && (*it)[0] <= 'Z')    /// daca am o productie formata din cel putin 3 neterminale
                {
                    aux = *it;
                    st[n++] = *disponibile.begin();     /// adaug o stare noua
                    disponibile.erase(st[n - 1]);
                    aux.erase(0, 1);
                    p[int(st[n - 1])].insert(aux);  /// starea noua are productia egala cu ceea ce se afla in aux, in afara de primul caracter
                    aux = string(1, (*it)[0]) + string(1, st[n - 1]);
                    copie.insert(aux);  /// aux va deveni aux[0] + st[n - 1]
                    aux = *p[int(st[n - 1])].begin();   /// aici, aux este productia noului neterminal
                    for(j = 0; j < n; j++)
                        if(j != i)  /// cautam aparitiile productiei in alte elemente si inlocuim cand apare
                        {
                            copie2 = {};
                            for(itr = p[int(st[j])].begin(); itr != p[int(st[j])].end(); itr++)
                                if(itr->size() > 2 && itr->find(aux) != string::npos)
                                {
                                    aux2 = *itr;
                                    aux2.replace(itr->find(aux), aux.size(), string(1, st[n - 1]));
                                    copie2.insert(aux2);
                                }
                                else
                                    copie2.insert(*itr);
                            p[int(st[j])] = copie2;
                        }
                    it++;
                    while(it != p[int(st[i])].end())    /// cautam aparitiile productiei si in alte productii ale starii curente
                    {
                        if(it->size() > 2 && it->find(aux) != string::npos)
                        {
                            aux2 = *it;
                            aux2.replace(it->find(aux), aux.size(), string(1, st[n - 1]));
                            copie.insert(aux2);
                        }
                        else
                            copie.insert(*it);
                        it++;
                    }
                }
                else
                    copie.insert(*it);
            p[int(st[i])] = copie;
        }
    }
}

void afisare(ostream& g, const int& n, const char st[], const char& start, const set<string> p[])
{
    int i;
    set<string>::iterator it;

    g << n << ' ' << start << '\n';
    for(i = 0; i < n; i++)
    {
        g << st[i] << ' ' << p[int(st[i])].size() << ": ";
        for(it = p[int(st[i])].begin(); it != p[int(st[i])].end(); it++)
            g << *it << ' ';
        g << '\n';
    }
}

int main()
{
    ifstream f("gramatica.in");
    ofstream g("gramatica.out");

    int nr_stari;
    char start, stare[260];    /// starile sunt caractere din codul ASCII
    bool elim[260];            /// elim[i] = true, daca starea i a fost eliminata
    set<string> prod[260];    /// prod[i] = productiile starii care se gaseste pe pozitia i in codul ASCII

    citire(f, nr_stari, stare, start, prod, elim);

    eliminare_lambda_productii(nr_stari, stare, start, prod, elim);
    eliminare_redenumiri(nr_stari, stare, start, prod, elim);
    eliminare_productii_inutile(nr_stari, stare, start, prod, elim);
    adaugare_neterminale_noi(nr_stari, stare, start, prod);
    adaugare_neterminale_noi_2(nr_stari, stare, start, prod);

    afisare(g, nr_stari, stare, start, prod);

    f.close();
    g.close();

    return 0;
}
