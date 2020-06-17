package com.rafel.huawei;

import javax.swing.*;
import java.text.DecimalFormat;
import java.util.*;

class Utils {

    private static List<Integer> trace;

    private static Set<Integer> searched = new HashSet<>();

    private static List<List<String>> temp = new ArrayList<>();

    private static int[][] matrix = Digraph.adjacencyMatrix;

    private static List<String> nodes = Digraph.getNodes();

    private static boolean[] visited;

    private static boolean isSame(String s1, String s2) {
        return s1.equals(s2);
    }

    private static String sortId(List<String> list) {

        int min = Integer.MAX_VALUE;
        int mark = 0;
        for (int index = 0; index < list.size(); index++) {
            if (Integer.parseInt(list.get(index)) < min) {
                min = Integer.parseInt(list.get(index));
                mark = index;
            }
        }

        String[] a = new String[list.size()];

        // 数组循环左移，保持相对位置不变
        for (int i = 0; i < list.size(); i++) {
            a[((i - mark) + list.size()) % a.length] = list.get(i);
        }
        StringBuffer sb = new StringBuffer();

        for (int j = 0; j < a.length; j++) {
            if (j < a.length - 1) sb.append(a[j]).append(",");
            else sb.append((a[j]));

        }

        return sb.toString();
    }

    private static void dfs(int head, int cur, int depth, List<Integer> trace) {
        visited[cur] = true;
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
                if (depth < 7 && !visited[v] && head < v) {

                    dfs(head, v, depth + 1, trace);

                }
            }

        }
        visited[cur] = false;
        trace.remove(trace.size() - 1);
    }

    // 剪枝改进版dfs
    private static void impDfs(int v, int depth) {
        int j = trace.indexOf(v);
        if (j != -1 && depth >= 3 && depth <= 7) {

//            if (trace.size() - j < 3 || trace.size() - j > 7) return;

            List<String> circles = new ArrayList<>();
            while (j < trace.size()) {

                circles.add(String.valueOf(trace.get(j)));
                j++;
            }
            //System.out.println(circles);

            temp.add(circles);
            return;
        }

        trace.add(v);
        for (int i = 0; i < matrix.length; i++) {

            if (matrix[v][i] == 1) {

                searched.add(i);
                impDfs(i, depth + 1);


            }

        }
//        System.out.println(Digraph.getNodes().get(trace.get(trace.size()-1)));
        trace.remove(trace.size() - 1);
    }


    static List<String> getResult(List<String> dataList) {

        long t = System.currentTimeMillis();

        Digraph digraph = new Digraph();

        DecimalFormat df = new DecimalFormat("0.00");

        // 构建有向图
        for (String i : dataList) {
            String[] temp = i.split(",");
            digraph.addLine(temp[0], temp[1]);
        }

        System.out.println("有向图构建完成" + "运行时间(s)：" + (System.currentTimeMillis() - t) * 0.001);

        visited = new boolean[matrix.length];
        trace = new ArrayList<>();

        for (int i = 0; i < matrix.length; i++) {

//            System.out.println("程序运行中:" + df.format((float) i / matrix.length * 100) + "%");
//                if (searched.contains(i))
//                    continue;
//                trace = new ArrayList<>();
//                impDfs(i, i, , 1);
            dfs(i, i, 1, trace);
        }

        //        List<List<String>> temp = new ArrayList<>();

        List<String> result = new ArrayList<>();

        List<String> circle = new ArrayList<>();

        // 将所有结果添加到临时列表，包括重复的
//        for (int i = 0; i < digraph.getNodes().size(); i++) {
//            temp.addAll(digraph.find(i));
//
//        }


        // 数组排序去重
        temp.sort((a1, a2) -> {

            if (a1.size() == a2.size()) {
                for (int i = 0; i < a1.size(); i++) {
                    if (a1.get(i).equals(a2.get(i)))
                        continue;
                    return a1.get(i).compareTo(a2.get(i));
                }

            } else return a1.size() - a2.size();

            return 0;
        });

        // 转换成字符串
        for (List<String> list : temp) {

            StringBuffer sb = new StringBuffer();

            for (int j = 0; j < list.size(); j++) {
                if (j < list.size() - 1) sb.append(list.get(j)).append(",");
                else sb.append((list.get(j)));

            }

            circle.add(sb.toString());

        }

//        result.add(circle.get(0));
//        for (int i = 1; i < temp.size(); i++) {
//            if (!isSame(circle.get(i), circle.get(i - 1))) result.add(circle.get(i));
//        }

        return circle;
    }

}
