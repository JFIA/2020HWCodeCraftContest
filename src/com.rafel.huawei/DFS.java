package com.rafel.huawei;

import java.io.IOException;
import java.text.DecimalFormat;
import java.util.*;

public class DFS implements Config {

    private int[][] matrix = new int[MAX_NODE_COUNT][MAX_NODE_COUNT];

    private int[][] r_matrix = new int[MAX_NODE_COUNT][MAX_NODE_COUNT];

    private int[] time = new int[MAX_NODE_COUNT];

    private List<List<String>> temp = new ArrayList<>();

    private List<Integer> trace;

    private boolean[] vis = new boolean[MAX_NODE_COUNT];

    private List<List<Integer>> result = new ArrayList<>();

    private int ds = 0;

    private void dfs(int x) {

        vis[x] = true;

        for (int i = 0; i < matrix[x].length; i++) {

            if (matrix[x][i] == 1 && !vis[i]) dfs(i);

        }

        time[ds] = x;
        ds++;

    }

    private List<Integer> rdfs(int x, List<Integer> trace) {
        vis[x] = true;
        trace.add(x);
//        System.out.println(trace);
        for (int i = 0; i < r_matrix[x].length; i++) {
            if (r_matrix[x][i] == 1 && !vis[i]) rdfs(i, trace);

        }

        return trace;
    }

    private void addLine(String startNode, String endNode) {

        int startIndex = Integer.parseInt(startNode);
        int endIndex = Integer.parseInt(endNode);

        if (startIndex >= 0 && endIndex >= 0) {

            matrix[startIndex][endIndex] = 1;
            r_matrix[endIndex][startIndex] = 1;
        }
    }

    public void constructG() throws IOException {
        List<String> dataList = FileRead.getFile();
        // 构建有向图
        for (String i : dataList) {
            String[] temp = i.split(",");
            addLine(temp[0], temp[1]);
        }
    }

    private void impdfs(int head, int cur, int depth, List<Integer> trace) {

        vis[cur] = true;

        trace.add(cur);

        for (int v = 0; v < matrix[cur].length; v++) {

            if (matrix[cur][v] == 1) {

                if (head == v && depth >= 3 && depth <= 7) {

//            if (trace.size() - j < 3 || trace.size() - j > 7) return;

                    List<String> circles = new ArrayList<>();
                    for (int i : trace) {

                        circles.add(String.valueOf(i));

                    }
//                    System.out.println(circles);

                    temp.add(circles);
                }
                if (depth < 7 && !vis[v] && head < v) {

                    impdfs(head, v, depth + 1, trace);

                }
            }

        }

        vis[cur] = false;
        trace.remove(trace.size() - 1);
    }

    public static void main(String[] args) throws IOException {

        long t = System.currentTimeMillis();

        DFS dfsMethod = new DFS();

        DecimalFormat df = new DecimalFormat("0.00");

        dfsMethod.constructG();

        System.out.println("有向图构建完成" + "运行时间(s)：" + (System.currentTimeMillis() - t) * 0.001);

        for (int i = 0; i < dfsMethod.matrix.length; i++) {

            if (!dfsMethod.vis[i]) {
                dfsMethod.dfs(i);
            }
        }

        System.out.println("第一遍dfs完成" + " " + (System.currentTimeMillis() - t) * 0.001);

        dfsMethod.vis = null;

        dfsMethod.vis = new boolean[MAX_NODE_COUNT];

        for (int i = dfsMethod.matrix.length - 1; i >= 0; i--) {

            List<Integer> trace = new ArrayList<>();
            if (!dfsMethod.vis[dfsMethod.time[i]]) {
                List<Integer> circle = dfsMethod.rdfs(dfsMethod.time[i], trace);
                if (circle.size() >= 3)
                    dfsMethod.result.add(circle);

            }

        }

        System.out.println("反向dfs完成" + " " + (System.currentTimeMillis() - t) * 0.001);

        dfsMethod.vis = null;

        dfsMethod.vis = new boolean[MAX_NODE_COUNT];

        dfsMethod.trace = new ArrayList<>();

        System.out.println(dfsMethod.result.get(0).size());

        for (int j = 0; j < dfsMethod.result.size(); j++) {
            System.out.println("程序运行中:" + df.format((float) j / dfsMethod.result.size() * 100) + "%");
            if (dfsMethod.result.get(j).size() == 3) {
                List<String> circles=new ArrayList<>();
                circles.add(String.valueOf(dfsMethod.result.get(j).get(0)));
                circles.add(String.valueOf(dfsMethod.result.get(j).get(2)));
                circles.add(String.valueOf(dfsMethod.result.get(j).get(1)));
                dfsMethod.temp.add(circles);

            } else {
                for (int i = 0; i < dfsMethod.result.get(j).size(); i++) {
                    dfsMethod.impdfs(dfsMethod.result.get(j).get(i), dfsMethod.result.get(j).get(i), 1, dfsMethod.trace);
                }
            }
        }
//        for (int i = 0; i < dfsMethod.result.size(); i++) {
//            System.out.println("程序运行中:" + df.format((float) i / dfsMethod.result.size() * 100) + "%");
//            dfsMethod.impdfs(dfsMethod.result.get(i), dfsMethod.result.get(i), 1, dfsMethod.trace);
//        }

        List<String> circle = new ArrayList<>();

        // 数组排序
        dfsMethod.temp.sort((a1, a2) -> {

            if (a1.size() == a2.size()) {
                for (int i = 0; i < a1.size(); i++) {
                    if (a1.get(i).equals(a2.get(i)))
                        continue;
                    return Integer.parseInt(a1.get(i))-Integer.parseInt(a2.get(i));
                }

            } else return a1.size() - a2.size();

            return 0;
        });

        // 输出转换成字符串
        for (List<String> list : dfsMethod.temp) {

            StringBuffer sb = new StringBuffer();

            for (int j = 0; j < list.size(); j++) {
                if (j < list.size() - 1) sb.append(Integer.parseInt(list.get(j))).append(",");
                else sb.append((Integer.parseInt(list.get(j))));

            }

            circle.add(sb.toString());

        }

        System.out.println(circle);

        System.out.println((System.currentTimeMillis() - t) * 0.001 + " " + circle.size());

    }

}
