#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <assert.h>

#include "diftree.h"
#include "common.h"

const char* Poison = NULL;

#define TREECHECK   if (int errors = TreeVerr(tree))                                         \
                        DBG TreeGraphDump(tree, errors, __LINE__, __func__, __FILE__);

static int Piccounter = 1;

int TreeCtor(Tree* tree, NodeType type, double val, OperType optype, char* varvalue, char* graphfile)
{
    DBG assert(tree != NULL);

    if ((tree->anchor = (Node*) calloc(1, sizeof(Node))) == NULL)
        return MEMERR;

    tree->anchor->type = type;
    tree->anchor->val = val;
    tree->anchor->optype = optype;
    tree->anchor->varvalue = varvalue;

    tree->anchor->ancestor = NULL;
    tree->anchor->rightchild = NULL;
    tree->anchor->leftchild = NULL;
    tree->anchor->tree = tree;

    tree->size = 1;

    tree->graphlog = graphfile;

    FILE* fp = fopen(tree->graphlog, "w");
    fclose(fp);

    TREECHECK

    return NOERR;
}

int TreeVerr(Tree* tree)
{
    DBG assert(tree != NULL);

    int errors = 0;

    if (tree->size < 0)
        errors += SIZEERR;

    return errors;
}

int NodeVerr (Node* node, int* errors)
{
    DBG assert(node != NULL);

    if (node->leftchild)
        *errors += NodeVerr(node->leftchild, errors);
    if (node->rightchild)
        *errors += NodeVerr(node->rightchild, errors);

    return 0;
}

int TreeGraphDump (Tree* tree, int errors, int line, const char* func, const char* file)
{
    DBG assert(tree != NULL);

    char picture[15] = "picture";
    sprintf(picture, "%d.dot", Piccounter);
    char picturesrc[15] = "picture";
    sprintf(picturesrc, "%d.png", Piccounter);

    FILE* pic = fopen(picture, "w");

    fprintf(pic, "strict graph {\n");
    fprintf(pic, "\trankdir = TB\n");
    fprintf(pic, "\t\"info\" [shape = \"record\", style = \"filled\", fillcolor = \"grey\", label = \"{size = %d|anchor = %p}\"]\n", tree->size, tree->anchor);

    int counter = 1;
    NodeDump(tree->anchor, &counter, pic);

    fprintf(pic, "}");

    fclose(pic);
    Piccounter += 1;

    char src[80] = "";
    sprintf(src, "dot -Tpng %s -o %s", picture, picturesrc);

    system(src);

    FILE* graphlog = fopen(tree->graphlog, "a");

    fprintf(graphlog, "<pre>\n");

    fprintf(graphlog, "<h2>TreeDump on %d line in %s in %s</h2>\n", line, func, file);
    fprintf(graphlog, "<img src = \"%s\">\n", picturesrc);

    fprintf(graphlog, "<hr>\n");

    fclose(graphlog);

    return NOERR;
}

int NodeDump(Node* node, int* counter, FILE* pic)
{
    int left = 0;
    int right = 0;
    if (node->leftchild)
        left = NodeDump(node->leftchild, counter, pic);
    if (node->rightchild)
        right = NodeDump(node->rightchild, counter, pic);

    fprintf(pic, "\t\"node%d\" [shape = \"record\", style = \"filled\", fillcolor = \"green\", label = \"{",*counter);
    TypePrint(pic, node->type);
    fprintf(pic, "|");
    if (node->optype)
    {
        OpTypePrint(pic, node->optype);
    }
    else if (node->val)
        fprintf(pic, "%lg", node->val);
    else if (node->varvalue)
        fprintf(pic, "%s", node->varvalue);
    else
        fprintf(pic, "%.0lf", 0);
    fprintf(pic, "}\"]\n");
    int curr = *counter;
    if (node->leftchild)
        fprintf(pic, "\t\"node%d\" -- \"node%d\"\n", curr, left);
    if (node->rightchild)
        fprintf(pic, "\t\"node%d\" -- \"node%d\"\n", curr, right);

    (*counter)++;

    return *counter - 1;
}

Node* AddRightChild(Tree* tree, Node* ancestor)
{
    DBG assert(ancestor != NULL);

    if (ancestor->rightchild)
        return ancestor;

    if ((ancestor->rightchild = (Node*) calloc(1, sizeof(Node))) == NULL)
        return ancestor;

    tree->size++;

    ancestor->rightchild->type = UNKNOWN_TYPE;
    ancestor->rightchild->val = 0;
    ancestor->rightchild->optype = OP_UNKNOWN;
    ancestor->rightchild->varvalue = NULL;

    ancestor->rightchild->leftchild = NULL;
    ancestor->rightchild->rightchild = NULL;
    ancestor->rightchild->ancestor = ancestor;
    ancestor->rightchild->tree = ancestor->tree;

    TREECHECK

    return ancestor->rightchild;
}

Node* AddLeftChild(Tree* tree, Node* ancestor)
{
    DBG assert(ancestor != NULL);

    if (ancestor->leftchild)
        return ancestor;

    if ((ancestor->leftchild = (Node*) calloc(1, sizeof(Node))) == NULL)
        return ancestor;

    tree->size++;

    ancestor->leftchild->type = UNKNOWN_TYPE;
    ancestor->leftchild->val = 0;
    ancestor->leftchild->optype = OP_UNKNOWN;
    ancestor->leftchild->varvalue = NULL;

    ancestor->leftchild->leftchild = NULL;
    ancestor->leftchild->rightchild = NULL;
    ancestor->leftchild->ancestor = ancestor;
    ancestor->leftchild->tree = ancestor->tree;

    TREECHECK

    return ancestor->leftchild;
}

int NodeDetor(Tree* tree, Node* node)
{
    DBG assert (node != NULL);

    if (node->leftchild)
        NodeDetor(tree, node->leftchild);

    else if (node->rightchild)
        NodeDetor(tree, node->leftchild);

    tree->size--;

    free(node);

    return NOERR;
}

Node* TreeDepthSearch(Node* node, char* searchvar)
{
    assert(node != NULL);

    Node* searchleft = NULL;
    Node* searchright = NULL;

    if (node->leftchild)
        searchleft = TreeDepthSearch(node->leftchild, searchvar);
    if (node->rightchild)
        searchright = TreeDepthSearch(node->rightchild, searchvar);

    if (stricmp(node->varvalue, searchvar) == 0)
        return node;
    else if (searchright)
        return searchright;
    else if (searchleft)
        return searchleft;
    else
        return NULL;
}

int TreeDetor(Tree* tree)
{
    DBG assert (tree != NULL);

    TREECHECK

    NodeDetor(tree, tree->anchor);

    tree->size = -1;
    tree->graphlog = NULL;
    tree = NULL;

    return NOERR;
}

int TypePrint(FILE* fp, NodeType type)
{
    if (type == UNKNOWN_TYPE)
        fprintf(fp, "%s", "UNKNOWN");
    if (type == VAR_TYPE)
        fprintf(fp, "%s", "VAR");
    if (type == OP_TYPE)
        fprintf(fp, "%s", "OP");
    if (type == NUM_TYPE)
        fprintf(fp, "%s", "NUM");

    return NOERR;
}

int OpTypePrint(FILE* fp, OperType opertype)
{
    if (opertype == OP_UNKNOWN)
        fprintf(fp, "%s", "UNKNOWN");
    else if (opertype == OP_ADD)
        fprintf(fp, "%s", "ADD");
    else if (opertype == OP_MUL)
        fprintf(fp, "%s", "MUL");
    else if (opertype == OP_SUB)
        fprintf(fp, "%s", "SUB");
    else if (opertype == OP_DIV)
        fprintf(fp, "%s", "DIV");
    else if (opertype == OP_SIN)
        fprintf(fp, "%s", "SIN");
    else if (opertype == OP_COS)
        fprintf(fp, "%s", "COS");
    else if (opertype == OP_POWER)
        fprintf(fp, "%s", "^");
    else if (opertype == OP_LOG)
        fprintf(fp, "%s", "LOG");
    else if (opertype == OP_LN)
        fprintf(fp, "%s", "LN");
    else if (opertype == OP_TG)
        fprintf(fp, "%s", "TG");
    else if (opertype == OP_CTG)
        fprintf(fp, "%s", "CTG");
    else if (opertype == OP_SQRT)
        fprintf(fp, "%s", "SQRT");
    else if (opertype == OP_ARCSIN)
        fprintf(fp, "%s", "ARCSIN");
    else if (opertype == OP_ARCCOS)
        fprintf(fp, "%s", "ARCCOS");
    else if (opertype == OP_ARCTG)
        fprintf(fp, "%s", "ARCTG");
    else if (opertype == OP_ARCCTG)
        fprintf(fp, "%s", "ARCCTG");
    else if (opertype == OP_SH)
        fprintf(fp, "%s", "SH");
    else if (opertype == OP_CH)
        fprintf(fp, "%s", "CH");

    return NOERR;
}
