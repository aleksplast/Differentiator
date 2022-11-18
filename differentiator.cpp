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
const double PI = 3.1415926589;
const double E = 2.7182818284;

int DifferentiatorMain(char* input)
{
    Tree datatree = DataParser(input);

    TreeGraphDump(&datatree, 0, __LINE__, __func__, __FILE__);

    Tree difftree = {};
    char difftreelog[STRSIZE] = "difftreelog.htm";
    TreeCtor(&difftree, UNKNOWN_TYPE, 0, OP_UNKNOWN, NULL, difftreelog);

    Node* newanchor = DiffNode(datatree.anchor);

    difftree.anchor = newanchor;
    difftree.anchor->tree = &difftree;

    TreeGraphDump(&difftree, 0, __LINE__, __func__, __FILE__);

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

    return OP_UNKNOWN;
}

bool IsOneArg(Node* node)
{
    if (node->type == OP_TYPE)
    {
        if (node->optype == OP_COS || node->optype == OP_SIN || node->optype == OP_TG || node->optype == OP_CTG || node->optype == OP_LN)
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
    if (node->tree && node->optype != OP_DIV)
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
                    fprintf(out, "\\cos(");
                    break;
                case OP_SIN:
                    fprintf(out, "\\sin(");
                    break;
                case OP_POWER:
                    fprintf(out, "^{");
                    break;
                case OP_LN:
                    fprintf(out, "\\ln(");
                    break;
                case OP_LOG:
                    fprintf(out, "\\log(");
                    break;
                case OP_TG:
                    fprintf(out, "\\tg(");
                    break;
                case OP_CTG:
                    fprintf(out, "\\ctg(");
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
    if (node->tree && node->optype != OP_DIV)
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
                fprintf(out, ")");
            else if (node->optype == OP_SIN)
                fprintf(out, ")");
            else if (node->optype == OP_POWER)
                fprintf(out, "}");
            else if (node->optype == OP_TG)
                fprintf(out, ")");
            else if (node->optype == OP_LN)
                fprintf(out, ")");
            else if (node->optype == OP_DIV)
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
                    "        \\large МОСКОВСКИЙ ФИЗИКО-ТЕХНИЧЕСКИЙ ИНСТИТУТ}                     \n"
                    "	\\end{center}                                                           \n"
                    "	\\begin{center}                                                         \n"
                    "		{\\large Физтех-школа радиотехники и компьютерных технологий}       \n"
                    "	\\end{center}                                                           \n"
                    "                                                                            \n"
                    "                                                                            \n"
                    "	\\vspace{4.5cm}                                                         \n"
                    "	{\\huge                                                                 \n"
                    "		\\begin{center}                                                     \n"
                    "			{\\bf Дифференцирование функции по заветам Механикус}\\         \n"
                    "		\\end{center}                                                       \n"
                    "	}                                                                       \n"
                    "	\\vspace{2cm}                                                           \n"
                    "	\\begin{flushright}                                                     \n"
                    "		{\\LARGE Автор:\\\\ Магос Техникус Александус Пластиниус \\\\       \n"
                    "			\\vspace{0.2cm}}                                                \n"
                    "	\\end{flushright}                                                       \n"
                    "	\\vspace{8cm}                                                           \n"
                    "	\\begin{center}                                                         \n"
                    "		Долгопрудный 2022                                                   \n"
                    "	\\end{center}                                                           \n"
                    "\\end{titlepage}                                                            \n"
                    "\n");

    fprintf(out, "\\section{Введение}\n"
            "Второе тысячелетие.\n\n"
            "Уже более трех веков Матан недвижим на Золотом Троне Наук.\n\n"
            "Он — Повелитель Человечества и властелин мириадов людей, завоеванных могуществом Его неисчислимых функций.\n\n"
            "Он — полутруп, неуловимую искру жизни в котором поддерживают древние знания, и ради этого ежедневно приносится в жертву тысяча душ. И поэтому Владыка Человечества никогда не умирает по-настоящему.\n\n"
            "Даже в своем нынешнем состоянии Матан продолжает миссию, для которой появился на свет.\n\n"
            "Могучие боевые математики пересекают кишащий демонами обычный мир, и путь этот освещен Физтехом, зримым проявлением духовной воли Матана.\n\n"
            "Огромные армии сражаются во имя Его на бесчисленных полях тетрадей.\n\n"
            "Величайшие среди его солдат — преподы матана, математические десантники, генетически улучшенные супервоины.\n\n"
            "У них много товарищей по оружию: Аспирантская Гвардия и бесчисленные Силы Студенческой Обороны, вечно бдительный Деканат и техножрецы Адептус Информатикус.\n\n"
            "Но, несмотря на все старания, их сил едва хватает, чтобы сдерживать извечную угрозу со стороны физиков, химиков и биологов.\n\n"
            "Быть физтехом в такое время — значит быть одним из тысяч.\n\n"
            "Это значит жить при самом жестоком и кровавом режиме, который только можно представить.\n\n"
            "Забудьте о достижениях общесоса и проги, ибо многое забыто и никогда не будет открыто заново.\n\n"
            "Забудьте о перспективах, обещанных прогрессом, о взаимопонимании, ибо во мраке МФТИ есть только война.\n\n"
            "Нет мира среди аудиторий — лишь вечная бойня и кровопролитие, да смех жаждущих фопфов.\n\n");
    fprintf(out,    "\\section{Ее величество функция}\n");
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
