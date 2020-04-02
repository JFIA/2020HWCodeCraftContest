package com.rafel.huawei;

import java.io.*;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;

public class FileRead {

    private String localFilePath;

    public FileRead(String localFilePath) {
        this.localFilePath = localFilePath;
    }

    public String getLocalFilePath() {
        return localFilePath;
    }

    public void setLocalFilePath(String localFilePath) {
        this.localFilePath = localFilePath;
    }

    public List<String> getFile() throws IOException {

        long t1=System.currentTimeMillis();

        List<String> dataList = new ArrayList<>();

        File file=new File(localFilePath);

        InputStreamReader reader = new InputStreamReader(new FileInputStream(file), StandardCharsets.UTF_8);

        BufferedReader bufferedReader=new BufferedReader(reader);
        String str=null;
        while ((str = bufferedReader.readLine()) != null){
            dataList.add(str);

        }

        reader.close();
        bufferedReader.close();


        return dataList;
    }
}
