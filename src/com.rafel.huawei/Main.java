package com.rafel.huawei;

import java.io.IOException;
import java.util.List;

public class Main {

    public static void main(String[] args) throws IOException {

        long t1 = System.currentTimeMillis();

        FileRead fileRead = new FileRead("/Users/hujie/Desktop/华为软挑/初赛/test_data.txt");
        List<String> dataList = fileRead.getFile();

        fileRead.setLocalFilePath("/Users/hujie/Desktop/华为软挑/初赛/result.txt");
        List<String> dataResult = fileRead.getFile();

        List<String> reslut = Utils.getResult(dataList);

        int count = 0;
        for (String s : reslut) {

            for (String value : dataResult) {

                if (s.equals(value)) {
                    count++;
                    break;
                }
            }
        }

        System.out.println("acc：" + (count / reslut.size()) * 100 + "% " + "运行时间：" + (System.currentTimeMillis() - t1));

    }
}
