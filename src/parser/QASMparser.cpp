//
// Created by mingz on 2021/8/16.
//

#include "QASMparser.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

using namespace std;

QASMparser::QASMparser() {
    this->gate_num = 0;
    this->qubit_num = 0;
}

QASMparser::QASMparser(string filename) {
    this->gate_num = 0;
    this->qubit_num = 0;
    this->filename = filename;
}

int QASMparser::GetGateNum() {
    return this->gate_num;
}

int QASMparser::GetQubitNum() {
    return this->qubit_num;
}

map<int, GateNode> QASMparser::GetGateInfo() {
    return this->GateInfo;
}

void QASMparser::ReadFile(string filename) {
    this->filename = filename;
}

void QASMparser::GenerateGateInfo() {
    ifstream read_file;
    read_file.open(this->filename, ios::binary);
    string line;
    int gateid = 1;
    while (getline(read_file, line)) {
        if (regex_search(line, regex("cx"))) {
            GateNode gate_node;
            gate_node.Name = "cx";
            gate_node.criticality = 0;
            regex pattern(R"(\d+)");
            sregex_iterator iter(line.begin(), line.end(), pattern);
            gate_node.controlQubit = stoi((*iter)[0]);
            iter++;
            gate_node.targetQubit = stoi((*iter)[0]);
            this->GateInfo.insert({gateid, gate_node});
            this->gate_num++;
        } else if (regex_search(line, regex("x"))) {
            GateNode gate_node;
            gate_node.Name = "x";
            gate_node.criticality = 0;
            regex pattern(R"(\d+)");
            sregex_iterator iter(line.begin(), line.end(), pattern);
            gate_node.targetQubit = stoi((*iter)[0]);
            gate_node.controlQubit = -1;
            this->GateInfo.insert({gateid, gate_node});
            this->gate_num++;
        } else if (regex_search(line, regex("h"))) {
            GateNode gate_node;
            gate_node.Name = "h";
            gate_node.criticality = 0;
            regex pattern(R"(\d+)");
            sregex_iterator iter(line.begin(), line.end(), pattern);
            gate_node.targetQubit = stoi((*iter)[0]);
            gate_node.controlQubit = -1;
            this->GateInfo.insert({gateid, gate_node});
            this->gate_num++;
        } else if (regex_search(line, regex("tdg"))) {
            GateNode gate_node;
            gate_node.Name = "tdg";
            gate_node.criticality = 0;
            regex pattern(R"(\d+)");
            sregex_iterator iter(line.begin(), line.end(), pattern);
            gate_node.targetQubit = stoi((*iter)[0]);
            gate_node.controlQubit = -1;
            this->GateInfo.insert({gateid, gate_node});
            this->gate_num++;
        } else if (regex_search(line, regex("t"))) {
            GateNode gate_node;
            gate_node.Name = "t";
            gate_node.criticality = 0;
            regex pattern(R"(\d+)");
            sregex_iterator iter(line.begin(), line.end(), pattern);
            gate_node.targetQubit = stoi((*iter)[0]);
            gate_node.controlQubit = -1;
            this->GateInfo.insert({gateid, gate_node});
            this->gate_num++;
        } else if (regex_search(line, regex("qreg"))) {
            regex pattern(R"(\d+)");
            sregex_iterator iter(line.begin(), line.end(), pattern);
            this->qubit_num = stoi((*iter)[0]);
        }
        gateid++;
        //cout << "line:" << line.c_str() << endl;
    }
    map<int, GateNode>::iterator iter;
    for (iter = this->GateInfo.begin(); iter != this->GateInfo.end(); iter++) {
        cout << iter->first << " " << iter->second.Name << endl;
    }
}
