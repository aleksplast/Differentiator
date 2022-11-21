#include <stdio.h>
#include <assert.h>

#include "recursivedescent.h"

const char* s = NULL;

int GetG(const char* str)
{
    s = str;
    int val = GetE();
    assert(*s == '\0');

    return val;
}

int GetE()
{
    int val = GetT();

    while (*s == '+' || *s == '-')
    {
        int op = *s;
        s++;

        int val2 = GetT();
        if (op == '+')
            val += val2;
        else
            val -= val2;
    }

    return val;
}

int GetT()
{
    int val = GetP();

    while (*s == '*' || *s == '/')
    {
        int op = *s;
        s++;

        int val2 = GetP();
        if (op == '*')
            val *= val2;
        else
            val /= val2;
    }

    return val;
}

int GetP()
{
    int val = 0;
    if (*s == '(')
    {
        s++;
        val = GetE();
        assert(*s == ')');
        s++;
    }
    else
        val = GetN();

    return val;
}

int GetN()
{
    int val = 0;

    const char* sOld = s;

    while (*s >= '0' && *s <= '9')
    {
        val = 10 * val + *s - '0';
        s++;
    }

    assert(s != sOld);

    return val;
}
