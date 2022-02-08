//
// Created by PC on 2021/12/30.
//

#include <iostream>
#include <cassert>
#include <cmath>
#include "../parser/Environment.h"
#include "Connect.h"
#include <Python.h>

PyObject *PyList_From(vector<vector<int>> vector1);

using namespace std;

vector<vector<int>> CallPythonFunction(vector<vector<int>> cmap, vector<int> initialLayout, vector<int> finalLayout)
{
    //导入文件（模块）
    PyObject* pModule = PyImport_ImportModule("token_swap_4_appr");
    if(!pModule){
        cout<<"Python get module failed." << endl;
        vector<vector<int>> null_swap;
        return null_swap;
    }
    cout << "Python get module succeed." << endl;
    //找到函数的地址
    PyObject * pFunc = nullptr;
    pFunc = PyObject_GetAttrString(pModule, "TS4");

    //创建参数：
    PyObject* pArgs = PyTuple_New(3);
    PyObject* args1;
    for(int i=0; i <cmap.size(); i++){
        PyObject* tmp;
        for (int j = 0; j < cmap[i].size(); ++j) {
            PyList_Append(tmp, PyLong_FromLong(cmap[i][j]));
        }
        PyList_Append(args1,tmp);
    }
    PyObject* args2;
    for(int i=0; i <initialLayout.size(); i++){
        PyList_Append(args2, PyLong_FromLong(initialLayout[i]));
    }
    PyObject* args3;
    for(int i=0; i <cmap.size(); i++){
        PyList_Append(args3, PyLong_FromLong(finalLayout[i]));
    }
    //设置参数的值
    PyTuple_SetItem(pArgs, 0, args1);
    PyTuple_SetItem(pArgs, 1, args2);
    PyTuple_SetItem(pArgs, 2, args3);
    //调用函数
    PyObject *pReturnValue = PyObject_CallObject(pFunc, pArgs);

    //转换为C语言类型并返回

    return reinterpret_cast<const vector<vector<int>> &>(pReturnValue);
}

Connect::Connect(vector<vector<int>> cmap, vector<int> initialLayout, vector<int> finalLayout)
{
    vector<vector<int>> swap_Sequence;
    Py_SetProgramName(L"token_swap_4_appr");
    Py_SetPythonHome(L"E:\\Program File\\anaconda3");
    Py_Initialize();
//    PyRun_SimpleString("import sys");
//    PyRun_SimpleString("sys.path.append('./')");
//    PyObject* pModule = PyImport_ImportModule("token_swap_4_appr");
//    PyObject* pFunc = PyObject_GetAttrString(pModule, "Hello");
//    PyObject* pArgs = PyTuple_New(0);
//    PyObject *pReturnValue = PyObject_CallObject(pFunc, pArgs);
    swap_Sequence = CallPythonFunction(cmap, initialLayout, finalLayout);
    Py_Finalize();
    this->swapSequence = swap_Sequence;
}