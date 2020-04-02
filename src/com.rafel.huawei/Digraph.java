package com.rafel.huawei;

import java.util.ArrayList;
import java.util.List;

public class Digraph {

    private static int MAX_NODE_COUNT = 6000;

    /**
     * node集合
     */

    private static List<String> nodes = new ArrayList<>();

    public List<String> getNodes() {
        return nodes;
    }

    public void setNodes(List<String> nodes) {
        Digraph.nodes = nodes;
    }

    /**
     * 有向图的邻接矩阵
     */

    private static int[][] adjacencyMatrix = new int[MAX_NODE_COUNT][MAX_NODE_COUNT];

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
        int startIndex = addNode(startNode);
        int endIndex = addNode(endNode);
        if (startIndex >= 0 && endIndex >= 0) {
            adjacencyMatrix[startIndex][endIndex] = 1;
        }
    }

    /**
     * 寻找闭环
     */
    public List<String> find(int v) {
        // 从出发节点到当前节点的轨迹
        List<Integer> trace = new ArrayList<>();
        //返回值
        List<String> circle = new ArrayList<>();
        List<List<String>> reslut = new ArrayList<>();
        if (adjacencyMatrix.length > 0) {
            findCycle(v, trace, reslut);
        }
//        if (reslut.size() == 0) {
//            reslut.add("no cycle!");
//        }

        for (List<String> list : reslut) {
            int max = Integer.MAX_VALUE;
            int mark = 0;
            for (int index = 0; index < list.size(); index++) {
                if (Integer.parseInt(list.get(index)) < max) {
                    max = Integer.parseInt(list.get(index));
                    mark = index;
                }
            }
            String[] a = new String[list.size()];

            for (int i = 0; i < list.size(); i++) {
                a[((i - mark) + list.size()) % a.length] = list.get(i);
            }
            StringBuffer sb = new StringBuffer();

            for (int j = 0; j < a.length; j++) {
                if (j < a.length - 1) sb.append(a[j]).append(",");
                else sb.append((a[j]));

            }
            circle.add(sb.toString());

        }

        return circle;
    }

    /**
     * findCycle
     */

    private static void findCycle(int v, List<Integer> trace, List<List<String>> reslut) {
        int j;
        //添加闭环信息
        if ((j = trace.indexOf(v)) != -1) {

            List<String> list = new ArrayList<>();
            while (j < trace.size()) {

                list.add(nodes.get(trace.get(j)));
                j++;
            }
            if (list.size() <= 7 && list.size() > 2) reslut.add(list);

            return;
        }
        trace.add(v);
        for (int i = 0; i < nodes.size(); i++) {
            if (adjacencyMatrix[v][i] == 1) {
                findCycle(i, trace, reslut);
            }
        }
        trace.remove(trace.size() - 1);
    }

}