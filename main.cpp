#include <iostream>
#include <map>
#include "src/parser/QASMparser.h"
#include "src/parser/Environment.h"
#include "src/search/SearchNode.h"
#include "src/expander/DefaultExpander.h"
#include "src/expander/Search.h"
#include <time.h>
#include <io.h>
#include <fstream>
#include <stdio.h>
#include <stdbool.h>

using namespace std;

void PrintPath(SearchResult a) {
    cout << "how many path has done: " << a.finalPath.size() << endl;
    for (int i = 0; i < a.finalPath.size(); i++) {
        for (int j = 0; j < a.finalPath[i].actions.size(); j++) {
            cout << a.finalPath[i].actions[j].gateID << " " << a.finalPath[i].actions[j].gateName << " "
                 << a.finalPath[i].actions[j].controlQubit << " " << a.finalPath[i].actions[j].targetQubit << "   ";
        }
        cout << endl;
    }
    int count = 0;
    for (int i = 0; i < a.searchNodeNum.size(); i++) {
        count = count + a.searchNodeNum[i];
    }
    cout << "search node number: " << count << endl;
    int pattern = 0;
    for (int i = 0; i < a.finalPath.size(); i++) {
        if (a.finalPath[i].pattern == true) {
            pattern++;
        }
    }
    cout << "pattern number: " << pattern << endl;
    cout << "cycle num is " << a.cycleNum << endl;

}

void getFileNames(string path, vector<string>& files)
{
    //文件句柄
    //注意：我发现有些文章代码此处是long类型，实测运行中会报错访问异常
    intptr_t hFile = 0;
    //文件信息
    struct _finddata_t fileinfo;
    string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
    {
        do
        {
            //如果是目录,递归查找
            //如果不是,把文件绝对路径存入vector中
            if ((fileinfo.attrib & _A_SUBDIR))
            {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    getFileNames(p.assign(path).append("\\").append(fileinfo.name), files);
            }
            else
            {
                files.push_back(p.assign(path).append("\\").append(fileinfo.name));
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
}

void ShortCircuit(){
    clock_t startTime,endTime;
    ofstream outFile;
    outFile.open("../short_cir_out_2.txt");
    vector<vector<int>> coupling_qx2,coupling_1x5;
    coupling_1x5={{0, 1},{1, 2},{2,3},{3,4}};
    coupling_qx2={{0, 1},{1, 2},{2,3},{3,4},{0,2},{2,4}};
    vector<string> fileNames;
    string path("../circuits/small"); 	//自己选择目录测试
    getFileNames(path, fileNames);
    for (const auto &ph : fileNames) {
        std::cout << ph << "\n";
    }
    for(int fi=0;fi<fileNames.size();fi++){
        startTime=clock();
        string fileN;
        fileN=fileNames[fi];
        outFile<<fileN<<endl;
        outFile<<"-------------------------------------------------------------------"<<endl;
        Environment *env = new Environment(fileN, coupling_1x5);
        vector<int> mapping={0,1,2,3,4};
        vector<vector<int>> dagTable = env->getGateDag();
        Search *sr = new Search(env);
        vector<int> qubitSate={0,0,0,0,0};
        vector<ActionPath> newPath;
        SearchNode *sn =new SearchNode(mapping,qubitSate,dagTable, env, 0, newPath);
        sn->PrintNode();
        SearchResult a = sr->SearchCircuit2(sn);
        endTime=clock();
        outFile << "how many path has done: " << a.finalPath.size() << endl;
        for (int i = 0; i < a.finalPath.size(); i++) {
            for (int j = 0; j < a.finalPath[i].actions.size(); j++) {
                outFile << a.finalPath[i].actions[j].gateID << " " << a.finalPath[i].actions[j].gateName << " "
                     << a.finalPath[i].actions[j].controlQubit << " " << a.finalPath[i].actions[j].targetQubit << "   ";
            }
            outFile << endl;
        }
        int count = 0;
        for (int i = 0; i < a.searchNodeNum.size(); i++) {
            count = count + a.searchNodeNum[i];
            outFile <<a.searchNodeNum[i]<<"  ";
        }
        outFile << " \n search node number: " << count << endl;
        int pattern = 0;
        for (int i = 0; i < a.finalPath.size(); i++) {
            if (a.finalPath[i].pattern == true) {
                pattern++;
            }
        }
        outFile << "pattern number: " << pattern << endl;
        outFile << "cycle num is " << a.cycleNum << endl;

    }
    outFile.close();
}

int main() {
//    ShortCircuit();
    clock_t startTime,endTime;
    startTime=clock();
    string fname = "../circuits/small/test1.qasm";
    string fname1 = "../circuits/small/4gt11_82.qasm";
    string fname2= "../circuits/small/rd32-v0_66.qasm";
    string fname3= "../circuits/small/alu-v1_28.qasm";
    string fname4= "../circuits/small/alu-v1_29.qasm";
    string fname5= "../circuits/small/alu-v2_33.qasm";

    vector<vector<int>> coupling;
    vector<vector<int>> coupling_qx2,coupling_1x5;
    coupling_1x5={{0, 1},{1, 2},{2,3},{3,4}};
    coupling={{0,1},{1, 2},{2,3},{3,4},{4,5},{5,6},{6,7}};
    coupling_qx2={{0, 1},{1, 2},{2,3},{3,4},{0,2},{2,4}};
    Environment *env = new Environment(fname3, coupling_1x5);
    vector<int> mapping={0,1,2,3,4};
    vector<int> mapping8={0,1,2,3,4,5,6,7};
    vector<vector<int>> dagTable = env->getGateDag();
    vector<int> executedgateIDs={};
    //vector<vector<int>> dagTable5=env->getNewKLayerDag(executedgateIDs,10);
    Search *sr = new Search(env);
    vector<int> qubitSate={0,0,0,0,0};
    vector<int> qubitSate8={0,0,0,0,0,0,0,0};
    vector<ActionPath> newPath;
    SearchNode *sn =new SearchNode(mapping,qubitSate,dagTable, env, 0, newPath);
    sn->PrintNode();
    SearchResult a = sr->SearchCircuit2(sn);
    //SearchResult a= sr->SearchSmoothWithInitialMapping(mapping8,8);
    PrintPath(a);
    endTime=clock();
    cout<<"total time is "<<(double )(endTime-startTime)/CLOCKS_PER_SEC<<" s "<<endl;
    cout << "how many path has done: " << a.finalPath.size() << endl;

    return 0;
}
