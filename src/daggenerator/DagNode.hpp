//
// Created by wonder on 2021/12/20.
//

#ifndef STRONGESTMAPPER_DAGNODE_HPP
#define STRONGESTMAPPER_DAGNODE_HPP
class DagNode{
    public:
        DagNode* controlParent;
        DagNode* targetParent;
        DagNode* controlChild;
        DagNode* targetChild;
        vector<DagNode*> rootChild;
        int gateID;//to denote the gate in GateNode
        int criticality;//length of circuit from here until furthest leaf
};
#endif //STRONGESTMAPPER_DAGNODE_HPP
