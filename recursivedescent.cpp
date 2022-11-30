#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <strings.h>
#include "differentiator.h"

int x = 0;
const int Cmdsize = 15;

Node** LexicAnalizer(char* str)
{
    char** s = &str;

    Node** nodes = (Node**) calloc(strlen(str) + 2, sizeof(Node*));

    Node** oldnodes = nodes;

    StringAnalizer(s, &nodes);

    return oldnodes;
}

int StringAnalizer(char** s, Node*** nodes)
{
    double val = 0;
    char cmd[40] = "";
    int len = 0;

    while(isspace(**s))
        (*s)++;

    while (**s != '\0')
    {
        while(isspace(**s))
            (*s)++;
        if (**s == '+')
        {
            **nodes = CreateNode(OP_TYPE, 0, OP_ADD, NULL, NULL, NULL, NULL, NULL);
            (*s)++;
            (*nodes)++;
        }
        else if (**s == '-')
        {
            **nodes = CreateNode(OP_TYPE, 0, OP_SUB, NULL, NULL, NULL, NULL, NULL);
            (*s)++;
            (*nodes)++;
        }
        else if (**s == '*')
        {
            **nodes = CreateNode(OP_TYPE, 0, OP_MUL, NULL, NULL, NULL, NULL, NULL);
            (*s)++;
            (*nodes)++;
        }
        else if (**s == '/')
        {
            **nodes = CreateNode(OP_TYPE, 0, OP_DIV, NULL, NULL, NULL, NULL, NULL);
            (*s)++;
            (*nodes)++;
        }
        else if (**s == '^')
        {
            **nodes = CreateNode(OP_TYPE, 0, OP_POWER, NULL, NULL, NULL, NULL, NULL);
            (*s)++;
            (*nodes)++;
        }
        else if (**s == '(')
        {
            **nodes = CreateNode(OP_TYPE, 0, OP_OPBRC, NULL, NULL, NULL, NULL, NULL);
            (*s)++;
            (*nodes)++;
        }
        else if (**s == ')')
        {
            **nodes = CreateNode(OP_TYPE, 0, OP_CLBRC, NULL, NULL, NULL, NULL, NULL);
            (*s)++;
            (*nodes)++;
        }
        else if (sscanf(*s, "%lg%n", &val, &len) == 1)
        {
            **nodes = CreateNode(NUM_TYPE, val, OP_UNKNOWN, NULL, NULL, NULL, NULL, NULL);
            (*s) += len;
            (*nodes)++;
        }
        else if (sscanf(*s, "%[^()]%n", cmd, &len) == 1)
        {
            if (OperType optype = IsOper(cmd))
                **nodes = CreateNode(OP_TYPE, 0, optype, NULL, NULL, NULL, NULL, NULL);
            else
                **nodes = CreateNode(VAR_TYPE, 0, OP_UNKNOWN, cmd, NULL, NULL, NULL, NULL);
            *s += len;
            (*nodes)++;
        }
    }

    **nodes = CreateNode(OP_TYPE, 0, OP_END, NULL, NULL, NULL, NULL, NULL);
            (*s)++;
            (*nodes)++;

    return NOERR;
}

Node* GetG(Node** nodes)
{
    Node* node = GetE(&nodes);

    return node;
}

Node* GetE(Node*** arr)
{
    Node* currnode = GetT(arr);
    Node* nodeL = NULL;

    while ((**arr)->optype == OP_ADD || (**arr)->optype == OP_SUB)
    {
        nodeL = currnode;
        currnode = **arr;
        (*arr)++;

        Node* nodeR = GetT(arr);
        currnode->leftchild = nodeL;
        currnode->rightchild = nodeR;
    }

    return currnode;
}

Node* GetT(Node*** arr)
{
    Node* currnode = GetPower(arr);
    Node* nodeL = NULL;

    while ((**arr)->optype == OP_MUL || (**arr)->optype == OP_DIV)
    {
        nodeL = currnode;
        currnode = **arr;
        (*arr)++;

        Node* nodeR = GetPower(arr);
        currnode->leftchild = nodeL;
        currnode->rightchild = nodeR;
    }

    return currnode;
}

Node* GetPower(Node*** arr)
{
    Node* currnode = GetP(arr);
    Node* nodeL = NULL;

    while((**arr)->optype == OP_POWER)
    {
        nodeL = currnode;
        currnode = **arr;
        (*arr)++;
        Node* nodeR = GetP(arr);
        currnode->leftchild = nodeL;
        currnode->rightchild = nodeR;
    }

    return currnode;
}

Node* GetF(Node*** arr)
{
    Node* node = NULL;
    char cmd[Cmdsize] = "";
    int len = 0;

    if (((**arr)->optype || (**arr)->varvalue) && (**arr)->optype != OP_END && (**arr)->optype != OP_CLBRC && (**arr)->optype != OP_OPBRC)
    {
        node = **arr;
        (*arr)++;
        Node* nodeR = GetP(arr);

        if (node->optype)
            node->rightchild = nodeR;
        else
            node;
    }
    else
        Node* node = GetP(arr);

    return node;
}

Node* GetP(Node*** arr)
{
    Node* node = **arr;
    if (node->optype == OP_OPBRC)
    {
        (*arr)++;
        node = GetE(arr);
        (*arr)++;
    }
    else if (node->varvalue)
        node = GetV(arr);
    else if (node->val)
        node = GetN(arr);
    else
        node = GetF(arr);

    return node;
}

Node* GetV(Node*** arr)
{
    Node* node = **arr;

    (*arr)++;

    return node;
}

Node* GetN(Node*** arr)
{
    Node* node = **arr;

    (*arr)++;

    return node;
}

bool GetM(char** s)
{
    bool minus = false;
    const char* snew = *s - 1;
    char prevchar = *snew;

    if (prevchar != 'x' || prevchar <= '0' || prevchar >= '9')
        minus = true;

    return minus;
}
