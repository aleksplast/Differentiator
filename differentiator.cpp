#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>

#include "TXLib.h"
#include "differentiator.h"

const int STRSIZE = 30;
const int CHARSTR = 2;
static bool CHANGE = true;
const double EPSILON = 1e-10;

int DifferentiatorMain(char* input)
{
    Tree datatree = DataParser(input);

    TreeGraphDump(&datatree, 0, __LINE__, __func__, __FILE__);

    Tree difftree = {};
    char difftreelog[STRSIZE] = "difftreelog.htm";
    TreeCtor(&difftree, UNKNOWN_TYPE, 0, OP_UNKNOWN, NULL, difftreelog);

    Node* newanchor = DiffNode(datatree.anchor);

    double x = 0;
    scanf("%lg", &x);

    difftree.anchor = newanchor;
    difftree.anchor->tree = &difftree;

    CreateAncestor(difftree.anchor, NULL, &difftree);
    SimplifyFunc(&difftree);

    TreeGraphDump(&difftree, 0, __LINE__, __func__, __FILE__);
    printf("func = %lg\n", CalculateNode(datatree.anchor, x));
    printf("derivative = %lg\n", CalculateNode(difftree.anchor, x));

//    NodePrint(difftree.anchor);

    FILE* out = fopen("out.txt", "w");
    TeXPrint(&datatree, &difftree, out);

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
    else if (strcmp(str, "^") == 0)
        return OP_POWER;
    else if (strcmp(str, "tg") == 0)
        return OP_TG;
    else if (strcmp(str, "log") == 0)
        return OP_LOG;
    else if (strcmp(str, "ln") == 0)
        return OP_LN;
    else if (strcmp(str, "ctg") == 0)
        return OP_CTG;
    else if (strcmp(str, "sqrt") == 0)
        return OP_SQRT;
    else if (strcmp(str, "arcsin") == 0)
        return OP_ARCSIN;
    else if (strcmp(str, "arccos") == 0)
        return OP_ARCCOS;
    else if (strcmp(str, "arctg") == 0)
        return OP_ARCTG;
    else if (strcmp(str, "arcctg") == 0)
        return OP_ARCCTG;
    else if (strcmp(str, "sh") == 0)
        return OP_SH;
    else if (strcmp(str, "ch") == 0)
        return OP_CH;

    return OP_UNKNOWN;
}

bool IsOneArg(Node* node)
{
    if (node->type == OP_TYPE)
    {
        if (node->optype == OP_COS || node->optype == OP_SIN || node->optype == OP_TG || node->optype == OP_CTG ||
            node->optype == OP_LN || node->optype == OP_SQRT || node->optype == OP_ARCSIN || node->optype == OP_ARCCOS
            || node->optype == OP_ARCTG || node->optype == OP_ARCCTG || node->optype == OP_CH || node->optype == OP_SH)
            return true;
        else
            return false;
    }
    else
        return false;

    return false;
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

Node* DiffNode(Node* node)
{
    DBG assert(node != NULL);

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
                    return DIV(SUB(MUL(DL, CR), MUL(CL, DR)), POWER(CR, CREATENUM(2)));
                case OP_SIN:
                    return MUL(COS(NULL, CR), DR);
                case OP_COS:
                    return MUL(MUL(SIN(NULL, CR), CREATENUM(-1)), DR);
                case OP_POWER:
                    return DiffPower(node);
                case OP_LOG:
                    return DiffLog(node);
                case OP_LN:
                    return MUL(DIV(CREATENUM(1), CR), DR);
                case OP_TG:
                    return MUL(DIV(CREATENUM(1), POWER(COS(NULL, CR), CREATENUM(2))), DR);
                case OP_CTG:
                    return MUL(MUL(CREATENUM(-1), DIV(CREATENUM(1), POWER(SIN(NULL, CR), CREATENUM(2)))), DR);
                case OP_SQRT:
                    return MUL(DIV(CREATENUM(1), MUL(CREATENUM(2), SQRT(NULL, CR))), DR);
                case OP_ARCSIN:
                    return MUL(DIV(CREATENUM(1), SQRT(NULL, SUB(CREATENUM(1), POWER(CR, CREATENUM(2))))), DR);
                case OP_ARCCOS:
                    return MUL(MUL(CREATENUM(-1), DIV(CREATENUM(1), SQRT(NULL, SUB(CREATENUM(1), POWER(CR, CREATENUM(2)))))), DR);
                case OP_ARCTG:
                    return MUL(DIV(CREATENUM(1), ADD(CREATENUM(1), POWER(CR, CREATENUM(2)))), DR);
                case OP_ARCCTG:
                    return MUL(MUL(CREATENUM(-1), DIV(CREATENUM(1), ADD(CREATENUM(1), POWER(CR, CREATENUM(2))))), DR);
                case OP_SH:
                    return MUL(CH(NULL, CR), DR);
                case OP_CH:
                    return MUL(SH(NULL, CR), DR);
                case OP_UNKNOWN:
                    return node;
                default:
                    return node;
            }
        case UNKNOWN_TYPE:
            break;
        default:
            break;
    }

    return node;
}

Node* DiffPower(Node* node)
{
    DBG assert(node != NULL);

    char var[CHARSTR] = "x";
    Node* searchright = NULL;
    Node* searchleft = NULL;

    if (node->leftchild)
        searchleft = TreeDepthSearch(node->leftchild, var);
    if (node->rightchild)
        searchright = TreeDepthSearch(node->rightchild, var);

    if (searchleft && searchright)
        return MUL(POWER(CREATEVAR(e), MUL(LN(NULL, CL), CR)), DiffNode(MUL(LN(NULL, CL), CR)));
    else if (searchleft)
        return MUL(MUL(CR, POWER(CL, SUB(CR, CREATENUM(1)))), DL);
    else if (searchright)
        return MUL(MUL(POWER(CL,CR), LN(NULL, CL)), DR);
    else
        return CREATENUM(0);

    return node;
}

Node* DiffLog(Node* node)
{
    DBG assert(node != NULL);

    char var[2] = "x";
    Node* searchleft = TreeDepthSearch(node->leftchild, var);
    Node* searchright = TreeDepthSearch(node->rightchild, var);

    if (searchleft && searchright)
        return DiffNode(DIV(LN(NULL, CR), LN(NULL, CL)));
    else if (searchleft)
        return DiffNode(DIV(LN(NULL, CR), LN(NULL, CL)));
    else if (searchright)
        return MUL(DIV(CREATENUM(1), MUL(LN(NULL, CL), CR)), DR);
    else
        return CREATENUM(0);
}

int CreateAncestor(Node* node, Node* ancestor, Tree* tree)
{
    DBG assert (node != NULL);
    DBG assert (node != NULL);

    node->tree = tree;

    if (node->leftchild)
        CreateAncestor(node->leftchild, node, tree);
    if (node->rightchild)
        CreateAncestor(node->rightchild, node, tree);

    node->ancestor = ancestor;

    return NOERR;
}

int SimplifyFunc(Tree* tree)
{
    DBG assert(tree != NULL);

    while (CHANGE)
    {
        SimplifyConstants(tree->anchor);

        DeleteMeaninglessNodes(tree->anchor);
    }

    return NOERR;
}

int SimplifyConstants(Node* node)
{
    DBG assert(node != NULL);

    CHANGE = false;

    if (node->leftchild)
        SimplifyConstants(node->leftchild);

    if (node->rightchild)
        SimplifyConstants(node->rightchild);

    SimplifyConstantNode(node);

    return NOERR;
}

int SimplifyConstantNode(Node* node)
{
    DBG assert(node != NULL);

    if (node->type != OP_TYPE)
        return NOERR;

    if (!node->leftchild)
        return NOERR;

    if (node->leftchild->type != NUM_TYPE || node->rightchild->type != NUM_TYPE)
        return NOERR;

    if (node->optype == OP_SUB)
    {
        node->type = NUM_TYPE;
        node->val = node->leftchild->val - node->rightchild->val;
        node->varvalue = NULL;
        node->optype = OP_UNKNOWN;
        node->leftchild = NULL;
        node->rightchild = NULL;
        CHANGE = true;
    }
    if (node->optype == OP_MUL)
    {
        node->type = NUM_TYPE;
        node->val = node->leftchild->val * node->rightchild->val;
        node->varvalue = NULL;
        node->optype = OP_UNKNOWN;
        node->leftchild = NULL;
        node->rightchild = NULL;
        CHANGE = true;
    }
    if (node->optype == OP_DIV)
    {
        node->type = NUM_TYPE;
        node->val = node->leftchild->val / node->rightchild->val;
        node->varvalue = NULL;
        node->optype = OP_UNKNOWN;
        node->leftchild = NULL;
        node->rightchild = NULL;
        CHANGE = true;
    }
    if (node->optype == OP_ADD)
    {
        node->type = NUM_TYPE;
        node->val = node->leftchild->val + node->rightchild->val;
        node->varvalue = NULL;
        node->optype = OP_UNKNOWN;
        node->leftchild = NULL;
        node->rightchild = NULL;
        CHANGE = true;
    }
    if (node->optype == OP_POWER)
    {
        node->type = NUM_TYPE;
        node->val = pow(node->leftchild->val, node->rightchild->val);
        node->varvalue = NULL;
        node->optype = OP_UNKNOWN;
        node->leftchild = NULL;
        node->rightchild = NULL;
        CHANGE = true;
    }
    return NOERR;
}

int DeleteMeaninglessNodes(Node* node)
{
    DBG assert(node != NULL);

    if (node->leftchild)
        DeleteMeaninglessNodes(node->leftchild);
    if (node->rightchild)
        DeleteMeaninglessNodes(node->rightchild);

    CheckNDeleteNode(node);

    return NOERR;
}

int CheckNDeleteNode(Node* node)
{
    DBG assert (node != NULL);

    if (node->type != OP_TYPE)
        return NOERR;
    if (!node->leftchild || !node->rightchild)
        return NOERR;

    if (node->rightchild->type == NUM_TYPE && (node->optype == OP_ADD || node->optype == OP_MUL))
    {
        Node* temp = node->rightchild;
        node->rightchild = node->leftchild;
        node->leftchild = temp;
    }

    if (IS_OP(OP_MUL) && IS_LVAL(1))
    {
        if (node == node->tree->anchor)
            DeleteAnchor(node);

        else if (node == node->ancestor->leftchild)
        {
            node->ancestor->leftchild = node->rightchild;
            node->rightchild->ancestor = node->ancestor;
            free(node->leftchild);
            free(node);
        }
        else if (node == node->ancestor->rightchild)
        {
            node->ancestor->rightchild = node->rightchild;
            node->rightchild->ancestor = node->ancestor;
            free(node->leftchild);
            free(node);
        }
        CHANGE = true;
    }

    if (IS_OP(OP_MUL) && IS_LVAL(0))
    {
        if (node == node->tree->anchor)
        {
            node->type = NUM_TYPE;
            node->val = 0;
            node->varvalue = NULL;
            node->optype = OP_UNKNOWN;
            node->leftchild = NULL;
            node->rightchild = NULL;
            node->ancestor = NULL;
            node->tree->anchor = node;
        }
        else
        {
            node->type = NUM_TYPE;
            node->val = 0;
            node->varvalue = NULL;
            node->optype = OP_UNKNOWN;
            node->leftchild = NULL;
            node->rightchild = NULL;
        }
        CHANGE = true;
    }

    if (IS_OP(OP_ADD) && IS_LVAL(0))
    {
        if (node == node->tree->anchor)
            DeleteAnchor(node);

        else if (node == node->ancestor->leftchild)
        {
            node->ancestor->leftchild = node->rightchild;
            node->rightchild->ancestor = node->ancestor;
            free(node->leftchild);
            free(node);
        }
        else if (node == node->ancestor->rightchild)
        {
            node->ancestor->rightchild = node->rightchild;
            node->rightchild->ancestor = node->ancestor;
            free(node->leftchild);
            free(node);
        }
        CHANGE = true;
    }

    if (IS_OP(OP_SUB) && IS_LVAL(0))
    {
        if (node == node->tree->anchor)
            DeleteAnchor(node);

        else if (node == node->ancestor->leftchild)
        {
            node->ancestor->leftchild = node->rightchild;
            node->rightchild->ancestor = node->ancestor;
            free(node->leftchild);
            free(node);
        }
        else if (node == node->ancestor->rightchild)
        {
            node->ancestor->rightchild = node->rightchild;
            node->rightchild->ancestor = node->ancestor;
            free(node->leftchild);
            free(node);
        }
        CHANGE = true;
    }

    if (IS_OP(OP_POWER) && IS_RVAL(0))
    {
        node->type = NUM_TYPE;
        node->val = 1;
        node->varvalue = NULL;
        node->optype = OP_UNKNOWN;
        node->leftchild = NULL;
        node->rightchild = NULL;

        if (node == node->tree->anchor)
            node->ancestor = NULL;

        CHANGE = true;
    }

    if (IS_OP(OP_POWER) && IS_RVAL(1))
    {
        if (node == node->tree->anchor)
        {
            node->tree->anchor = node->leftchild;
            free(node->rightchild);
            free(node);
        }

        else if (node == node->ancestor->leftchild)
        {
            node->ancestor->leftchild = node->leftchild;
            node->leftchild->ancestor = node->ancestor;
            free(node->rightchild);
            free(node);
        }
        else if (node == node->ancestor->rightchild)
        {
            node->ancestor->rightchild = node->leftchild;
            node->leftchild->ancestor = node->ancestor;
            free(node->rightchild);
            free(node);
        }
        CHANGE = true;
    }

    return NOERR;
}

int DeleteAnchor(Node* node)
{
    DBG assert(node != NULL);

    node->tree->anchor = node->rightchild;
    free(node);
    free(node->leftchild);

    return NOERR;
}

double CalculateNode(Node* node, double x)
{
    DBG assert(node != NULL);

    switch (node->type)
    {
        case NUM_TYPE:
            return node->val;
        case VAR_TYPE:
            switch(*(node->varvalue))
            {
                case 'x':
                    return x;
                case 'e':
                    return 1e1;
                default:
                    return 0;
            }
        case OP_TYPE:
            switch (node->optype)
            {
                case OP_ADD:
                    return CalculateNode(node->leftchild, x) + CalculateNode(node->rightchild,x);
                case OP_MUL:
                    return CalculateNode(node->leftchild, x) * CalculateNode(node->rightchild,x);
                case OP_DIV:
                    return CalculateNode(node->leftchild, x) / CalculateNode(node->rightchild,x);
                case OP_SUB:
                    return CalculateNode(node->leftchild, x) - CalculateNode(node->rightchild,x);
                case OP_SIN:
                    return sin(CalculateNode(node->rightchild, x));
                case OP_COS:
                    return cos(CalculateNode(node->rightchild, x));
                case OP_TG:
                    return tan(CalculateNode(node->rightchild,x));
                case OP_POWER:
                    return pow(CalculateNode(node->leftchild, x), CalculateNode(node->rightchild,x));
                case OP_LOG:
                    return (log(CalculateNode(node->rightchild, x)))/(log(CalculateNode(node->leftchild,x)));
                case OP_LN:
                    return log(CalculateNode(node->rightchild,x));
                case OP_CTG:
                    return 1/(tan(CalculateNode(node->rightchild,x)));
                case OP_SQRT:
                    return sqrt(CalculateNode(node->rightchild,x));
                case OP_ARCCOS:
                    return acos(CalculateNode(node->rightchild,x));
                case OP_ARCSIN:
                    return asin(CalculateNode(node->rightchild,x));
                case OP_ARCTG:
                    return atan(CalculateNode(node->rightchild,x));
                case OP_ARCCTG:
                    return txPI - atan(CalculateNode(node->rightchild,x));
                case OP_SH:
                    return sinh(CalculateNode(node->rightchild,x));
                case OP_CH:
                    return cosh(CalculateNode(node->rightchild,x));
                case OP_UNKNOWN:
                    return 0;
                default:
                    return 0;
            }
        case UNKNOWN_TYPE:
            return 0;
        default:
            return 0;
    }
}

int TeXPrint(Tree* orig, Tree* difftree, FILE* out)
{
    PrepareTeXFile(out);

    fprintf(out, "$$");
    TeXNodePrint(orig->anchor, out);
    fprintf(out, "$$\n");

    fprintf(out, "$$");
    TeXNodePrint(difftree->anchor, out);
    fprintf(out, "$$");

    return NOERR;
}

int TeXNodePrint(Node* node, FILE* out)
{
    TeXDataStartPrint(node, out);
//    if (node->optype != OP_SIN && node->optype != OP_COS && node->optype != OP_UNKNOWN && !node->tree)
//        fprintf(out, "(");

    if (node->leftchild && !IsOneArg(node))
        TeXNodePrint(node->leftchild, out);

    TeXDataPrint(node, out);

    if (node->rightchild)
        TeXNodePrint(node->rightchild, out);

    TeXDataEndPrint(node, out);

    return NOERR;
}

int TeXDataStartPrint(Node* node, FILE* out)
{
    if (node == node->tree->anchor && node->optype != OP_DIV)
        return NOERR;

    switch (node->type)
    {
        case NUM_TYPE:
            break;
        case VAR_TYPE:
            break;
        case OP_TYPE:
        {
            if (node->optype == OP_COS)
                break;
            else if (node->optype == OP_SIN)
                break;
            else if (node->optype == OP_POWER)
                break;
            else if (node->optype == OP_LOG)
                break;
            else if (node->optype == OP_TG)
                break;
            else if (node->optype == OP_CTG)
                break;
            else if (node->optype == OP_DIV)
                fprintf(out, "\\frac{");
            else if (node->optype == OP_LN)
                break;
            else if (node->optype == OP_SQRT)
                break;
            else if (node->optype == OP_ARCSIN)
                break;
            else if (node->optype == OP_ARCCOS)
                break;
            else if (node->optype == OP_ARCTG)
                break;
            else if (node->optype == OP_ARCCTG)
                break;
            else if (node->optype == OP_SH)
                break;
            else if (node->optype == OP_CH)
                break;
            else
                fprintf(out, "(");
            break;
        }
        case UNKNOWN_TYPE:
            break;
        default:
            break;
    }

    return NOERR;
}

int TeXDataPrint(Node* node, FILE* out)
{
    switch (node->type)
    {
        case NUM_TYPE:
            fprintf(out, "{%.0lf}", node->val);
            break;
        case VAR_TYPE:
            fprintf(out, "{%s}", node->varvalue);
            break;
        case OP_TYPE:
            switch (node->optype)
            {
                case OP_ADD:
                    fprintf(out, "+");
                    break;
                case OP_SUB:
                    fprintf(out, "-");
                    break;
                case OP_MUL:
                    fprintf(out, "\\cdot");
                    break;
                case OP_DIV:
                    fprintf(out, "}{");
                    break;
                case OP_COS:
                    fprintf(out, "\\cos{");
                    break;
                case OP_SIN:
                    fprintf(out, "\\sin{");
                    break;
                case OP_POWER:
                    fprintf(out, "^{");
                    break;
                case OP_LN:
                    fprintf(out, "\\ln{");
                    break;
                case OP_LOG:
                    fprintf(out, "\\log{");
                    break;
                case OP_TG:
                    fprintf(out, "\\tg{");
                    break;
                case OP_CTG:
                    fprintf(out, "\\ctg{");
                    break;
                case OP_SQRT:
                    fprintf(out, "\\sqrt{");
                    break;
                case OP_ARCSIN:
                    fprintf(out, "\\arcsin{");
                    break;
                case OP_ARCCOS:
                    fprintf(out, "\\arccos{");
                    break;
                case OP_ARCTG:
                    fprintf(out, "\\arctg{");
                    break;
                case OP_ARCCTG:
                    fprintf(out, "\\arcctg{");
                    break;
                case OP_SH:
                    fprintf(out, "\\sinh{");
                    break;
                case OP_CH:
                    fprintf(out, "\\cosh{");
                    break;
                case OP_UNKNOWN:
                    break;
                default:
                    break;
            }
            break;
        case UNKNOWN_TYPE:
            break;
        default:
            break;
    }

    return NOERR;
}

int TeXDataEndPrint(Node* node, FILE* out)
{
    if (node == node->tree->anchor && !IsOneArg(node))
        return NOERR;

    switch (node->type)
    {
        case NUM_TYPE:
            break;
        case VAR_TYPE:
            break;
        case OP_TYPE:
        {
            if (node->optype == OP_COS)
                fprintf(out, "}");
            else if (node->optype == OP_SIN)
                fprintf(out, "}");
            else if (node->optype == OP_POWER)
                fprintf(out, "}");
            else if (node->optype == OP_TG)
                fprintf(out, "}");
            else if (node->optype == OP_CTG)
                fprintf(out, "}");
            else if (node->optype == OP_LN)
                fprintf(out, "}");
            else if (node->optype == OP_DIV)
                fprintf(out, "}");
            else if (node->optype == OP_SQRT)
                fprintf(out, "}");
            else if (node->optype == OP_ARCSIN)
                fprintf(out, "}");
            else if (node->optype == OP_ARCCOS)
                fprintf(out, "}");
            else if (node->optype == OP_ARCTG)
                fprintf(out, "}");
            else if (node->optype == OP_ARCCTG)
                fprintf(out, "}");
            else if (node->optype == OP_SH)
                fprintf(out, "}");
            else if (node->optype == OP_CH)
                fprintf(out, "}");
            else if (node->optype == OP_LOG)
                fprintf(out, "}");
            else
                fprintf(out, ")");
            break;
        }
        case UNKNOWN_TYPE:
            break;
        default:
            break;
    }

    return NOERR;
}

int PrepareTeXFile(FILE* out)
{
    fprintf(out,    "\\documentclass[a4paper,12pt]{article}                    \n"
                    "\\usepackage[a4paper,top=1.3cm,bottom=2cm,left=1.5cm,right=1.5cm,marginparwidth=0.75cm]{geometry}\n"
                    "\\usepackage{cmap}                                                          \n"
                    "\\usepackage{mathtext}                                                      \n"
                    "\\usepackage[T2A]{fontenc}                                                  \n"
                    "\\usepackage[utf8]{inputenc}                                                \n"
                    "\\usepackage[english,russian]{babel}                                        \n"
                    "\\usepackage{multirow}                                                      \n"
                    "                                                                            \n"
                    "\\usepackage{graphicx}                                                      \n"
                    "                                                                            \n"
                    "\\usepackage{wrapfig}                                                       \n"
                    "\\usepackage{tabularx}                                                      \n"
                    "                                                                            \n"
                    "\\usepackage{hyperref}                                                      \n"
                    "\\usepackage[rgb]{xcolor}                                                   \n"
                    "\\usepackage{amsmath,amsfonts,amssymb,amsthm,mathtools}                     \n"
                    "                                                                            \n"
                    "\\mathtoolsset{showonlyrefs=true}                                           \n"
                    "                                                                            \n"
                    "\\usepackage{euscript}	                                                     \n"
                    "\\usepackage{mathrsfs}                                                      \n"
                    "                                                                            \n"
                    "\\DeclareMathOperator{\\sgn}{\\mathop{sgn}}                                 \n"
                    "                                                                            \n"
                    "\\newcommand*{\\hm}[1]{#1\nobreak\\discretionary{}                          \n"
                    "{\\hbox{$\\mathsurround=0pt #1$}}{}}                                        \n"
                    "                                                                            \n"
                    "\\date{\\today}                                                             \n"
                    "                                                                            \n"
                    "\\begin{document}                                                           \n"
                    "                                                                            \n"
                    "\\begin{titlepage}                                                          \n"
                    "	\\begin{center}                                                         \n"
                    "		{                                                                   \n"
                    "        \\large ���������� ������-����������� ��������}                     \n"
                    "	\\end{center}                                                           \n"
                    "	\\begin{center}                                                         \n"
                    "		{\\large ������-����� ������������ � ������������ ����������}       \n"
                    "	\\end{center}                                                           \n"
                    "                                                                            \n"
                    "                                                                            \n"
                    "	\\vspace{4.5cm}                                                         \n"
                    "	{\\huge                                                                 \n"
                    "		\\begin{center}                                                     \n"
                    "			{\\bf ����������������� ������� �� ������� ���������}\\         \n"
                    "		\\end{center}                                                       \n"
                    "	}                                                                       \n"
                    "	\\vspace{2cm}                                                           \n"
                    "	\\begin{flushright}                                                     \n"
                    "		{\\LARGE �����:\\\\ ����� �������� ���������� ���������� \\\\       \n"
                    "			\\vspace{0.2cm}}                                                \n"
                    "	\\end{flushright}                                                       \n"
                    "	\\vspace{8cm}                                                           \n"
                    "	\\begin{center}                                                         \n"
                    "		������������ 2022                                                   \n"
                    "	\\end{center}                                                           \n"
                    "\\end{titlepage}                                                            \n"
                    "\n");

    fprintf(out, "\\section{��������}\n"
            "������ �����������.\n\n"
            "��� ����� ���� ����� ����� �������� �� ������� ����� ����.\n\n"
            "�� � ���������� ������������ � ��������� �������� �����, ����������� ����������� ��� ������������ �������.\n\n"
            "�� � ��������, ���������� ����� ����� � ������� ������������ ������� ������, � ���� ����� ��������� ���������� � ������ ������ ���. � ������� ������� ������������ ������� �� ������� ��-����������.\n\n"
            "���� � ����� �������� ��������� ����� ���������� ������, ��� ������� �������� �� ����.\n\n"
            "������� ������ ���������� ���������� ������� �������� ������� ���, � ���� ���� ������� ��������, ������ ����������� �������� ���� ������.\n\n"
            "�������� ����� ��������� �� ��� ��� �� ������������ ����� ��������.\n\n"
            "���������� ����� ��� ������ � ������� ������, �������������� ����������, ����������� ���������� ����������.\n\n"
            "� ��� ����� ��������� �� ������: ������������ ������� � ������������ ���� ������������ �������, ����� ���������� ������� � ���������� ������� ������������.\n\n"
            "��, �������� �� ��� ��������, �� ��� ���� �������, ����� ���������� �������� ������ �� ������� �������, ������� � ��������.\n\n"
            "���� �������� � ����� ����� � ������ ���� ����� �� �����.\n\n"
            "��� ������ ���� ��� ����� �������� � �������� ������, ������� ������ ����� �����������.\n\n"
            "�������� � ����������� �������� � �����, ��� ������ ������ � ������� �� ����� ������� ������.\n\n"
            "�������� � ������������, ��������� ����������, � ���������������, ��� �� ����� ���� ���� ������ �����.\n\n"
            "��� ���� ����� ��������� � ���� ������ ����� � �������������, �� ���� �������� ������.\n\n");
    fprintf(out,    "\\section{�� ���������� �������}\n");
    return NOERR;
}

int DataPrint(Node* node, FILE* out)
{
    switch (node->type)
    {
        case NUM_TYPE:
            fprintf(out, "{%.0lf}", node->val);
            break;
        case VAR_TYPE:
            fprintf(out, "{%s}", node->varvalue);
            break;
        case OP_TYPE:
            switch (node->optype)
            {
                case OP_ADD:
                    fprintf(out, "+");
                    break;
                case OP_SUB:
                    fprintf(out, "-");
                    break;
                case OP_MUL:
                    fprintf(out, "*");
                    break;
                case OP_DIV:
                    fprintf(out, "/");
                    break;
                case OP_COS:
                    fprintf(out, "cos");
                    break;
                case OP_SIN:
                    fprintf(out, "sin");
                    break;
                case OP_POWER:
                    fprintf(out, "^");
                    break;
                case OP_TG:
                    fprintf(out, "tg");
                    break;
                case OP_CTG:
                    fprintf(out, "ctg");
                    break;
                case OP_LN:
                    fprintf(out, "ln");
                    break;
                case OP_LOG:
                    fprintf(out, "log");
                    break;
                case OP_UNKNOWN:
                    fprintf(out, "UNKNOWN OP\n");
                    break;
                default:
                    fprintf(out, "UNKNOWN OP\n");
                    break;
            }
            break;
        case UNKNOWN_TYPE:
            break;
        default:
            break;
    }

    return NOERR;
}

int NodePrint(Node* node)
{
    assert(node != NULL);

    printf("(");

    if (node->leftchild)
        NodePrint(node->leftchild);

    DataPrint(node, stdout);

    if (node->rightchild)
        NodePrint(node->rightchild);

    printf(")");

    return NOERR;
}

int compare(const double a, const double b)
{
    DBG assert(isfinite(a));
    DBG assert(isfinite(b));

    if (fabs(a-b) < EPSILON)
        return 0;
    if ((a-b) > EPSILON)
        return 1;
    if ((a-b) < EPSILON)
        return -1;

    return NOERR;
}
