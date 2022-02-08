//
// Created by PC on 2021/12/24.
//

#include <iostream>
#include <cassert>
#include <cmath>
#include "../parser/Environment.h"

// return true
bool constant_slice(Environment* env, int slice_layers)
{
    vector<vector<vector<int>>> slice_dag;
//    vector<vector<int>> gateDag = env->gateDag;
    int dag_depth = env->getDagDepth();
    float slices = dag_depth/slice_layers;
    int slice_number = ceil(slices);
    for(int i=0; i<slice_number; i++)
    {
        vector<vector<int>> current_slice;
        for(int j=0; j<slice_layers; j++)
        {
//            current_slice.push_back(dag_depth[i])
        }
        slice_dag.push_back(current_slice);
    }
//    reuturn
}