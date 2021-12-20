//
// Created by wonder on 2021/12/20.
//

#ifndef STRONGESTMAPPER_DAGGENERATOR_H
#define STRONGESTMAPPER_DAGGENERATOR_H


class DagGenerator {
    struct DagNode{
        DagNode* ControlParents;
        DagNode* TargetParents;
        DagNode* ControlChild;
        DagNode* TargetChild;
        int GateId;
    };
public:
    DagGenerator();
    DagGenerator(vector<GateNode>);
    void ReadFile(string filename);
    int GetQubitNum();
    int GetGateNum();

    vector<GateNode> GetGateInfo();
    void GenerateGateInfo();
private:
    string filename;
    int gate_num;
    int qubit_num;
    vector<GateNode> GateInfo;
};


#endif //STRONGESTMAPPER_DAGGENERATOR_H
