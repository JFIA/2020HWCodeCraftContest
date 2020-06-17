package com.rafel.huawei;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;

public class FileRead implements Config{

    private List<String> resultList = new ArrayList<>();

    private static List<String> dataList = new ArrayList<>();


    public List<String> getResultFile() throws IOException {

        File file = new File(RESULT_PATH);

        InputStreamReader reader = new InputStreamReader(new FileInputStream(file), StandardCharsets.UTF_8);

        BufferedReader bufferedReader = new BufferedReader(reader);
        String str = null;
        while ((str = bufferedReader.readLine()) != null) {
            resultList.add(str);

        }

        reader.close();
        bufferedReader.close();

        return resultList;
    }

    public static List<String> getFile() throws IOException {

        long t1 = System.currentTimeMillis();

        File file = new File("data/test_data2.txt");

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
}
