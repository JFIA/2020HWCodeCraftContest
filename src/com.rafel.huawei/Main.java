package com.rafel.huawei;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.*;

public class Main {

    // 数组长度
    private int MAX_NODE_COUNT=280001;

    private HashMap<Integer, List<Integer>> map = new HashMap<>();

    private HashMap<Integer, Integer> idMap = new HashMap<>();

    private List<Integer> nodes = new ArrayList<>();

    private HashMap<Integer, List<Integer>> r_map = new HashMap<>();

    private LinkedList<LinkedList<Integer>> temp = new LinkedList<>();

    public void constructGraph() throws IOException {

        List<String> dataList = getFile();

        HashSet<Integer> all_nodes = new HashSet<>();

        for (String i : dataList) {
            String[] temp = i.split(",");

            Integer startIndex = Integer.parseInt(temp[0]);
            Integer endIndex = Integer.parseInt(temp[1]);

            all_nodes.add(startIndex);

            all_nodes.add(endIndex);
//            if (!temp_map.containsKey(startIndex)) {
//                List<Integer> list = new ArrayList<>();
//                list.add(endIndex);
//                temp_map.put(startIndex, list);
//
//
//            } else {
//                List<Integer> list = temp_map.get(startIndex);
//                list.add(endIndex);
//
//            }
//            if (!temp_rmap.containsKey(endIndex)) {
//                List<Integer> list = new ArrayList<>();
//                list.add(startIndex);
//                temp_rmap.put(endIndex, list);
//
//            } else {
//                List<Integer> list = temp_rmap.get(endIndex);
//                list.add(startIndex);
//
//            }
        }

        nodes = new ArrayList<>(all_nodes);
        Collections.sort(nodes);
        List<Integer> newNodes = new ArrayList<>();
        HashMap<Integer, Integer> idHash = new HashMap<>();

        int index = 1;

        for (Integer id : nodes) {

            newNodes.add(index);
            idMap.put(index, id);
            idHash.put(id, index);

            index++;

        }

        nodes = newNodes;

        for (String i : dataList) {
            String[] temp = i.split(",");

            Integer startIndex = idHash.get(Integer.parseInt(temp[0]));
            Integer endIndex = idHash.get(Integer.parseInt(temp[1]));

            if (!map.containsKey(startIndex)) {
                List<Integer> list = new ArrayList<>();
                list.add(endIndex);
                map.put(startIndex, list);

            } else {
                List<Integer> list = map.get(startIndex);
                list.add(endIndex);

            }
            if (!r_map.containsKey(endIndex)) {
                List<Integer> list = new ArrayList<>();
                list.add(startIndex);
                r_map.put(endIndex, list);

            } else {
                List<Integer> list = r_map.get(endIndex);
                list.add(startIndex);

            }
        }

//        for (int i : map.keySet()) {
//
//            map.get(i).sort(Comparator.comparingInt(Integer::intValue));
//
//        }
//
//        for (int i : r_map.keySet()) {
//            r_map.get(i).sort(Comparator.comparingInt(Integer::intValue));
//
//        }

    }

    private void dfs(boolean[] vis, int head, int cur, int depth, List<Integer> trace, HashSet<Integer> pathSet1,
                     HashSet<Integer> pathSet2, HashSet<Integer> pathSet3) {

        if (depth >= 3 && depth <= 6 && pathSet1.contains(cur)) {

            LinkedList<Integer> circle = new LinkedList<>();

            for (int i : trace) {

                circle.add(idMap.get(i));
            }

            temp.add(circle);
        }

        if (map.get(cur) == null) {

            return;
        }

        List<Integer> list = map.get(cur);

        for (int next : list) {

            if (depth < 7 && !vis[next] && head < next) {

                if (depth == 4 && !pathSet3.contains(next)) continue;
                else if (depth == 5 && !pathSet2.contains(next)) continue;
                else if (depth == 6) {
                    if (pathSet1.contains(next)) {
                        LinkedList<Integer> circle = new LinkedList<>();
                        for (int i : trace) {
                            circle.add(idMap.get(i));
                        }
                        circle.add(idMap.get(next));
                        temp.add(circle);
                    }
                    continue;
                }

                vis[next] = true;

                trace.add(next);

                dfs(vis, head, next, depth + 1, trace, pathSet1, pathSet2, pathSet3);

                vis[next] = false;
                trace.remove(trace.size() - 1);

            }


        }

    }

    public List<String> getFile() throws IOException {

        long t1 = System.currentTimeMillis();

        List<String> dataList=new ArrayList<>();

        String DATA_PATH = "/data/test_data.txt";
        File file = new File(DATA_PATH);

        InputStreamReader reader = new InputStreamReader(new FileInputStream(file), StandardCharsets.UTF_8);

        BufferedReader bufferedReader = new BufferedReader(reader);
        String str = null;
        while ((str = bufferedReader.readLine()) != null) {
            dataList.add(str);

        }

        reader.close();
        bufferedReader.close();

        System.out.println("读取时间：" + (System.currentTimeMillis() - t1)*0.001);

        return dataList;
    }

    public void writeFile() throws IOException {

        String OUTPUT_PATH = "/projects/student/result.txt";
        File file = new File(OUTPUT_PATH);
        FileOutputStream fos = new FileOutputStream(file);
        BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(fos));

        writer.write(temp.size() + "\n");

        for (List<Integer> list : temp) {

            for (int j = 0; j < list.size(); j++) {
                if (j < list.size() - 1) writer.write(list.get(j) + ",");

                else writer.write(list.get(j) + "\n");

            }
        }
        writer.close();
        fos.close();

    }

    public void sortCircle() {

        // 数组排序
        temp.sort((a1, a2) -> {

            if (a1.size() == a2.size()) {
                for (int i = 0; i < a1.size(); i++) {
                    if (a1.get(i) == (a2.get(i)))
                        continue;
                    return a1.get(i) - a2.get(i);
                }

            } else return a1.size() - a2.size();

            return 0;
        });

    }

    public void findCycle(){

        LinkedList<Integer> trace = new LinkedList<>();

        boolean[] vis = new boolean[MAX_NODE_COUNT];
        HashSet<Integer> pathSet1 = new HashSet<>();
        HashSet<Integer> pathSet2 = new HashSet<>();
        HashSet<Integer> pathSet3 = new HashSet<>();


        for (int start : nodes) {

            if (map.get(start) != null && r_map.get(start) != null) {

                trace.clear();
                trace.add(start);
                vis[start] = true;

                pathSet1.clear();
                pathSet2.clear();
                pathSet3.clear();

                for (int i : r_map.get(start)) {
                    if (i <= start && r_map.get(i) != null) continue;
                    pathSet1.add(i);
                    pathSet2.add(i);
                    pathSet3.add(i);
                    if (r_map.get(i) != null)
                        for (int k : r_map.get(i)) {
                            if (k <= start) continue;
                            pathSet2.add(k);
                            pathSet3.add(k);
                            if (r_map.get(k) != null)
                                for (int m : r_map.get(k)) {
                                    if (m <= start) continue;
                                    pathSet3.add(m);
                                }
                        }
                }

                pathSet1.add(start);

                dfs(vis, start, start, 1, trace, pathSet1, pathSet2, pathSet3);

            }
        }

    }

    public static void main(String[] args) throws IOException {

        long t = System.currentTimeMillis();

        Main main=new Main();

        main.constructGraph();

        System.out.println("图构建完成：" + (System.currentTimeMillis() - t) * 0.001);

        main.findCycle();

        System.out.println("查找完成：" + (System.currentTimeMillis() - t) * 0.001);

        main.sortCircle();

        System.out.println("排序完成：" + (System.currentTimeMillis() - t) * 0.001);

        main.writeFile();

        System.out.println("输出完成：" + (System.currentTimeMillis() - t) * 0.001);

    }
}
