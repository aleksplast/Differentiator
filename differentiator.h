#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include "diftree.h"
#include "text-sort.h"
#include "common.h"
#include "dsl.h"
#include "recursivedescent.h"

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

int DeleteAnchor (Node* node);

double CalculateNode(Node* node, double x);

Tree DiffFuncOnce(Tree* datatree, Phrases* phrases, FILE* out);

int MaclaurinSeries(Tree* datatree, Tree* difftree, Phrases* phrases, FILE* out);

int TeXPrint(Node* node, FILE* out);

int PrepareTeXFile(FILE* out);

int TeXNodePrint(Node* node, FILE* out);

int NodePrint(Node* node);

int DataPrint(Node* node, FILE* out);

int TeXDataStartPrint(Node* node, FILE* out);

int TeXDataEndPrint(Node* node, FILE* out);

int TeXDataPrint(Node* node, FILE* out);

Node* CreateNode(NodeType type, double val, OperType optype, char* varvalue, Node* leftchild, Node* rightchild);

int Factorial(int n);

OperType IsOper(char* str);

double IsVal(char* str);

bool IsOneArg(Node* node);

int compare(const double a, const double b);

#endif //DIFFERENTIATOR_H
