#include "list_t.h"

#define assert(condition)                                                   \
        if (!condition)                                                     \
        {                                                                   \
        printf("ASSERTION %s in %s (%d)\n", #condition, __FILE__, __LINE__);\
        abort();                                                            \
        }

//--------------------------------------------------------
//! Macro to choose DEBUG mode
//!
//! @note write '1' to 'if' to use DEBUG mode
//! @note write '0' to 'if' not to use DEBUG mode
//--------------------------------------------------------
#if (1)
    #define ASSERT_LIST(); \
            if (!ListOK ())\
            {                 \
            Dump();    \
            assert(0);        \
            }
#else
    #define ASSERT_STACK();
#endif

List_t::List_t () :
    canaryleft (Crashcan1),

    head (0),
    tail (0),
    free (1),
    size (Min_list_size),
    numofelem (0),

    canaryright (Crashcan2)
    {
        data = new Elem [size] {};

        data[0].value = Poison;
        data[0].next = 0;
        data[0].prev = 0;

        for (int i = 1; i < size; i++)
        {
            data[i].next  = i + 1;
            data[i].prev = -1;
        }
        data[size - 1].next = 0;

        ASSERT_LIST()
    }

List_t::~List_t ()
{
    ASSERT_LIST()

    if (data != (Elem*) Smallptr)
        delete[] (data);

    data = (Elem*) Smallptr;
}

void List_t::PushBack (const ListElem_t value)
{
    ASSERT_LIST()

    if (!data[free].next)
        Grow ();

    ASSERT_LIST()

    const int free_c = free;

    if (!head && !tail)
    {
        head = free;
        tail = free;

        free = data[free_c].next;

        data[free_c].value = value;
        data[free_c].next = 0;
        data[free_c].prev = 0;

        numofelem++;

        ASSERT_LIST()
    }

    else
    {
        free = data[free_c].next;

        data[free_c].value = value;
        data[free_c].next = 0;
        data[free_c].prev = tail;

        data[tail].next = free_c;
        tail = free_c;

        numofelem++;

        ASSERT_LIST()
    }
}

void List_t::PushFront (const ListElem_t value)
{
    ASSERT_LIST()

    if (!data[free].next)
        Grow ();

    ASSERT_LIST()

    const int free_c = free;

    if (!head && !tail)
    {
        free = data[free_c].next;

        head = free_c;
        tail = free_c;

        data[free_c].value = value;
        data[free_c].next = 0;
        data[free_c].prev = 0;

        numofelem++;

        ASSERT_LIST()
    }

    else
    {
        free = data[free_c].next;

        data[free_c].value = value;
        data[free_c].next = head;
        data[free_c].prev = 0;

        data[head].prev = free_c;
        head = free_c;

        numofelem++;

        ASSERT_LIST()
    }
}

ListElem_t List_t::PopBack ()
{
    if (!head && !tail)
    {
        head--;
        tail--;
        printf ("EMPTY LIST\n");

        ASSERT_LIST()
    }

    else
    {
        const int tail_c = tail;

        tail = data[tail_c].prev;

        if (tail == 0)
            head = 0;

        data[tail_c].next = free;
        data[tail_c].prev = -1;

        free = tail_c;

        data[tail].next = 0;

        numofelem--;

        ASSERT_LIST()

        return data[tail_c].value;
    }
}

ListElem_t List_t::PopFront ()
{
    if (!tail && !head)
    {
        head--;
        tail--;
        printf ("EMPTY LIST\n");

        ASSERT_LIST()
    }

    else
    {
        const int head_c = head;

        head = data[head_c].next;

        if (head == 0)
            tail = 0;

        data[head_c].prev = -1;
        data[head_c].next = free;

        free = head_c;

        data[head].prev = 0;

        numofelem--;

        ASSERT_LIST()

        return data[head_c].value;
    }
}

bool List_t::ListOK ()
{
    return this && canaryleft == Crashcan1 && canaryright == Crashcan2 &&
            data && head >= 0 && tail >= 0 && free && data[0].next == 0 &&
            data[0].prev == 0 && data[tail].next == 0 && data[head].prev == 0 &&
            data[0].value == Poison && numofelem >= 0 && ListElemOK ();
}

bool List_t::ListElemOK ()
{
    bool isOK = 1;

    int current = head;

    for (int i = 0; i < numofelem; i++)
    {
        if (current != head)
        {
            if (data[data[current].prev].next != current)
                isOK = 0;
        }

        else
        {
            if (data[current].prev != 0)
                isOK = 0;
        }

        if (current != tail)
        {
            if (data[data[current].next].prev != current)
                isOK = 0;
        }

        else
        {
            if (data[current].next != 0)
                isOK = 0;
        }

        if (current == 0)
            isOK = 0;

        current = data[current].next;
    }

    if (current != 0)
        isOK = 0;

    if (data[0].next != 0 || data[0].prev != 0 || data[0].value != Poison)
        isOK = 0;

    return isOK;
}

bool List_t::dump (const int value, FILE* file_error_info)
{
    fprintf (file_error_info, "%d", value);
}

bool List_t::dump (const double value, FILE* file_error_info)
{
    fprintf (file_error_info, "%f\\n", value);

    if (isnan (value))
    {
        fprintf (file_error_info, "not a number");

        return 0;
    }

    return 1;
}

bool List_t::dump (const void* value, FILE* file_error_info)
{
    ListElem_t* val = (ListElem_t*) value;

    if (val == NULL)
    {
        fprintf (file_error_info, "%p\\nzero pointer", val);

        return 0;
    }

    else
        fprintf (file_error_info, "%p", val);

    return 1;
}

void List_t:: Dump ()
{
    FILE* output_file = fopen ("Dump.dot", "w");
    assert (output_file);

    fprintf (output_file, "digraph List\n{\n\trankdir = LR\n");
    fprintf (output_file, "\t\tnode [ shape = \"box\", color = \"black\" ]\n");
    fprintf (output_file, "\t\tedge [ color = \"black\" ]\n\n");

    if (canaryleft != Crashcan1)
        fprintf (output_file, "\tCanaryLEFT [ label = \"CanaryLEFT = %d\\nBUT EXPECTED %d\","
                 "color = \"red\" ]\n", canaryleft, Crashcan1);
    else
        fprintf (output_file, "\tCanaryLEFT [ label = \"CanaryLEFT = %d\" ]\n", canaryleft);

    if (canaryright != Crashcan2)
        fprintf (output_file, "\tCanaryRIGHT [ label = \"CanaryRIGHT = %d\\nBUT EXPECTED %d\","
                 "color = \"red\" ]\n", canaryright, Crashcan2);
    else
        fprintf (output_file, "\tCanaryRIGHT [ label = \"CanaryRIGHT = %d\" ]\n", canaryright);

    if (data == nullptr)
        fprintf (output_file, "\tDATA [label = \"DATA = %p\\nzero pointer\", color = \"red\" ]\n", data);

    else
        fprintf (output_file, "\tDATA [label = \"DATA = %p\" ]\n", data);

    if (size >= 0)
        fprintf (output_file, "\tsize [ label = \"size = %d\" ]\n\n", size);
    else
        fprintf (output_file, "\tsize [ label = \"size = %d\\n<0\", color = \"red\"]\n\n", size);

    fprintf (output_file, "\telem0 [ shape = \"record\", label = \"<index>index = 0 | { next = %d | value = %d | prev = %d}\" ]\n",
             data[0].prev, data[0].value, data[0].next);

    if (data[0].prev != 0 || data[0].next != 0 || data[0].value != Poison)
        fprintf (output_file, "\telem0 [ color = \"red\" ]\n\n");

    int number = 1;

    for (int current = head; current != 0; current = data[current].next)
    {
        fprintf (output_file, "\telem%d [ shape = \"record\", label = \" <index>%d\\n", number, number);

        if (current == head)
            fprintf (output_file, "HEAD\\n");

        if (current == tail)
            fprintf (output_file, "TAIL\\n");

        fprintf (output_file, "index = %d | {<prev> prev\\n %d | value\\n ", current, data[current].prev);

        bool isnormal = dump (data[current].value, output_file);

        fprintf (output_file, "| <next> next\\n%d} \", color = ", data[current].next);

        if (isnormal)
            fprintf (output_file, "\"blue\" ]\n");

        else
            fprintf (output_file, "\"red\" ]\n");

        number++;
    }

    fprintf (output_file, "\n");

    number = 1;

    printf ("%d", numofelem);
    for (int current = head; current != 0; current = data[current].next)
    {
        fprintf (output_file, "\telem%d:<prev> -> elem%d:<index>\n", number, number - 1);

        fprintf (output_file, "\telem%d:<next> -> elem%d:<index> [ color = \"green\" ]\n", number, (number + 1) % (numofelem + 1));

        number++;
    }


    number = 1;

    for (int current = free; current != 0; current = data[current].next)
    {
        fprintf (output_file, "\tfrem%d [ shape = \"record\", label = \" <index>%d\\n", number, number);

        if (current == free)
            fprintf (output_file, "FREE\\n");

        fprintf (output_file, "index = %d | {<prev> prev\\n %d | <next> next\\n%d} \", color = ",
                 current, data[current].prev, data[current].next);

        bool isnormal = data[current].prev == -1;

        if (isnormal)
            fprintf (output_file, "\"blue\" ]\n");

        else
            fprintf (output_file, "\"red\" ]\n");

        number++;
    }

    fprintf (output_file, "\n");

    number = 1;

    for (int current = free; data[current].next != 0; current = data[current].next)
    {
        fprintf (output_file, "\tfrem%d:<next> -> frem%d:<index> [ color = \"green\" ]\n", number, number + 1);

        number++;
    }

    fprintf (output_file, "}");
    fclose (output_file);


    system ("dot -Tpng Dump.dot -o Dump.png");
    /*FILE* output_file = fopen ("Dump.txt", "w");
    assert (output_file);

    if (canaryleft == Crashcan1)
        fprintf (output_file, "LEFT  canary is OK! [%d]\n", Crashcan1);
    else
        fprintf (output_file, "LEFT  canary is WRONG! [%d] but expected [%d]\n", canaryleft, Crashcan1);

    if (canaryright == Crashcan2)
        fprintf (output_file, "RIGHT canary is OK! [%d]\n\n", Crashcan2);
    else
        fprintf (output_file, "RIGHT canary is WRONG! [%d] but expected [%d]\n\n", canaryleft, Crashcan1);

    if (head >= 0 && &head)
        fprintf (output_file, "HEAD [%p] = %d\n", &head, head);
    else
        fprintf (output_file, "HEAD [%p] = %d            ERROR!!!\n", &head, head);

    if (tail >= 0 && &tail)
        fprintf (output_file, "TAIL [%p] = %d\n", &tail, tail);
    else
        fprintf (output_file, "TAIL [%p] = %d            ERROR!!!\n", &tail, tail);

    if (free >= 0 && &free)
        fprintf (output_file, "FREE [%p] = %d\n\n", &free, free);
    else
        fprintf (output_file, "FREE [%p] = %d            ERROR!!!\n\n", &free, free);

    if (size >= Min_list_size)
        fprintf (output_file, "SIZE [%p] = %d\n\n", &size, size);
    else
        fprintf (output_file, "SIZE [%p] = %d            ERROR!!! Little size!!!\n\n", &size, size);

    if (data)
        fprintf (output_file, "DATA [%p] \n", data);
    else
        fprintf (output_file, "DATA [%p]                 ERROR!!!\n", data);

    fprintf (output_file, "{\n");

    for (int i = 0; i < size; i++)
    {
        fprintf (output_file, "[%6d]:\n", i);

        if (data[i].next >= 0)
            fprintf (output_file, "          %6d\n", data[i].next);
        else
            fprintf (output_file, "          %6d         ERROR!!!\n", data[i].next);

        if (data[i].prev >= 0 || data[i].prev == -1)
            fprintf (output_file, "          %6d\n", data[i].prev);
        else
            fprintf (output_file, "          %6d         ERROR!!!\n", data[i].prev);

        dump (data[i].value, output_file);

        fprintf (output_file, "\n");
    }
    fprintf (output_file, "}");

    fclose (output_file);*/
}

void List_t::Grow ()
{
    ASSERT_LIST()

    const int buffer_size_li = size + Add_size_li;
    assert(buffer_size_li > size);

    Elem* buffer = new Elem [buffer_size_li] {};
    assert (buffer);

    std::copy (data, data + size, buffer);

    delete[] (data);

    data = buffer;

    data[size - 1].next = size;

    for (int i = size; i < size + Add_size_li - 1; i++)
    {
        data[i].next = i + 1;
        data[i].prev = -1;
    }

    size += Add_size_li;
    data[size - 1].prev = -1;

    ASSERT_LIST()
}

void List_t::Insert (const int position, const ListElem_t value)
{
    ASSERT_LIST()

    if (position >= size || data[position].prev == -1 || position == 0)
    {
        printf ("BAD ADDRESS\n");
        assert (0);
    }

    else
    {
        if (position == tail)
        {
            PushBack (value);
        }
        else
        {
            if (!data[free].next)
                Grow ();

            const int free_c = free;
            free = data[free].next;

            data[free_c].value = value;

            data[free_c].next = data[position].next;
            data[free_c].prev = position;

            data[data[position].next].prev = free_c;
            data[position].next = free_c;

            numofelem++;
        }
    }



    ASSERT_LIST()
}

ListElem_t List_t::Erase (const int position)
{
    ASSERT_LIST()

    if (position >= size || data[position].prev == -1 || position == 0)
    {
        printf ("BAD ADDRESS\n");
        assert (0);
    }
    else
    {
        if (position == head)
        {
            return PopFront ();
        }
        else if (position == tail)
        {
            return PopBack ();
        }
        else
        {
            data[data[position].next].prev = data[position].prev;
            data[data[position].prev].next = data[position].next;

            data[position].next = free;
            free = position;
            data[position].prev = -1;

            numofelem--;

            ASSERT_LIST()

            return data[position].value;
        }
    }
}

int List_t::Head ()
{
    ASSERT_LIST()

    return head;
}


int List_t::Tail ()
{
    ASSERT_LIST()

    return tail;
}

int List_t::Size ()
{
    ASSERT_LIST()

    return numofelem;
}

Elem List_t::Element (const int index)
{
    ASSERT_LIST()

    if (index != tail && index != head && data[data[index].next].prev != index)
    {
        printf ("BAD INDEX\n");
        assert (0);
    }

    return data[index];
}

int* List_t::InOrder ()
{
    ASSERT_LIST()

    int* buffer = new int [numofelem];
    assert (buffer);

    int i = 0;

    for (int current = head; current != 0; current = data[current].next)
    {
        buffer[i] = current;
        i++;
    }

    ASSERT_LIST()

    return buffer;
}
