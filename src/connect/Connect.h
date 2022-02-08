//
// Created by PC on 2021/12/30.
//

#ifndef STRONGESTMAPPER_CONNECT_H
#define STRONGESTMAPPER_CONNECT_H

#include <iostream>
#include <vector>
#include <Python.h>

using namespace std;

class Connect{
public:
    vector<vector<int>> swapSequence;
    Connect(vector<vector<int>> cmap, vector<int> initialLayout, vector<int> finalLayout);
};
#endif //STRONGESTMAPPER_CONNECT_H
