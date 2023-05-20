#include <stdio.h>
#include <stdlib.h>

typedef struct gnd { // goal, next, distance
	int g;
	int n;
	int d;
}gnd;

int main(int argc, char* argv[]) {
	if (argc != 4) {
		printf("usage: linkstate topologyfile messagesfile changesfile\n");
		return 0;
	}
	FILE* t_file, * m_file, * c_file, * output;
	t_file = fopen(argv[1], "r");
	if (t_file == NULL) { printf("Error: open input file.\n"); return 0; }
	m_file = fopen(argv[2], "r");
	if (m_file == NULL) { printf("Error: open input file.\n"); return 0; }
	c_file = fopen(argv[3], "r");
	if (c_file == NULL) { printf("Error: open input file.\n"); return 0; }
	output = fopen("output_ls.txt", "w");
	if (output == NULL) { printf("Error: open input file.\n"); return 0; }
	//노드의 개수(num)을 읽고, gnd의 2차원 배열 할당
	int num;
	fscanf(t_file, "%d", &num);
	gnd** arr = (gnd**)malloc(sizeof(gnd*)*num);
	for(int i=0;i<num;++i) arr[i] = (gnd*)malloc(sizeof(gnd)*num);
	//gnd arr[num][num];
	//arr[][].d를 999로 초기화
	for (int i = 0; i < num; ++i) {
		for (int j = 0; j < num; ++j) {
			arr[i][j].d = 999;
		}
	}
	//topology 파일을 읽어서 top[][3]에 저장, 초기 routing table 작성
	for (int i = 0; i < num; ++i) {
		for (int j = 0; j < num; ++j) {
			arr[i][j].g = j;
			if (i == j) {
				arr[i][i].n = i;
				arr[i][i].d = 0;
			}
		}
	}
	int top[100][3];
	int a, b, c, index = 0, ii = 0;
	while (EOF != fscanf(t_file, "%d %d %d", &a, &b, &c)) {
		//fscanf(t_file, "%d %d %d", &a,&b,&c);
		top[ii][0] = a; top[ii][1] = b; top[ii][2] = c;
		index++;
		arr[a][b].n = b; arr[a][b].d = c;
		arr[b][a].n = a; arr[b][a].d = c;
		ii++;
	}
	//0,1로 이루어진 nbd[][] 만들기 - i가 j와 인접해있으면 nbd[i][j]=1
	int** nbd = (int**)malloc(sizeof(int*) * num);
	for (int i = 0; i < num; ++i) nbd[i] = (int*)malloc(sizeof(int) * num);
	//int nbd[num][num];
	for (int i = 0; i < num; ++i) {
		for (int j = 0; j < num; ++j) {
			nbd[i][j] = 0;
		}
	}
	for (int i = 0; i < index; ++i) {
		nbd[top[i][0]][top[i][1]] = 1; nbd[top[i][1]][top[i][0]] = 1;
	}
	//SPT 만들기
	int* spt = (int*)malloc(sizeof(int) * num);
	for (int i = 0; i < num; ++i) spt[i] = 0;
	//1트
	//table 수렴시키기
	int count;
	int min, mindex;
	for (int x = 0; x < num; ++x) {
		for (int i = 0; i < num; i++) spt[i] = 0;
		spt[x] = 1; count = 0;
		while(count < num-2){
			min = 998;
			for (int y = 0; y < num; ++y) { // spt에 추가할 원소 찾기
				if (x == y) continue;
				if (nbd[x][y] && !spt[y]) { // 이웃된 노드이면서 spt에 없는 경우
					if (arr[x][y].d < min) {
						min = arr[x][y].d;
						mindex = y;
					}
				}
			}
			spt[mindex] = 1; //spt에 추가
			for (int y = 0; y < num; ++y) { // spt에 추가된 원소의 top으로부터 정보를 받아서 업데이트
				for (int k = 0; k < index; ++k) {
					if ((top[k][0] == mindex) && !spt[top[k][1]]) {
						if (arr[x][top[k][1]].d > arr[x][mindex].d + top[k][2]) {
							arr[x][top[k][1]].d = arr[x][mindex].d + top[k][2];
							arr[x][top[k][1]].n = arr[x][mindex].n;
						}
					}
					else if ((top[k][1] == mindex) && !spt[top[k][0]]) {
						if (arr[x][top[k][0]].d > arr[x][mindex].d + top[k][2]) {
							arr[x][top[k][0]].d = arr[x][mindex].d + top[k][2];
							arr[x][top[k][0]].n = arr[x][mindex].n;
						}
					}
				}
			}
			for (int y = 0; y < num; ++y) { // nbd 합집합으로 늘려주기
				if (nbd[mindex][y]) nbd[x][y] = 1;
			}
			count++;
		}
	}
	//updated
	
	//route table print하기
	for (int i = 0; i < num; ++i) {
		for (int j = 0; j < num; ++j) {
			if (arr[i][j].d != 999)
				fprintf(output, "%d %d %d\n", arr[i][j].g, arr[i][j].n, arr[i][j].d);
		}
		fprintf(output, "\n");
	}
	//message 돌리기
	int from, to; char str[1000];
	while (EOF != fscanf(m_file, "%d %d %[^\n]", &from, &to, str)) {
		//fscanf(m_file, "%d %d %[^\n]",&from,&to,str);
		if (arr[from][to].d < 999) {
			fprintf(output, "from %d to %d cost %d hops %d ", from, to, arr[from][to].d, from);
			while (arr[from][to].g != arr[from][to].n) {
				from = arr[from][to].n;
				fprintf(output, "%d ", from);
			}
			fprintf(output, "message %s\n", str);
		}
		else {
			fprintf(output, "from %d to %d cost infinite hops unreachable message %s\n", from, to, str);
		}
	}
	fprintf(output, "\n");
	//2트~
		//while : change file을 다 읽을 때 까지
	int q, w, e, temp, sz = 0;
	while (EOF != fscanf(c_file, "%d %d %d", &q, &w, &e)) {
		//top변경
		temp = index;
		for (int i = 0; i < temp; ++i) {
			if (top[i][0] == q) {
				if (top[i][1] == w) {
					if (e == -999) { top[i][2] = 999; sz++; continue; }
					else { top[i][2] = e; sz++; continue; }
				}
			}
			else if (top[i][0] == w) {
				if (top[i][1] == q) {
					if (e == -999) { top[i][2] = 999; sz++; continue; }
					else { top[i][2] = e; sz++; continue; }
				}
			}
		}
		if (!sz) {
			top[index][0] = q;
			top[index][1] = w;
			top[index][2] = e;
			index++;
		}

		//nbd변경
		for (int i = 0; i < num; ++i) {
			for (int j = 0; j < num; ++j) {
				nbd[i][j] = 0;
			}
		}
		for (int i = 0; i < index; ++i) {
			if (top[i][2] < 999) { nbd[top[i][0]][top[i][1]] = 1; nbd[top[i][1]][top[i][0]] = 1; }
		}
		//arr초기화 후 변경
		for (int i = 0; i < num; ++i) {
			for (int j = 0; j < num; ++j) {
				arr[i][j].g = j;
				if (i == j) {
					arr[i][i].n = i;
					arr[i][i].d = 0;
				}
			}
		}
		for (int i = 0; i < num; ++i) {
			for (int j = 0; j < num; ++j) {
				if (i == j) arr[i][j].d = 0;
				else arr[i][j].d = 999;
			}
		}
		for (int i = 0; i < index; ++i) {
			arr[top[i][0]][top[i][1]].n = top[i][1]; arr[top[i][0]][top[i][1]].d = top[i][2];
			arr[top[i][1]][top[i][0]].n = top[i][0]; arr[top[i][1]][top[i][0]].d = top[i][2];
		}
		//table 수렴시키기
		for (int x = 0; x < num; ++x) {
			for (int i = 0; i < num; i++) spt[i] = 0;
			spt[x] = 1; count = 0;
			while (count < num - 2) {
				min = 998;
				for (int y = 0; y < num; ++y) { // spt에 추가할 원소 찾기
					if (x == y) continue;
					if (nbd[x][y] && !spt[y]) { // 이웃된 노드이면서 spt에 없는 경우
						if (arr[x][y].d < min) {
							min = arr[x][y].d;
							mindex = y;
						}
					}
				}
				spt[mindex] = 1; //spt에 추가
				for (int y = 0; y < num; ++y) { // spt에 추가된 원소의 top으로부터 정보를 받아서 업데이트
					for (int k = 0; k < index; ++k) {
						if ((top[k][0] == mindex) && !spt[top[k][1]]) {
							if (arr[x][top[k][1]].d > arr[x][mindex].d + top[k][2]) {
								arr[x][top[k][1]].d = arr[x][mindex].d + top[k][2];
								arr[x][top[k][1]].n = arr[x][mindex].n;
							}
						}
						else if ((top[k][1] == mindex) && !spt[top[k][0]]) {
							if (arr[x][top[k][0]].d > arr[x][mindex].d + top[k][2]) {
								arr[x][top[k][0]].d = arr[x][mindex].d + top[k][2];
								arr[x][top[k][0]].n = arr[x][mindex].n;
							}
						}
					}
				}
				for (int y = 0; y < num; ++y) { // nbd 합집합으로 늘려주기
					if (nbd[mindex][y]) nbd[x][y] = 1;
				}
				count++;
			}
		}
		//route table print하기
		for (int i = 0; i < num; ++i) {
			for (int j = 0; j < num; ++j) {
				if (arr[i][j].d != 999)
					fprintf(output, "%d %d %d\n", arr[i][j].g, arr[i][j].n, arr[i][j].d);
			}
			fprintf(output, "\n");
		}
		//message 돌리기
		rewind(m_file);
		while (EOF != fscanf(m_file, "%d %d %[^\n]", &from, &to, str)) {
			if (arr[from][to].d < 999) {
				fprintf(output, "from %d to %d cost %d hops %d ", from, to, arr[from][to].d, from);
				while (arr[from][to].g != arr[from][to].n) {
					from = arr[from][to].n;
					fprintf(output, "%d ", from);
				}
				fprintf(output, "message %s\n", str);
			}
			else {
				fprintf(output, "from %d to %d cost infinite hops unreachable message %s\n", from, to, str);
			}
		}
		fprintf(output, "\n");
	}
	//free해주기
	fclose(t_file); fclose(m_file); fclose(c_file); fclose(output);
	for (int i = 0; i < num; ++i) {
		free(arr[i]); free(nbd[i]);
	}
	free(arr); free(nbd); free(spt);
	printf("Complete. Output file written to output_ls.txt.\n");
}
