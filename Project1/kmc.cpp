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

// �����͸� ������ Ŭ������ ����. ����, ���� cluster�� number, gene number ���� ������ ����.
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

	// �ٸ� �����Ϳ� euclidean distance�� ���ϴ� �Լ�.
	double getDist(riboData data) {
		double dist = 0;
		for (int i = 0; i < 79; i++) {
			dist += (ex_result[i] - data.ex_result[i]) * (ex_result[i] - data.ex_result[i]);
		}

		return sqrt(dist);
	}
};

// data�� ���� ����.
vector<riboData> dataVec;
// Ŭ�����͸� ���� ����. �����Ҵ��� ���ص� �����.
vector<riboData*> *clusters;
// �� Ŭ�������� �߽����� ���� ����.
riboData* *center_c;
// Euclidean distance�� ���� priority queue. ����� ������� �켱���� ť�� ����.
priority_queue < pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
// �����ϰ� choice�� �� �ߺ����� �ʵ��� �ϴ� visit �迭
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

	// ribo-data.txt ������ ���� �ش� ������ ����.
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
				// 79���� ���� �����͸� tokenize�Ͽ� ����
				rData.setResult(tokIdx, stod(tok));
				tokIdx++;
			}

			// data vector�� ����.
			dataVec.push_back(rData);
		}
		ribo_file.close();
	}

	// nonribo-data.txt ������ ���� �ش� ������ ����.
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

			// data vector�� ����.
			dataVec.push_back(rData);
		}
		nonRibo_file.close();
	}

	// �� ���� Ŭ�����ͷ� ���� ������ �Է� ����.
	int kVal;
	bool changed = false;
	cout << "Enter k value. \n";
	cin >> kVal;
	center_c = new riboData* [kVal + 1];
	clusters = new vector<riboData*>[kVal+1];
	//�� Ŭ�������� �ʱ� �߽����� �����ϰ� �����Ѵ�.
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
		// �� �����ͺ��� Ŭ�����͵��� �߽ɰ� �Ÿ��� ���Ͽ� ���� ����� Ŭ�����Ϳ� ���ϵ��� �Ѵ�.
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

		// �� Ŭ�����ͺ��� ���� �����͵��� �߽��� ���� �� �߽ɰ� ���� ����� �Ÿ��� �����͸� priority_queue�� ����� �����Ѵ�.
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
			// ���� �߽��� �ٲ���ٸ� ���� �ܰ踦 �ݺ��Ѵ�.
			if (pq.top().second != center_c[i]->getGNum()) {
				changed = true;
				center_c[i] = &dataVec[pq.top().second - 1];
			}
		}
		
		// �߽��� �ٲ��� �ʾҴٸ� �����Ѵ�.
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