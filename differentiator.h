#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include "diftree.h"
#include "text-sort.h"
#include "common.h"
#include "dsl.h"

int DifferentiatorMain(char* input);

Tree DataParser(char* inputdata);

int FillCurrNode(Node* currnode, char* input);

Node* NodeCopy(Node* node);

int CreateAncestor(Node* node, Node* ancestor, Tree* tree);

Node* DiffNode(Node* node);

Node* DiffPower(Node* node);

Node* DiffLog(Node* node);

int SimplifyConstantNode(Node* node);

int SimplifyConstants(Node* node);

int SimplifyFunc(Tree* tree);

int DeleteMeaninglessNodes(Node* node);

int CheckNDeleteNode(Node* node);

int DeleteAnchor (Node* node);

double CalculateNode(Node* node, double x);

int MaclaurinSeries(Tree* tree, FILE* out);

int TeXPrint(Tree* orig, Tree* difftree, FILE* out);

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
