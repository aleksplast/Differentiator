#include <stdio.h>
#include <assert.h>

#include "recursivedescent.h"

const char* s = NULL;
int x = 0;

int GetG(const char* str, const int var)
{
    s = str;
    x = var;

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
    else if (*s == 'x')
        val = GetV();
    else
        val = GetN();

    return val;
}

int GetV()
{
    int val = 0;

    if (*s == 'x')
    {
        val = x;
        s++;
    }

    return val;
}

int GetN()
{
    int val = 0;
    bool minus = false;

    const char* sOld = s;

    if (*s == '-')
        minus = GetM();

    if (minus)
        s++;

    while (*s >= '0' && *s <= '9')
    {
        val = 10 * val + *s - '0';
        s++;
    }

    if (minus)
        val = -val;

    assert(s != sOld);

    return val;
}

bool GetM()
{
    bool minus = false;
    const char* snew = s - 1;
    char prevchar = *snew;

    if (prevchar != 'x' || prevchar <= '0' || prevchar >= '9')
        minus = true;

    return minus;
}
