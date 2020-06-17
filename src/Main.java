import java.io.*;
import java.util.*;
import java.util.concurrent.*;
public class Main{
    public static boolean DEBUG = false;
    public  ArrayList<Integer> vertex = new ArrayList<Integer>();     //顶点集合
    public  ConcurrentHashMap<Integer,ArrayList<Integer>> adjacency_list = new ConcurrentHashMap<>(); //顶点邻接表
    public  HashMap<Integer, ArrayList<Integer>> Inverse_adjlist = new HashMap<>();    // 逆邻接表
    public  ArrayList<ArrayList<Integer>> ans =
            new ArrayList<ArrayList<Integer>>(4000000);         //答案，初始大小为400w,放置扩容
    public  HashMap<Integer,Integer> mapId = new HashMap<>();                //顶点映射
    public static String test_data ="src/data/2755223/test_data.txt";
    public static String ans_data ="src/data/tmp.txt";
    public static int MutiWriteThreshold = 250;
    public static int MutiFindCycle = 6000;
    public static int vertexMax = 150000;
    /**
     * 获取所有顶点和它们的邻接表
     */
    public void getVer_Edge() {
        parallelRead reader = new parallelRead(test_data);
        reader.getData();
        //读入数据 +  拓扑剪枝
        ConcurrentHashMap<Integer,Integer> fromFre = reader.fromFre;
        this.adjacency_list =reader.adjacency_list;
        CopyOnWriteArrayList<DataCell> dataSource = reader.dataSource;
        HashSet<Integer> vset = new HashSet<>(reader.adjacency_list.keySet());
        System.out.println("vertex number "+vset.size()+"");
        pruningPre(fromFre,this.adjacency_list,vset);
        this.vertex = new ArrayList<>(vset);
        // 映射
        Collections.sort(this.vertex);
        ArrayList<Integer> newVertex = new ArrayList<Integer>();
        HashMap<Integer,Integer> idMap = new HashMap<>();
        //映射节点
        int id = 1;
        for(Integer v:this.vertex){
            newVertex.add(id);
            mapId.put(id,v);
            idMap.put(v,id);
            id++;
        }
        this.vertex = newVertex;
        //重置邻接表+构建逆邻接表
        this.adjacency_list.clear();
        DataCell cell  = null;
        for(DataCell c:dataSource){
            cell = c;
            // 若不存在删除之后的点集合中就返回
            if(!vset.contains(cell.IDFrom)||!vset.contains(cell.IDTo)){
                continue;
            }
            cell.IDFrom = idMap.get(cell.IDFrom);
            cell.IDTo = idMap.get(cell.IDTo);
            if(cell.IDFrom==null||cell.IDTo==null){
                continue;
            }
            // 邻接表
            if(!this.adjacency_list.containsKey(cell.IDFrom)){
                ArrayList<Integer> val =  new ArrayList<Integer>();
                val.add(cell.IDTo);
                this.adjacency_list.put(cell.IDFrom,val);
            }else {
                ArrayList<Integer> valFrom = this.adjacency_list.get(cell.IDFrom);
                valFrom.add(cell.IDTo);
            }
            //加上次句，放置邻接表空指针
            if(!this.adjacency_list.containsKey(cell.IDTo)){
                this.adjacency_list.put(cell.IDTo,new ArrayList<>());
            }
            //逆邻接表
            if(!Inverse_adjlist.containsKey(cell.IDTo)){
                ArrayList<Integer> val =  new ArrayList<Integer>();
                val.add(cell.IDFrom);
                Inverse_adjlist.put(cell.IDTo,val);
            }else {
                ArrayList<Integer> valTo = Inverse_adjlist.get(cell.IDTo);
                valTo.add(cell.IDFrom);
            }
            //加上次句，防治邻接表空指针
            if(!Inverse_adjlist.containsKey(cell.IDFrom)){
                Inverse_adjlist.put(cell.IDFrom,new ArrayList<>());
            }
        }
        System.out.println("after topo vertex =  " +this.vertex.size());
        //根据顶点数修改配置
    }
    public void pruningPre( ConcurrentHashMap<Integer,Integer>fre,ConcurrentHashMap<Integer, ArrayList<Integer>> link,
                            HashSet<Integer> vset){
        Queue<Integer> zeroDegree = new LinkedList<>();
        for (Integer v : vset) {
            if (fre.get(v)==null||fre.get(v)==0)
                zeroDegree.add(v);
        }
        while (!zeroDegree.isEmpty()) {
            Integer cur = zeroDegree.poll();
            vset.remove(cur);     // 从可行的顶点集合删去
            //遍历邻接表
            for (Integer next : link.get(cur)) {
                int d = fre.get(next);
                fre.put(next,d-1);
                if (d==1)
                    zeroDegree.add(next);
            }
        }
    }
    /**
     * 写入文件
     */
    public void writeData(){
        System.out.println("cycle count = " + this.ans.size());
        Collections.sort(this.ans,new sortComp());
        //先删除本地文件
        if (DEBUG){
            File anFile = new File(ans_data);
            if(anFile.canRead())
                if(!anFile.delete())
                    System.out.println(ans_data+"删除失败");
        }
        //开始写入文件
        BufferedWriter out = null;
        try {
            out = new BufferedWriter(new FileWriter(ans_data,true));
            out.write(this.ans.size()+"\n");
        } catch (IOException e) {
            e.printStackTrace();
        }
        /*
            根据ans大小决定是否开启线程
         */
        if(this.ans.size()>2.2*MutiWriteThreshold){
            ExecutorService executor = Executors.newFixedThreadPool(4);
            int step = MutiWriteThreshold;
            int start = 0;
            ArrayList<Future> allFutrue = new ArrayList<>();
            while (start<=this.ans.size()-1){
                int end = Math.min(start+step,this.ans.size()-1);
                final  int f_st = start;
                final   ArrayList<ArrayList<Integer>> f_ans = this.ans;
                Future t1= executor.submit(new Callable<String>() {
                    @Override
                    public String call() throws Exception {
                        StringBuilder ansStr = new StringBuilder();
                        for (int j = f_st; j <=end; j++) {
                            ArrayList<Integer> e = f_ans.get(j);
                            for(Integer i:e)
                                ansStr.append(String.valueOf(i)+",");
                            ansStr.deleteCharAt(ansStr.length()-1);
                            ansStr.append("\n");
                        }
                        return ansStr.toString();
                    }
                });
                allFutrue.add(t1);
                if(end==this.ans.size()-1) break;
                start=end+1;
                step+=5;
            }
            for(int i=0;i<allFutrue.size();i++){
                try {
                    String ansLine = (String)allFutrue.get(i).get();
                    out.write(ansLine);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            executor.shutdown();
            try {
                out.flush();
                out.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }else {
            StringBuilder ansStr = new StringBuilder();
            for (int j = 0; j <this.ans.size(); j++) {
                ArrayList<Integer> e = this.ans.get(j);
                for(Integer i:e)
                    ansStr.append(String.valueOf(i)+",");
                ansStr.deleteCharAt(ansStr.length()-1);
                ansStr.append("\n");
            }
            try {
                out.write(ansStr.toString());
                out.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
    //类似双向DFS
    public void Bi_DFS(){
        // 单线程执行
        if(this.vertex.size()<2*MutiFindCycle){
            System.out.println("single  find cycle....");
            oneTreadFindCycle();
            return;
        }
        System.out.println("mutithread  find cycle....");
        ExecutorService executor = Executors.newFixedThreadPool(4);
        final ArrayList<Integer> vertex = this.vertex;
        int step = MutiFindCycle;
        int start = 0;
        ArrayList<Future> allFutrue = new ArrayList<>();
        while (start<=this.vertex.size()-1){
            int end = Math.min(start+step,this.vertex.size()-1);
            final  int f_st = start;
            final  HashMap<Integer,Integer> f_mapId = this.mapId;
            final  HashMap<Integer, ArrayList<Integer>> f_Inverse_adj = this.Inverse_adjlist;
            Future t1= executor.submit(new Callable<ArrayList<ArrayList<Integer>>>() {
                @Override
                public ArrayList<ArrayList<Integer>> call() throws Exception {
                    ArrayList<ArrayList<Integer>> ansSplit = new  ArrayList<ArrayList<Integer>>(400000);
                    LinkedList<Integer> path = new LinkedList<>();
                    boolean[] visit  = new boolean[vertexMax];
                    HashSet<Integer> ansSet1 = new HashSet<>();
                    HashSet<Integer> ansSet2 = new HashSet<>();
                    HashSet<Integer> ansSet3 = new HashSet<>();
                    ArrayList<ArrayList<Integer>> ans_split = new ArrayList<ArrayList<Integer>>(500000);
                    for (int j = f_st; j <=end; j++) {
                        int start = vertex.get(j);
                        int minId = start;
                        path.clear();
                        path.add(start);
                        visit[start] = true;
                        ansSet1.clear();ansSet2.clear();ansSet3.clear();
//                      3重 for 循环实现,替代 dfs
                        for (int i:f_Inverse_adj.get(start)){
                            if( i<=minId) continue;
                            ansSet1.add(i);ansSet2.add(i);ansSet3.add(i);
                            for(int k:f_Inverse_adj.get(i)){
                                if(k<=minId) continue;
                                ansSet2.add(k);ansSet3.add(k);
                                for(int m:f_Inverse_adj.get(k)){
                                    if(m<=minId) continue;
                                    ansSet3.add(m);
                                }
                            }
                        }
                        ansSet1.add(start);
                        ansSplit.clear();
                        dfs(minId,start,ansSet1,ansSet2,ansSet3,visit,path,ansSplit);
                        if(DEBUG&&start%1000==0)
                            System.out.println(j+"-->"+ans_split.size());
                        if(ansSplit.size()>0){
                            Collections.sort(ansSplit,new sortComp());
                            //映射回去
                            for (ArrayList<Integer> e:ansSplit){
                                for (int i = 0; i <e.size(); i++) {
                                    e.set(i,f_mapId.get(e.get(i)));
                                }
                            }
                            ans_split.addAll(ansSplit);
                        }
                    }
                    return ans_split;
                }
            });
            allFutrue.add(t1);
            if(end==this.vertex.size()-1) break;
            start=end+1;
            step += 5;
        }
        for(Future t:allFutrue){
            try {
                ArrayList<ArrayList<Integer>> ansLine = (ArrayList<ArrayList<Integer>>)t.get();
                for(ArrayList<Integer> l:ansLine)
                    this.ans.add(l);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        executor.shutdown();
        System.out.println("find "+this.ans.size()+" cycle");
    }
    public void  oneTreadFindCycle(){
        ArrayList<ArrayList<Integer>> ansSplit = new  ArrayList<ArrayList<Integer>>(3000);
        LinkedList<Integer> path = new LinkedList<>();
        boolean[] visit  = new boolean[vertexMax];
        HashSet<Integer> ansSet1 = new HashSet<>();
        HashSet<Integer> ansSet2 = new HashSet<>();
        HashSet<Integer> ansSet3 = new HashSet<>();
        for (int j = 0; j <this.vertex.size(); j++) {
            int start = this.vertex.get(j);
            int minId = start;
            path.clear();
            path.add(start);
            visit[start] = true;
            ansSet1.clear();ansSet2.clear();ansSet3.clear();
            for (int i:this.Inverse_adjlist.get(start)){
                if( i<=minId) continue;
                ansSet1.add(i);ansSet2.add(i);ansSet3.add(i);
                for(int k:Inverse_adjlist.get(i)){
                    if(k<=minId) continue;
                    ansSet2.add(k);ansSet3.add(k);
                    for(int m:Inverse_adjlist.get(k)){
                        if(m<=minId) continue;
                        ansSet3.add(m);
                    }
                }
            }
            ansSet1.add(start);
            ansSplit.clear();
            dfs(minId,start,ansSet1,ansSet2,ansSet3,visit,path,ansSplit);
            if(ansSplit.size()>0){
                //映射回去
                for (ArrayList<Integer> e:ansSplit){
                    for (int i = 0; i <e.size(); i++) {
                        e.set(i,this.mapId.get(e.get(i)));
                    }
                }
                this.ans.addAll(ansSplit);
            }
        }
    }
    public void dfs(int  minid,int cur,
                    HashSet<Integer> ansSet1, HashSet<Integer> ansSet2,
                    HashSet<Integer> ansSet3,
                    boolean[] visit, LinkedList<Integer> path,
                    ArrayList<ArrayList<Integer>> ans
    ){
        if(path.size()<=6&&path.size()>=3&&ansSet1.contains(cur)){
            ans.add(new ArrayList<>(path));
        }
        if(path.size()==7) return ;
        ArrayList<Integer> link = this.adjacency_list.get(cur);
        for(int next:link){
            if(next>minid&&(!visit[next])){
                if(path.size()==4&&!ansSet3.contains(next))continue;
                else if(path.size()==5&&!ansSet2.contains(next)) continue;
                else if(path.size()==6) {
                    if(ansSet1.contains(next)){
                        ArrayList<Integer> l = new ArrayList<>(path);
                        l.add(next);
                        ans.add(l);
                    }
                    continue;
                }
                visit[next] = true;
                path.add(next);
                dfs(minid,next,ansSet1,ansSet2,ansSet3,visit,path,ans);
                path.removeLast();
                visit[next] = false;
            }
        }
    }
    public static void main(String[] args) {
        long startTimeall=System.currentTimeMillis();
        long startTime=System.currentTimeMillis();
        Main  main = new Main();
        if (args.length >= 1&&args[0].equals("debug")) DEBUG = true;
        if(!DEBUG){
            Main.test_data= "data/temp.txt";
            Main.ans_data ="data/result.txt";
        }
        main.getVer_Edge();
        long endTime=System.currentTimeMillis();                                     //获得当前时间
        System.out.printf("load data and Construct Graph time  %f\n",(endTime-startTime)*1.0/1000);
        startTime=System.currentTimeMillis();
        main.Bi_DFS();
        endTime=System.currentTimeMillis();                                     //获得当前时间
        System.out.printf("find cycle time %f\n",(endTime-startTime)*1.0/1000);
        startTime=System.currentTimeMillis();
        main.writeData();
        endTime=System.currentTimeMillis();                //获得当前时间
        System.out.printf("write data time  %f\n",(endTime-startTime)*1.0/1000);
        endTime=System.currentTimeMillis();                //获得当前时间
        System.out.printf("all time  %f\n",(endTime-startTimeall)*1.0/1000);
    }
}
// 并行读取
class parallelRead {
    static ExecutorService executor;
    CountDownLatch count;
    public int threadNumInOneFile = 4;
    public char endCh = '\n';
    private String readFile = "";
    public parallelRead(String readFile) {
        this.readFile = readFile;
    }
    public  ConcurrentHashMap<Integer,ArrayList<Integer>> adjacency_list =
            new ConcurrentHashMap<>(200000);//顶点邻接表
    public  ConcurrentHashMap<Integer,Integer> fromFre =
            new ConcurrentHashMap<>(200000); //入度集合
    public  CopyOnWriteArrayList<DataCell> dataSource = new CopyOnWriteArrayList<DataCell> ();
    public static double MutiThreadDataSize = 3*0.73;
    StringBuffer ansStr = new StringBuffer();
    public  void  getData(){
        long startTime=System.currentTimeMillis();
        RandomAccessFile accessFile = null;
        try {
            accessFile = new RandomAccessFile(this.readFile, "r");
            double fileSize = accessFile.length()*1.0/1024/1024;
            System.out.printf("file size %.4f\n",fileSize);
            if(fileSize>MutiThreadDataSize){
                System.out.println("muti thread  read");
                threadPoolInFile();
            }else{
                System.out.println("single thread  read");
                singleThread();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        long endTime=System.currentTimeMillis();
        System.out.printf("load data  time  %.5f\n",(endTime-startTime)*1.0/1000);
    }
    public void singleThread(){
        try {
            RandomAccessFile accessFile = new RandomAccessFile(this.readFile, "r");
            int len = (int)accessFile.length();
            byte[] bs = new byte[len];
            accessFile.read(bs,0,len);
            String line = new String(bs);
            accessFile.close();
            createGraph(line);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    public void threadPoolInFile() {
        if (!this.readFile.equals("")) {
            executor = new ThreadPoolExecutor(4, 4, 60L,
                    TimeUnit.SECONDS, new LinkedBlockingQueue<>());
            count = new CountDownLatch(threadNumInOneFile);
            read(threadNumInOneFile,this.readFile);
            try {
                count.await();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            executor.shutdown();
        }
    }
    /**
     * 计算每个线程在文件中的开始和结束位置，并开启线程
     */
    public void read(int threadCount, String filePath) {
        try {
            RandomAccessFile accessFile = new RandomAccessFile(filePath, "r");
            long fileLength = accessFile.length();
            long gap = fileLength / threadCount;
            long start[] = new long[threadCount];
            long end[] = new long[threadCount];
            long checkIndex = 0;
            for (int i = 0; i < threadCount; i++) {
                start[i] = checkIndex;
                if (i + 1 == threadCount) {
                    end[i] = fileLength;
                    break;
                }
                checkIndex += gap;
                long stepGap = getGapVal(checkIndex, accessFile, endCh);
                checkIndex += stepGap;
                end[i] = checkIndex;
            }
            runTask(threadCount, start, end, filePath, null);
        } catch (Exception e) {
        }
    }
    /**
     * 防止将文件分给不同的线程后，同一行被拆开
     */
    private long getGapVal(long checkIndex, RandomAccessFile accessFile, char c) throws Exception {
        accessFile.seek(checkIndex);
        long count = 0;
        char ch;
        while ((ch = (char) accessFile.read()) != c) count++;
        return ++count;
    }
    /**
     * 开启线程
     */
    public void runTask(int threadCount, long[] start, long[] endIndex, String filePath,
                        RandomAccessFile accessFile) throws Exception {
        for (int i = 0; i < threadCount; i++) {
            long begin = start[i];
            long end = endIndex[i];
            executor.execute(() -> {
                try {
                    readData(begin, end, filePath, null);
                    count.countDown();
                } catch (Exception e) {
                    System.out.printf(e.toString());
                }
            });
        }
    }
    /**
     * 线程读取其所对应要读的文件范围
     */
    public void readData(long start, long end, String filePath, RandomAccessFile accessFile)
            throws FileNotFoundException, IOException {
        if (null == accessFile) {
            accessFile = new RandomAccessFile(filePath, "r");
        }
        int len = (int)(end-start);
        accessFile.seek(start);
        ArrayList<DataCell> cellDate = new ArrayList<>();
        byte[] bs = new byte[len];
        accessFile.read(bs,0,len);
        accessFile.close();
        String line = new String(bs);
        createGraph(line.trim());
    }

    public void createGraph(String lines){
        String[] splits = lines.split("\n");
        DataCell cell = null;
        ArrayList<DataCell> cells = new ArrayList<>(100000);
        for(String line :splits){
            cell =  new DataCell(line);
            cells.add(cell);
            if(!this.adjacency_list.containsKey(cell.IDFrom)){
                ArrayList<Integer> val =  new ArrayList<Integer>();
                val.add(cell.IDTo);
                this.adjacency_list.put(cell.IDFrom,val);
            }else {
                ArrayList<Integer> valFrom = this.adjacency_list.get(cell.IDFrom);
                valFrom.add(cell.IDTo);
            }
            //加上次句，防止邻接表空指针
            if(!this.adjacency_list.containsKey(cell.IDTo)){
                this.adjacency_list.put(cell.IDTo,new ArrayList<>());
            }
            if(!fromFre.containsKey(cell.IDTo)){
                fromFre.put(cell.IDTo,1);
            }else fromFre.put(cell.IDTo,fromFre.get(cell.IDTo)+1);
        }
        dataSource.addAll(cells);
    }
}
//####################  获取环    ######################
// 图数据对象
class DataCell{
    public Integer IDFrom =0;
    public Integer IDTo =0;
    public Integer amount = 0;
    public DataCell(String line) {
        String[] row = line.trim().split(",");
        try {
            this.IDFrom = Integer.valueOf(row[0]);
            this.IDTo = Integer.valueOf(row[1]);
            this.amount =Integer.valueOf(row[2]);
        }catch (Exception e){
            int a=1;
        }

    }
}
// 用来排序
class sortComp implements Comparator<ArrayList<Integer>>{
    @Override
    public int compare(ArrayList<Integer> o1, ArrayList<Integer> o2) {
        if(o1.size()!=o2.size())
            return o1.size()-o2.size();
        for (int i = 0; i < o1.size(); i++) {
            if(o1.get(i)== o2.get(i))
                continue;
            return o1.get(i)-o2.get(i);
        }
        return  0;
    }
}