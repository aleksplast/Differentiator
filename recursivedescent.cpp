#include <stdio.h>
#include <assert.h>

#include "differentiator.h"

const char* s = NULL;
int x = 0;

Node* GetG(const char* str, const int var)
{
    s = str;
    x = var;

    Node* node = GetE();
    assert(*s == '\0');

    return node;
}

Node* GetE()
{
    Node* node = GetT();

    while (*s == '+' || *s == '-')
    {
        int op = *s;
        s++;

        Node* nodeR = GetT();
        if (op == '+')
            node = ADD(node, nodeR);
        else
            node = SUB(node, nodeR);
    }

    return node;
}

Node* GetT()
{
    Node* node = GetP();

    while (*s == '*' || *s == '/')
    {
        int op = *s;
        s++;

        Node* nodeR = GetP();
        if (op == '*')
            node = MUL(node, nodeR);
        else
            node = DIV(node, nodeR);
    }

    return node;
}

Node* GetP()
{
    Node* node = NULL;
    if (*s == '(')
    {
        s++;
        node = GetE();
        assert(*s == ')');
        s++;
    }
    else if (*s == 'x')
        node = GetV();
    else
        node = GetN();

    return node;
}

Node* GetV()
{
    Node* node = 0;

    if (*s == 'x')
        s++;

    return NEWVAR("x");
}

Node* GetN()
{
    int val = 0;
    bool minus = false;
    Node* node = NULL;

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

    return NEWNUM(val);
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
