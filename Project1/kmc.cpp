#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <queue>
#include <iomanip>
#include <ctime>
#include <cstdlib>
using namespace std;

// 데이터를 저장할 클래스를 정의. 벡터, 속한 cluster의 number, gene number 등의 정보를 가짐.
class riboData {
private:
	double ex_result[79];
	bool val_isRibo;
	int clusterNum;
	int geneNum;

public:
	riboData() {
		for (int i = 0; i < 79; i++) {
			ex_result[i] = 0;
		}
		val_isRibo = false;
		clusterNum = 0;
		geneNum = 0;
	}

	void setResult(int idx, double value) {
		ex_result[idx] = value;
	}

	double getResult(int idx) {
		return ex_result[idx];
	}

	void addResult(riboData data) {
		for (int i = 0; i < 79; i++) {
			ex_result[i] += data.ex_result[i];
		}
	}

	void divResult(double n) {
		for (int i = 0; i < 79; i++) {
			ex_result[i] /= n;
		}
	}

	void clearResult() {
		for (int i = 0; i < 79; i++) {
			ex_result[i] = 0;
		}
	}

	void setRibo(bool chk) {
		val_isRibo = chk;
	}

	bool isRibo() {
		return val_isRibo;
	}

	void setCNum(int n) {
		clusterNum = n;
	}

	int getCNum() {
		return clusterNum;
	}

	void setGNum(int n) {
		geneNum = n;
	}

	int getGNum() {
		return geneNum;
	}

	// 다른 데이터와 euclidean distance를 구하는 함수.
	double getDist(riboData data) {
		double dist = 0;
		for (int i = 0; i < 79; i++) {
			dist += (ex_result[i] - data.ex_result[i]) * (ex_result[i] - data.ex_result[i]);
		}

		return sqrt(dist);
	}
};

// data를 담을 벡터.
vector<riboData> dataVec;
// 클러스터를 담을 벡터. 동적할당을 해준뒤 사용함.
vector<riboData*> *clusters;
// 각 클러스터의 중심점을 담을 벡터.
riboData* *center_c;
// Euclidean distance를 담을 priority queue. 가까운 순서대로 우선순위 큐에 삽입.
priority_queue < pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
// 랜덤하게 choice할 때 중복되지 않도록 하는 visit 배열
bool isRandom[2467];

int main() {
	ios::sync_with_stdio(false);
	cin.tie(NULL);
	cout.tie(NULL);
	srand((unsigned int)time(NULL));
	cout << fixed << setprecision(2);

	int tokIdx = 0, geneNumber = 1;
	riboData rData;

	ifstream ribo_file, nonRibo_file;

	// ribo-data.txt 파일을 열고 해당 내용을 읽음.
	ribo_file.open("ribo-data.txt");
	if (ribo_file.is_open()) {
		while (!ribo_file.eof()) {
			tokIdx = 0;
			string str, tok;
			getline(ribo_file, str);
			stringstream sstr(str);

			rData.setRibo(true);
			rData.setGNum(geneNumber);
			geneNumber++;
			while (sstr >> tok) {
				// 79개의 벡터 데이터를 tokenize하여 저장
				rData.setResult(tokIdx, stod(tok));
				tokIdx++;
			}

			// data vector에 담음.
			dataVec.push_back(rData);
		}
		ribo_file.close();
	}

	// nonribo-data.txt 파일을 열고 해당 내용을 읽음.
	nonRibo_file.open("nonribo-data.txt");
	if (nonRibo_file.is_open()) {
		while (!nonRibo_file.eof()) {
			tokIdx = 0;
			string str, tok;
			getline(nonRibo_file, str);
			stringstream sstr(str);

			rData.setRibo(false);
			rData.setGNum(geneNumber);
			geneNumber++;
			while (sstr >> tok) {
				rData.setResult(tokIdx, stod(tok));
				tokIdx++;
			}

			// data vector에 담음.
			dataVec.push_back(rData);
		}
		nonRibo_file.close();
	}

	// 몇 개의 클러스터로 나눌 것인지 입력 받음.
	int kVal;
	bool changed = false;
	cout << "Enter k value. \n";
	cin >> kVal;
	center_c = new riboData* [kVal + 1];
	clusters = new vector<riboData*>[kVal+1];
	//각 클러스터의 초기 중심점은 랜덤하게 설정한다.
	for (int i = 1; i <= kVal; i++) {
		int randomVal = rand() % 2467;
		while (true) {
			if (!isRandom[randomVal]) {
				isRandom[randomVal] = 1;
				center_c[i] = &dataVec[randomVal];
				break;
			}
			randomVal = rand() % 2467;
		}
	}

	while (true) {
		changed = false;
		for (int i = 1; i <= kVal; i++) {
			clusters[i].clear();
		}
		// 각 데이터별로 클러스터들의 중심과 거리를 비교하여 가장 가까운 클러스터에 속하도록 한다.
		for (int i = 0; i < dataVec.size(); i++) {
			int clus = 0, nowDist = 2100000000;
			for (int j = 1; j <= kVal; j++) {
				if (dataVec[i].getDist(*center_c[j]) < nowDist) {
					clus = j;
					nowDist = dataVec[i].getDist(*center_c[j]);
				}
			}

			dataVec[i].setCNum(clus);
			clusters[clus].push_back(&dataVec[i]);
		}

		// 각 클러스터별로 속한 데이터들의 중심을 구한 뒤 중심과 가장 가까운 거리의 데이터를 priority_queue를 사용해 추출한다.
		for (int i = 1; i <= kVal; i++) {
			rData.clearResult();
			pq = priority_queue < pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>>();
			for (int j = 0; j < clusters[i].size(); j++) {
				rData.addResult(*clusters[i][j]);
			}
			rData.divResult(clusters[i].size());


			for (int j = 0; j < clusters[i].size(); j++) {
				pq.push(make_pair(rData.getDist(*clusters[i][j]), (*clusters[i][j]).getGNum()));
			}
			// 만약 중심이 바뀌었다면 이전 단계를 반복한다.
			if (pq.top().second != center_c[i]->getGNum()) {
				changed = true;
				center_c[i] = &dataVec[pq.top().second - 1];
			}
		}
		
		// 중심이 바뀌지 않았다면 종료한다.
		if (!changed)
			break;
	}

	cout << "K = " << kVal << '\n';
	cout << "RIBO genes : gene number 1 to 121\n";
	cout << "NONRIBO genes : gene number 122 to 2467\n";
	for (int i = 1; i <= kVal; i++) {
		cout << "Cluster " << i << ':';
		cout << "Total number of genes = " << clusters[i].size() << '\n';
		for (int j = 0; j < clusters[i].size(); j++) {
			cout << "index: " << j + 1 << " Gene number = " << clusters[i][j]->getGNum() << '\n';
		}
	}

	ofstream outfile;
	outfile.open("kmc.out");
	if (outfile.is_open()) {
		outfile << "K = " << kVal << '\n';
		outfile << "RIBO genes : gene number 1 to 121\n";
		outfile << "NONRIBO genes : gene number 122 to 2467\n";
		for (int i = 1; i <= kVal; i++) {
			outfile << "Cluster " << i << ':';
			outfile << "Total number of genes = " << clusters[i].size() << '\n';
			for (int j = 0; j < clusters[i].size(); j++) {
				outfile << "index: " << j + 1 << " Gene number = " << clusters[i][j]->getGNum() << '\n';
			}
		}

		outfile.close();
	}
	system("PAUSE");
	return 0;
}