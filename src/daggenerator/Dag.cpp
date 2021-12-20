//
// Created by wonder on 2021/12/20.
//
#include<iostream>
#include "Dag.h"

Dag::Dag()
{
    std::cout<<"Dag initialized without gates"<<endl;
}
Dag::Dag(vector<GateNode> Gates)
{
    Dag::gate_num = Gates.size();

}
