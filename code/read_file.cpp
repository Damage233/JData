#include "stdafx.h"

#include "read_file.h"
#include "logistics.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
void read_node(vector<Node>& vec)
{
	ifstream infile("input_node.txt");
	string line;
	getline(infile, line);
	int lineNum = 0;
	while (getline(infile, line)) {
		Node tempNode;
		istringstream stream(line);
		char c;
		if (lineNum == 0) {
			stream >> tempNode.id
				>> tempNode.type
				>> tempNode.lng >> tempNode.lat
				>> c >> c
				>> tempNode.first_receive_tm >> tempNode.last_receive_tm;
			tempNode.weight = tempNode.volume = 0;
			tempNode.first_int_tm = 0;
			tempNode.last_int_tm = 960;
			tempNode.isServer = false;
		}
		else if (lineNum>0 && lineNum<1001) {
			stream >> tempNode.id
				>> tempNode.type
				>> tempNode.lng >> tempNode.lat
				>> tempNode.weight >> tempNode.volume
				>> tempNode.first_receive_tm >> tempNode.last_receive_tm;
			tempNode.first_int_tm = changeTime(tempNode.first_receive_tm);
			tempNode.last_int_tm = changeTime(tempNode.last_receive_tm);
			tempNode.isServer = false;
		}
		else {
			stream >> tempNode.id
				>> tempNode.type
				>> tempNode.lng >> tempNode.lat;
			tempNode.weight = tempNode.volume = 0;
			tempNode.first_receive_tm = tempNode.last_receive_tm = "";
			tempNode.first_int_tm = -1;
			tempNode.last_int_tm = -1;
			tempNode.isServer = false;
		}
		++lineNum;
		vec.push_back(tempNode);
	}

}
void read_distance(vector<vector<int>>& dis, vector<vector<int>>& time)
{
	ifstream infile("input_distance-time.txt");
	string line;
	getline(infile, line);

	for (int i = 0; i <= 1100; ++i) {

		vector<int> di, tm;
		for (int j = 0; j <= 1100; ++j)
		{
			if (i == j) {
				di.push_back(0);
				tm.push_back(0);
			}
			else {
				getline(infile, line);
				istringstream stream(line);
				int a, d, t;
				char c;
				stream >> a >> c
					>> a >> c
					>> a >> c
					>> d >> c
					>> t >> c;
				di.push_back(d);
				tm.push_back(t);
			}
		}
		dis.push_back(di);
		time.push_back(tm);
	}
}
void whrit_answer(vector<AnswerRecord> answer)
{
	// 写文件
	ofstream outFile;
	outFile.open("Result.csv"); // 打开模式可省略
	//outFile << "派车单号" << ',' << "车型" << ',' << "顺序" << ','
	//	<< "配送中心出发时间" << ',' << "配送中心返回时间" << ',' << "里程(m)" << ','
	//	<< "运输成本(元)" << ',' << "充电成本(元)" << ',' << "等待成本(元)" << ','
	//	<< "固定成本(元)" << ',' << "总成本(元)" << ',' << "充电次数" 
	//	<< endl;
	outFile 
		<< "trans_code" << ',' << "vehicle_type" << ',' << "dist_seq" << ','
		<< "distribute_lea_tm" << ',' << "distribute_arr_tm" << ',' << "distance" << ','
		<< "trans_cost" << ',' << "charge_cost" << ',' << "wait_cost" << ','
		<< "fixed_use_cost" << ',' << "total_cost" << ',' << "charge_cnt"
		<< endl;
	for (size_t i = 0; i < answer.size(); ++i)
	{
		outFile
			<< answer[i].trans_code << ','
			<< answer[i].vehicle_type << ','
			<< answer[i].dist_seq << ','
			<< answer[i].distribute_lea_tm<< ','
			<< answer[i].distribute_arr_tm<< ','
			<< answer[i].distance << ',';
		outFile
			<< setiosflags(ios::fixed) << setprecision(2)
			<< answer[i].trans_cost << ',';
		outFile
			<< answer[i].charge_cost << ',';
		outFile
			<< setiosflags(ios::fixed) << setprecision(2)
			<< answer[i].wait_cost << ',';
		outFile
			<< answer[i].fixed_use_cost << ',';
		outFile
			<< setiosflags(ios::fixed) << setprecision(2)
			<< answer[i].total_cost << ',';
		outFile
			<< answer[i].charge_cnt << ','
			<< endl;
	}
	outFile.close();
}

void showVehicle(const Vehicle& vehicle)
{
	cout << vehicle.id << endl;
	cout << vehicle.name << endl;
	cout << vehicle.volume << endl;
	cout << vehicle.weight << endl;
	cout << vehicle.driving_range << endl;
	cout << vehicle.charge_tm << endl;
	cout << vehicle.unit_trans_cost << endl;
	cout << vehicle.vechile_cost << endl;
	cout << endl;
}
void showNodeList(const vector<Node>& vec)
{
	for (auto node : vec)
	{
		cout << node.id << " " << node.type << " " << node.lng << " " << node.lat << " " << node.weight << " "
			<< node.volume << " " << node.first_receive_tm << " " << node.last_receive_tm << " "
			<< node.first_int_tm << " " << node.last_int_tm << endl;
	}
}
void showResult(const vector<CarRoute>& result)
{
	cout << "result:" << endl;
	for (size_t i = 0; i < result.size(); ++i)
	{
		cout << result[i].cartype << " " << result[i].leaveTime << " " << result[i].backTime << " "
			<< result[i].totalMileage << " " << result[i].chargeNum << " " << result[i].waitTime << " " << endl;
		for (size_t j = 0; j < result[i].route.size(); ++j)
			cout << result[i].route[j] << ",";
		cout << endl;
	}
}









