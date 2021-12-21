//
// Created by wonder on 2021/12/20.
//
#include<iostream>
#include "Dag.hpp"
#include<queue>

Dag::Dag()
{
    std::cout<<"Dag initialized without gates!"<<endl;
}
Dag::Dag(vector<GateNode> Gates, int qubitNum)
{
    Dag::gate_num = Gates.size();
    DagNode* node = new DagNode;
    root = node;
    root->targetParent = NULL;
    root->controlParent = NULL;
    root->gateID = 0;
    root->criticality = 0;
    vector<GateNode>::iterator it;
    int* qubitState = new int[qubitNum];
    for(it = Gates.begin();it != Gates.end(); it ++)
    {
        DagNode* node = new DagNode;

    }
}

int *Dag::topologicalSort(Dag* dag)
{
    vector<int> topo_list;
    DagNode* root = dag->root;
    queue<DagNode*> q;
    vector<bool> visited(qubit_num);
    q.push(root);
    visited[0] = true;
    while(!q.empty())
    {
        DagNode* cur = q.front();
        topo_list.push_back(cur->gateID);
        DagNode* control_child = cur->controlChild;
        DagNode* target_child = cur->targetChild;
        int control_child_id = cur->controlChild->gateID;
        int target_child_id = cur->targetChild->gateID;
        if(control_child!=NULL && target_child != NULL)
        {
            if (target_child_id < control_child_id){
                q.push(target_child);
                q.push(control_child);
                topo_list.push_back(target_child_id);
                topo_list.push_back(control_child_id);
            } else if (target_child_id > control_child_id){
                q.push(control_child);
                q.push(target_child);
                topo_list.push_back(control_child_id);
                topo_list.push_back(target_child_id);
            }
        }
        q.pop();
    }
    int topological_list[qubit_num];
    for(int i=0; i<qubit_num; i++){
        topological_list[i] = topo_list[i];
    }
    return topological_list;
}
int* frontLayer(Dag* dag)
{
    vector<GateNode>::iterator it;
//    for(it = dag->root)
}
