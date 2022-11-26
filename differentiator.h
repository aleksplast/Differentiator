#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include "diftree.h"
#include "text-sort.h"
#include "common.h"
#include "dsl.h"
#include "recursivedescent.h"

enum Side
{
    LEFT,
    RIGHT,
};

struct Variable
{
    char name[20];
    double val;
    double inaccuracy;
};

typedef poem Phrases;

/////// MAIN /////////////

int DifferentiatorMain(char* input);

Tree DiffFuncOnce(Tree* datatree, char* var, Phrases* diffphrases, Phrases* simpphrases, FILE* out);

int MaclaurinSeries(Tree* datatree, Tree* difftree, char* var, Phrases* diffphrases, Phrases* simpphrases, FILE* out);

int TangentEquation(Tree* datatree, Tree* difftree, double x, FILE* out);

double FullInaccuracy(Tree* datatree, Variable* variables, long varsize, Phrases* diffphrases, Phrases* simpphrases, FILE* out);

int CreateGraphic(Tree* datatree, double left, double right, FILE* out);

/////////////////////////



////// Parser ////////////

Tree DataParser(char* inputdata);

int GetPhrases(Phrases* phrases, char* input);

Variable* GetParameters(Phrases* phrases, char* input);

int FillCurrNode(Node* currnode, char* input);

//////////////////////////



////// Differentiation //////

Node* DiffNode(Node* node, Phrases* phrases, char* var, FILE* out);

Node* DiffPower(Node* node, Phrases* phrases, char* var, FILE* out);

Node* DiffLog(Node* node, Phrases* phrases, char* var, FILE* out);

/////////////////////////////



////// Simplyfying //////////

int SimplifyConstantNode(Node* node, Phrases* phrases, FILE* out);

int SimplifyConstants(Node* node, Phrases* phrases, FILE* out);

int SimplifyFunc(Tree* tree, Phrases* phrases, FILE* out);

int DeleteMeaninglessNodes(Node* node, Phrases* phrases, FILE* out);

int CheckNDeleteNode(Node* node, Phrases* phrases, FILE* out);

int ChangeNodeIntoNum(Node* node, double val);

int ChangeIntoChild(Node* node, Side side);

int DeleteAnchor (Node* node, Side side);

/////////////////////////////



/////// Calculating /////////

int PrepareCalcNode(Node* node, Variable* variables, long varsize);

double CalculateNode(Node* node, double x);

int CountSubTreeSize(Node* node, int* size);

int SubTreeSize(Node* node);

/////////////////////////////



/////// Printing ////////////
int Priority(Node* node);

int TeXPrint(Node* node, char* repl, FILE* out);

int PrepareTeXFile(FILE* out);

int TeXNodePrint(Node* startnode, Node* node, FILE* out);

int TeXDataStartPrint(Node* startnode, Node* node, FILE* out);

int TeXDataEndPrint(Node* startnode, Node* node, FILE* out);

int TeXDataPrint(Node* node, FILE* out);

int TeXClose(FILE* out);

int DataPrint(Node* node, FILE* out);

int NodePrint(Node* node);

///////////////////////////////



/////// Recursive descent
Node* GetG(const char* str, const int var);

Node* GetN();

Node* GetE();

Node* GetT();

Node* GetP();

Node* GetV();

bool GetM();

/////////////////////////////////



//////// Another functions ////////

Node* CreateNode(NodeType type, double val, OperType optype, char* varvalue, Tree* tree, Node* ancestor, Node* leftchild, Node* rightchild);

int Factorial(int n);

OperType IsOper(char* str);

double IsVal(char* str);

bool IsOneArg(Node* node);

Node* NodeCopy(Node* node);

int CreateAncestor(Node* node, Node* ancestor, Tree* tree);

int compare(const double a, const double b);

#endif //DIFFERENTIATOR_H
