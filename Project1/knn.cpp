#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cmath>
#include <queue>
#include <iomanip>
using namespace std;

// �����͸� ������ Ŭ������ ����. ����, ribo-data����, nonribo-data���� �а��ϴ� ������ ������ ��, knn ������ ������ ����.
class riboData {
private:
	double ex_result[79];
	bool val_isRibo;
	bool knn_isRibo;

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

	double getDist(riboData data) {
		double dist = 0;
		for (int i = 0; i < 79; i++) {
			dist += (ex_result[i] - data.ex_result[i]) * (ex_result[i] - data.ex_result[i]);
		}

		return sqrt(dist);
	}
};

// ribo-data�� ���� ����
vector<riboData> riboVec;
// nonribo-data�� ���� ����
vector<riboData> nonRiboVec;
// Euclidean distance�� ���� priority queue. ����� ������� �켱���� ť�� ����.
priority_queue < pair<double, bool>, vector<pair<double, bool>>, greater<pair<double, bool>>> pq;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(NULL);
	cout.tie(NULL);
	cout << fixed<<setprecision(2);

	int tokIdx = 0;
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
			rData.setKnnRibo(true);
			while (sstr >> tok) {
				// 79���� ���� �����͸� tokenize�Ͽ� ����
				rData.setResult(tokIdx, stod(tok));
				tokIdx++;
			}

			// ribo-data vector�� ����.
			riboVec.push_back(rData);
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
			rData.setKnnRibo(false);
			while (sstr >> tok) {
				rData.setResult(tokIdx, stod(tok));
				tokIdx++;
			}
			
			// nonribo-data vector�� ����.
			nonRiboVec.push_back(rData);
		}
		nonRibo_file.close();
	}

	// k��, p��, 6-fold cross validation�� ���Ͽ� ribo-vec�� nonribo-vec�� idx�� ������ �������� ����.
	int kVal, sumRibo = 0, riboStart = 0, riboEnd = 0, nonRiboStart = 0, nonRiboEnd = 0;
	double p, dist;
	cout << "Enter k and p value.\n";
	cin >> kVal >> p;
	for (int i = 0; i < 6; i++) {
		//switch case���� �̿��Ͽ� 6���� part�� ����.
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

		// ribo-vector�� �����͸� ribo-vec�� �ش� �����Ͱ� ���� set�� �����ϰ� distance�� ����Ͽ� priority_queue�� ����.
		for (int j = riboStart; j < riboEnd; j++) {
			for (int k = 0; k < 121; k++) {
				if (k < riboStart || k >= riboEnd) {
					pq.push(make_pair(riboVec[j].getDist(riboVec[k]), riboVec[k].knnRibo()));
				}
			}

			for (int k = 0; k < 2346; k++) {
				pq.push(make_pair(riboVec[j].getDist(nonRiboVec[k]), nonRiboVec[k].knnRibo()));
			}

			// ������� priority_queue���� pop�� �� �ش� �����Ͱ� ribo���� nonribo���� check.
			sumRibo = 0;
			for (int k = 0; k < kVal; k++) {
				if (pq.top().second) {
					sumRibo++;
				}
				pq.pop();
			}

			// check�� ����� p���� ������ nonribo��. �ݴ�� ribo.
			if ((double)sumRibo / kVal < p)
				riboVec[j].setKnnRibo(0);
			else
				riboVec[j].setKnnRibo(1);

			// priority_queue�� �ʱ�ȭ.
			pq = priority_queue < pair<double, bool>, vector<pair<double, bool>>, greater<pair<double, bool>>>();
		}
		

		// nonribo-vector�� �����͸� nonribo-vec�� �ش� �����Ͱ� ���� set�� �����ϰ� distance�� ����Ͽ� priority_queue�� ����.
		for (int j = nonRiboStart; j < nonRiboEnd; j++) {
			for (int k = 0; k < 121; k++) {
				pq.push(make_pair(nonRiboVec[j].getDist(riboVec[k]), riboVec[k].knnRibo()));
			}

			for (int k = 0; k < 2346; k++) {
				if (k < nonRiboStart || k >= nonRiboEnd) {
					pq.push(make_pair(nonRiboVec[j].getDist(nonRiboVec[k]), nonRiboVec[k].knnRibo()));
				}
			}

			// ������� priority_queue���� pop�� �� �ش� �����Ͱ� ribo���� nonribo���� check.
			sumRibo = 0;
			for (int k = 0; k < kVal; k++) {
				if (pq.top().second) {
					sumRibo++;
				}
				pq.pop();
			}

			// check�� ����� p���� ������ nonribo��. �ݴ�� ribo.
			if ((double)sumRibo / kVal < p)
				nonRiboVec[j].setKnnRibo(0);
			else
				nonRiboVec[j].setKnnRibo(1);

			// priority_queue�� �ʱ�ȭ.
			pq = priority_queue < pair<double, bool>, vector<pair<double, bool>>, greater<pair<double, bool>>>();
		}
		
		
	}

	//���� True positive, true negative, false positive, false negative
	double tp = 0, tn = 0, fp = 0, fn = 0;
	// ���� sensitivity, specificity, accuracy
	double sens, spec, accu;

	// ribo-vector�� ���� ���.
	for (int i = 0; i < riboVec.size(); i++) {
		// Positive �̰� knn���� ���� ���� ���ٸ� True Positive
		if (riboVec[i].isRibo() && riboVec[i].isRibo() == riboVec[i].knnRibo()) {
			tp++;
		}
		// Negative �̰� knn���� ���� ���� ���ٸ� True Negative
		else if (!riboVec[i].isRibo() && riboVec[i].isRibo() == riboVec[i].knnRibo()) {
			tn++;
		}
		// Positive �̰� knn���� ���� ���� �ٸ��ٸ� False Positive
		else if (riboVec[i].isRibo() && riboVec[i].isRibo() != riboVec[i].knnRibo()) {
			fp++;
		}
		// Negative �̰� knn���� ���� ���� �ٸ��ٸ� False Negative
		else if (!riboVec[i].isRibo() && riboVec[i].isRibo() != riboVec[i].knnRibo()) {
			fn++;
		}
	}

	// nonribo-vector�� ���� ���.
	for (int i = 0; i < nonRiboVec.size(); i++) {
		// Positive �̰� knn���� ���� ���� ���ٸ� True Positive
		if (nonRiboVec[i].isRibo() && nonRiboVec[i].isRibo() == nonRiboVec[i].knnRibo()) {
			tp++;
		}
		// Negative �̰� knn���� ���� ���� ���ٸ� True Negative
		else if (!nonRiboVec[i].isRibo() && nonRiboVec[i].isRibo() == nonRiboVec[i].knnRibo()) {
			tn++;
		}
		// Positive �̰� knn���� ���� ���� �ٸ��ٸ� False Positive
		else if (nonRiboVec[i].isRibo() && nonRiboVec[i].isRibo() != nonRiboVec[i].knnRibo()) {
			fp++;
		}
		// Negative �̰� knn���� ���� ���� �ٸ��ٸ� False Negative
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
	return 0;
}