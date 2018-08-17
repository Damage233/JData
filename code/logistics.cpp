#include "stdafx.h"
#include "logistics.h"
#include "population.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

#include <string>
using namespace std;


int changeTime(const string& str) //��ʱ�任��int
{
	if (str == "00:00")
		return 960;
	else {
		istringstream stream(str);
		int h, m;
		char c;
		stream >> h >> c >> m;
		return (h - 8) * 60 + m;
	}
}
bool lastTimeCompare(Node node1, Node node2)
{
	if (node1.last_int_tm == node2.last_int_tm)
		return node1.first_int_tm<node2.first_int_tm;
	else
		return node1.last_int_tm<node2.last_int_tm;
}
//�����ڵ� �����������·��
vector<CarRoute> execute(vector<Node>& seller, const vector<Node>& charger, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle)
{
	//int doNum = CURCAL_TIME;                //ģ���˻��㷨��ѭ������
	//vector<CarRoute> minResult;
	//double minResultCost = MAX_DOUBLE;
	//srand(SEED);
	//while (--doNum)
	//{
		vector<CarRoute> result;
		while (!isAllserver(seller))
		{
			CarRoute curCar;
			curCar.route.push_back(0);              //����������Ϊ���������
			while (isConstraint(curCar, vehicle))             //ѡ��ڵ�
			{
				int indexMin;
				indexMin = nextVisitIndex(curCar, seller, distance, time, vehicle); //��ͨ�����վ��ֱ�ӿ��Է��ʵ�����̻�
				//if (rand() % 100 < probability)
				//	indexMin = nextVisitIndex(curCar, seller, distance, time, vehicle); //��ͨ�����վ��ֱ�ӿ��Է��ʵ�����̻�
				//else
				//	indexMin = nextRandomVisitIndex(curCar, seller, distance, time, vehicle);

				if (indexMin != -1) //�ҵ�����һ�����ʵ��̻�
				{
					visitSeller(curCar, seller[indexMin], distance, time);
				}
				else
				{
					if (!isGoback(curCar, vehicle))
					{
						int sellIndex = nextSellCharger(curCar, seller, charger, distance, time, vehicle); //�ȵ����̻���ȥ���



						if (sellIndex != -1) //�ҵ����� �ȵ����̻���ȥ��������̻�
						{
							visitSeller(curCar, seller[sellIndex], distance, time); //�����̻�

							int closeChargerIndex = sellMatchCharger[seller[sellIndex].id]; //������վ����
							visitCharge(curCar, charger[closeChargerIndex], distance, time);
						}
						else
						{
							//cout << "���ֽ�����";
							//for (size_t i = 0; i < curCar.route.size(); ++i)
							//	cout << curCar.route[i] << " ";
							//cout << endl;
							break;
						}

					}
					else
					{
						//cout << "�����ӽ����أ����ֽ���:";
						//for (size_t i = 0; i < curCar.route.size(); ++i)
						//	cout << curCar.route[i] << " ";
						//cout << endl;
						break;
					}
				}
			}
			result.push_back(curCar);
		}
		//changeVehicleType(result, distance, vehicle);		//ת��������ͺ�
		return result;
	//	double curResultCost = calAllCost(result, vehicle);
	//	std::cout << "curResultCost:" << curResultCost << endl;
	//	if (curResultCost < minResultCost)
	//	{
	//		minResultCost = curResultCost;
	//		minResult = result;
	//	}
	//	//����seller��״̬ isServer
	//	for (size_t i = 0; i < seller.size(); ++i)
	//	{
	//		seller[i].isServer = false;
	//	}
	//}
	//return minResult;
}

bool isAllserver(const vector<Node>& seller)
{
	for (size_t i = 0; i < seller.size(); ++i)
	{
		if (seller[i].isServer == false)
			return false;
	}
	return true;
}
bool isConstraint(const CarRoute& curCar,const vector<Vehicle>& vehicle) //Լ������
{
	if (curCar.curVolume > vehicle[curCar.cartype - 1].volume) //��������Լ��
	{
		return false;
	}
	if (curCar.curWeight > vehicle[curCar.cartype - 1].weight)  //��������Լ��
	{
		return false;
	}
	if (curCar.curMileage > vehicle[curCar.cartype - 1].driving_range)//���Լ�� ��ע�⣺�Ȳ�������;�������
	{
		return false;
	}
	if (curCar.curTime > 960)  //����ʱ��Լ��
	{
		return false;
	}


	return true;
}

//����curCar ����������Ƿ����ֱ�ӷ���sell
bool canVisit(CarRoute curCar, const vector<Vehicle>& vehicle, const Node& sell
	, const vector<vector<int>>& distance, const vector<vector<int>>& time)
{
	curCar.curVolume += sell.volume;
	curCar.curWeight += sell.weight;

	int lastIndex = curCar.route.back(); //·�������һ���̻��ڵ�
	curCar.curMileage = curCar.curMileage + distance[lastIndex][sell.id]
		+ distance[sell.id][0] - distance[lastIndex][0];
	//ʱ�䴰Լ����
	// ����ʱ��������̻�����ٽ���ʱ��֮ǰ
	int arriveTime = curCar.curTime - time[lastIndex][0] + time[lastIndex][sell.id];
	if (arriveTime > sell.last_int_tm)
		return false;

	int waitT = sell.first_int_tm > arriveTime ? sell.first_int_tm - arriveTime : 0;

	curCar.curTime = arriveTime + waitT + 30 + time[sell.id][0];
	return isConstraint(curCar, vehicle);
}

double calWeight(const CarRoute& curCar, const vector<Vehicle>& vehicle, const Node& sell
	, const vector<vector<int>>& distance, const vector<vector<int>>& time)
{
	int lastIndex = curCar.route.back();
	double dis = distance[lastIndex][sell.id];
	double arriveTime = curCar.curTime - time[lastIndex][0] + time[lastIndex][sell.id];
	//ʱ�䴰Լ��
	if (arriveTime > sell.last_int_tm)
		return MAX_DOUBLE;

	double waitTime = sell.first_int_tm > arriveTime ? sell.first_int_tm - arriveTime : 0;
	double canDelayTime = min(static_cast<double>(sell.last_int_tm - sell.first_int_tm)
		, sell.last_int_tm - arriveTime);

	//return A * time[lastIndex][sell.id]+ B * waitTime + C * canDelayTime ; //��ʱ��ΪȨ��

	return A * (dis * vehicle[curCar.cartype - 1].unit_trans_cost)  //�Է���
		+ B * (waitTime * 0.40) + C * (canDelayTime * 0.40);
	//return A * (dis * vehicle[curCar.cartype - 1].unit_trans_cost)
	//		+ C * (canDelayTime * 0.4);
}
double calRandomWeight(const CarRoute& curCar, const vector<Vehicle>& vehicle, const Node& sell
	, const vector<vector<int>>& distance, const vector<vector<int>>& time)
{
	int lastIndex = curCar.route.back();
	double dis = distance[lastIndex][sell.id];
	double arriveTime = curCar.curTime - time[lastIndex][0] + time[lastIndex][sell.id];
	//ʱ�䴰Լ��
	if (arriveTime > sell.last_int_tm)
		return MAX_DOUBLE;

	double waitTime = sell.first_int_tm > arriveTime ? sell.first_int_tm - arriveTime : 0;
	double canDelayTime = min(static_cast<double>(sell.last_int_tm - sell.first_int_tm)
		, sell.last_int_tm - arriveTime);

	return 1 * time[lastIndex][sell.id] + 0.2 * waitTime + 0.2 * canDelayTime; //��ʱ��ΪȨ��
}

void visitSeller(CarRoute& curCar, Node& sell,
	const vector<vector<int>>& distance, const vector<vector<int>>& time)
{
	//����sell�̻�
	sell.isServer = true;
	curCar.curVolume += sell.volume;
	curCar.curWeight += sell.weight;

	int lastIndex = curCar.route.back();
	curCar.curMileage = curCar.curMileage + distance[lastIndex][sell.id]
		+ distance[sell.id][0] - distance[lastIndex][0];

	int arriveTime = curCar.curTime - time[lastIndex][0] + time[lastIndex][sell.id];
	int waitT = sell.first_int_tm > arriveTime ?
		sell.first_int_tm - arriveTime : 0;
	//���³���ʱ��,����ʱ��Ϊ �����ڵ�һ�ε����̻��ĵȴ�ʱ��
	if (curCar.leaveTime == -1)
	{
		curCar.leaveTime = waitT;
	}
	else {
		curCar.waitTime += waitT;
	}

	curCar.curTime = arriveTime + waitT + 30 + time[sell.id][0];
	//����ʱ��+�ȴ�+����+����

	curCar.route.push_back(sell.id);
	curCar.totalMileage = curCar.totalMileage + distance[lastIndex][sell.id]
		+ distance[sell.id][0] - distance[lastIndex][0];

	curCar.backTime = curCar.curTime;
	//curCar.waitTime += waitT;

}

int nextVisitIndex(const CarRoute& curCar, const vector<Node>& seller,
	const vector<vector<int>>& distance, const vector<vector<int>>& time, const vector<Vehicle>& vehicle)
{
	double minWeight = MAX_DOUBLE;
	int indexMin = -1; //������һ�����ʵ��̻�
	for (size_t i = 0; i < seller.size(); ++i)   //�����̻���Ѱ����һ���ڵ�
	{
		if (!seller[i].isServer&&canVisit(curCar, vehicle, seller[i], distance, time)) //sell[i]δ�����ʣ��ҿ��Է���
		{                                                                         //  ���ҿ��Է���
			double weight = calWeight(curCar, vehicle, seller[i], distance, time);
			if (weight < minWeight)
			{
				minWeight = weight;
				indexMin = i;
			}

		}
	}
	return indexMin;
}
int nextRandomVisitIndex(const CarRoute& curCar, const vector<Node>& seller,
	const vector<vector<int>>& distance, const vector<vector<int>>& time, const vector<Vehicle>& vehicle)
{
	double minWeight = MAX_DOUBLE;
	int indexMin = -1; //������һ�����ʵ��̻�
	for (size_t i = 0; i < seller.size(); ++i)   //�����̻���Ѱ����һ���ڵ�
	{
		if (!seller[i].isServer&&canVisit(curCar, vehicle, seller[i], distance, time)) //sell[i]δ�����ʣ��ҿ��Է���
		{                                                                         //  ���ҿ��Է���
			double weight = calRandomWeight(curCar, vehicle, seller[i], distance, time);
			if (weight < minWeight)
			{
				minWeight = weight;
				indexMin = i;
			}

		}
	}
	return indexMin;
}


bool isGoback(const CarRoute& curCar, const vector<Vehicle>& vehicle)
{
	if (curCar.curVolume / vehicle[curCar.cartype - 1].volume > GOBACK) //��ǰ�����Ѿ�ʹ��80%ֱ�ӷ���
	{
		//cout << "��������" << curCar.curVolume << " " << vehicle[curCar.cartype - 1].volume << endl;
		return true;
	}
	if (curCar.curWeight / vehicle[curCar.cartype - 1].weight > GOBACK)
	{
		//cout << "���ؼ���" << curCar.curWeight << " " << vehicle[curCar.cartype - 1].weight << endl;
		return true;
	}
	return false;
}
//��ȥ�껧 ��ȡ���վ
int nextSellCharger(const CarRoute& curCar, const vector<Node>& seller, const vector<Node>& charger,
	const vector<vector<int>>& distance, const vector<vector<int>>& time, const vector<Vehicle>& vehicle)
{
	double minWeight = MAX_DOUBLE;
	int indexMin = -1; //������һ�����ʵ��̻�
	for (size_t i = 0; i < seller.size(); ++i)   //�����̻���Ѱ����һ���ڵ�
	{
		if (!seller[i].isServer&&bySellChargeCanVisit(curCar, vehicle, seller[i], distance, time, charger))
			//sell[i]δ�����ʣ��������Ե���sell����֮����Ҫȡ���վ�����ܷ���
		{
			double weight = calWeight(curCar, vehicle, seller[i], distance, time);
			if (weight < minWeight)
			{
				minWeight = weight;
				indexMin = i;
			}

		}
	}
	return indexMin;
}
//������bug
bool bySellChargeCanVisit(CarRoute curCar, const vector<Vehicle>& vehicle, const Node& sell,
	const vector<vector<int>>& distance, const vector<vector<int>>& time, const vector<Node>& charger)

{
	int closeChargerIndex = sellMatchCharger[sell.id];

	curCar.curVolume += sell.volume;
	curCar.curWeight += sell.weight;

	int lastIndex = curCar.route.back(); //·�������һ���̻��ڵ�
										 //cout<<curCar.curMileage<<" "<<lastIndex<<" "<<sell.id<<" "<<closeChargerIndex<<endl;
	curCar.curMileage = curCar.curMileage + distance[lastIndex][sell.id]
		+ distance[sell.id][charger[closeChargerIndex].id] - distance[lastIndex][0];
	//cout<<curCar.curMileage<<endl;
	//ʱ�䴰Լ����
	// ����ʱ��������̻�����ٽ���ʱ��֮ǰ
	int arriveTime = curCar.curTime - time[lastIndex][0] + time[lastIndex][sell.id];
	if (arriveTime > sell.last_int_tm)
		return false;

	int waitT = sell.first_int_tm > arriveTime ? sell.first_int_tm - arriveTime : 0;

	curCar.curTime = arriveTime + waitT + 30 + time[sell.id][charger[closeChargerIndex].id] + time[charger[closeChargerIndex].id][0];

	//cout<<curCar.curMileage<<" "<<curCar.curVolume<<" "<<curCar.curWeight<<" "<<curCar.curTime<<endl;

	return isConstraint(curCar, vehicle);

}

int findCharge( Node sell,const vector<vector<int>>& distance,  const vector<Node>& charger)
{
	int chargerIndex = -1;
	double minDis = MAX_DOUBLE;
	for (size_t i = 0; i < charger.size(); ++i)
	{
		if (distance[sell.id][charger[i].id] < minDis)
		{
			minDis = distance[sell.id][charger[i].id];
			chargerIndex = i;
		}
	}
	return chargerIndex;
}

void visitCharge(CarRoute& curCar,const Node& charger,
	const vector<vector<int>>& distance, const vector<vector<int>>& time)
{
	//���ʳ��վ
	++curCar.chargeNum;     //������+1
	int lastIndex = curCar.route.back();

	curCar.totalMileage = curCar.totalMileage - distance[lastIndex][0]
		+ distance[lastIndex][charger.id] + distance[charger.id][0];

	curCar.curMileage = distance[charger.id][0];

	int arriveTime = curCar.curTime - time[lastIndex][0] + time[lastIndex][charger.id];

	curCar.curTime = arriveTime + 30 + time[charger.id][0];
	//����ʱ��+���+����

	curCar.route.push_back(charger.id);
	curCar.backTime = curCar.curTime;
}

vector<AnswerRecord> getAnswer(const vector<CarRoute>& result, const vector<Vehicle>& vehicle)
{
	vector<AnswerRecord> answer;
	for (size_t i = 0; i < result.size(); ++i)
	{
		AnswerRecord record;
		record.trans_code = toTransCode(i);
		record.vehicle_type = to_string(result[i].cartype);
		record.dist_seq = toDistSeq(result[i].route);
		record.distribute_lea_tm = toTime(result[i].leaveTime);
		record.distribute_arr_tm = toTime(result[i].backTime);
		record.distance = result[i].totalMileage;
		//������λС��
		record.trans_cost = result[i].totalMileage * vehicle[result[i].cartype - 1].unit_trans_cost;

		record.charge_cost = result[i].chargeNum * 50;
		//������λС��
		record.wait_cost = result[i].waitTime * 0.4;

		record.fixed_use_cost = vehicle[result[i].cartype - 1].vechile_cost;
		record.total_cost = record.trans_cost + record.charge_cost + record.wait_cost + record.fixed_use_cost;
		record.charge_cnt = result[i].chargeNum;
		answer.push_back(record);
	}
	return answer;
}

string toTransCode(size_t i)
{
	i = i + 1;
	if (i < 10)
		return string("DP000") + to_string(i);
	if (i >= 10 && i <100)
		return string("DP00") + to_string(i);
	if (i >= 100 && i <1000)
		return string("DP0") + to_string(i);
	return
		string("DP") + to_string(i);
}

string toDistSeq(vector<int> vec)
{
	string str;
	for (size_t i = 0; i < vec.size(); ++i)
	{
		str += to_string(vec[i]) + ";";
	}
	str += to_string(0);
	return str;
}

string toTime(int num)
{
	int hh = num / 60 + 8;
	int mm = num % 60;
	string strhh;
	string strmm;
	if (hh < 10)
		strhh = "0" + to_string(hh);
	else
		strhh = to_string(hh);

	if (mm < 10)
		strmm = "0" + to_string(mm);
	else
		strmm = to_string(mm);
	//    string strhh = hh > 9 ? to_string(hh) : string("0")+to_string(hh);
	//    string strmm = mm > 9 ? to_string(mm) : string("0")+to_string(mm);
	return strhh + ":" + strmm;

}

//vector<vector<Node>>  kMeans(const vector<Node>& seller,const int k)
//{
//	vector<vector<Node>> kSellers;
//	if (seller.size() < k)
//		return kSellers;
//
//	vector<Node> oneSeller;
//	for (size_t i = 0; i < k; ++i)    //���k���ռ�
//		kSellers.push_back(oneSeller);
//	vector<Node> coreSeller;	//��¼ÿ�ص�����
//	for (size_t i = 0; i < k; ++i)	//���ĳ�ʼ��
//		coreSeller.push_back(seller[i]); 
//	int cir = 100;
//	int signal = 1;
//	while (--cir && signal) 
//	{
//		for (size_t i = 0; i < k; ++i)
//		{
//			kSellers[i].clear();
//		}
//		signal = 0;
//		for (size_t i = 0; i < seller.size(); ++i)
//		{
//			int coreIndex = sellMatchCharger[seller[i].id];
//			kSellers[coreIndex].push_back(seller[i]);
//		}
//		for (size_t i = 0; i < k; ++i)
//		{
//			Node newCore = calNewCore(kSellers[i]);
//			if (newCore.lat != coreSeller[i].lat || newCore.lng != coreSeller[i].lng)
//				signal = 1;
//			coreSeller[i].lat = newCore.lat;
//			coreSeller[i].lng = newCore.lng;
//		}
//	}
//	return kSellers;
//
//}
int  findeCoreIndex(Node sell, const vector<Node>& coreSeller)
{
	int index = 0;
	double minDis = distanceNode(sell,coreSeller[0]);
	for (size_t i = 0; i < coreSeller.size(); ++i)
	{
		if (distanceNode(sell,coreSeller[i]) < minDis)
		{
			minDis = distanceNode(sell,coreSeller[i]);
			index = i;
		}
	}
	return index;
}

Node calNewCore(const vector<Node>& seller)
{
	double sumLng = 0.0;
	double sumLat = 0.0;
	for (size_t i = 0; i < seller.size(); ++i)
	{
		sumLng += seller[i].lng;
		sumLat += seller[i].lat;
	}
	Node node;
	node.lng = sumLng / seller.size();
	node.lat = sumLat / seller.size();
	return node;
}

double distanceNode(Node node1, Node node2)
{
	return (node1.lng - node2.lng)*(node1.lng - node2.lng) + (node1.lat - node2.lat)*(node1.lat - node2.lat);
}

void changeVehicleType(vector<CarRoute>& result, const vector<vector<int>>& distance, const vector<Vehicle>& vehicle)
{
	for (size_t i = 0; i < result.size(); ++i)
	{
		if (canChangeCarType(result[i], distance,vehicle))
		{
			result[i].cartype = 1;
		}
	}
}
bool canChangeCarType(const CarRoute& cRoute, const vector<vector<int>>& distance, const vector<Vehicle>& vehicle)
{
	if (cRoute.curVolume > vehicle[0].volume)
		return false;
	if (cRoute.curWeight > vehicle[0].weight)
		return false;
	double dis = 0;
	for (size_t i = 1; i < cRoute.route.size(); ++i)
	{
		if (cRoute.route[i] < 1001) //1001�ǳ��վ����С���
			dis += distance[cRoute.route[i - 1]][cRoute.route[i]];
		else
		{
			dis += distance[cRoute.route[i - 1]][cRoute.route[i]];
			if (dis > vehicle[0].driving_range)  //��û�����վ �� �ƶ�����Լ��
				return false;
			dis = 0;
		}
	}
	dis += distance[cRoute.route.back()][0];
	if (dis > vehicle[0].driving_range)
		return false;
	return true;
}


//���Ļ������ͺ�
void changeVehicleType2(vector<CarRoute>& result, const vector<vector<int>>& distance, 
	const vector<vector<int>>& time ,const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger)
{
	for (size_t i = 0; i < result.size(); ++i)
	{
		//���Ըı�͸ı䣬�����Ըı�ͷ���false
		isChangeVehicleType(result[i], distance, time, vehicle,nodeList,charger);

	}
}
//�������ͺ��Ƿ���Ը��ģ��������ֱ�Ӹ��ģ���������ԣ�����false
bool isChangeVehicleType(CarRoute& car, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger)
{

	CarRoute tempCar;
	if (car.cartype == 2)
		tempCar.cartype = 1;
	else
		tempCar.cartype = 2;

	tempCar.route.push_back(0);

	for (size_t i = 1; i < car.route.size(); ++i)
	{
		Node sell = nodeList[car.route[i]];
		int lastIndex = tempCar.route.back();
		if (car.route[i] < 1001)
		{

			if (canVisit(tempCar, vehicle, sell, distance, time))
			{
				visitSeller(tempCar, sell, distance, time);
			}
			else if (bySellChargeCanVisit(tempCar, vehicle, sell, distance, time, charger))  //���Կ��ǸĽ�
			{
				visitSeller(tempCar, sell, distance, time);
				int chargeIndex = sellMatchCharger[sell.id];
				visitCharge(tempCar, charger[chargeIndex], distance, time);
			}
			else
				return false;
		}
	}

	//����car���ͺ�
	if(calOneCost(car, vehicle) > calOneCost(tempCar,vehicle))
		car = tempCar;
	return true;
}
bool lessVehicleType(CarRoute car1, CarRoute car2)
{
	return car1.cartype < car2.cartype;
}


