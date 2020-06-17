package com.rafel.huawei;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Stack;

public class Digraph implements Config{

//    private static int MAX_NODE_COUNT = 6009;

    //private static int[] visited=new int[MAX_NODE_COUNT];

    /**
     * node集合
     */

    private static List<String> nodes = new ArrayList<>();

    public static List<String> getNodes() {
        return nodes;
    }

    public void setNodes(List<String> nodes) {
        Digraph.nodes = nodes;
    }

    /**
     * 有向图的邻接矩阵
     */

    static int[][] adjacencyMatrix = new int[MAX_NODE_COUNT][MAX_NODE_COUNT];

    private static int addNode(String nodeName) {
        if (!nodes.contains(nodeName)) {
            if (nodes.size() >= MAX_NODE_COUNT) {
                System.out.println("nodes超长:" + nodeName);
                return -1;
            }
            nodes.add(nodeName);
            return nodes.size() - 1;
        }

        return nodes.indexOf(nodeName);
    }

    public void addLine(String startNode, String endNode) {

//        int startIndex = addNode(startNode);
//        int endIndex = addNode(endNode);
        int startIndex =Integer.parseInt(startNode);
        int endIndex = Integer.parseInt(endNode);

        if (startIndex >= 0 && endIndex >= 0) {
//            visited[startIndex]=-1;
//            visited[endIndex]=-1;
            adjacencyMatrix[startIndex][endIndex] = 1;
        }
    }

    /**
     * 寻找闭环
     */

    public List<List<String>> find(int v) {

        // 从出发节点到当前节点的轨迹
        List<Integer> trace = new ArrayList<>();
        //返回值

        List<List<String>> result = new ArrayList<>();
        if (adjacencyMatrix.length > 0) {
            findCycle(v, trace, result);
//            dfs(v, trace, result);
        }
//        if (result.size() == 0) {
//            result.add("no cycle!");
//        }


//        for (List<String> list : result) {
//
//            circle.add(Utils.sortId(list));
//
//        }

        return result;
    }


    /**
     * findCycle
     */

    // 递归dfs
    private static void findCycle(int v, List<Integer> trace, List<List<String>> result) {
        int j;
        //添加闭环信息
        if ((j = trace.indexOf(v)) != -1) {

            List<String> list = new ArrayList<>();
            while (j < trace.size()) {

                list.add(nodes.get(trace.get(j)));
                j++;
            }
            if (list.size() <= 7 && list.size() > 2) result.add(list);

            return;
        }

        trace.add(v);
        // visited[v]=1;

        for (int i = 0; i < nodes.size(); i++) {
            if (adjacencyMatrix[v][i] == 1) {
//                //添加闭环信息
//                if ((j = trace.indexOf(i)) != -1) {
//
//                    List<String> list = new ArrayList<>();
//                    while (j < trace.size()) {
//
//                        list.add(nodes.get(trace.get(j)));
//                        j++;
//                    }
//                    if (list.size() <= 7 && list.size() > 2) result.add(list);
//
//                    continue;
//                }
//                if(visited[i]==1) continue;
                findCycle(i, trace, result);
            }
        }
        trace.remove(trace.size() - 1);
        // visited[v]=-1;
    }

    // 非递归dfs
    public void dfs(int v, List<Integer> trace, List<List<String>> result) {
        int j;
        Stack<Integer> stack = new Stack<>();
        stack.push(v);
        trace.add(v);
        while (!stack.isEmpty()) {

            int curNode = stack.peek();
            int colNo = 0;
            for (colNo = 0; colNo < nodes.size(); colNo++) {
                if (adjacencyMatrix[curNode][colNo] == 1) {
                    if ((j = trace.indexOf(colNo)) != -1) {

                        List<String> list = new ArrayList<>();

                        while (j < trace.size()) {

                            list.add(nodes.get(trace.get(j)));
                            j++;
                        }
                        if (list.size() <= 7 && list.size() > 2) result.add(list);

                        continue;

                    }
                    stack.push(colNo);
                    trace.add(colNo);
                    break;
                }
            }
            if (colNo == nodes.size()) {
                stack.pop();
                trace.remove(trace.size() - 1);
            }
        }
    }

}