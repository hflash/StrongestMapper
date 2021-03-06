//
// Created by mingz on 2021/8/16.
// read QASM files, convert to daggenerator
//

#ifndef SEARCHCOMPILER_QASMPARSER_H
#define SEARCHCOMPILER_QASMPARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

struct GateNode
{
    string Name;
    int controlQubit;
    int targetQubit;
    int criticality;
};

class QASMparser {
public:
    QASMparser();
    QASMparser(string filename);
    void ReadFile(string filename);
    int GetQubitNum();
    int GetGateNum();

    map<int,GateNode> GetGateInfo();
    void GenerateGateInfo();
private:
    string filename;
    int gate_num;
    int qubit_num;
    map<int,GateNode> GateInfo;
    void ReadGate();

};

#endif //SEARCHCOMPILER_QASMPARSER_H
