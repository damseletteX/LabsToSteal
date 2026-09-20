//3: Двойным хешированием
#include <iostream>
#define M 20
#define n 7
#define EMPTY -1
struct Elem{
    std::string surname;
    int birthYear, groupNum;
};

int countstep(int, int);
Elem** create_table(int);
void insert(Elem**, Elem*, int);
Elem* seacrh(Elem**, int, int);

int main(){
    Elem** studs=new Elem*[n];
    for (int i = 0; i < n; i++)
    {
        studs[i] = new Elem();
        std::cout << "\nenter surname, then birth year, then group number.\n";
        std::cin >> studs[i]->surname >> studs[i]->birthYear >> studs[i]->groupNum;
    }
    Elem** table = create_table(M);
    for (int i = 0; i < n; i++) insert(table, studs[i], M);
    while(1){
        std::cout << "Enter the year to search by birth year. Press 0 to return:\n";
        int year;
        std::cin >> year;
        switch (year)
        {
        case 0:
            std::cout << "returning";
            return 0;
        default:
            Elem* res = seacrh(table, year, M);
            std::cout << "Found: " << res->surname << " " << res->birthYear << " " << res->groupNum << std::endl;
            break;
        }
    }
    return 0;
}

int hash1(int k, int size) { return k % size; }
Elem** create_table(int size){
    Elem** table= (Elem**)malloc(size*sizeof(Elem*));
    for (int i = 0; i < size; i++) table[i] = NULL;
    return table;
}

void insert(Elem** table, Elem* stud, int size){
    int i = 0, idx = hash1(stud->birthYear, size), step = countstep(stud->birthYear, size);
    while(table[idx]!=nullptr && i < size){
        i++;
        idx = (hash1(stud->birthYear, size) + i * step)%size;
        
    }
    if (table[idx] == nullptr) table[idx] = stud;
}

int countstep(int k, int size){ return 1 + (k % (size-2)); }
Elem* seacrh(Elem** table, int k, int size){
    int i = 0, idx = hash1(k, size), step = countstep(k, size);
    while(table[idx]!=nullptr && table[idx]->birthYear != k && i < size){
        i++;
        idx = (hash1(k, size) + i * step) % size;
    }
    if(table[idx]==nullptr || table[idx]->birthYear!=k) std::cout << "Not found.\n";
    return table[idx];
}