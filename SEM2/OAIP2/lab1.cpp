#include <iostream>
using namespace std;

int RecursiveLSD(int, int);
int BaseLSD(int, int);
int main()
{
    setlocale(LC_ALL, "ru_RU.UTF-8"); 
    cout << "Введите значение N и M соответственно:\n";
    int N, M;
    cin >> N >> M;
    cout << "НОД равен: " << RecursiveLSD(N, M) << '\n';
    cout << "НОД равен: " << BaseLSD(N, M) << '\n';
}

int RecursiveLSD(int N, int M) { 
    return M % N == 0 ? N : RecursiveLSD(M % N, N); 
}

int BaseLSD(int N, int M){
    int ans;
    while(M%N != 0){
        int buf = M;
        M = N;
        N = buf%N;
    }
    ans = N;
    return ans;
}