#include <iostream>
#include "src/parser/QASMparser.h"
using namespace std;

int main() {
    QASMparser qasm_parser("../circuits/mod5mils_65.qasm");
    qasm_parser.GenerateGateInfo();
    cout<<"qubit num "<<qasm_parser.GetQubitNum()<<"\n";
    cout<<"gate num "<<qasm_parser.GetGateNum()<<"\n";
    int qubit_num=qasm_parser.GetQubitNum();
    int g_num=qasm_parser.GetGateNum();
    vector<GateNode> gate_info=qasm_parser.GetGateInfo();
//    cout<<"gate num "<<sizeof(gate_info)<<"\n";
    for(int i=0;i<g_num;i++){
        cout<<"gate "<<i<<" : "<<gate_info[i].Name<<" "<<gate_info[i].qubit1<<" "<<gate_info[i].qubit2<<"\n";
    }
    return 0;
}
