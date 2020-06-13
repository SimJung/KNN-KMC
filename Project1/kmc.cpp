#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <queue>
#include <iomanip>
using namespace std;

// 데이터를 저장할 클래스를 정의. 벡터, ribo-data인지, nonribo-data인지 분간하는 변수가 원래의 값, knn 이후의 값으로 나뉨.
class riboData {
private:
	double ex_result[79];
	bool val_isRibo;

public:
	riboData() {
		for (int i = 0; i < 79; i++) {
			ex_result[i] = 0;
		}
		val_isRibo = false;
	}

	void setResult(int idx, double value) {
		ex_result[idx] = value;
	}

	double getResult(int idx) {
		return ex_result[idx];
	}

	void setRibo(bool chk) {
		val_isRibo = chk;
	}

	bool isRibo() {
		return val_isRibo;
	}

	double getDist(riboData data) {
		double dist = 0;
		for (int i = 0; i < 79; i++) {
			dist += (ex_result[i] - data.ex_result[i]) * (ex_result[i] - data.ex_result[i]);
		}

		return sqrt(dist);
	}
};

// ribo-data를 담을 벡터
vector<riboData> riboVec;
// nonribo-data를 담을 벡터
vector<riboData> nonRiboVec;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(NULL);
	cout.tie(NULL);
	cout << fixed << setprecision(2);

	int tokIdx = 0;
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
			while (sstr >> tok) {
				// 79개의 벡터 데이터를 tokenize하여 저장
				rData.setResult(tokIdx, stod(tok));
				tokIdx++;
			}

			// ribo-data vector에 담음.
			riboVec.push_back(rData);
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
			while (sstr >> tok) {
				rData.setResult(tokIdx, stod(tok));
				tokIdx++;
			}

			// nonribo-data vector에 담음.
			nonRiboVec.push_back(rData);
		}
		nonRibo_file.close();
	}

	return 0;
}