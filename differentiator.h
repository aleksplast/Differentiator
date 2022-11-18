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

Node* DiffNode(Node* node);

Node* DiffPower(Node* node);

Node* DiffLog(Node* node);

int TeXPrint(Tree* orig, Tree* difftree, FILE* out);

int PrepareTeXFile(FILE* out);

int TeXNodePrint(Node* node, FILE* out);

int NodePrint(Node* node);

int DataPrint(Node* node, FILE* out);

int TeXDataStartPrint(Node* node, FILE* out);

int TeXDataEndPrint(Node* node, FILE* out);

int TeXDataPrint(Node* node, FILE* out);

Node* CreateNode(NodeType type, double val, OperType optype, char* varvalue, Node* leftchild, Node* rightchild);

OperType IsOper(char* str);

double IsVal(char* str);

bool IsOneArg(Node* node);

int compare(const double a, const double b);

#endif //DIFFERENTIATOR_H
