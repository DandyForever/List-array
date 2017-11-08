#pragma once

#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <iostream>

typedef int ListElem_t;

const ListElem_t Crashcan1 = 666666666, Crashcan2 = 999999999, Poison = 1000000000;
const int Min_list_size = 10, Smallptr = 7, Add_size_li = 100;

struct Elem
{
    ListElem_t value;
    int next;
    int prev;
};

class List_t
{
    private:
        int canaryleft;

        Elem* data;

        int head;
        int tail;
        int free;
        int size;
        int numofelem;

        int canaryright;

        bool dump                     (const int value, FILE* file_error_info);
        bool dump                     (const double value, FILE* file_error_info);
        bool dump                     (const void* value, FILE* file_error_info);
        void Grow                     ();

    public:
        bool ListElemOK               ();
        bool ListOK                   ();
        void Dump                     ();

        List_t                        ();
        ~List_t                       ();

        int Head                      ();
        int Tail                      ();
        int Size                      ();
        Elem Element                  (const int index);

        void PushBack                 (const ListElem_t value);
        void PushFront                (const ListElem_t value);
        void Insert                   (const int position, const ListElem_t value);
        ListElem_t PopBack            ();
        ListElem_t PopFront           ();
        ListElem_t Erase              (const int position);

        int* InOrder                  ();
};
