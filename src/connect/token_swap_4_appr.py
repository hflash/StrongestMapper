import networkx as nx


def ConstructDirectedGraph(pDistance, undirectedGraph, targetPbit2Lbit, targetLbit2Pbit, adjacency):
    dGraph = nx.DiGraph()
    dGraph.add_nodes_from(undirectedGraph.nodes)
    for node in undirectedGraph.nodes:
        if undirectedGraph.nodes[node]["lbit"] != targetPbit2Lbit[node]:
            for adjNode in adjacency[node]:
                targetPbitOfNode = targetLbit2Pbit[undirectedGraph.nodes[node]["lbit"]]
                if pDistance[adjNode][targetPbitOfNode] < pDistance[node][targetPbitOfNode]:
                    dGraph.add_edge(node, adjNode)
    return dGraph


# returns the swap process of physical bits as a list [[7, 8], [8, 3], [3, 2], [2, 1], [1, 0], [6, 7], ...]
def TS4(edgeList, originPbit2Lbit, targetPbit2Lbit):
    nodeList = [node for edge in edgeList for node in edge]
    nodeList = list(set(nodeList))
    physicalGraph = nx.Graph()
    physicalGraph.add_nodes_from(nodeList)
    physicalGraph.add_edges_from(edgeList)
    pDistance = dict(nx.all_pairs_shortest_path_length(physicalGraph))  # distance between each 2 nodes
    targetLbit2Pbit = {}
    notInPositionCount = 0
    adjacencyDict = dict(physicalGraph.adjacency())
    for pIndex in range(len(targetPbit2Lbit)):
        physicalGraph.nodes[pIndex]["lbit"] = originPbit2Lbit[pIndex]
        targetLbit2Pbit[targetPbit2Lbit[pIndex]] = pIndex
        if originPbit2Lbit[pIndex] != targetPbit2Lbit[pIndex]:
            notInPositionCount += 1
    swapSequence = []
    while notInPositionCount > 0:
        assistDiGraph = ConstructDirectedGraph(pDistance, physicalGraph, targetPbit2Lbit, targetLbit2Pbit,
                                               adjacencyDict)
        diAdjacencyDict = dict(assistDiGraph.adjacency())
        cycleNodes = []
        for node in diAdjacencyDict:
            if diAdjacencyDict[node] == {}:
                continue
            else:
                cycleNodes.append(node)
                nodePointer = node
                while diAdjacencyDict[nodePointer] != {}:
                    possibleNextNodeList = list(diAdjacencyDict[nodePointer].keys())
                    nodePointer = None
                    for candidate in possibleNextNodeList:
                        if nodePointer is None or physicalGraph.nodes[nodePointer]['lbit'] == targetPbit2Lbit[
                            nodePointer]:
                            nodePointer = candidate
                    if nodePointer in cycleNodes:
                        break
                    else:
                        cycleNodes.append(nodePointer)
                if diAdjacencyDict[nodePointer] == {}:
                    swapPhyInd1 = cycleNodes[-1]
                    swapPhyInd2 = cycleNodes[-2]
                    swapSequence.append([swapPhyInd1, swapPhyInd2])
                    temp = physicalGraph.nodes[swapPhyInd1]['lbit']
                    physicalGraph.nodes[swapPhyInd1]['lbit'] = physicalGraph.nodes[swapPhyInd2]['lbit']
                    physicalGraph.nodes[swapPhyInd2]['lbit'] = temp
                    notInPositionCount += 1
                else:
                    swapInd = -1
                    while cycleNodes[swapInd] != nodePointer:
                        swapPhyInd1 = cycleNodes[swapInd]
                        swapPhyInd2 = cycleNodes[swapInd - 1]
                        swapSequence.append([swapPhyInd1, swapPhyInd2])
                        temp = physicalGraph.nodes[swapPhyInd1]['lbit']
                        physicalGraph.nodes[swapPhyInd1]['lbit'] = physicalGraph.nodes[swapPhyInd2]['lbit']
                        physicalGraph.nodes[swapPhyInd2]['lbit'] = temp
                        if physicalGraph.nodes[swapPhyInd1]['lbit'] == targetPbit2Lbit[swapPhyInd1]:
                            notInPositionCount -= 1
                        swapInd -= 1
                    if physicalGraph.nodes[cycleNodes[swapInd]]['lbit'] == targetPbit2Lbit[cycleNodes[swapInd]]:
                        notInPositionCount -= 1
                break
    return swapSequence
def Hello():
    print("hello world!")

# Press the green button in the gutter to run the script.
if __name__ == '__main__':
    cmap = [[0, 1], [1, 2], [3, 2], [3, 8], [4, 3], [4, 9], [5, 6], [7, 6], [7, 8], [7, 8], [9, 8]]
    originLayout = [2, 3, 5, 7, 4, 9, 8, 0, 6, 1]
    targetLayout = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
    print(TS4(cmap, originLayout, targetLayout))
