#include <iostream>
#include <string>

struct Vertex {
    int inf; // key
    std::string text;
    Vertex* left = nullptr, *right = nullptr;
};

Vertex* Add(Vertex*, int);
void prettyView(Vertex* p, std::string prefix, bool isLeft);
void keyView(Vertex* p);
void reverseView(Vertex* p);
void directView(Vertex* p);
Vertex* search(Vertex* p, int k);
void DelBranch(Vertex* p);
void Cut(Vertex*&, int);
void DeleteAll(Vertex*);
void collectSorted(Vertex*, Vertex**, int&);
Vertex* buildBalanced(Vertex** arr, int left, int right);

int main() {
    Vertex* root = nullptr;
    int choice;
    do {
        std::cout << "\n1-add 2-search 3-delete branch 4-view 5-balance 6-quit\n";
        std::cin >> choice;
        switch (choice) {
            case 1: {
                int k;
                std::cout << "Key: ";
                std::cin >> k;
                root = Add(root, k);
                break;
            }
            case 2: {
                int k;
                std::cout << "Key: ";
                std::cin >> k;
                Vertex* r = search(root, k);
                if (r) std::cout << r->inf << " " << r->text << "\n";
                else std::cout << "Not found.\n";
                break;
            }
            case 3: {
                int k;
                std::cout << "Key: ";
                std::cin >> k;
                Cut(root, k);
                break;
            }
            case 4: {
                prettyView(root, "", false);
                std::cout << "Direct: ";    directView(root);
                std::cout << "\nReverse: "; reverseView(root);
                std::cout << "\nKey: "; keyView(root);
                std::cout << "\n";
                break;
            }
            case 5: {
                int idx = 0;
                Vertex* arr[100];
                collectSorted(root, arr, idx);
                root = buildBalanced(arr, 0, idx - 1);
                std::cout << "Tree is balanced.\n";
                break;
            }
        }
    } while (choice != 6);
    DeleteAll(root);
    return 0;
}

Vertex* Add(Vertex* p, int k) {
    if (p == nullptr) {
        std::cout << "Text: ";
        std::string data;
        std::cin >> data;
        p = new Vertex();
        p->inf = k;
        p->text = data;
        return p;
    }
    if (k < p->inf) p->left = Add(p->left, k);
    else p->right = Add(p->right, k);
    return p;
}

void keyView(Vertex* p) {
    if (p) {
        keyView(p->left);
        std::cout << p->inf << " " << p->text << "  ";
        keyView(p->right);
    }
}
void prettyView(Vertex* p, std::string prefix, bool isLeft) {
    if (!p) return;
    prettyView(p->right, prefix + (isLeft ? "|   " : "    "), false);
    std::cout << prefix << "+-- " << p->inf << " (" << p->text << ")\n";
    prettyView(p->left, prefix + (isLeft ? "    " : "|   "), true);
}

void reverseView(Vertex* p) {
    if (p) {
        reverseView(p->left);
        reverseView(p->right);
        std::cout << p->inf << " " << p->text << "  ";
    }
}

void directView(Vertex* p) {
    if (p) {
        std::cout << p->inf << " " << p->text << "  ";
        directView(p->left);
        directView(p->right);
    }
}

Vertex* search(Vertex* p, int k) {
    if (p == nullptr) return nullptr;
    if (p->inf == k) return p;
    if (k < p->inf) return search(p->left, k);
    else return search(p->right, k);
}

void DelBranch(Vertex* p) {
    if (!p) return;
    DelBranch(p->left);
    DelBranch(p->right);
    delete p;
}

void Cut(Vertex*& p, int k) {
    if (!p) return;
    if (p->inf == k) {
        DelBranch(p);
        p = nullptr;
        return;
    }
    if (k < p->inf) Cut(p->left, k);
    else Cut(p->right, k);
}

void DeleteAll(Vertex* p) {
    if (!p) return;
    DeleteAll(p->left);
    DeleteAll(p->right);
    delete p;
}

void collectSorted(Vertex* p, Vertex** arr, int& idx) {
    if (!p) return;
    collectSorted(p->left, arr, idx);
    arr[idx++] = p;
    collectSorted(p->right, arr, idx);
}

Vertex* buildBalanced(Vertex** arr, int left, int right) {
    if (left > right) return nullptr;
    int mid = (left + right) / 2;
    Vertex* node = arr[mid];
    node->left = buildBalanced(arr, left, mid - 1);
    node->right = buildBalanced(arr, mid + 1, right);
    return node;
}