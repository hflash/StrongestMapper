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

QASMparser::QASMparser(){
    this->gate_num=0;
    this->qubit_num=0;
}

QASMparser::QASMparser(string filename){
    this->gate_num=0;
    this->qubit_num=0;
    this->filename=filename;
}

int QASMparser::GetGateNum() {
    return this->gate_num;
}

int QASMparser::GetQubitNum() {
    return this->qubit_num;
}

vector<GateNode> QASMparser::GetGateInfo() {
    return this->GateInfo;
}

void QASMparser::ReadFile(string filename) {
    this->filename=filename;
}

void QASMparser::GenerateGateInfo() {
    ifstream read_file;
    read_file.open(this->filename, ios::binary);
    string line;
    while (getline(read_file, line)) {
        if (regex_search(line, regex("cx"))){
            GateNode gate_node;
            gate_node.Name="cx";
            regex pattern(R"(\d+)");
            sregex_iterator iter(line.begin(), line.end(), pattern);
            gate_node.qubit1=stoi((*iter)[0]);
            iter++;
            gate_node.qubit2=stoi((*iter)[0]);
            this->GateInfo.push_back(gate_node);
            this->gate_num++;
        }
        else if (regex_search(line, regex("x"))){
            GateNode gate_node;
            gate_node.Name="x";
            regex pattern(R"(\d+)");
            sregex_iterator iter(line.begin(), line.end(), pattern);
            gate_node.qubit1=stoi((*iter)[0]);
            gate_node.qubit2=-1;
            this->GateInfo.push_back(gate_node);
            this->gate_num++;
        }
        else if (regex_search(line, regex("h"))){
            GateNode gate_node;
            gate_node.Name="h";
            regex pattern(R"(\d+)");
            sregex_iterator iter(line.begin(), line.end(), pattern);
            gate_node.qubit1=stoi((*iter)[0]);
            gate_node.qubit2=-1;
            this->GateInfo.push_back(gate_node);
            this->gate_num++;
        }
        else if (regex_search(line, regex("tdg"))){
            GateNode gate_node;
            gate_node.Name="tdg";
            regex pattern(R"(\d+)");
            sregex_iterator iter(line.begin(), line.end(), pattern);
            gate_node.qubit1=stoi((*iter)[0]);
            gate_node.qubit2=-1;
            this->GateInfo.push_back(gate_node);
            this->gate_num++;
        }
        else if (regex_search(line, regex("t"))){
            GateNode gate_node;
            gate_node.Name="t";
            regex pattern(R"(\d+)");
            sregex_iterator iter(line.begin(), line.end(), pattern);
            gate_node.qubit1=stoi((*iter)[0]);
            gate_node.qubit2=-1;
            this->GateInfo.push_back(gate_node);
            this->gate_num++;
        }
        else if (regex_search(line, regex("qreg"))){
            regex pattern(R"(\d+)");
            sregex_iterator iter(line.begin(), line.end(), pattern);
            this->qubit_num=stoi((*iter)[0]);
        }
        //cout << "line:" << line.c_str() << endl;
    }
}
