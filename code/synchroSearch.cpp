#include "stdafx.h"
#include "logistics.h"
#include "population.h"
#include "synchroSearch.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>

#include <string>
using namespace std;


//初始化vector<CarRoute> result
vector<CarRoute> initResult()
{
	vector<CarRoute> result;
	CarRoute car;
	car.route.push_back(0);
	for (size_t i = 0; i < INIT_CAR_NUM; ++i)
	{
		car.cartype = 1;
		result.push_back(car);
	}
	return result;
}

//同步搜索
vector<CarRoute> synchroSearch(vector<Node>& seller, const vector<Node>& charger, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle)
{
	vector<CarRoute> result = initResult();
	//按照最晚到达时间最商铺排序
	sort(seller.begin(), seller.end(), timeCompare);
	for (auto& sell : seller)
	{
		int carIndex = whichCarVisitThisSell(result, sell,charger, distance, time, vehicle);
		if (carIndex != -1)
		{
			if (canVisit(result[carIndex], vehicle, sell, distance, time))
			{
				visitSeller(result[carIndex], sell, distance, time);
			}
			else if (bySellChargeCanVisit(result[carIndex], vehicle, sell, distance, time, charger))
			{
				visitSeller(result[carIndex], sell, distance, time); //到达商户

				int closeChargerIndex = findCharge(sell, distance, charger); //最近充电站坐标
				visitCharge(result[carIndex], charger[closeChargerIndex], distance, time);
			}
		}
		else
		{
			CarRoute tem;
			tem.route.push_back(0);
			result.push_back(tem);
			visitSeller(result.back(), sell, distance, time);
		}
	}
	deleteEmptyCar(result);

	return result;
}

//seller排序的比较函数
bool timeCompare(const Node& node1, const Node& node2)
{
	return node1.last_int_tm < node2.last_int_tm;
}

//哪辆车去服务这家商铺
int whichCarVisitThisSell(vector<CarRoute>& result,const Node& sell, const vector<Node>& charger, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle)
{
	int carIndex = -1;
	double minvisitCost = MAX_DOUBLE;

	for (size_t i = 0; i < result.size(); ++i)
	{
		CarRoute curCar = result[i];
		//货车curCar是否可以不经过充电 直接服务商铺
		if (canVisit(curCar, vehicle, sell, distance, time))
		{
			double curCost = 0;
			//路径花费
			curCost +=  distance[curCar.route.back()][sell.id] * vehicle[curCar.cartype-1].unit_trans_cost;
			////等待花费,首次出发没有等待花费
			//if (curCar.route.back()!=0)
			//{
			//	int arriveTime = curCar.curTime - time[curCar.route.back()][0] + time[curCar.route.back()][sell.id];
			//	int waitT = sell.first_int_tm > arriveTime ? sell.first_int_tm - arriveTime : 0;
			//	curCost += 0.4 * waitT;

			//}

			if (curCost < minvisitCost)
			{
				carIndex = i;
				minvisitCost = curCost;
			}
		}
		//货车curCar经过充电站充电才能服务商铺
		else if(bySellChargeCanVisit(curCar, vehicle, sell, distance, time,charger))
		{
			double curCost = 0;

			//路径花费
			curCost += distance[curCar.route.back()][sell.id] * vehicle[curCar.cartype - 1].unit_trans_cost;
			//int closeChargerIndex = findCharge(curCar, sell, distance, time, charger);
			//curCost += (distance[curCar.route.back()][sell.id] + distance[sell.id][charger[closeChargerIndex].id])
			//	* vehicle[curCar.cartype - 1].unit_trans_cost;

			////充电花费
			//curCost += 50;

			////等待花费
			//if (curCar.route.back() != 0)
			//{
			//	int arriveTime = curCar.curTime - time[curCar.route.back()][0] + time[curCar.route.back()][sell.id];
			//	int waitT = sell.first_int_tm > arriveTime ? sell.first_int_tm - arriveTime : 0;
			//	curCost += 0.4 * waitT;

			//}

			if (curCost < minvisitCost)
			{
				carIndex = i;
				minvisitCost = curCost;
			}		
		}
	}
	return carIndex;
}

//删除空的货车
void deleteEmptyCar(vector<CarRoute>& result)
{
	vector<CarRoute> ret;
	for (auto car : result)
	{
		if (car.route.size() > 1)
			ret.push_back(car);
	}
	result = ret;
}