//
// Created by wonder on 2021/12/20.
//
#include<iostream>
#include "Dag.hpp"

Dag::Dag()
{
    std::cout<<"Dag initialized without gates!"<<endl;
}
Dag::Dag(vector<GateNode> Gates)
{
    Dag::gate_num = Gates.size();
    DagNode* node = new DagNode;
    root = node;
    root->targetParent = NULL;
    root->controlParent = NULL;
    root->gateID = 0;
    root->criticality = 0;
    vector<GateNode>::iterator it;
    for(it = Gates.begin();it != Gates.end(); it ++)
    {
        DagNode* node = 0;
    }
}
