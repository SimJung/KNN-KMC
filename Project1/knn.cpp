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
	bool knn_isRibo;
	int geneNum;

public:
	riboData() {
		for (int i = 0; i < 79; i++) {
			ex_result[i] = 0;
		}
		val_isRibo = false;
		knn_isRibo = false;
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

	void setKnnRibo(bool chk) {
		knn_isRibo = chk;
	}

	bool knnRibo() {
		return knn_isRibo;
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

// ribo-data를 담을 벡터
vector<riboData> riboVec;
// nonribo-data를 담을 벡터
vector<riboData> nonRiboVec;
// Euclidean distance를 담을 priority queue. 가까운 순서대로 우선순위 큐에 삽입.
priority_queue < pair<double, bool>, vector<pair<double, bool>>, greater<pair<double, bool>>> pq;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(NULL);
	cout.tie(NULL);
	cout << fixed<<setprecision(2);

	int tokIdx = 0, gn = 1;
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
			rData.setKnnRibo(true);
			rData.setGNum(gn);
			gn++;
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
			rData.setKnnRibo(false);
			rData.setGNum(gn);
			gn++;
			while (sstr >> tok) {
				rData.setResult(tokIdx, stod(tok));
				tokIdx++;
			}
			
			// nonribo-data vector에 담음.
			nonRiboVec.push_back(rData);
		}
		nonRibo_file.close();
	}

	// k값, p값, 6-fold cross validation을 위하여 ribo-vec과 nonribo-vec의 idx를 지정할 변수들을 선언.
	int kVal, sumRibo = 0, riboStart = 0, riboEnd = 0, nonRiboStart = 0, nonRiboEnd = 0;
	double p, dist;
	cout << "Enter k and p value.\n";
	cin >> kVal >> p;
	for (int i = 0; i < 6; i++) {
		//switch case문을 이용하여 6개의 part로 나눔.
		switch (i)
		{
		case 0:
			riboStart = 0;
			riboEnd = 20;
			nonRiboStart = 0;
			nonRiboEnd = 391;
			break;

		case 1:
			riboStart = 20;
			riboEnd = 40;
			nonRiboStart = 391;
			nonRiboEnd = 782;
			break;

		case 2:
			riboStart = 40;
			riboEnd = 60;
			nonRiboStart = 782;
			nonRiboEnd = 1173;
			break;

		case 3:
			riboStart = 60;
			riboEnd = 80;
			nonRiboStart = 1173;
			nonRiboEnd = 1564;
			break;

		case 4:
			riboStart = 80;
			riboEnd = 100;
			nonRiboStart = 1564;
			nonRiboEnd = 1955;
			break;

		case 5:
			riboStart = 100;
			riboEnd = 121;
			nonRiboStart = 1955;
			nonRiboEnd = 2346;
			break;
		}

		// ribo-vector의 데이터를 ribo-vec의 해당 데이터가 속한 set을 제외하고 distance를 계산하여 priority_queue에 삽입.
		for (int j = riboStart; j < riboEnd; j++) {
			for (int k = 0; k < 121; k++) {
				if (k < riboStart || k >= riboEnd) {
					pq.push(make_pair(riboVec[j].getDist(riboVec[k]), riboVec[k].knnRibo()));
				}
			}

			for (int k = 0; k < 2346; k++) {
				pq.push(make_pair(riboVec[j].getDist(nonRiboVec[k]), nonRiboVec[k].knnRibo()));
			}

			// 순서대로 priority_queue에서 pop한 뒤 해당 데이터가 ribo인지 nonribo인지 check.
			sumRibo = 0;
			for (int k = 0; k < kVal; k++) {
				if (pq.top().second) {
					sumRibo++;
				}
				pq.pop();
			}

			// check한 결과가 p보다 낮으면 nonribo임. 반대면 ribo.
			if ((double)sumRibo / kVal < p)
				riboVec[j].setKnnRibo(0);
			else
				riboVec[j].setKnnRibo(1);

			// priority_queue를 초기화.
			pq = priority_queue < pair<double, bool>, vector<pair<double, bool>>, greater<pair<double, bool>>>();
		}
		

		// nonribo-vector의 데이터를 nonribo-vec의 해당 데이터가 속한 set을 제외하고 distance를 계산하여 priority_queue에 삽입.
		for (int j = nonRiboStart; j < nonRiboEnd; j++) {
			for (int k = 0; k < 121; k++) {
				pq.push(make_pair(nonRiboVec[j].getDist(riboVec[k]), riboVec[k].knnRibo()));
			}

			for (int k = 0; k < 2346; k++) {
				if (k < nonRiboStart || k >= nonRiboEnd) {
					pq.push(make_pair(nonRiboVec[j].getDist(nonRiboVec[k]), nonRiboVec[k].knnRibo()));
				}
			}

			// 순서대로 priority_queue에서 pop한 뒤 해당 데이터가 ribo인지 nonribo인지 check.
			sumRibo = 0;
			for (int k = 0; k < kVal; k++) {
				if (pq.top().second) {
					sumRibo++;
				}
				pq.pop();
			}

			// check한 결과가 p보다 낮으면 nonribo임. 반대면 ribo.
			if ((double)sumRibo / kVal < p)
				nonRiboVec[j].setKnnRibo(0);
			else
				nonRiboVec[j].setKnnRibo(1);

			// priority_queue를 초기화.
			pq = priority_queue < pair<double, bool>, vector<pair<double, bool>>, greater<pair<double, bool>>>();
		}
		
		
	}

	//각각 True positive, true negative, false positive, false negative
	double tp = 0, tn = 0, fp = 0, fn = 0;
	// 각각 sensitivity, specificity, accuracy
	double sens, spec, accu;

	// ribo-vector에 대해 계산.
	for (int i = 0; i < riboVec.size(); i++) {
		// Positive 이고 knn값과 원래 값이 같다면 True Positive
		if (riboVec[i].isRibo() && riboVec[i].isRibo() == riboVec[i].knnRibo()) {
			tp++;
		}
		// Negative 이고 knn값과 원래 값이 같다면 True Negative
		else if (!riboVec[i].isRibo() && riboVec[i].isRibo() == riboVec[i].knnRibo()) {
			tn++;
		}
		// Positive 이고 knn값과 원래 값이 다르다면 False Positive
		else if (riboVec[i].isRibo() && riboVec[i].isRibo() != riboVec[i].knnRibo()) {
			fp++;
		}
		// Negative 이고 knn값과 원래 값이 다르다면 False Negative
		else if (!riboVec[i].isRibo() && riboVec[i].isRibo() != riboVec[i].knnRibo()) {
			fn++;
		}
	}

	// nonribo-vector에 대해 계산.
	for (int i = 0; i < nonRiboVec.size(); i++) {
		// Positive 이고 knn값과 원래 값이 같다면 True Positive
		if (nonRiboVec[i].isRibo() && nonRiboVec[i].isRibo() == nonRiboVec[i].knnRibo()) {
			tp++;
		}
		// Negative 이고 knn값과 원래 값이 같다면 True Negative
		else if (!nonRiboVec[i].isRibo() && nonRiboVec[i].isRibo() == nonRiboVec[i].knnRibo()) {
			tn++;
		}
		// Positive 이고 knn값과 원래 값이 다르다면 False Positive
		else if (nonRiboVec[i].isRibo() && nonRiboVec[i].isRibo() != nonRiboVec[i].knnRibo()) {
			fp++;
		}
		// Negative 이고 knn값과 원래 값이 다르다면 False Negative
		else if (!nonRiboVec[i].isRibo() && nonRiboVec[i].isRibo() != nonRiboVec[i].knnRibo()) {
			fn++;
		}
	}

	sens = tp / (tp + fn);
	spec = tn / (tn + fp);
	accu = (tp + tn) / (tp + tn + fp + fn);
	cout << "k : " << kVal << '\n';
	cout << "p : " << p << '\n';
	cout << "Sensitivity : " << sens << '\n';
	cout << "Specificity : " << spec << '\n';
	cout << "Accuracy : " << accu << '\n';

	ofstream outfile;
	outfile.open("knn.out");
	if (outfile.is_open()) {
		outfile << "k: " << kVal << '\n';
		outfile << "p : " << p << '\n';
		outfile << "Sensitivity : " << sens << '\n';
		outfile << "Specificity : " << spec << '\n';
		outfile << "Accuracy : " << accu << '\n';

		outfile.close();
	}
	system("PAUSE");
	return 0;
}