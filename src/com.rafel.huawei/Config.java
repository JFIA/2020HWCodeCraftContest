package com.rafel.huawei;

public interface Config {
    String DATA_PATH = "data/test_data.txt";
    String GEN_PATH = "data/gen_data.txt";
    String RESULT_PATH = "data/result.txt";
    String OUTPUT_PATH = "data/output_result.txt";

    // 数组长度
    int MAX_NODE_COUNT=280001;
    // 生成节点个数
    int V_COUNT = 11000;
    int E_COUNT = 70000;
}
