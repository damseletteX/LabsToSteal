#include <iostream>
#include <set>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

using Graph = set<pair<string, string>>;

struct Relation {
    set<string> X;
    set<string> Y;
    Graph G;
};

// ===== ДОПОЛНИТЕЛЬНЫЕ МНОЖЕСТВА =====
// B ⊆ Y — для образа
set<string> IMAGE_SET = {"b", "c"};

// A ⊆ X — для прообраза
set<string> PREIMAGE_SET = {"x1", "x2"};

// ---------- ОПЕРАЦИИ НАД ГРАФАМИ ----------
Graph getUnion(const Graph& A, const Graph& B) {
    Graph r = A;
    for (auto& p : B) r.insert(p);
    return r;
}

Graph getIntersection(const Graph& A, const Graph& B) {
    Graph r;
    for (auto& p : A)
        if (B.count(p)) r.insert(p);
    return r;
}

Graph getDifference(const Graph& A, const Graph& B) {
    Graph r;
    for (auto& p : A)
        if (!B.count(p)) r.insert(p);
    return r;
}

Graph getInversion(const Graph& G) {
    Graph r;
    for (auto& p : G)
        r.insert({p.second, p.first});
    return r;
}

Graph getComposition(const Graph& A, const Graph& B) {
    Graph r;
    for (auto& a : A)
        for (auto& b : B)
            if (a.second == b.first)
                r.insert({a.first, b.second});
    return r;
}

Graph cartesian(const Relation& R) {
    Graph g;
    for (auto& x : R.X)
        for (auto& y : R.Y)
            g.insert({x, y});
    return g;
}

// ---------- ОПЕРАЦИИ НАД СООТВЕТСТВИЯМИ ----------
Relation relationUnion(const Relation& A, const Relation& B) {
    Relation R;
    R.X = A.X; R.Y = A.Y;
    R.X.insert(B.X.begin(), B.X.end());
    R.Y.insert(B.Y.begin(), B.Y.end());
    R.G = getUnion(A.G, B.G);
    return R;
}

Relation relationIntersection(const Relation& A, const Relation& B) {
    Relation R;
    for (auto& x : A.X) if (B.X.count(x)) R.X.insert(x);
    for (auto& y : A.Y) if (B.Y.count(y)) R.Y.insert(y);
    R.G = getIntersection(A.G, B.G);
    return R;
}

Relation relationDifference(const Relation& A, const Relation& B) {
    Relation R = A;
    R.G = getDifference(A.G, B.G);
    return R;
}

Relation relationInversion(const Relation& A) {
    Relation R;
    R.X = A.Y;
    R.Y = A.X;
    R.G = getInversion(A.G);
    return R;
}

Relation relationComposition(const Relation& A, const Relation& B) {
    Relation R;
    R.X = A.X;
    R.Y = B.Y;
    R.G = getComposition(A.G, B.G);
    return R;
}

Relation relationComplement(const Relation& A) {
    Relation R;
    R.X = A.X;
    R.Y = A.Y;
    R.G = getDifference(cartesian(A), A.G);
    return R;
}

// ---------- ОБРАЗ МНОЖЕСТВА ----------
Relation relationImage(const Relation& R) {
    Relation A;
    A.X = R.X;
    A.Y = R.Y;

    for (auto& p : R.G)
        if (IMAGE_SET.count(p.second))
            A.X.insert(p.first);

    for (auto& x : A.X)
        A.G.insert({x, x});

    return A;
}

// ---------- ПРООБРАЗ МНОЖЕСТВА ----------
Relation relationPreimage(const Relation& R) {
    Relation A;
    A.X = R.X;
    A.Y = R.Y;

    for (auto& p : R.G)
        if (PREIMAGE_SET.count(p.first))
            A.Y.insert(p.second);

    for (auto& y : A.Y)
        A.G.insert({y, y});

    return A;
}

// ---------- ПРИОРИТЕТ ----------
int pr(char c) {
    if (c=='!'||c=='\''||c=='#'||c=='@') return 3;
    if (c=='*'||c=='&') return 2;
    if (c=='|'||c=='\\') return 1;
    return 0;
}

bool isOp(const string& s) {
    return s=="*"||s=="&"||s=="|"||s=="\\"||
           s=="'"||s=="!"||s=="#"||s=="@";
}

// ---------- ОПЗ ----------
vector<string> toRPN(const string& e) {
    stack<char> st;
    vector<string> out;

    for (size_t i=0;i<e.size();i++) {
        if (isalnum(e[i])) {
            string t;
            while (i<e.size() && isalnum(e[i])) t+=e[i++];
            i--; out.push_back(t);
        }
        else if (e[i]=='(') st.push(e[i]);
        else if (e[i]==')') {
            while (!st.empty() && st.top()!='(') {
                out.push_back(string(1,st.top())); st.pop();
            }
            st.pop();
        }
        else {
            while (!st.empty() && pr(st.top())>=pr(e[i])) {
                out.push_back(string(1,st.top())); st.pop();
            }
            st.push(e[i]);
        }
    }

    while (!st.empty()) {
        out.push_back(string(1,st.top())); st.pop();
    }

    return out;
}

// ---------- ПОИСК ----------
Relation findRel(const string& n, const vector<pair<string,Relation>>& v) {
    for (auto& p : v) if (p.first==n) return p.second;
    return {};
}

// ---------- ВЫЧИСЛЕНИЕ ----------
Relation eval(const vector<string>& rpn,
              const vector<pair<string,Relation>>& rels) {
    stack<Relation> st;

    for (auto& t : rpn) {
        if (t=="'") { auto a=st.top(); st.pop(); st.push(relationInversion(a)); }
        else if (t=="!") { auto a=st.top(); st.pop(); st.push(relationComplement(a)); }
        else if (t=="#") { auto a=st.top(); st.pop(); st.push(relationImage(a)); }
        else if (t=="@") { auto a=st.top(); st.pop(); st.push(relationPreimage(a)); }
        else if (isOp(t)) {
            auto b=st.top(); st.pop();
            auto a=st.top(); st.pop();
            if (t=="&") st.push(relationIntersection(a,b));
            if (t=="|") st.push(relationUnion(a,b));
            if (t=="\\") st.push(relationDifference(a,b));
            if (t=="*") st.push(relationComposition(a,b));
        }
        else st.push(findRel(t, rels));
    }
    return st.top();
}

// ---------- MAIN ----------
int main() {
    ifstream in("input.txt");

    string expr; getline(in, expr);
    expr.erase(remove(expr.begin(),expr.end(),' '),expr.end());

    int n; in>>n;
    vector<pair<string,Relation>> rels;

    for (int i=0;i<n;i++) {
        Relation R; string name;
        in>>name;
        int k; string s;

        in>>k; while(k--) { in>>s; R.X.insert(s); }
        in>>k; while(k--) { in>>s; R.Y.insert(s); }
        in>>k; while(k--) { string a,b; in>>a>>b; R.G.insert({a,b}); }

        rels.push_back({name,R});
    }

    auto rpn = toRPN(expr);
    Relation res = eval(rpn, rels);

    ofstream out("output.txt");
    out<<"X = { "; for(auto& x:res.X) out<<x<<" "; out<<"}\n";
    out<<"Y = { "; for(auto& y:res.Y) out<<y<<" "; out<<"}\n";
    out<<"G = { "; for(auto& p:res.G) out<<"<"<<p.first<<","<<p.second<<"> "; out<<"}\n";
}
