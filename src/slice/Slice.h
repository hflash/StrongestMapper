//
// Created by PC on 2021/12/24.
//

#ifndef STRONGESTMAPPER_SLICE_H
#define STRONGESTMAPPER_SLICE_H

#include<iostream>
#include "../parser/QASMparser.h"
#include<vector>
#include<string>
#include<set>

using namespace std;

class Slice{
public:
    bool constant_slice(Environment* env, vector<vector<int>> dag, int slice_layers);
    bool adaptive_slice(vector<vector<int>> dag);
};





#endif //STRONGESTMAPPER_SLICE_H

