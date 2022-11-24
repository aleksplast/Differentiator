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

typedef poem Phrases;

int DifferentiatorMain(char* input);

Tree DataParser(char* inputdata);

int GetPhrases(Phrases* phrases, char* input);

int FillCurrNode(Node* currnode, char* input);

Node* NodeCopy(Node* node);

int CreateAncestor(Node* node, Node* ancestor, Tree* tree);

Node* DiffNode(Node* node, Phrases* phrases, FILE* out);

Node* DiffPower(Node* node, Phrases* phrases, FILE* out);

Node* DiffLog(Node* node, Phrases* phrases, FILE* out);

int SimplifyConstantNode(Node* node);

int SimplifyConstants(Node* node);

int SimplifyFunc(Tree* tree);

int DeleteMeaninglessNodes(Node* node);

int CheckNDeleteNode(Node* node);

int ChangeNodeIntoNum(Node* node, double val);

int ChangeIntoChild(Node* node, Side side);

int DeleteAnchor (Node* node, Side side);

double CalculateNode(Node* node, double x);

int CountSubTreeSize(Node* node, int* size);

int SubTreeSize(Node* node);

Tree DiffFuncOnce(Tree* datatree, Phrases* diffphrases, FILE* out);

int MaclaurinSeries(Tree* datatree, Tree* difftree, Phrases* phrases, FILE* out);

int TangentEquation(Tree* datatree, Tree* difftree, double x, FILE* out);

int CreateGraphic(Tree* datatree, double left, double right, FILE* out);

int Priority(Node* node);

int TeXPrint(Node* node, char* repl, FILE* out);

int PrepareTeXFile(FILE* out);

int TeXNodePrint(Node* startnode, Node* node, FILE* out);

int TeXDataStartPrint(Node* startnode, Node* node, FILE* out);

int TeXDataEndPrint(Node* startnode, Node* node, FILE* out);

int TeXDataPrint(Node* node, FILE* out);

Node* CreateNode(NodeType type, double val, OperType optype, char* varvalue, Tree* tree, Node* ancestor, Node* leftchild, Node* rightchild);

int Factorial(int n);

OperType IsOper(char* str);

double IsVal(char* str);

bool IsOneArg(Node* node);

int compare(const double a, const double b);

#endif //DIFFERENTIATOR_H
