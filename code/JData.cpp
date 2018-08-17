// JData.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "read_file.h"
#include "logistics.h"
#include "population.h"
#include "synchroSearch.h"

#include <iostream>
#include <algorithm>
#include <iomanip>
#include <Windows.h>
#include <algorithm>
using namespace std;
const int K = 10;

vector<int> sellMatchCharger;  //�±��Ӧsell��id�������� charger���±�

int main()
{
	DWORD start_time = GetTickCount();
	{
		//double t = 43.5;
		//cout << setiosflags(ios::fixed) << setprecision(2) << t << endl;
		vector<Vehicle> vehicle;    //���泵����Ϣ
		vehicle.push_back(Vehicle(1, "iveco", 12, 2.0, 100000, 0.5, 0.012, 200));
		vehicle.push_back(Vehicle(2, "truck", 16, 2.5, 120000, 0.5, 0.014, 300));
		//showVehicle(vehicle[0]);
		//showVehicle(vehicle[1]);
		vector<Node> nodeList;  //����input_node����
		read_node(nodeList);    //��ȡinput_node.txt����

		vector<Node> seller(nodeList.begin() + 1, nodeList.begin() + 1001); //1-1000�̻���Ϣ
		vector<Node> charger(nodeList.begin() + 1001, nodeList.end());

		vector<vector<int>> distance, time;
		read_distance(distance, time);

		//vector<vector<Node>> kSellers;            //���յ���λ�÷���
		//kSellers = kMeans(seller, K);

		//vector<CarRoute> result,tempResult;    //������
		//for (size_t i = 0; i < kSellers.size(); ++i)
		//{
		//	tempResult = execute(kSellers[i], charger, distance, time, vehicle);
		//	result.insert(result.end(), tempResult.begin(), tempResult.end());
		//}

		calSellChargerMatch(sellMatchCharger, distance, charger, seller);	//����sellMatchCharger;
																			//�±��Ӧsell��id�������� charger���±�

		//vector<CarRoute> result;
		//result = execute(seller, charger, distance, time, vehicle);

		//cout << "after execute,total cost:" << calAllCost(result, vehicle) << endl;


		vector<CarRoute> result;
		result = synchroSearch(seller, charger, distance, time, vehicle);
		cout << "after synchroSearch,total cost:" << calAllCost(result, vehicle) << endl;


		//�Ŵ��㷨
		inheritance(result, distance, time, vehicle, nodeList,charger);



		//cout << endl;
		//showResult(result);

		//changeVehicleType(result,distance,vehicle);		//ת��������ͺ�
		changeVehicleType2(result, distance, time, vehicle, nodeList, charger);

		sort(result.begin(), result.end(), lessVehicleType);

		vector<AnswerRecord> answer;
		answer = getAnswer(result, vehicle);

		whrit_answer(answer);

	}
	DWORD end_time = GetTickCount(); //������ʱ
	cout << "The run time is:" << (end_time - start_time) / 1000 << "s!" << endl; //���ʱ��
	return 0;
}


