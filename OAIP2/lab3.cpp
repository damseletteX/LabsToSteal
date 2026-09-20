#include <stdio.h>
#include <stdlib.h>
//Написать программу по созданию, добавлению (в начало, в конец), про
//смотру(с начала, с конца) и решению приведенной в подразделе 3.3 задачи для
//двунаправленных линейных списков.
//3. удалить из созданного списка отрицательные элементы
struct Queue
{
    int info;
    Queue *next=NULL, *prev=NULL;
};

void create(Queue**, Queue**, int);
void viewQueue(Queue*, Queue*, int);
void deleteQueue(Queue**, Queue**);
void deleteNegative(Queue**, Queue**);
Queue* addTo(int, Queue**, Queue**, int);

int main(){
    Queue *start=NULL, *end=NULL;
    int n = 0, a = 0, kod = 0;
    while(1){
        printf("\nChoose an action:\n1) create a queue   2) add an element\n3) delete negative elements   4) view the queue\n5) delete the queue.\nenter anything else to exit.\n");
        scanf("%d", &n);
        switch (n)
        {
        case 1:
            printf("Enter the 1st element: ");
            scanf("%d", &a);
            create(&start, &end, a);
            break;
        case 2:
            printf("Add to...\n1) the beginning    2) the end.\n");
            scanf("%d", &kod);
            printf("Now enter the element: ");
            scanf("%d", &a);
            kod == 1 ? start = addTo(kod, &start, &end, a) : end = addTo(kod, &start, &end, a);
            break;
        case 3:
            deleteNegative(&start, &end);
            printf("Negative elements have been deleted.\n");
            break;
        case 4:
            printf("Do you want to view the queue from...\n1) The beginning 2) The end.\n");
            scanf("%d", &a);
            printf("Your queue now is:\n");
            viewQueue(start, end, a);
            break;
        case 5:
            deleteQueue(&start, &end);
            printf("The whole queue has been deleted.\n");
            break;
        default:
            printf("Returning...\n");
            deleteQueue(&start, &end);
            return 0;
        }
    }
}

void create(Queue** start, Queue** end, int a){
    Queue* start = (Queue*)malloc(sizeof(Queue));
    (*start)->info = a;
    //t->next = t->prev=NULL;
    *start=*end;
}
// void viewQueue(Queue* start, Queue* end, int k){
//     if(start == NULL) {
//         printf("No queue exists!");
//         return;
//     }
//     Queue* t;
//     k == 1 ? t = start : t = end;
//     while(t){
//         printf("%d ", t->info);
//         k == 1 ? t=t->next : t=t->prev;
//     }
// }

void viewQueue(Queue** start, Queue** end, int k){
    if(!start){
        printf("Not found\n");
        return;
    }
    Queue* t;
    if(k){
        t = *start;
        while (t)
        {
            printf("%d ", t->info);
            t=t->next;
        }
    }
    else {
        while(t){
            printf("%d ", t->info);
            t=t->prev;
        }
    }
}

void deleteQueue(Queue** start, Queue** end){
    Queue* t = *start;
    while(t){
        Queue* a=t;
        t=t->next;
        free(a);
    }
    *start = NULL;
    *end = NULL;
}
void deleteNegative(Queue** start, Queue** end){
    Queue* t = *start;
    while(t){
        Queue* ptr = t;
        if(t->info < 0){
            if(ptr->prev == NULL){
                *start = ptr->next;
                (*start)->prev=NULL;
            }
            else if(ptr->next == NULL){
                *end = ptr->prev;
                (*end)->next = NULL;
            }
            else{
                ptr->next->prev=ptr->prev;
                ptr->prev->next=ptr->next;
            }
            t=ptr->next;
            free(ptr);
        }
        else{
            t=ptr->next;
        }
    }
}
Queue* addTo(int k, Queue** start, Queue** end, int inf){
    Queue *el = (Queue*)malloc(sizeof(Queue));
    el->info=inf;
    el->next=el->prev=NULL;
    if(*start==NULL){
        printf("No queue found! Creating a new one...");
        *start = *end = el;
    }
    switch (k)
    {
    case 1:
        el->next=*start;
        (*start)->prev = el;
        el->prev=NULL;
        return el;
    case 2:
        el->next=NULL;
        el->prev=*end;
        (*end)->next = el;
        return el;
    default:
        printf("Wrong kod! Returning.");
        return NULL;
    }
}   