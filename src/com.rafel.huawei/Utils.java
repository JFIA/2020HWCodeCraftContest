package com.rafel.huawei;

import java.util.ArrayList;
import java.util.List;

class Utils {

    private static boolean isSame(List<String> list, String s2) {
        if (list.size() == 0) return false;
        for (String str : list) {
            if (str.equals(s2)) return true;
        }
        return false;
    }

    static List<String> getResult(List<String> dataList) {

        Digraph digraph=new Digraph();

        for (String i : dataList) {
            String[] temp = i.split(",");
            digraph.addLine(temp[0], temp[1]);
        }

        List<String> reslut = new ArrayList<>();

        for (int i = 0; i < digraph.getNodes().size(); i++) {

            for (String s : digraph.find(i)) {
                if (!Utils.isSame(reslut, s)) reslut.add(s);

            }

        }
        return reslut;
    }

}
