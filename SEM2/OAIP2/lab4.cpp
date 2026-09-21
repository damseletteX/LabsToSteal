#include <stdlib.h>
#include <iostream>
#include <iomanip>
struct Node{
    char data;
    Node* next;
};

struct Stack{
    Node* top = nullptr;

    void push(char inf){
        Node* node = (Node*)malloc(sizeof(Node));
        node->data = inf;
        node->next = top;
        top = node;
    }
    char pop(){
        if(!top) throw "Stack empty!";
        char val = top->data;
        Node* temp = top;
        top = top->next;
        free(temp);
        return val;
    }
    bool is_empty() { return top == nullptr; }
};

struct NodeD{
    double data;
    NodeD* next;
};

struct StackD{
    NodeD* top = nullptr;
    void push(double inf){
        NodeD* node = (NodeD*)malloc(sizeof(NodeD));
        node->next = top;
        node->data = inf;
        top = node;
    }
    double pop(){
        if(!top) throw "stack empty!\n";
        NodeD* temp = top;
        double val = top->data;
        top = top->next;
        free(temp);
        return val;
    }
    bool is_empty() { return top == nullptr; }
};

bool isdigit(char a) { return (a <= 39 && a >= 30); }
bool isalpha(char a) { return (a <= 122 && a >= 97); }
int get_priority(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

std::string toRPN(const std::string& expr){
    std::string output = "";
    Stack ops;
    for (int i = 0; i < (int)expr.size(); i++)
    {
        char ch = expr[i];
        if(ch == ' ') continue;
        if(isalpha(ch) || isdigit(ch)) {
            output += ch;
            output += ' ';
        }
        else if(ch == '(') ops.push(ch);
        else if(ch == ')'){
            while(ops.top && ops.top->data != '('){
                output += ops.pop();
                output += ' ';
            }
            ops.pop();
        }
        else {
            while(ops.top && get_priority(ops.top->data) >= get_priority(ch)){
                output += ops.pop();
                output += ' ';
            }
            ops.push(ch);
        }
    }
    while (ops.top) {
        output += ops.pop();
        output += ' ';
    }
    return output;
}

double evalRPN(const std::string& rpn, double a, double b, double c, double d, double e){
    double vars[5] = {a, b, c, d, e};
    StackD st;
    for (int i = 0; i < (int)rpn.size(); i++) {
        char ch = rpn[i];
        if (ch == ' ') continue;

        if (isalpha(ch)) { //превращаем букву в индекс массива аской
            st.push(vars[ch - 'a']); // 'a'=0, 'b'=1, 'c'=2, 'd'=3, 'e'=4
        } else {
            double op2 = st.pop();
            double op1 = st.pop();
            switch (ch) {
                case '+': st.push(op1 + op2); break;
                case '-': st.push(op1 - op2); break;
                case '*': st.push(op1 * op2); break;
                case '/': st.push(op1 / op2); break;
                case '^': st.push(pow(op1, op2)); break;
            }
        }
    }
    return st.pop();
}

int main(){
    std::string expr = "a-b";
    double a = 3.1, b = 5.4, c = 0.2, d = 9.6, e = 7.8;
    std::cout << "Infix form: " << expr << std::endl;
    std::string rpn = toRPN(expr);
    std::cout << "Reverse polish notation: " << rpn << std::endl;
    double res = evalRPN(rpn, a, b, c, d, e);
    std::cout << std::fixed;
    std::cout.precision(2);
    std::cout << "Vars : a = " << a << " b= "<< b
            << " c= " << c << " d= " << d << " e= " << e << std::endl;
    std::cout << "Result: " << res << std::endl;
    return 0;
}