#include<stdio.h>
#include<string>
#include<vector>
#include<unordered_map>
#include<unordered_set>
#include<set>
#include<algorithm>
#include<time.h>
#include<algorithm>
#include<iostream>
#include<queue>
#include<sys/time.h>
#include<pthread.h>
#include<sys/stat.h>
#include<sys/mman.h> 
#include<unistd.h>
#include<fcntl.h>
#include<thread>
#include<memory.h>

// #include <string.h>
//#define DEBUG
#define MAX_NODE_COUNT 50001
#define MMAP


int FileSize(const char* fname)
{
	struct stat statbuf;
	if (stat(fname, &statbuf) == 0)
		return statbuf.st_size;
	return -1;
}
using namespace std;
const int threadNum = 4;
bool singleThread = false;

bool cmp(vector<unsigned>& a1, vector<unsigned>& a2){

	if (a1.size() == a2.size()) {
		for (int i = 0; i < a1.size(); ++i) {
			if (a1[i] == a2[i])
				continue;
			return a1[i] < a2[i] ? true : false;
		}
	}
	else return a1.size() < a2.size() ? true : false;
	return true;
}

	vector<pair<timeval, string>> timeVec;
	void addTime(const string& info)
	{
#ifdef DEBUG
		timeval timeNow;
		gettimeofday(&timeNow, NULL);
		timeVec.emplace_back(timeNow, info);
#endif
	}
	void printTime()
	{
#ifdef DEBUG
		for (auto timeNow : timeVec)
			printf("%s, tv_sec = %d, tv_usec = %d\n", timeNow.second.c_str(), timeNow.first.tv_sec, timeNow.first.tv_usec);
		timeVec.clear();
#endif
	}

	const int threadNumInOneFile = 4;
	char endCh = '\n';
	string readFile = "/data/test_data.txt";
	static char* memSrc = NULL;
	int filesize;
	const int MutiThreadDataSize = 2 * 1024 * 1024;
	unsigned dataRead[threadNumInOneFile][3];
	static int pthreadInd;
	pthread_mutex_t mutex1;
	unsigned dataSource[threadNumInOneFile][200000][2]; 
	int dataSoup[threadNumInOneFile] = {0};
	int dataSouSize = 0;
//	vector<unordered_map<int, int>> inOutDegree;//0x10 out 0x01 in

	void* readData(void* arg);

	int getGapVal(long checkIndex, char c)
	{
		char* start = memSrc + checkIndex;
		long count = 0;
		while ((*start) != c)
		{
			++start;
			++count;
		}
		return ++count;
	}

	void singleThreadRead()
	{
		dataSouSize = 1;
		dataRead[0][0] = 0;
		dataRead[0][1] = filesize;
		dataRead[0][2] = 0;
		readData(dataRead[0]);
	}

	void read(int threadCount, string filePath) {
		int gap = filesize / threadCount;
		vector<int> start(threadCount);
		vector<int> end(threadCount);
		int checkIndex = 0;
		int i;
		int stepGap;
		for (i = 0; i < threadCount; ++i) {
			start[i] = checkIndex;
			if (i + 1 == threadCount) {
				end[i] = filesize;
				break;
			}
			checkIndex += gap;
			stepGap = getGapVal(checkIndex, endCh);
			checkIndex += stepGap;
			end[i] = checkIndex;
		}
		vector<pthread_t> thread(threadCount);
		
		for (i = 0; i < threadCount; ++i)
		{
			dataRead[i][0] = start[i];
			dataRead[i][1] = end[i];
			dataRead[i][2] = i;
		}
		for (i = 0; i < threadCount; ++i)
		{
			if (pthread_create(&thread[i], NULL, readData, dataRead[i]) != 0)
			{
				printf("pthread_create fail\n");
				return;
			}
		}
		for (i = 0; i < threadCount; ++i)
		{
			if (pthread_join(thread[i], NULL) != 0)
			{
				printf("pthread_join fail\n");
				return;
			}
		}
	}

	void threadPoolInFile()
	{
		dataSouSize = threadNumInOneFile;
		read(threadNumInOneFile, readFile);
	}


	bool getData()
	{
		pthreadInd = 0;
		int fd = open(readFile.c_str(), O_RDONLY);
		if (fd == -1)return false;
		filesize = FileSize(readFile.c_str());
//		memSrc = (char *)malloc(filesize);
		memSrc = (char *)mmap(memSrc, filesize, PROT_READ, MAP_SHARED, fd, 0);
		if (filesize>MutiThreadDataSize){
			threadPoolInFile();
		}
		else{
			singleThreadRead();
		}
		munmap(memSrc, filesize);
		close(fd);
	}

void* readData(void* arg)
{
	int* para = (int *)arg;
	int start = para[0];
	int end = para[1];
	int threadInd = para[2];
	int len = end - start;
	char* memStart = (char *)memSrc + start;
	char* memEnd = (char *)memSrc + end;
	unsigned from = 0, to = 0, val = 0;
	int cnt = 0;
	while (memStart < memEnd)
	{
		from = 0, to = 0, val = 0;
		while (*memStart != ',')
		{
			from = from * 10 + (*memStart - '0');
			++memStart;
		}
		++memStart;
		while (*memStart != ',')
		{
			to = to * 10 + (*memStart - '0');
			++memStart;
		}
		++memStart;
		while (*memStart != '\n')
		{
			++memStart;
		}
		++memStart;
		if(from >= MAX_NODE_COUNT || to >= MAX_NODE_COUNT)continue;
		dataSource[threadInd][dataSoup[threadInd]][0] = from;
		dataSource[threadInd][dataSoup[threadInd]][1] = to;
		++dataSoup[threadInd];
		//			inOutDegree[threadInd][from] |= 0x10;
		//			inOutDegree[threadInd][to] |= 0x01;
	}
}

int size;
int MutiWriteThreshold = 1000;
char* memDest;
char* start;
string writeFile = "/projects/student/result.txt";
int fd;
FILE * file;
int wrFileSize;
unsigned validNodeCnt;
int fileByteSize[6] = {0, 0, 0, 0, 0};


int map[MAX_NODE_COUNT][50];
int r_map[MAX_NODE_COUNT][50];
int pmap[MAX_NODE_COUNT];
int pr_map[MAX_NODE_COUNT];

int indegree[MAX_NODE_COUNT];
bool validInd[MAX_NODE_COUNT];

unsigned nodes[MAX_NODE_COUNT];
unsigned all_nodes[MAX_NODE_COUNT];

unordered_map<unsigned, int> idHash;

int nodeCnt = 0;
unsigned data[threadNum][3];
int trac[threadNum][7];
bool pathSe1[threadNum][MAX_NODE_COUNT];
bool pathSe2[threadNum][MAX_NODE_COUNT];
bool pathSe3[threadNum][MAX_NODE_COUNT];
bool vi[threadNum][MAX_NODE_COUNT] = {false};
int ans3[threadNum][3 * 250000];
int ans4[threadNum][4 * 250000];
int ans5[threadNum][5 * 500000];
int ans6[threadNum][6 * 1000000];
int ans7[threadNum][7 * 1000000];
int ans8[threadNum][7 * 1000000];
char wrHash[MAX_NODE_COUNT][10];
int wrSizeHash[MAX_NODE_COUNT];
int q[threadNum][MAX_NODE_COUNT];
char numHash[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

int* ans[6][threadNum] = {ans3[0], ans3[1], ans3[2], ans3[3],
								ans4[0], ans4[1], ans4[2], ans4[3],
								ans5[0], ans5[1], ans5[2], ans5[3],
								ans6[0], ans6[1], ans6[2], ans6[3],
								ans7[0], ans7[1], ans7[2], ans7[3],
								ans8[0], ans8[1], ans8[2], ans8[3]
								};
int ansp[threadNum][6] = {0};
int ansSize[threadNum][6] = {0};
int frontWr[6] = {0};
int backWr[6] = {0};
unsigned smallNum[MAX_NODE_COUNT] = {0};
	char* writeInt(unsigned i, char* dest)
	{
		vector<unsigned> res(10);
		int index = 0;

		while (i != 0)
		{
			res[index++] = i % 10;
			i /= 10;
		}

		while (index > 0)
		{
			--index;
			*dest = res[index] + '0';
			++dest;
		}
		return dest;
	}

	void writeFileInit(int linesize,int totalSize)
	{
		int res[10];
		int index = 0;
		int num = linesize;
		int firstLineSize = 1;
		if(num == 0)
		{
			firstLineSize += 1;
		}
		while (num != 0)
		{
			res[index++] = num % 10;
			num /= 10;
		}
		firstLineSize += index;
		wrFileSize = firstLineSize + totalSize;
#ifdef DEBUG
			printf("filesize %d\n",firstLineSize + totalSize);
#endif
#ifdef MMAP
		fd = open(writeFile.c_str(), O_RDWR | O_CREAT, 0600);
		ftruncate(fd, wrFileSize);
		memDest = (char *)mmap(NULL, wrFileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		start = memDest;
#else
		file = fopen(writeFile.c_str(), "w+");
#endif
	}

	void writeFileEnd(int linesize)
	{
#ifdef MMAP
		munmap(memDest, wrFileSize);
		close(fd);
#else 
		fclose(file);		
#endif

	}

	void writeFileHead(int filesize)
	{
#ifdef MMAP
		size = filesize;
		start = writeInt(size, start);
		*start = '\n'; ++start;
#else
		fprintf(file, "%d\n", filesize);
#endif
	}
	int cnt5 = 0;

	void writeFileData(int begin, int end, int offset)
	{
		int cnt = 0;
		int threadSplit[threadNum];
		int sum = 0, cursum = 0;
		char* start4;
		int rem ;
		int * ptmp;
		int num;
		int size;
		char * wrChar;
		for (int i = begin; i < end; ++i)
		{
			start4 = start;
			sum = 0;
			cursum = 0;
			for(int m = 0; m < i; ++m)			
			{
				start4 += fileByteSize[m];
			}
			for(int j = 0; j < threadNum; ++j)
			{
				threadSplit[j] = 0;
				sum += ansp[j][i];
			}
			cnt = 0;
			if(sum == 0)continue;
			rem = cnt % threadNum;
			while(cnt < backWr[i] && cursum < sum)
			{
				while(ans[i][rem][threadSplit[rem]] != cnt)
				{
					++cnt;
					rem = cnt % threadNum;
				}
				frontWr[i] = cnt;
				while(ans[i][rem][threadSplit[rem]] == cnt && cursum < sum)
				{
					ptmp = &(ans[i][rem][threadSplit[rem]]);
					for (int k = 0; k < i + 3; ++k)
					{
#ifdef MMAP
						num = *(ptmp + k);
						size = wrSizeHash[num];
						wrChar = wrHash[num];
						for(int tmp = 0; tmp < size; ++tmp)
						{
							*start4 = *wrChar;
							++start4;
							++wrChar;
						}

						*start4 = k == i + 2 ? '\n' : ',';
						++start4;
#else
						fprintf(file, "%d", nums[i]);
						if (i != nums.size() - 1)
						{
							fprintf(file, ",");
						}
						else
						{
							fprintf(file, "\n");
						}
#endif				
					}			
					threadSplit[rem] += i + 3;
					cursum += 3 + i;
				}
#if 0
			for(int m = 0; m < msize; ++m)
			{
				int tmp = ansp[m][i] / (i + 3);
				int* ptmp = ans[i][m];
				for (int j = 0; j < tmp; ++j)
				{
					
					for (int k = 0; k < i + 3; ++k)
					{
#ifdef MMAP
						int num = ptmp[j * (i + 3) + k];
						int size = wrSizeHash[num];
						char * wrChar = wrHash[num];
						for(int tmp = 0; tmp < size; tmp++)
						{
							*start = *wrChar;
							++start;
							++wrChar;
						}

						*start = k == i + 2 ? '\n' : ',';
						++start;
#else
						fprintf(file, "%d", nums[i]);
						if (i != nums.size() - 1)
						{
							fprintf(file, ",");
						}
						else
						{
							fprintf(file, "\n");
						}
#endif				
					}
				}
			}
#endif
			}
		}
	}	
		
	void writeFileDataBack(int begin, int end, int offset)
	{
		int cnt = 0;
		int threadSplit[threadNum];
		int sum = 0, cursum = 0;
		char* start4;
		for (int i = begin; i < end; ++i)
		{
			start4 = start;
			sum = 0;
			for(int m = 0; m <= i; ++m)			
			{
				start4 += fileByteSize[m];
			}
			for(int j = 0; j < threadNum; ++j)
			{
				threadSplit[j] = ansp[j][i] - (i + 3);
				sum += ansp[j][i];
			}
			if(sum == 0)continue;
			if(i != 4)cnt = nodeCnt;
			else cnt = cnt5;
			cursum = 0;
			int rem = cnt % threadNum;
			while(cnt > frontWr[i]  && cursum < sum)
			{
				while(ans[i][rem][threadSplit[rem]] != cnt)
				{
					cnt--;
					rem = cnt % threadNum;
				}
				if(cnt == 0)break;
				backWr[i] = cnt;
				while(ans[i][rem][threadSplit[rem]] == cnt)
				{
					int * ptmp = &(ans[i][rem][threadSplit[rem]]);
					for (int k = i + 2; k >= 0; --k)
					{
#ifdef MMAP
						int num = *(ptmp + k);
						int size = wrSizeHash[num];
						char * wrChar = wrHash[num];
						char * startTmp = start4 - size - 1;
						for(int tmp = 0; tmp < size; ++tmp)
						{
							*startTmp = *wrChar;
							++startTmp;
							++wrChar;
						}

						*startTmp = k == i + 2 ? '\n' : ',';
						start4 -= size + 1;
#else
						fprintf(file, "%d", nums[i]);
						if (i != nums.size() - 1)
						{
							fprintf(file, ",");
						}
						else
						{
							fprintf(file, "\n");
						}
#endif				
					}			
					threadSplit[rem] -= i + 3;
					cursum += 3 + i;
				}
#if 0
			for(int m = 0; m < msize; ++m)
			{
				int tmp = ansp[m][i] / (i + 3);
				int* ptmp = ans[i][m];
				for (int j = 0; j < tmp; ++j)
				{
					
					for (int k = 0; k < i + 3; ++k)
					{
#ifdef MMAP
						int num = ptmp[j * (i + 3) + k];
						int size = wrSizeHash[num];
						char * wrChar = wrHash[num];
						for(int tmp = 0; tmp < size; tmp++)
						{
							*start = *wrChar;
							++start;
							++wrChar;
						}

						*start = k == i + 2 ? '\n' : ',';
						++start;
#else
						fprintf(file, "%d", nums[i]);
						if (i != nums.size() - 1)
						{
							fprintf(file, ",");
						}
						else
						{
							fprintf(file, "\n");
						}
#endif				
					}
				}
			}
#endif
			}
		}
	}	

	void writeFileDataBack5()
		{
			int cnt = 0;
			int threadSplit[threadNum];
			int sum = 0, cursum = 0;
			char* start4;

			start4 = start;
			sum = 0;
			for(int m = 0; m <= 5; ++m) 		
			{
				start4 += fileByteSize[m];
			}
			for(int j = 0; j < threadNum; ++j)
			{
				threadSplit[j] = ansp[j][5] - (7);
				sum += ansp[j][5];
			}
			if(sum == 0)return;
			cnt = nodeCnt;
			cursum = 0;
			int rem = cnt % threadNum;
			while(cnt > frontWr[5] && cursum < sum)
			{
				while(ans[5][rem][threadSplit[rem]] != cnt)
				{
					cnt--;
					rem = cnt % threadNum;
				}
				if(cnt == 0)break;
				backWr[5] = cnt;
				while(ans[5][rem][threadSplit[rem]] == cnt)
				{
					int * ptmp = &(ans[5][rem][threadSplit[rem]]);
					for (int k = 6; k >= 0; --k)
					{
						int num = *(ptmp + k);
						int size = wrSizeHash[num];
						char * wrChar = wrHash[num];
						char * startTmp = start4 - size - 1;
						for(int tmp = 0; tmp < size; ++tmp)
						{
							*startTmp = *wrChar;
							++startTmp;
							++wrChar;
						}

						*startTmp = k == 6 ? '\n' : ',';
						start4 -= size + 1;		
					}			
					threadSplit[rem] -= 7;
					cursum += 7;
				}
#if 0
			for(int m = 0; m < msize; ++m)
			{
				int tmp = ansp[m][i] / (i + 3);
				int* ptmp = ans[i][m];
				for (int j = 0; j < tmp; ++j)
				{
					
					for (int k = 0; k < i + 3; ++k)
					{
#ifdef MMAP
						int num = ptmp[j * (i + 3) + k];
						int size = wrSizeHash[num];
						char * wrChar = wrHash[num];
						for(int tmp = 0; tmp < size; tmp++)
						{
							*start = *wrChar;
							++start;
							++wrChar;
						}

						*start = k == i + 2 ? '\n' : ',';
						++start;
#else
						fprintf(file, "%d", nums[i]);
						if (i != nums.size() - 1)
						{
							fprintf(file, ",");
						}
						else
						{
							fprintf(file, "\n");
						}
#endif				
					}
				}
			}
#endif
			}
		}	

			void writeFileData5()
			{
				int cnt = 0;
				int threadSplit[threadNum];
				int sum = 0, cursum = 0;
				char* start4;
				int rem ;
				int * ptmp;
				int num;
				int size;
				char * wrChar;

					start4 = start;
					sum = 0;
					for(int m = 0; m < 5; ++m)			
					{
						start4 += fileByteSize[m];
					}
					for(int j = 0; j < threadNum; ++j)
					{
						threadSplit[j] = 0;
						sum += ansp[j][5];
					}
					if(sum == 0)return;
					cnt = cnt5;
					rem = cnt % threadNum;
					while(cnt < backWr[5] && cursum < sum)
					{
						while(ans[5][rem][threadSplit[rem]] != cnt)
						{
							++cnt;
							rem = cnt % threadNum;
						}
						frontWr[5] = cnt;
						while(ans[5][rem][threadSplit[rem]] == cnt && cursum < sum)
						{
							ptmp = &(ans[5][rem][threadSplit[rem]]);
							for (int k = 0; k < 7; ++k)
							{
								num = *(ptmp + k);
								size = wrSizeHash[num];
								wrChar = wrHash[num];
								for(int tmp = 0; tmp < size; ++tmp)
								{
									*start4 = *wrChar;
									++start4;
									++wrChar;
								}
		
								*start4 = k == 6 ? '\n' : ',';
								++start4;

							}			
							threadSplit[rem] += 7;
							cursum += 7;
						}
#if 0
					for(int m = 0; m < msize; ++m)
					{
						int tmp = ansp[m][i] / (i + 3);
						int* ptmp = ans[i][m];
						for (int j = 0; j < tmp; ++j)
						{
							
							for (int k = 0; k < i + 3; ++k)
							{
#ifdef MMAP
								int num = ptmp[j * (i + 3) + k];
								int size = wrSizeHash[num];
								char * wrChar = wrHash[num];
								for(int tmp = 0; tmp < size; tmp++)
								{
									*start = *wrChar;
									++start;
									++wrChar;
								}
		
								*start = k == i + 2 ? '\n' : ',';
								++start;
#else
								fprintf(file, "%d", nums[i]);
								if (i != nums.size() - 1)
								{
									fprintf(file, ",");
								}
								else
								{
									fprintf(file, "\n");
								}
#endif				
							}
						}
					}
#endif
					}
			}	
				

	void writeFileDataSingle()
	{
		char* startS = start;
		int num;
		int size;
		char * wrChar;
		int tmp;
		int* ptmp;
		for (int i = 0; i < 5; ++i)
		{
			for(int m = 0; m < 1; ++m)
			{
				tmp = ansp[m][i] / (i + 3);
				ptmp = ans[i][m];
				for (int j = 0; j < tmp; ++j)
				{
					
					for (int k = 0; k < i + 3; ++k)
					{
#ifdef MMAP
						num = ptmp[j * (i + 3) + k];
						size = wrSizeHash[num];
						wrChar = wrHash[num];
						for(int tmp = 0; tmp < size; ++tmp)
						{
							*startS = *wrChar;
							++startS;
							++wrChar;
						}

						*startS = k == i + 2 ? '\n' : ',';
						++startS;
#else
						fprintf(file, "%d", nums[i]);
						if (i != nums.size() - 1)
						{
							fprintf(file, ",");
						}
						else
						{
							fprintf(file, "\n");
						}
#endif				
					}
				}
			}
		}
	}	


/*	bool ConstructGraph()
	{
		int readThread = reader.dataSource.size();
		unsigned src, dest, val;
		
		idMap.reserve(MAX_NODE_COUNT);
		
		for (int i = 0; i < readThread; ++i)
			for (pair<unsigned, unsigned>& data : reader.dataSource[i])
		{
			src = data.first;
			dest = data.second;
			if (all_nodes.find(src) == all_nodes.end())
			{
				all_nodes.insert(src);
				idMap.emplace_back(src);
				idHash[src] = nodeCnt;
				++nodeCnt;
			}
			if (all_nodes.find(dest) == all_nodes.end())
			{
				all_nodes.insert(dest);
				idMap.emplace_back(dest);
				idHash[dest] = nodeCnt;
				++nodeCnt;
			}

			map.resize(nodeCnt);
			map[idHash[src]].push_back(idHash[dest]);
			r_map.resize(nodeCnt);
			r_map[idHash[dest]].push_back(idHash[src]);

			indegree[idHash[dest]]++;
		}

		printf("nodeCnt %d\n", nodeCnt);
		topo();
		printf("validnodeCnt %d\n", validNodeCnt);
		return true;
	}*/

	
/*	bool ConstructGraph()
	{
		int readThread = reader.dataSource.size();
		unsigned src, dest, val;
				
		for (int i = 0; i < readThread; ++i)
			for (pair<unsigned, unsigned>& data : reader.dataSource[i])
		{
			src = data.first;
			dest = data.second;
			if (all_nodes.find(src) == all_nodes.end())
			{
				all_nodes.insert(src);
				++nodeCnt;
			}
			if (all_nodes.find(dest) == all_nodes.end())
			{
				all_nodes.insert(dest);
				++nodeCnt;
			}
		}

		nodes = vector<unsigned>(all_nodes.begin(), all_nodes.end());
		for(int i = 0; i < nodes.size(); ++i)
		{
			idHash[nodes[i]] = i;
		}
		
		map.resize(nodeCnt);
		r_map.resize(nodeCnt);
		
		for (int i = 0; i < readThread; ++i)
			for (pair<unsigned, unsigned>& data : reader.dataSource[i])
		{
			src = data.first;
			dest = data.second;
			
			map[idHash[src]].push_back(idHash[dest]);
			r_map[idHash[dest]].push_back(idHash[src]);
		}
		printf("nodeCnt %d\n", nodeCnt);
		return true;
	}*/
#if 1
		void make_map(void)
		{
			int i, j;
			int first;
			int second;
			int idSrc;
			int idDest;

				for (i = 0; i <  dataSouSize; ++i)
					for (j = 0; j < dataSoup[i]; ++j)
				{
					first = dataSource[i][j][0];
					second = dataSource[i][j][1];
					if(smallNum[second] == 0)continue;
					idSrc = smallNum[first] - 1;
					idDest = smallNum[second] - 1;
					map[idSrc][pmap[idSrc]] = idDest;
					pmap[idSrc]++;
				}
		}

		void make_rmap(void)
		{
			int i, j;
			int first;
			int second;
			int idSrc;
			int idDest;

				for (i = 0; i <  dataSouSize; ++i)
					for (j = 0; j < dataSoup[i]; ++j)
				{
					first = dataSource[i][j][0];
					second = dataSource[i][j][1];
					if(smallNum[second] == 0)continue;
					idSrc = smallNum[first] - 1;
					idDest = smallNum[second] - 1;
					r_map[idDest][pr_map[idDest]] = idSrc;
					pr_map[idDest]++;

				}
		}

		void sort_map(int begin, int end)
		{
			int i;
			for(i = begin; i < end; ++i)
			{
				sort(map[i], map[i] + pmap[i]);
//				if(pmap[i] >= 50)printf("pmap %d too large, %d\n", i, pmap[i]);
				map[i][pmap[i]] = -1;
			}
		}

		void sort_rmap(int begin, int end)
		{
			int i;
			for(i = begin; i < end; ++i)
			{
				sort(r_map[i], r_map[i] + pr_map[i]);
//				if(pr_map[i] >= 50)printf("pr_map %d too large, %d\n", i, pr_map[i]);
				r_map[i][pr_map[i]] = -1;

			}
		}

		void make_intHash(int index)
		{
			int i;
			for(i = 0; i < nodeCnt; ++i)
			{
				if(i % threadNum != index)continue;
				int res[10];
				int index = 0;
				int num = nodes[i];
				char* start = wrHash[i];
				if(num == 0)
				{
					*start = '0';
					wrSizeHash[i] = 1;
					continue;
				}
				while (num != 0)
				{
					res[index++] = num % 10;
					num /= 10;
				}
				
				wrSizeHash[i] = index;
				while (index > 0)
				{
					--index;
					*start = numHash[res[index]];
					++start;
				}
			}
		}

		void make_intHash_withTopo(int index)
		{
			int i;
			for(i = 0; i < nodeCnt; ++i)
			{
				if(i % threadNum != index || !validInd[i])continue;
				int res[10];
				int index = 0;
				int num = nodes[i];
				char* start = wrHash[i];
				if(num == 0)
				{
					*start = '0';
					wrSizeHash[i] = 1;
					continue;
				}
				while (num != 0)
				{
					res[index++] = num % 10;
					num /= 10;
				}
				
				wrSizeHash[i] = index;
				while (index > 0)
				{
					--index;
					*start = numHash[res[index]];
					++start;
				}
			}
		}
		bool ConstructGraph()
		{
			nodeCnt = 0;
			unsigned src, dest, val;
	
			int idSrc;
			int idDest;
			int j = 0;
			int i, m;
			int nodeInd;

				for (i = 0; i <  dataSouSize; ++i)
					for (m = 0; m < dataSoup[i]; ++m)
				{
					smallNum[dataSource[i][m][0]] = 1;
				}
				nodeInd = 0;
				for (i = 0; i < MAX_NODE_COUNT; ++i)
				{
					if(smallNum[i])
					{
						nodes[nodeInd] = i;
						smallNum[i] = nodeInd + 1;
						++nodeInd;
					}
				}
#ifdef DEBUG
			addTime("construct hash end");
			printTime();
#endif
#ifdef DEBUG
			printf("nodeCnt %d, validNodeCnt %d, maxNode %d, minNode %d\n", j, nodeInd, nodes[0], nodes[nodeInd - 1]);
#endif
			nodeCnt = nodeInd;
			
			thread t1(make_map);
			thread t2(make_rmap);
			t1.join();
			t2.join();
			
#ifdef DEBUG
			addTime("make map end");
			printTime();
#endif
			thread t3(sort_map, 0, nodeCnt / 2);
			thread t4(sort_map, nodeCnt / 2, nodeCnt);
			thread t5(sort_rmap, 0, nodeCnt / 2);
			thread t6(sort_rmap, nodeCnt / 2, nodeCnt);

			t3.join();
			t4.join();
			t5.join();
			t6.join();
#ifdef DEBUG
			addTime("sort map end");
			printTime();
#endif
			return true;
		}

#endif
	void sortCircle(vector<vector<unsigned>>& temp)
	{
		sort(temp.begin(), temp.end(), cmp);
	}

	void DFS(vector<bool>& vis, int start, int cur, int depth, vector<int>& trace,
	vector<bool>& pathSet1, vector<bool>& pathSet2, vector<bool>& pathSet3, int threadInd, int * resp);

	void* BI_DFS(void* arg1);
	
	void* BI_DFS_ONE(void* arg1);
	void calculate()
	{
		
		cnt5 = nodeCnt / threadNum / 2 - (nodeCnt / threadNum / 2) % (threadNum * 2);
		
#ifdef DEBUG
			printf("multhread\n");
#endif
			int para[threadNum];
			for(int i = 0; i < threadNum; ++i)
			{
				para[i] = i;
			}
			for (int i = 0; i < threadNum; ++i)
			{
				data[i][0] = 0;
				data[i][1] = 0;
				data[i][2] = i;
			}
			thread t8(make_intHash, 0);
			thread t9(make_intHash, 1);
			thread t10(make_intHash, 2);
			thread t11(make_intHash, 3);
			t8.join();
			t9.join();
			t10.join();
			t11.join();

#ifdef DEBUG
			addTime("make intHash end");
			printTime();
#endif

#ifdef PTHREAD
			vector<pthread_t> thread(threadNum);
			for (int i = 1; i < threadNum; ++i)
			{
				if (pthread_create(&thread[i], NULL, BI_DFS, &(para[i])) != 0)
				{
					printf("pthread_create fail\n");
					return;
				}
//				timer->addTime(to_string(i) + "thread start:");
			}
			BI_DFS(&(para[0]));
			for (int i = 1; i < threadNum; ++i)
			{
				if (pthread_join(thread[i], NULL) != 0)
				{
					printf("pthread_join fail\n");
					return;
				}
//				timer->addTime(to_string(i) + "thread end:");
			}
#else
			thread t0(BI_DFS, &(para[0]));
			thread t1(BI_DFS, &(para[1]));
			thread t2(BI_DFS, &(para[2]));
			thread t3(BI_DFS, &(para[3]));
			t0.join();
			t1.join();
			t2.join();
			t3.join();
#endif
			addTime("DFS end");
			printTime();

//			printf("size1 %d, size2 %d, size3 %d, size4 %d, sizesum %d\n", 
//				res0.size(), res1.size(), res2.size(), res3.size(), linesize);
			int linesize = 0;
			for(int j = 0; j < threadNum; ++j)
			{
				for(int i = 0; i < 5; ++i)			
				{
					linesize += ansp[j][i] / (i + 3);					
					fileByteSize[i] += ansp[j][i];
					fileByteSize[i] += ansSize[j][i];
				}
					linesize += ansp[j][5] / 7;					
					fileByteSize[5] += ansp[j][5];
					fileByteSize[5] += ansSize[j][5];
			}


				for(int i = 0; i < 4; ++i)			
				{
					backWr[i] = nodeCnt;
				}
				backWr[4] = cnt5;
				backWr[5] = nodeCnt;
				frontWr[5] = cnt5;


			int totalSize = 0;
			for(int i = 0; i < 6; ++i)			
			{
				totalSize += fileByteSize[i];
			}

			writeFileInit(linesize, totalSize);


			writeFileHead(linesize);
#ifdef DEBUG
			printf("linesize %d\n", linesize);
			printf("3  %d, 4 %d, 5 %d, 6 %d, 7 %d, 8 %d total %d\n",fileByteSize[0], fileByteSize[1], fileByteSize[2], fileByteSize[3], fileByteSize[4],fileByteSize[5], totalSize);
#endif
#ifdef ONEWR			
			writeFileData(0, 5, 0);
#else
{
/*			thread t4(writeFileData, 4, 5, 0);
			thread t5(writeFileDataBack, 4, 5, 0);
			thread t6(writeFileData, 0, 4, 0);
			thread t7(writeFileDataBack, 0, 4, 0);
			thread t8(writeFileDataBack5);
			thread t9(writeFileData5);

			t4.join();
			t5.join();
			t6.join();
			t7.join();
			t8.join();
			t9.join();*/
thread t4(writeFileData, 4, 5, 0);
			thread t5(writeFileDataBack, 4, 5, 0);
			thread t6(writeFileData, 0, 4, 0);
			thread t7(writeFileDataBack, 0, 4, 0);
			thread t8(writeFileDataBack5);
			thread t9(writeFileData5);

			t4.join();
			t5.join();
			t6.join();
			t7.join();
			t8.join();
			t9.join();

}

#endif
			writeFileEnd(linesize);
	}


void DFS(vector<bool>& vis, int start, int cur, int depth, vector<int>& trace,
	vector<bool>& pathSet1, vector<bool>& pathSet2, vector<bool>& pathSet3, int threadInd, int * resp)
{
	if (depth >= 3 && depth <= 6 && pathSet1[cur]) {	
		int ind = depth - 3;
		int * res = ans[ind][threadInd];
	
		for (int i = 0; i < depth; ++i)
		{
			res[resp[ind] + i] = trace[i];
		}
		
		resp[ind] += depth;
	}
	
	for(int index = 0; index < 50 && map[cur][index] != -1; ++index)
	{
		int next = map[cur][index];
		if (depth < 7 && !vis[next] && start < next) {
			if (depth == 4 && pathSet3[next] == false) continue;
			else if (depth == 5 && pathSet2[next] == false) continue;
			else if (depth == 6) {
				if (pathSet1[next]) {
					int * res = ans[4][threadInd];
					int tmp = resp[4];
					res[tmp + 0] = trace[0];
					res[tmp + 1] = trace[1];
					res[tmp + 2] = trace[2];
					res[tmp + 3] = trace[3];
					res[tmp + 4] = trace[4];
					res[tmp + 5] = trace[5];
					res[tmp + 6] = next;
					
					resp[4] += 7;
				}
				continue;
			}

			vis[next] = true;

			trace.emplace_back(next);
			DFS(vis, start, next, depth + 1, trace, pathSet1, pathSet2, pathSet3, threadInd, resp);
			vis[next] = false;
			trace.pop_back();
		}
	}
}

void* BI_DFS(void* arg1)
{
	
	unsigned*  para = data[*(int*)arg1];
	unsigned begin = para[0], end = para[1], resIndex = para[2];
	int threadInd = resIndex;
	int* resp = ansp[resIndex];

#ifdef DEBUG
	timeval timeNow;
	gettimeofday(&timeNow, NULL);
	printf("%d start, tv_sec = %d, tv_usec = %d\n", threadInd, timeNow.tv_sec, timeNow.tv_usec);
#endif	
	int start, i, j, k; 
	int indexi, indexj, indexk;
	int * que = q[resIndex];
	int * anSize = ansSize[resIndex];
	int first, last;
	int trace[7];
	int pathset1[nodeCnt];
	int pathset2[nodeCnt];
	int pathset3[nodeCnt];
	memset(pathset1,-1,sizeof(int )*nodeCnt);
	memset(pathset2,-1,sizeof(int )*nodeCnt);
	memset(pathset3,-1,sizeof(int )*nodeCnt);

	bool vis[nodeCnt];
	memset(vis,false,sizeof(bool )*nodeCnt);

	// vector<bool> vis(nodeCnt, false);
	for (start = 0; start < nodeCnt; ++start)
	{
		if(start % threadNum != resIndex)continue;

		// vector<bool> pathset1(nodeCnt, false);
		// vector<bool> pathset2(nodeCnt, false);
		// vector<bool> pathset3(nodeCnt, false);



		if (map[start][0] != -1 && r_map[start][0] != -1)
		{
			first = 0;
			last = 0;
			trace[0] = start;
			vis[start] = true;
			for(indexi = 0; indexi < 50 && r_map[start][indexi] != -1; ++indexi)
			{
				i = r_map[start][indexi];
			
				if(i <= start)continue;
				pathset1[i] = start;
				pathset2[i] = start;
				pathset3[i] = start;
				que[last] = i;
				++last;
			}
			k = last;
			while(first < k)
			{
				j = que[first];
				for(indexi = 0; indexi < 50 && r_map[j][indexi] != -1; ++indexi)
				{
					i = r_map[j][indexi];				
					if(i <= start)continue; 
					pathset3[i] = start;
					pathset2[i] = start;
					que[last] = i;
					++last;
				}
				++first;
			}		
			while(first < last)
			{
				j = que[first];
				for(indexi = 0; indexi < 50 && r_map[j][indexi] != -1; ++indexi)
				{
					i = r_map[j][indexi];				
					if(i <= start)continue;
					pathset3[i] = start;
				}
				++first;
			}
			
			pathset1[start] = start;
			{	
				for(int index = 0; index < 50 && map[start][index] != -1; ++index)
				{
					int next = map[start][index];
					if (!vis[next] && start < next) {
			
						vis[next] = true;
						trace[1] = next;
									//			DFS(vis, start, next, 2, trace, pathset1, pathset2, pathset3, threadInd, resp);
						{
									int cur = next;
									for(int index = 0; index < 50 && map[cur][index] != -1; ++index)
									{
										int next = map[cur][index];
										if (!vis[next] && start < next) { 				
											vis[next] = true;
											trace[2] = next;
											
			//								DFS(vis, start, next, 3, trace, pathset1, pathset2, pathset3, threadInd, resp);
											{
														int cur = next;
														if (pathset1[cur] == start) {	
															int * res = ans[0][threadInd];
														
																res[resp[0] + 0] = trace[0];
																res[resp[0] + 1] = trace[1];
																res[resp[0] + 2] = trace[2];
															
															resp[0] += 3;
															anSize[0] += wrSizeHash[trace[0]];
															anSize[0] += wrSizeHash[trace[1]];
															anSize[0] += wrSizeHash[trace[2]];
														}
														
														for(int index = 0; index < 50 && map[cur][index] != -1; ++index)
														{
															int next = map[cur][index];
															if (!vis[next] && start < next) { 									
																vis[next] = true;
																trace[3] = next;
																
																//DFS(vis, start, next, 4, trace, pathset1, pathset2, pathset3, threadInd, resp);
																{
																			int cur = next;
																			if (pathset1[cur] == start) {	
																			
																				int * res = ans[1][threadInd];
																			
																				
																					res[resp[1] + 0] = trace[0];
																					res[resp[1] + 1] = trace[1];
																					res[resp[1] + 2] = trace[2];
																					res[resp[1] + 3] = trace[3];
															resp[1] += 4;
																				
															anSize[1] += wrSizeHash[trace[0]];
															anSize[1] += wrSizeHash[trace[1]];
															anSize[1] += wrSizeHash[trace[2]];
															anSize[1] += wrSizeHash[trace[3]];
																			}
																			
																			for(int index = 0; index < 50 && map[cur][index] != -1; ++index)
																			{
																				int next = map[cur][index];
																				if (!vis[next] && start < next) {
																					if (pathset3[next] != start) continue;
																																																						
																					vis[next] = true;
																					trace[4] = next;
																					
																					//DFS(vis, start, next, 5, trace, pathset1, pathset2, pathset3, threadInd, resp);
																					{
																								int cur = next;
																								if (pathset1[cur] == start) {																						
																									int * res = ans[2][threadInd];
																								
																									
																										res[resp[2] + 0] = trace[0];
																										res[resp[2] + 1] = trace[1];
																										res[resp[2] + 2] = trace[2];
																										res[resp[2] + 3] = trace[3];
																										res[resp[2] + 4] = trace[4];
																																															
																									
																									resp[2] += 5;
																									anSize[2] += wrSizeHash[trace[0]];
																									anSize[2] += wrSizeHash[trace[1]];
																									anSize[2] += wrSizeHash[trace[2]];
																									anSize[2] += wrSizeHash[trace[3]];
																									anSize[2] += wrSizeHash[trace[4]];
																								}
																								
																								for(int index = 0; index < 50 && map[cur][index] != -1; ++index)
																								{
																									int next = map[cur][index];
																									if (!vis[next] && start < next) {
																										
																										if (pathset2[next] != start) continue;
																										
																							
																										vis[next] = true;
																							
																										trace[5] = next;
																										//DFS(vis, start, next, 6, trace, pathset1, pathset2, pathset3, threadInd, resp);
																										{
																													int cur = next;
																													if (pathset1[cur] == start) {	
																														
																														int * res = ans[3][threadInd];
																													
																														
																															res[resp[3] + 0] = trace[0];
																															res[resp[3] + 1] = trace[1];
																															res[resp[3] + 2] = trace[2];
																															res[resp[3] + 3] = trace[3];
																															res[resp[3] + 4] = trace[4];
																															res[resp[3] + 5] = trace[5];
																														
																														
																														resp[3] += 6;
																														anSize[3] += wrSizeHash[trace[0]];
																														anSize[3] += wrSizeHash[trace[1]];
																														anSize[3] += wrSizeHash[trace[2]];
																														anSize[3] += wrSizeHash[trace[3]];
																														anSize[3] += wrSizeHash[trace[4]];
																														anSize[3] += wrSizeHash[trace[5]];
																													}
																													
																													for(int index = 0; index < 50 && map[cur][index] != -1; ++index)
																													{
																														int next = map[cur][index];
																														if (!vis[next] && start < next) {
																															
																																if (pathset1[next] == start) {
																																	if(trace[0] < cnt5)
																																	{
																																		ans[4][threadInd][resp[4] + 0] = trace[0];
																																		ans[4][threadInd][resp[4] + 1] = trace[1];
																																		ans[4][threadInd][resp[4] + 2] = trace[2];
																																		ans[4][threadInd][resp[4] + 3] = trace[3];
																																		ans[4][threadInd][resp[4] + 4] = trace[4];
																																		ans[4][threadInd][resp[4] + 5] = trace[5];
																																		ans[4][threadInd][resp[4] + 6] = next;
																																		
																																		resp[4] += 7;
																																		anSize[4] += wrSizeHash[trace[0]];
																																		anSize[4] += wrSizeHash[trace[1]];
																																		anSize[4] += wrSizeHash[trace[2]];
																																		anSize[4] += wrSizeHash[trace[3]];
																																		anSize[4] += wrSizeHash[trace[4]];
																																		anSize[4] += wrSizeHash[trace[5]];
																																		anSize[4] += wrSizeHash[next];
																																	}
																																	else
																																	{
																																		ans[5][threadInd][resp[5] + 0] = trace[0];
																																		ans[5][threadInd][resp[5] + 1] = trace[1];
																																		ans[5][threadInd][resp[5] + 2] = trace[2];
																																		ans[5][threadInd][resp[5] + 3] = trace[3];
																																		ans[5][threadInd][resp[5] + 4] = trace[4];
																																		ans[5][threadInd][resp[5] + 5] = trace[5];
																																		ans[5][threadInd][resp[5] + 6] = next;
																																		
																																		resp[5] += 7;
																																		anSize[5] += wrSizeHash[trace[0]];
																																		anSize[5] += wrSizeHash[trace[1]];
																																		anSize[5] += wrSizeHash[trace[2]];
																																		anSize[5] += wrSizeHash[trace[3]];
																																		anSize[5] += wrSizeHash[trace[4]];
																																		anSize[5] += wrSizeHash[trace[5]];
																																		anSize[5] += wrSizeHash[next];
																																	}
																																}
																																continue;
																															
																												
								
																														}
																													}
																												}																									
																										vis[next] = false;
																										
																									}
																								}
																							}																				
																					vis[next] = false;
																					
																				}
																			}
																		}															
																vis[next] = false;
																
															}
														}
													}										
											vis[next] = false;
											
										}
									}
								}					
						vis[next] = false;
						
					}
				}
			}
			vis[start] = false;
		}


//		DFS(vis, start, start, 1, trace, pathset1, pathset2, pathset3, threadInd, resp);


}

#ifdef DEBUG	
	gettimeofday(&timeNow, NULL);
	printf("%d end, tv_sec = %d, tv_usec = %d\n", threadInd, timeNow.tv_sec, timeNow.tv_usec);
#endif
}


void* BI_DFS_ONE(void* arg1)
{
	
	unsigned*  para = data[*(int*)arg1];
	unsigned begin = para[0], end = para[1], resIndex = para[2];
	int threadInd = resIndex;
	int* resp = ansp[resIndex];

#ifdef DEBUG
	timeval timeNow;
	gettimeofday(&timeNow, NULL);
	printf("%d start, tv_sec = %d, tv_usec = %d\n", threadInd, timeNow.tv_sec, timeNow.tv_usec);
#endif	
	int start, i, j, k; 
	int indexi, indexj, indexk;
	int * que = q[resIndex];
	int * anSize = ansSize[resIndex];
	int first, last;
	int trace[7];
	int pathset1[nodeCnt];
	int pathset2[nodeCnt];
	int pathset3[nodeCnt];
	memset(pathset1,-1,sizeof(int )*nodeCnt);
	memset(pathset2,-1,sizeof(int )*nodeCnt);
	memset(pathset3,-1,sizeof(int )*nodeCnt);

	bool vis[nodeCnt];
	memset(vis,false,sizeof(bool )*nodeCnt);

	// vector<bool> vis(nodeCnt, false);
	for (start = 0; start < nodeCnt; ++start)
	{
		if(!validInd[start] || start % threadNum != resIndex)continue;

		// vector<bool> pathset1(nodeCnt, false);
		// vector<bool> pathset2(nodeCnt, false);
		// vector<bool> pathset3(nodeCnt, false);



		if (map[start][0] != -1 && r_map[start][0] != -1)
		{
			first = 0;
			last = 0;
			trace[0] = start;
			vis[start] = true;
			for(indexi = 0; indexi < 50 && r_map[start][indexi] != -1; ++indexi)
			{
				i = r_map[start][indexi];
			
				if(i <= start || !validInd[i])continue;
				pathset1[i] = start;
				pathset2[i] = start;
				pathset3[i] = start;
				que[last] = i;
				++last;
			}
			k = last;
			while(first < k)
			{
				j = que[first];
				for(indexi = 0; indexi < 50 && r_map[j][indexi] != -1; ++indexi)
				{
					i = r_map[j][indexi];				
					if(i <= start || !validInd[i])continue; 
					pathset3[i] = start;
					pathset2[i] = start;
					que[last] = i;
					++last;
				}
				++first;
			}		
			while(first < last)
			{
				j = que[first];
				for(indexi = 0; indexi < 50 && r_map[j][indexi] != -1; ++indexi)
				{
					i = r_map[j][indexi];				
					if(i <= start || !validInd[i])continue;
					pathset3[i] = start;
				}
				++first;
			}
			
			pathset1[start] = start;
			{	
				for(int index = 0; index < 50 && map[start][index] != -1; ++index)
				{
					int next = map[start][index];
					if (!vis[next] && start < next && validInd[next]) {
			
						vis[next] = true;
						trace[1] = next;
									//			DFS(vis, start, next, 2, trace, pathset1, pathset2, pathset3, threadInd, resp);
						{
									int cur = next;
									for(int index = 0; index < 50 && map[cur][index] != -1; ++index)
									{
										int next = map[cur][index];
										if (!vis[next] && start < next && validInd[next]) { 				
											vis[next] = true;
											trace[2] = next;
											
			//								DFS(vis, start, next, 3, trace, pathset1, pathset2, pathset3, threadInd, resp);
											{
														int cur = next;
														if (pathset1[cur] == start) {	
															int * res = ans[0][threadInd];
														
																res[resp[0] + 0] = trace[0];
																res[resp[0] + 1] = trace[1];
																res[resp[0] + 2] = trace[2];
															
															resp[0] += 3;
															anSize[0] += wrSizeHash[trace[0]];
															anSize[0] += wrSizeHash[trace[1]];
															anSize[0] += wrSizeHash[trace[2]];
														}
														
														for(int index = 0; index < 50 && map[cur][index] != -1; ++index)
														{
															int next = map[cur][index];
															if (!vis[next] && start < next && validInd[next]) { 									
																vis[next] = true;
																trace[3] = next;
																
																//DFS(vis, start, next, 4, trace, pathset1, pathset2, pathset3, threadInd, resp);
																{
																			int cur = next;
																			if (pathset1[cur] == start) {	
																			
																				int * res = ans[1][threadInd];
																			
																				
																					res[resp[1] + 0] = trace[0];
																					res[resp[1] + 1] = trace[1];
																					res[resp[1] + 2] = trace[2];
																					res[resp[1] + 3] = trace[3];
															resp[1] += 4;
																				
															anSize[1] += wrSizeHash[trace[0]];
															anSize[1] += wrSizeHash[trace[1]];
															anSize[1] += wrSizeHash[trace[2]];
															anSize[1] += wrSizeHash[trace[3]];
																			}
																			
																			for(int index = 0; index < 50 && map[cur][index] != -1; ++index)
																			{
																				int next = map[cur][index];
																				if (!vis[next] && start < next && validInd[next]) {
																					if (pathset3[next] != start) continue;
																																																						
																					vis[next] = true;
																					trace[4] = next;
																					
																					//DFS(vis, start, next, 5, trace, pathset1, pathset2, pathset3, threadInd, resp);
																					{
																								int cur = next;
																								if (pathset1[cur] == start) {																						
																									int * res = ans[2][threadInd];
																								
																									
																										res[resp[2] + 0] = trace[0];
																										res[resp[2] + 1] = trace[1];
																										res[resp[2] + 2] = trace[2];
																										res[resp[2] + 3] = trace[3];
																										res[resp[2] + 4] = trace[4];
																																															
																									
																									resp[2] += 5;
																									anSize[2] += wrSizeHash[trace[0]];
																									anSize[2] += wrSizeHash[trace[1]];
																									anSize[2] += wrSizeHash[trace[2]];
																									anSize[2] += wrSizeHash[trace[3]];
																									anSize[2] += wrSizeHash[trace[4]];
																								}
																								
																								for(int index = 0; index < 50 && map[cur][index] != -1; ++index)
																								{
																									int next = map[cur][index];
																									if (!vis[next] && start < next && validInd[next]) {
																										
																										if (pathset2[next] != start) continue;
																										
																							
																										vis[next] = true;
																							
																										trace[5] = next;
																										//DFS(vis, start, next, 6, trace, pathset1, pathset2, pathset3, threadInd, resp);
																										{
																													int cur = next;
																													if (pathset1[cur] == start) {	
																														
																														int * res = ans[3][threadInd];
																													
																														
																															res[resp[3] + 0] = trace[0];
																															res[resp[3] + 1] = trace[1];
																															res[resp[3] + 2] = trace[2];
																															res[resp[3] + 3] = trace[3];
																															res[resp[3] + 4] = trace[4];
																															res[resp[3] + 5] = trace[5];
																														
																														
																														resp[3] += 6;
																														anSize[3] += wrSizeHash[trace[0]];
																														anSize[3] += wrSizeHash[trace[1]];
																														anSize[3] += wrSizeHash[trace[2]];
																														anSize[3] += wrSizeHash[trace[3]];
																														anSize[3] += wrSizeHash[trace[4]];
																														anSize[3] += wrSizeHash[trace[5]];
																													}
																													
																													for(int index = 0; index < 50 && map[cur][index] != -1; ++index)
																													{
																														int next = map[cur][index];
																														if (!vis[next] && start < next && validInd[next]) {
																															
																																if (pathset1[next] == start) {
																																	if(trace[0] < cnt5)
																																	{
																																		ans[4][threadInd][resp[4] + 0] = trace[0];
																																		ans[4][threadInd][resp[4] + 1] = trace[1];
																																		ans[4][threadInd][resp[4] + 2] = trace[2];
																																		ans[4][threadInd][resp[4] + 3] = trace[3];
																																		ans[4][threadInd][resp[4] + 4] = trace[4];
																																		ans[4][threadInd][resp[4] + 5] = trace[5];
																																		ans[4][threadInd][resp[4] + 6] = next;
																																		
																																		resp[4] += 7;
																																		anSize[4] += wrSizeHash[trace[0]];
																																		anSize[4] += wrSizeHash[trace[1]];
																																		anSize[4] += wrSizeHash[trace[2]];
																																		anSize[4] += wrSizeHash[trace[3]];
																																		anSize[4] += wrSizeHash[trace[4]];
																																		anSize[4] += wrSizeHash[trace[5]];
																																		anSize[4] += wrSizeHash[next];
																																	}
																																	else
																																	{
																																		ans[5][threadInd][resp[5] + 0] = trace[0];
																																		ans[5][threadInd][resp[5] + 1] = trace[1];
																																		ans[5][threadInd][resp[5] + 2] = trace[2];
																																		ans[5][threadInd][resp[5] + 3] = trace[3];
																																		ans[5][threadInd][resp[5] + 4] = trace[4];
																																		ans[5][threadInd][resp[5] + 5] = trace[5];
																																		ans[5][threadInd][resp[5] + 6] = next;
																																		
																																		resp[5] += 7;
																																		anSize[5] += wrSizeHash[trace[0]];
																																		anSize[5] += wrSizeHash[trace[1]];
																																		anSize[5] += wrSizeHash[trace[2]];
																																		anSize[5] += wrSizeHash[trace[3]];
																																		anSize[5] += wrSizeHash[trace[4]];
																																		anSize[5] += wrSizeHash[trace[5]];
																																		anSize[5] += wrSizeHash[next];
																																	}
																																}
																																continue;
																															
																												
								
																														}
																													}
																												}																									
																										vis[next] = false;
																										
																									}
																								}
																							}																				
																					vis[next] = false;
																					
																				}
																			}
																		}															
																vis[next] = false;
																
															}
														}
													}										
											vis[next] = false;
											
										}
									}
								}					
						vis[next] = false;
						
					}
				}
			}
			vis[start] = false;
		}


//		DFS(vis, start, start, 1, trace, pathset1, pathset2, pathset3, threadInd, resp);


}

#ifdef DEBUG	
	gettimeofday(&timeNow, NULL);
	printf("%d end, tv_sec = %d, tv_usec = %d\n", threadInd, timeNow.tv_sec, timeNow.tv_usec);
#endif
}

#ifdef DEBUG
// printf("%s, %d\n", __FILE__, __LINE__);
#endif

int main()
{
#ifdef DEBUG
	printf("main start\n");
#endif
#ifdef DEBUG
	addTime("start");
	printTime();
#endif	
	
	getData();
#ifdef DEBUG
	addTime("getdata end");
	printTime();
#endif	

	ConstructGraph();
#ifdef DEBUG	
	addTime("ConstructGraph end");
	printTime();
#endif
	calculate();

#ifdef DEBUG	
	addTime("write end");
	printTime();
#endif

	return 0;
}
