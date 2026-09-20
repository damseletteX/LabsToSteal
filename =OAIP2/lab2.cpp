#include <stdio.h>
#include <stdlib.h>

struct Stack{
    int el;
    Stack* next;
};

Stack* push(Stack*, int);
void viewStack(Stack*);
void deleteStack(Stack**);
Stack* pop(Stack*);
void sortPtr(Stack**);
void sortInfo(Stack*);
Stack* findMax(Stack*);

int main(){
    Stack* mystack=NULL;
    int n;
    printf("Enter amount of elements in the stack: ");
    scanf("%d", &n);
    getchar();
    printf("Enter the elements of your stack one by one: ");
    for (int i = 0; i < n; i++)
    {
        int input;
        scanf("%d", &input);
        getchar();
        mystack = push(mystack, input);
    }
    printf("Now your stack is: ");
    viewStack(mystack);
    printf("\nDo you wanna perform a sort?\nenter 1 for ptr sort, 2 for info sort, anything else for no sort:\n");
    int b;
    scanf("%d", &b);
    getchar();
    switch (b)
    {
    case 1:
        sortPtr(&mystack);
        printf("Now your stack is: ");
        viewStack(mystack);
        break;
    case 2:
        sortInfo(mystack);
        printf("Now your stack is: ");
        viewStack(mystack);
        break;
    default:
        break;
    }

    printf("\nNow let's remove the negative elements: ");
    Stack** ptr = &mystack;
    while(*ptr != NULL){
        if((*ptr)->el < 0){
            Stack* toDestroy = *ptr;
            *ptr = (*ptr)->next;
            pop(toDestroy);
        }
        else{
            ptr=&(*ptr)->next;
        }
    }
    // viewStack(mystack);
    // printf("Now let's find maximum and delete it.\n");
    // mystack = findMax(mystack);
    // viewStack(mystack);
    deleteStack(&mystack);
    return 0;
}

Stack* push(Stack* p, int value){
    Stack* t = (Stack*)malloc(sizeof(Stack));
    t->el = value;
    t->next = p;
    return t;
}

void viewStack(Stack* p){
    if(!p) { printf("Stack empty."); return; }
    while(p != NULL){
        printf("%d ", p->el);
        p=p->next;
    }
}

void deleteStack(Stack** p){
    Stack* t;
    while(*p != NULL){
        t = *p; // move address of the node to a bufer
        *p = (*p)->next; // move pointer to next node
        free(t); // delete everything at the address of buf
    }
}

Stack* pop(Stack* p){
    Stack *t = p;
    p=p->next;
    free(t);
    return p;
}

void sortPtr(Stack **p) {
    if ((*p) == NULL || (*p)->next == NULL) return;
    Stack *t = NULL, *t1, *r;
    *p = push(*p, 1000);
    if ((*p)->next->next == NULL) {
        Stack* tmp = *p;
        *p = (*p)->next;
        free(tmp);
        return;
    }
    do {
        for (t1=*p; t1->next->next != t; t1=t1->next)
            if (t1->next->el > t1->next->next->el){
            r = t1->next->next;
            t1->next->next = r->next;
            r->next = t1->next;
            t1->next = r;
            }
            t= t1->next;
        } while ((*p)->next->next != t);
    Stack* sentinel = *p;
    *p = (*p)->next;
    free(sentinel);
}

void sortInfo(Stack *p) {
Stack *t = NULL, *t1;
int r;
do { for (t1=p; t1 -> next != t; t1 = t1-> next)
if (t1->el > t1-> next -> el) {
r = t1-> el;
t1-> el = t1-> next -> el;
t1-> next -> el = r;
}
t = t1;
} while (p -> next != t);
}

Stack* findMax(Stack* head) {
    if (head == NULL) return NULL;

    Stack* maxPrev = NULL; 
    Stack* maxNode = head; 
    Stack* prev = NULL;
    Stack* curr = head;

    while (curr != NULL) {
        if (curr->el > maxNode->el) {
            maxNode = curr;
            maxPrev = prev;
        }
        prev = curr;
        curr = curr->next;
    }

    if (maxPrev == NULL) {
        head = head->next;
    } else {
        maxPrev->next = maxNode->next;
    }
    free(maxNode);
    return head;
}