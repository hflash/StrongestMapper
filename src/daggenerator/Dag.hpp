//
// Created by wonder on 2021/12/20.
//

#ifndef STRONGESTMAPPER_DAG_HPP
#define STRONGESTMAPPER_DAG_HPP


#include "../parser/QASMparser.h"
#include "DagNode.hpp"


class Dag{
public:
    Dag();
    Dag(vector<GateNode>, int);
    ~Dag();
    int* topologicalSort(Dag* dag);
    int* frontLayer(Dag* dag);
    Dag* firstKlayers(int K);
    Dag* deleteNodesWithID(int nodeID);

private:
    int gate_num;
    int qubit_num;
    int depth;
    DagNode* root;
};


#endif //STRONGESTMAPPER_DAG_HPP
