#include "stdafx.h"
#include "logistics.h"
#include "population.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

#include <string>
using namespace std;


int changeTime(const string& str) //把时间换成int
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
//给定节点 获得汽车分配路线
vector<CarRoute> execute(vector<Node>& seller, const vector<Node>& charger, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle)
{
	//int doNum = CURCAL_TIME;                //模拟退火算法，循环次数
	//vector<CarRoute> minResult;
	//double minResultCost = MAX_DOUBLE;
	//srand(SEED);
	//while (--doNum)
	//{
		vector<CarRoute> result;
		while (!isAllserver(seller))
		{
			CarRoute curCar;
			curCar.route.push_back(0);              //配送中心作为货车的起点
			while (isConstraint(curCar, vehicle))             //选择节点
			{
				int indexMin;
				indexMin = nextVisitIndex(curCar, seller, distance, time, vehicle); //不通过充电站，直接可以访问的最佳商户
				//if (rand() % 100 < probability)
				//	indexMin = nextVisitIndex(curCar, seller, distance, time, vehicle); //不通过充电站，直接可以访问的最佳商户
				//else
				//	indexMin = nextRandomVisitIndex(curCar, seller, distance, time, vehicle);

				if (indexMin != -1) //找到了下一个访问的商户
				{
					visitSeller(curCar, seller[indexMin], distance, time);
				}
				else
				{
					if (!isGoback(curCar, vehicle))
					{
						int sellIndex = nextSellCharger(curCar, seller, charger, distance, time, vehicle); //先到达商户再去充电



						if (sellIndex != -1) //找到可以 先到达商户再去充电的最佳商户
						{
							visitSeller(curCar, seller[sellIndex], distance, time); //到达商户

							int closeChargerIndex = sellMatchCharger[seller[sellIndex].id]; //最近充电站坐标
							visitCharge(curCar, charger[closeChargerIndex], distance, time);
						}
						else
						{
							//cout << "此轮结束：";
							//for (size_t i = 0; i < curCar.route.size(); ++i)
							//	cout << curCar.route[i] << " ";
							//cout << endl;
							break;
						}

					}
					else
					{
						//cout << "货车接近满载，此轮结束:";
						//for (size_t i = 0; i < curCar.route.size(); ++i)
						//	cout << curCar.route[i] << " ";
						//cout << endl;
						break;
					}
				}
			}
			result.push_back(curCar);
		}
		//changeVehicleType(result, distance, vehicle);		//转变货车的型号
		return result;
	//	double curResultCost = calAllCost(result, vehicle);
	//	std::cout << "curResultCost:" << curResultCost << endl;
	//	if (curResultCost < minResultCost)
	//	{
	//		minResultCost = curResultCost;
	//		minResult = result;
	//	}
	//	//更新seller的状态 isServer
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
bool isConstraint(const CarRoute& curCar,const vector<Vehicle>& vehicle) //约束条件
{
	if (curCar.curVolume > vehicle[curCar.cartype - 1].volume) //货车容量约束
	{
		return false;
	}
	if (curCar.curWeight > vehicle[curCar.cartype - 1].weight)  //货车载重约束
	{
		return false;
	}
	if (curCar.curMileage > vehicle[curCar.cartype - 1].driving_range)//里程约束 。注意：先不考虑中途充电的情况
	{
		return false;
	}
	if (curCar.curTime > 960)  //返程时间约束
	{
		return false;
	}


	return true;
}

//货车curCar 不经过充电是否可以直接访问sell
bool canVisit(CarRoute curCar, const vector<Vehicle>& vehicle, const Node& sell
	, const vector<vector<int>>& distance, const vector<vector<int>>& time)
{
	curCar.curVolume += sell.volume;
	curCar.curWeight += sell.weight;

	int lastIndex = curCar.route.back(); //路径的最后一个商户节点
	curCar.curMileage = curCar.curMileage + distance[lastIndex][sell.id]
		+ distance[sell.id][0] - distance[lastIndex][0];
	//时间窗约束，
	// 到达时间必须在商户的最迟接收时间之前
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
	//时间窗约束
	if (arriveTime > sell.last_int_tm)
		return MAX_DOUBLE;

	double waitTime = sell.first_int_tm > arriveTime ? sell.first_int_tm - arriveTime : 0;
	double canDelayTime = min(static_cast<double>(sell.last_int_tm - sell.first_int_tm)
		, sell.last_int_tm - arriveTime);

	//return A * time[lastIndex][sell.id]+ B * waitTime + C * canDelayTime ; //以时间为权重

	return A * (dis * vehicle[curCar.cartype - 1].unit_trans_cost)  //以费用
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
	//时间窗约束
	if (arriveTime > sell.last_int_tm)
		return MAX_DOUBLE;

	double waitTime = sell.first_int_tm > arriveTime ? sell.first_int_tm - arriveTime : 0;
	double canDelayTime = min(static_cast<double>(sell.last_int_tm - sell.first_int_tm)
		, sell.last_int_tm - arriveTime);

	return 1 * time[lastIndex][sell.id] + 0.2 * waitTime + 0.2 * canDelayTime; //以时间为权重
}

void visitSeller(CarRoute& curCar, Node& sell,
	const vector<vector<int>>& distance, const vector<vector<int>>& time)
{
	//访问sell商户
	sell.isServer = true;
	curCar.curVolume += sell.volume;
	curCar.curWeight += sell.weight;

	int lastIndex = curCar.route.back();
	curCar.curMileage = curCar.curMileage + distance[lastIndex][sell.id]
		+ distance[sell.id][0] - distance[lastIndex][0];

	int arriveTime = curCar.curTime - time[lastIndex][0] + time[lastIndex][sell.id];
	int waitT = sell.first_int_tm > arriveTime ?
		sell.first_int_tm - arriveTime : 0;
	//更新出发时间,出发时间为 货车在第一次到达商户的等待时间
	if (curCar.leaveTime == -1)
	{
		curCar.leaveTime = waitT;
	}
	else {
		curCar.waitTime += waitT;
	}

	curCar.curTime = arriveTime + waitT + 30 + time[sell.id][0];
	//到达时间+等待+服务+返回

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
	int indexMin = -1; //保存下一个访问的商户
	for (size_t i = 0; i < seller.size(); ++i)   //遍历商户，寻找下一个节点
	{
		if (!seller[i].isServer&&canVisit(curCar, vehicle, seller[i], distance, time)) //sell[i]未被访问，且可以访问
		{                                                                         //  ，且可以访问
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
	int indexMin = -1; //保存下一个访问的商户
	for (size_t i = 0; i < seller.size(); ++i)   //遍历商户，寻找下一个节点
	{
		if (!seller[i].isServer&&canVisit(curCar, vehicle, seller[i], distance, time)) //sell[i]未被访问，且可以访问
		{                                                                         //  ，且可以访问
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
	if (curCar.curVolume / vehicle[curCar.cartype - 1].volume > GOBACK) //当前容量已经使用80%直接返回
	{
		//cout << "容量即满" << curCar.curVolume << " " << vehicle[curCar.cartype - 1].volume << endl;
		return true;
	}
	if (curCar.curWeight / vehicle[curCar.cartype - 1].weight > GOBACK)
	{
		//cout << "载重即满" << curCar.curWeight << " " << vehicle[curCar.cartype - 1].weight << endl;
		return true;
	}
	return false;
}
//先去店户 再取充电站
int nextSellCharger(const CarRoute& curCar, const vector<Node>& seller, const vector<Node>& charger,
	const vector<vector<int>>& distance, const vector<vector<int>>& time, const vector<Vehicle>& vehicle)
{
	double minWeight = MAX_DOUBLE;
	int indexMin = -1; //保存下一个访问的商户
	for (size_t i = 0; i < seller.size(); ++i)   //遍历商户，寻找下一个节点
	{
		if (!seller[i].isServer&&bySellChargeCanVisit(curCar, vehicle, seller[i], distance, time, charger))
			//sell[i]未被访问，汽车可以到达sell，但之后需要取充电站充电才能返回
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
//可能有bug
bool bySellChargeCanVisit(CarRoute curCar, const vector<Vehicle>& vehicle, const Node& sell,
	const vector<vector<int>>& distance, const vector<vector<int>>& time, const vector<Node>& charger)

{
	int closeChargerIndex = sellMatchCharger[sell.id];

	curCar.curVolume += sell.volume;
	curCar.curWeight += sell.weight;

	int lastIndex = curCar.route.back(); //路径的最后一个商户节点
										 //cout<<curCar.curMileage<<" "<<lastIndex<<" "<<sell.id<<" "<<closeChargerIndex<<endl;
	curCar.curMileage = curCar.curMileage + distance[lastIndex][sell.id]
		+ distance[sell.id][charger[closeChargerIndex].id] - distance[lastIndex][0];
	//cout<<curCar.curMileage<<endl;
	//时间窗约束，
	// 到达时间必须在商户的最迟接收时间之前
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
	//访问充电站
	++curCar.chargeNum;     //充电次数+1
	int lastIndex = curCar.route.back();

	curCar.totalMileage = curCar.totalMileage - distance[lastIndex][0]
		+ distance[lastIndex][charger.id] + distance[charger.id][0];

	curCar.curMileage = distance[charger.id][0];

	int arriveTime = curCar.curTime - time[lastIndex][0] + time[lastIndex][charger.id];

	curCar.curTime = arriveTime + 30 + time[charger.id][0];
	//到达时间+充电+返回

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
		//保留两位小数
		record.trans_cost = result[i].totalMileage * vehicle[result[i].cartype - 1].unit_trans_cost;

		record.charge_cost = result[i].chargeNum * 50;
		//保留两位小数
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
//	for (size_t i = 0; i < k; ++i)    //获得k个空集
//		kSellers.push_back(oneSeller);
//	vector<Node> coreSeller;	//记录每簇的中心
//	for (size_t i = 0; i < k; ++i)	//簇心初始化
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
		if (cRoute.route[i] < 1001) //1001是充电站的最小编号
			dis += distance[cRoute.route[i - 1]][cRoute.route[i]];
		else
		{
			dis += distance[cRoute.route[i - 1]][cRoute.route[i]];
			if (dis > vehicle[0].driving_range)  //还没到充电站 ， 移动距离约束
				return false;
			dis = 0;
		}
	}
	dis += distance[cRoute.route.back()][0];
	if (dis > vehicle[0].driving_range)
		return false;
	return true;
}


//更改货车的型号
void changeVehicleType2(vector<CarRoute>& result, const vector<vector<int>>& distance, 
	const vector<vector<int>>& time ,const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger)
{
	for (size_t i = 0; i < result.size(); ++i)
	{
		//可以改变就改变，不可以改变就返回false
		isChangeVehicleType(result[i], distance, time, vehicle,nodeList,charger);

	}
}
//货车的型号是否可以更改，如果可以直接更改，如果不可以，返回false
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
			else if (bySellChargeCanVisit(tempCar, vehicle, sell, distance, time, charger))  //可以考虑改进
			{
				visitSeller(tempCar, sell, distance, time);
				int chargeIndex = sellMatchCharger[sell.id];
				visitCharge(tempCar, charger[chargeIndex], distance, time);
			}
			else
				return false;
		}
	}

	//更改car的型号
	if(calOneCost(car, vehicle) > calOneCost(tempCar,vehicle))
		car = tempCar;
	return true;
}
bool lessVehicleType(CarRoute car1, CarRoute car2)
{
	return car1.cartype < car2.cartype;
}


