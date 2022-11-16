#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include <assert.h>

#include "differentiator.h"

static int iter = 0;
const int STRSIZE = 20;

int DifferentiatorMain(char* input)
{
    Tree datatree = DataParser(input);

    TreeGraphDump(&datatree, 0, __LINE__, __func__, __FILE__);

    Tree difftree = {};
    TreeCtor(&difftree, UNKNOWN_TYPE, 0, OP_UNKNOWN, NULL, "difftreelog.htm");

    Node* newanchor = DiffNode(datatree.anchor);

    difftree.anchor = newanchor;

    TreeGraphDump(&difftree, 0, __LINE__, __func__, __FILE__);

    NodePrint(difftree.anchor);

    return NOERR;
}

OperType IsOper(char* str)
{
    if (strcmp(str, "*") == 0)
        return OP_MUL;
    else if (strcmp(str, "+") == 0)
        return OP_ADD;
    else if (strcmp(str, "/") == 0)
        return OP_DIV;
    else if (strcmp(str, "-") == 0)
        return OP_SUB;
    else if (strcmp(str, "sin") == 0)
        return OP_SIN;
    else if (strcmp(str, "cos") == 0)
        return OP_COS;

    return OP_UNKNOWN;
}

Tree DataParser(char* inputdata)
{
    Tree difftree = {};
    FILE* data = fopen(inputdata, "r");
    long size = FileSize(data);
    char* input = (char*) calloc(size, sizeof(char));

    fread(input, 1, size, data);

    char* log = (char*) calloc(STRSIZE, sizeof(char));
    log = "graphlog.htm";

    TreeCtor(&difftree, UNKNOWN_TYPE, 0, OP_UNKNOWN, NULL, log);

    Node* currnode = difftree.anchor;

    for (int counter = 0; counter < size; counter++)
    {
        while(isspace(*(input + counter)))
            counter += 1;

        if (*(input + counter) == ')')
        {
            currnode = currnode->ancestor;
            continue;
        }
        else if (*(input + counter) == '(')
        {
            if (currnode->leftchild)
            {
                AddRightChild(&difftree, currnode);
                currnode = currnode->rightchild;
                continue;
            }
            else
            {
                AddLeftChild(&difftree, currnode);
                currnode = currnode->leftchild;
                continue;
            }
        }
        else
        {
            counter += FillCurrNode(currnode, input + counter);
        }
    }

    difftree.anchor = difftree.anchor->leftchild;
    free(difftree.anchor->ancestor);
    difftree.anchor->ancestor = NULL;
    difftree.size--;

    return difftree;
}

int FillCurrNode(Node* currnode, char* input)
{
    double val = 0;
    char* str = (char*) calloc(STRSIZE, sizeof(char));
    int len = 0;

    if (sscanf(input, "%lg%n", &val, &len) == 1)
    {
        currnode->type = NUM_TYPE;
        currnode->val = val;

        return len - 1;
    }
    else if (sscanf(input, "%[^()]%n", str, &len) == 1)
    {
        if (OperType optype = IsOper(str))
        {
            currnode->type = OP_TYPE;
            currnode->optype = optype;
        }
        else
        {
            currnode->type = VAR_TYPE;
            currnode->varvalue = str;
        }

        return len - 1;
    }

    return len;
}

Node* NodeCopy(Node* node)
{
    DBG assert(node != NULL);

    Node* newnode = (Node*) calloc(1, sizeof(Node));

    newnode->optype = node->optype;
    newnode->val = node->val;
    newnode->type = node->type;
    newnode->varvalue = node->varvalue;

    if (node->leftchild)
        newnode->leftchild = NodeCopy(node->leftchild);
    if (node->rightchild)
        newnode->rightchild = NodeCopy(node->rightchild);

    return newnode;
}

Node* DiffNode(Node* node)
{
    switch (node->type)
    {
        case NUM_TYPE:
            return CREATENUM(0);
        case VAR_TYPE:
            if (*(node->varvalue) == 'x')
                return CREATENUM(1);
            else
                return CREATENUM(0);
        case OP_TYPE:
            switch (node->optype)
            {
                case OP_ADD:
                    return ADD(DL, DR);
                case OP_SUB:
                    return SUB(DL, DR);
                case OP_MUL:
                    return ADD(MUL(DL, CR), MUL(CL, DR));
                case OP_DIV:
                    return DIV(SUB(MUL(DL, CL), MUL(CL, DR)), MUL(CL, CL));
                case OP_SIN:
                    return MUL(COS(NULL, CR), DR);
                case OP_COS:
                    return MUL(MUL(SIN(NULL, CR), CREATENUM(-1)), DR);
            }
    }

    return node;
}

Node* CreateNode(NodeType type, double val, OperType optype, char* varvalue, Node* leftchild, Node* rightchild)
{
    Node* newnode = (Node*) calloc(1, sizeof(Node));

    newnode->optype = optype;
    newnode->val = val;
    newnode->type = type;
    newnode->varvalue = varvalue;

    newnode->leftchild = leftchild;
    newnode->rightchild = rightchild;

    return newnode;
}

int TeXPrint(Tree* tree, char* output)
{
    FILE* out = fopen(output, "w");

    return NOERR;
}

int NodePrint(Node* node)
{
    assert(node != NULL);

    printf(" (");

    if (node->leftchild)
        NodePrint(node->leftchild);

    DataPrint(node, stdout);

    if (node->rightchild)
        NodePrint(node->rightchild);

    printf(" )");

    return NOERR;
}

int DataPrint(Node* node, FILE* out)
{
    switch (node->type)
    {
        case NUM_TYPE:
            fprintf(out, " %.0lf", node->val);
            break;
        case VAR_TYPE:
            fprintf(out, " %s", node->varvalue);
            break;
        case OP_TYPE:
            switch (node->optype)
            {
                case OP_ADD:
                    fprintf(out, " +");
                    break;
                case OP_SUB:
                    fprintf(out, " -");
                    break;
                case OP_MUL:
                    fprintf(out, " *");
                    break;
                case OP_DIV:
                    fprintf(out, " /");
                    break;
                case OP_COS:
                    fprintf(out, " cos");
                    break;
                case OP_SIN:
                    fprintf(out, " sin");
                    break;
            }
            break;
    }
}
