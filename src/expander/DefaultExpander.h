//
// Created by mingz on 2021/12/29.
//

#ifndef STRONGESTMAPPER_DEFAULTEXPANDER_H
#define STRONGESTMAPPER_DEFAULTEXPANDER_H

#include "Expander.h"
#include <algorithm>

using namespace std;


class DefaultExpander: public Expander {
private:
    Environment* env;
public:
//    void DeleteVectorElement(vector<int> V,int element);
    DefaultExpander(Environment* env);
    /* True: there is more cnot in an unscheduled
     * False: there is no cnot in an unscheduled, no need for swap
     */
    bool IsMoreCnot(SearchNode* node);
    bool IsMoreSwap(SearchNode* node);
    /*Given the ready gates, give their combination,
     * if not all of them are combined, then the pattern is encountered
     */
    vector<vector<int>> ComReadyGates(vector<int> readyGates);
    /*For nodes that do not need swap, execute the path to the end
     *Set findBestNode to true
     *Set the final path to actionPath
     *The number of expands in this step is 1
     */
    void expandWithoutSwap(SearchNode* node);
    /* Input idle logic bits,
     * Output a combination of swaps that can be executed
     */
    vector<vector<vector<int>>> SwapCom(vector<int> qubitState,vector<int> l2pmapping);
    vector<vector<vector<int>>> SwapCom1(vector<int> qubitState,vector<int> l2pmapping,vector<vector<int>> dTable);

    bool expand( DefaultQueue* nodes,SearchNode* node);

    bool expand1( DefaultQueue* nodes,SearchNode* node);
    bool IsCycle(vector<ActionPath> actionPath,int qubitNum);
    bool IsCycle1(vector<ActionPath> actionPath,int qubitNum);
};


#endif //STRONGESTMAPPER_DEFAULTEXPANDER_H
