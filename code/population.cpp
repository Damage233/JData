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

//计算sellMatchCharger;下标对应sell的id，内容是 charger的下标
void calSellChargerMatch(vector<int> &vec, const vector<vector<int>>& distance,
	const vector<Node>& charger, const vector<Node>& seller)
{
	vec.push_back(0);
	for (auto sell : seller)
	{
		int chargerIndex = findCharge(sell, distance, charger);
		vec.push_back(chargerIndex);
	}
}

//单个个体，交叉，变异，自我交换的遗传算法
void inheritance(vector<CarRoute>& result, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger)
{
	//单个个体，交叉，变异，自我交换的遗传算法

	int c = CIRCAL_TIME;
	while (--c)
	{
		DWORD start_time = GetTickCount();
		for (size_t i = 0; i < result.size(); ++i)
		{
			for (size_t j = 0; j < result.size(); ++j)
				if (i != j)
					exchangeCarRoute2(result[i], result[j], distance, time, vehicle, nodeList,charger); //交叉操作

		}
		for (size_t i = 0; i < result.size(); ++i)
		{
			for (size_t j = 0; j < result.size(); ++j)
				if (i != j)
					addOrDeleteCarRoute2(result[i], result[j], distance, time, vehicle, nodeList, charger); //变异操作

		}
		for (size_t i = 0; i < result.size(); ++i)
		{
			selfExchangeCarRoute(result[i], distance, time, vehicle, nodeList, charger); //自我交换
		}
		for (size_t i = 0; i < result.size(); ++i)
		{
			selfAddOrDeleteCarRoute(result[i], distance, time, vehicle, nodeList, charger); //自我调整
		}
		std::cout << "Iterator" << CIRCAL_TIME - c << ":" << calAllCost(result, vehicle) << endl;
		DWORD end_time = GetTickCount();
		cout <<"本次迭代用时："<< (end_time - start_time) / 1000 << "s" << endl;

		//每迭代 2 次 ， 写入文件
		if ((CIRCAL_TIME - c) % 5 == 0)
		{
			//vector<CarRoute> tem = result;

			//changeVehicleType(result, distance, vehicle);		//转变货车的型号
			changeVehicleType2(result, distance, time, vehicle, nodeList, charger);
			deleteEmptyCar(result);

			sort(result.begin(), result.end(), lessVehicleType);

			vector<AnswerRecord> answer;
			answer = getAnswer(result, vehicle);

			whrit_answer(answer);
		}

	}
	
}
	
////以种群优化迭代，遗传算法
//void inheritance2(vector<CarRoute>& result, const vector<vector<int>>& distance,
//	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList)
//{
//	vector<vector<CarRoute>> population = initPopulation(result); //种群初始化
//	int cir = ITERATION_NUM;
//	while (--cir)
//	{
//		//所有操作（交叉、变异）放在一起，在验证有效性
//		for(auto& pop : population)
//		for (size_t i = 0; i < pop.size(); ++i)
//		{
//			for (size_t j = 0; j < pop.size(); ++j)
//				if (i != j)
//					allOperationCarRoute(pop[i], pop[j], distance, time, vehicle, nodeList); 
//
//		}
//		//排序
//		sort(population.begin(), population.end(), compare);
//
//		////更新群落，保留较好的一半，用好的替代不好的
//		updatePopulation(population);
//
//		cout << "inheritance" << ITERATION_NUM - cir << ":" << calAllCost(population[0], vehicle) << endl;
//
//		//每迭代10次 ， 写入文件
//		if ((ITERATION_NUM - cir) % 10 == 0)
//		{
//			vector<CarRoute> tem = population[0];
//
//			changeVehicleType(tem, distance, vehicle);		//转变货车的型号
//			sort(tem.begin(), tem.end(), lessVehicleType);
//
//			vector<AnswerRecord> answer;
//			answer = getAnswer(tem, vehicle);
//
//			whrit_answer(answer);
//		}
//
//	}
//	// population[0] 比一定是最小的，简单起见，compare只比较总距离
//	result = population[0];
//}


//单个方案的费用
double calAllCost(const vector<CarRoute>& result, const vector<Vehicle>& vehicle)
{
	////保留两位小数
	//record.trans_cost = result[i].totalMileage * vehicle[result[i].cartype - 1].unit_trans_cost;

	//record.charge_cost = result[i].chargeNum * 50;
	////保留两位小数
	//record.wait_cost = result[i].waitTime * 0.4;

	//record.fixed_use_cost = vehicle[result[i].cartype - 1].vechile_cost;
	double all_cost = 0;
	for (size_t i = 0; i < result.size(); ++i)
	{
		all_cost += result[i].totalMileage * vehicle[result[i].cartype - 1].unit_trans_cost;
		all_cost += result[i].chargeNum * 50;
		all_cost += result[i].waitTime * 0.4;
		all_cost += vehicle[result[i].cartype - 1].vechile_cost;
	}
	return all_cost;
}

//单车费用
double calOneCost(const CarRoute& car, const vector<Vehicle>& vehicle)
{
	////保留两位小数
	//record.trans_cost = result[i].totalMileage * vehicle[result[i].cartype - 1].unit_trans_cost;

	//record.charge_cost = result[i].chargeNum * 50;
	////保留两位小数
	//record.wait_cost = result[i].waitTime * 0.4;

	//record.fixed_use_cost = vehicle[result[i].cartype - 1].vechile_cost;
	if (car.route.size() == 1)
		return 0;

	double all_cost = 0;

	all_cost += car.totalMileage * vehicle[car.cartype - 1].unit_trans_cost;
	all_cost += car.chargeNum * 50;
	all_cost += car.waitTime * 0.4;
	all_cost += vehicle[car.cartype - 1].vechile_cost;

	return all_cost;
}

//择优交换，否则不变，遍历单交叉
void exchangeCarRoute(CarRoute& car1, CarRoute& car2, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger)
{
	CarRoute temCar1(car1);
	CarRoute temCar2(car2);
	double minCost = calOneCost(car1, vehicle) + calOneCost(car2, vehicle);
	for (size_t i = 1; i < temCar1.route.size(); ++i)
	{
		for (size_t j = 1; j < temCar2.route.size(); ++j)
		{
			swap(temCar1.route[i], temCar2.route[j]);  //单交叉
			 //isLegal 返回true时，会修改temCar的状态
			if (isLegal(temCar1, distance, time, vehicle, nodeList,charger) 
				&& isLegal(temCar2, distance, time, vehicle, nodeList,charger))
			{
				double cost1 = calOneCost(temCar1, vehicle);
				double cost2 = calOneCost(temCar2, vehicle);
				if (cost1 + cost2 < minCost) //效果好
				{
					car1 = temCar1;
					car2 = temCar2;
					minCost = cost1 + cost2;
					return;
				}
				else  //用最好的结果，还原temCar的状态
				{
					temCar1 = car1;
					temCar2 = car2;
				}

			}
			else
			{
				temCar1 = car1;
				temCar2 = car2;
			}
		}
	}
}


//择优交叉，否则不变，随机单交叉
void exchangeCarRoute2(CarRoute& car1, CarRoute& car2, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger)
{
	CarRoute temCar1(car1);
	CarRoute temCar2(car2);
	double minCost = calOneCost(car1, vehicle) + calOneCost(car2, vehicle);
	int cir = REPEAT_NUM;
	while (--cir)
	{
		int num = rand() % CHANGE_NUM + 1;
		for (size_t i = 0; i < num; ++i)
		{
			//路径为空的情况
			if (temCar1.route.size() == 1 || temCar2.route.size() == 1)
				continue;
			int d1 = rand() % (temCar1.route.size() - 1) + 1;
			int d2 = rand() % (temCar2.route.size() - 1) + 1;
			swap(temCar1.route[d1], temCar2.route[d2]);
		}
		if (isLegal(temCar1, distance, time, vehicle, nodeList,charger) && isLegal(temCar2, distance, time, vehicle, nodeList,charger))
		{
			double cost1 = calOneCost(temCar1, vehicle);
			double cost2 = calOneCost(temCar2, vehicle);
			if (cost1 + cost2 < minCost ||
				(rand() % 100 > JUMP_P && cost1 + cost2 - minCost < ACCEPT * minCost)) //效果好
			{
				car1 = temCar1;
				car2 = temCar2;
				minCost = cost1 + cost2;
				return;
			}
			else  //用最好的结果，还原temCar的状态
			{
				temCar1 = car1;
				temCar2 = car2;
			}

		}
		else
		{
			temCar1 = car1;
			temCar2 = car2;
		}
	}

}


//合法就修改 ，不合法就不变
bool isLegal(CarRoute& car, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger)
{
	CarRoute tempCar;
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

	//改变货车型号
	isChangeVehicleType(tempCar, distance, time, vehicle, nodeList, charger);

	car = tempCar;
	return true;

}

//遍历增加或删减，否则不变
void addOrDeleteCarRoute(CarRoute& car1, CarRoute& car2, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger)
{
	CarRoute temCar1(car1);
	CarRoute temCar2(car2);
	double minCost = calOneCost(car1, vehicle) + calOneCost(car2, vehicle);
	for (size_t i = 1; i < temCar1.route.size(); ++i)
	{
		for (size_t j = 1; j < temCar2.route.size(); ++j)
		{
			int delValue = temCar1.route[i];
			temCar1.route.erase(temCar1.route.begin() + i);
			temCar2.route.insert(temCar2.route.begin() + j,delValue);

													   //isLegal 返回true时，会修改temCar的状态
			if (isLegal(temCar1, distance, time, vehicle, nodeList, charger)
				&& isLegal(temCar2, distance, time, vehicle, nodeList, charger))
			{
				double cost1 = calOneCost(temCar1, vehicle);
				double cost2 = calOneCost(temCar2, vehicle);
				if (cost1 + cost2 < minCost) //效果好
				{
					car1 = temCar1;
					car2 = temCar2;
					minCost = cost1 + cost2;
					return;
				}
				else  //用最好的结果，还原temCar的状态
				{
					temCar1 = car1;
					temCar2 = car2;
				}

			}
			else
			{
				temCar1 = car1;
				temCar2 = car2;
			}
		}
	}
}

//随机增加或删减，否则不变
void addOrDeleteCarRoute2(CarRoute& car1, CarRoute& car2, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger)
{
	CarRoute temCar1(car1);
	CarRoute temCar2(car2);
	double minCost = calOneCost(car1, vehicle) + calOneCost(car2, vehicle);
	typedef vector<int>::iterator Iter;
	int cir = REPEAT_NUM;
	while (--cir)
	{
		int num = rand() % CHANGE_NUM + 1;
		for (size_t i = 0; i < num; ++i)
		{
			//防止出现路径为空的情况
			if (temCar1.route.size() == 2 && temCar2.route.size() == 2)
			{
				continue;
			}
			if (temCar1.route.size() == 2)
				swap(temCar1, temCar2);
			if (temCar1.route.size() == 1 || temCar2.route.size() == 1)
				continue;
			int d1 = rand() % (temCar1.route.size() - 1) + 1;
			int d2 = rand() % (temCar2.route.size() - 1) + 1;
			Iter iter1 = temCar1.route.begin() + d1;
			Iter iter2 = temCar2.route.begin() + d2;
			int deleteVal = *iter1;
			temCar1.route.erase(iter1);
			temCar2.route.insert(iter2, deleteVal);
		}

		if (isLegal(temCar1, distance, time, vehicle, nodeList, charger) 
			&& isLegal(temCar2, distance, time, vehicle, nodeList, charger))
		{
			double cost1 = calOneCost(temCar1, vehicle);
			double cost2 = calOneCost(temCar2, vehicle);
			if (cost1 + cost2 < minCost ||
				(rand() % 100 > JUMP_P && cost1 + cost2 - minCost < ACCEPT * minCost)) //效果好
			{
				car1 = temCar1;
				car2 = temCar2;
				minCost = cost1 + cost2;
				return;
			}
			else  //用最好的结果，还原temCar的状态
			{
				temCar1 = car1;
				temCar2 = car2;
			}

		}
		else
		{
			temCar1 = car1;
			temCar2 = car2;
		}
	}
}




//群落初始化
vector<vector<CarRoute>> initPopulation(const vector<CarRoute>& result)
{
	vector<vector<CarRoute>> results;
	for (int i = 0; i < POPULATION_NUM; ++i)
	{
		results.push_back(result);
	}
	return results;
}


//个体之间的比较函数
bool compare(const vector<CarRoute>& a, const vector<CarRoute>& b)
{
	int disA = 0;
	for (auto i : a)
	{
		disA += i.totalMileage;
	}

	int disB = 0;
	for (auto i : b)
	{
		disB += i.totalMileage;
	}
	return disA < disB;
}


////更新群落，保留较好的一半，用好的替代不好的
void updatePopulation(vector<vector<CarRoute>>& population)
{
	typedef vector<vector<CarRoute>>::iterator Iter;
	Iter iter1 = population.begin();
	Iter iter2 = population.begin() + population.size() / 2;
	for (Iter i = iter2; i != population.end(); ++i, ++iter1)
		*i = *iter1;
}


//自我交换
void selfExchangeCarRoute(CarRoute& car1, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger)
{
	CarRoute temCar1(car1);
	double minCost = calOneCost(car1, vehicle);
	int cir = REPEAT_NUM;
	while (--cir)
	{
		int num = rand() % CHANGE_NUM + 1;

		for (size_t i = 0; i < num; ++i)
		{
			//路径为空的情况
			if (temCar1.route.size() == 1 || temCar1.route.size() == 2)
				continue;
			int d1 = rand() % (temCar1.route.size() - 1) + 1;
			int d2 = rand() % (temCar1.route.size() - 1) + 1;
			if (d1 == d2)
				continue;
			swap(temCar1.route[d1], temCar1.route[d2]);
		}
		if (isLegal(temCar1, distance, time, vehicle, nodeList, charger) )
		{
			double cost1 = calOneCost(temCar1, vehicle);
			if (cost1 < minCost) //效果好
			{
				car1 = temCar1;
				minCost = cost1;
				return;
			}
			else  //用最好的结果，还原temCar的状态
			{
				temCar1 = car1;
			}

		}
		else
		{
			temCar1 = car1;
		}
	}

}

//自我删除增加，也是调整位置
void selfAddOrDeleteCarRoute(CarRoute& car1, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger)
{
	CarRoute temCar1(car1);
	double minCost = calOneCost(car1, vehicle);
	typedef vector<int>::iterator Iter;
	int cir = REPEAT_NUM;
	while (--cir)
	{
		int num = rand() % CHANGE_NUM + 1;
		for (size_t i = 0; i < num; ++i)
		{
			//防止出现路径为空的情况
			if (temCar1.route.size() == 2 || temCar1.route.size() == 1)
			{
				continue;
			}
			int d1 = rand() % (temCar1.route.size() - 1) + 1;
			Iter iter1 = temCar1.route.begin() + d1;
			int deleteVal = *iter1;
			temCar1.route.erase(iter1);

			int d2 = rand() % (temCar1.route.size() - 1) + 1;
			Iter iter2 = temCar1.route.begin() + d2;
			temCar1.route.insert(iter2, deleteVal);
		}

		if (isLegal(temCar1, distance, time, vehicle, nodeList, charger) )
		{
			double cost1 = calOneCost(temCar1, vehicle);
			if (cost1 < minCost) //效果好
			{
				car1 = temCar1;
				minCost = cost1;
				return;
			}
			else  //用最好的结果，还原temCar的状态
			{
				temCar1 = car1;
			}

		}
		else
		{
			temCar1 = car1;
		}
	}
}

//所有操作放到一块，在进行验证
void allOperationCarRoute(CarRoute& car1, CarRoute& car2, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger)
{
	CarRoute temCar1(car1);
	CarRoute temCar2(car2);
	double minCost = calOneCost(car1, vehicle) + calOneCost(car2, vehicle);
	int cir = REPEAT_NUM;
	while (--cir)
	{
		int num = rand() % CHANGE_NUM + 1;
		//交叉
		for (size_t i = 0; i < num; ++i)
		{
			//路径为空的情况
			if (temCar1.route.size() == 1 || temCar2.route.size() == 1)
				break;
			int d1 = rand() % (temCar1.route.size() - 1) + 1;
			int d2 = rand() % (temCar2.route.size() - 1) + 1;
			swap(temCar1.route[d1], temCar2.route[d2]);
		}
		//变异
		typedef vector<int>::iterator Iter;
		for (size_t i = 0; i < num; ++i)
		{
			//防止出现路径为空的情况
			if (temCar1.route.size() == 2 && temCar2.route.size() == 2)
			{
				break;
			}
			if (temCar1.route.size() == 2)
				swap(temCar1, temCar2);
			if (temCar1.route.size() == 1 || temCar2.route.size() == 1)
				break;
			int d1 = rand() % (temCar1.route.size() - 1) + 1;
			int d2 = rand() % (temCar2.route.size() - 1) + 1;
			Iter iter1 = temCar1.route.begin() + d1;
			Iter iter2 = temCar2.route.begin() + d2;
			int deleteVal = *iter1;
			temCar1.route.erase(iter1);
			temCar2.route.insert(iter2, deleteVal);
		}
		//自我交叉
		for (size_t i = 0; i < num; ++i)
		{
			//路径为空的情况
			if (temCar1.route.size() <= 2)
				break;
			int d1 = rand() % (temCar1.route.size() - 1) + 1;
			int d2 = rand() % (temCar1.route.size() - 1) + 1;
			if (d1 == d2)
				continue;
			swap(temCar1.route[d1], temCar1.route[d2]);
		}
		for (size_t i = 0; i < num; ++i)
		{
			//路径为空的情况
			if (temCar2.route.size() <= 2)
				break;
			int d1 = rand() % (temCar2.route.size() - 1) + 1;
			int d2 = rand() % (temCar2.route.size() - 1) + 1;
			if (d1 == d2)
				continue;
			swap(temCar2.route[d1], temCar2.route[d2]);
		}
		//自我变异
		for (size_t i = 0; i < num; ++i)
		{
			//防止出现路径为空的情况
			if (temCar1.route.size() <=2 )
			{
				break;
			}
			int d1 = rand() % (temCar1.route.size() - 1) + 1;
			Iter iter1 = temCar1.route.begin() + d1;
			int deleteVal = *iter1;
			temCar1.route.erase(iter1);

			int d2 = rand() % (temCar1.route.size() - 1) + 1;
			Iter iter2 = temCar1.route.begin() + d2;
			temCar1.route.insert(iter2, deleteVal);
		}
		for (size_t i = 0; i < num; ++i)
		{
			//防止出现路径为空的情况
			if (temCar2.route.size() <= 2)
			{
				break;
			}
			int d1 = rand() % (temCar2.route.size() - 1) + 1;
			Iter iter1 = temCar2.route.begin() + d1;
			int deleteVal = *iter1;
			temCar2.route.erase(iter1);

			int d2 = rand() % (temCar2.route.size() - 1) + 1;
			Iter iter2 = temCar2.route.begin() + d2;
			temCar2.route.insert(iter2, deleteVal);
		}

		//验证序列有效性
		if (isLegal(temCar1, distance, time, vehicle, nodeList, charger) && isLegal(temCar2, distance, time, vehicle, nodeList, charger))
		{
			double cost1 = calOneCost(temCar1, vehicle);
			double cost2 = calOneCost(temCar2, vehicle);
			if (cost1 + cost2 <= minCost) //效果好
			{
				car1 = temCar1;
				car2 = temCar2;
				minCost = cost1 + cost2;
				break;
			}
			else  //用最好的结果，还原temCar的状态
			{
				temCar1 = car1;
				temCar2 = car2;
			}

		}
		else
		{
			temCar1 = car1;
			temCar2 = car2;
		}
	}

}


//可以直接访问，则访问改变car的状态；否则，返回fasle，不改变car状态
bool legalVisitSeller(CarRoute& car, Node& sell,
	const vector<vector<int>>& distance, const vector<vector<int>>& time, const vector<Vehicle>& vehicle)
{
	CarRoute curCar(car);
	//访问sell商户
	sell.isServer = true;
	curCar.curVolume += sell.volume;
	curCar.curWeight += sell.weight;

	int lastIndex = curCar.route.back();
	curCar.curMileage = curCar.curMileage + distance[lastIndex][sell.id]
		+ distance[sell.id][0] - distance[lastIndex][0];

	int arriveTime = curCar.curTime - time[lastIndex][0] + time[lastIndex][sell.id];

	//时间窗约束
	if (arriveTime > sell.last_int_tm)
		return false;

	int waitT = sell.first_int_tm > arriveTime ?
		sell.first_int_tm - arriveTime : 0;
	//更新出发时间,出发时间为 货车在第一次到达商户的等待时间
	if (curCar.leaveTime == -1)
	{
		curCar.leaveTime = waitT;
	}
	else 
	{
		curCar.waitTime += waitT;
	}

	curCar.curTime = arriveTime + waitT + 30 + time[sell.id][0];
	//到达时间+等待+服务+返回

	curCar.route.push_back(sell.id);
	curCar.totalMileage = curCar.totalMileage + distance[lastIndex][sell.id]
		+ distance[sell.id][0] - distance[lastIndex][0];

	curCar.backTime = curCar.curTime;
	//curCar.waitTime += waitT;

	if (!isConstraint(curCar, vehicle))
		return false;

	car = curCar;
	return true;

}

//如果可以先访问sell在去充电站，则返回true，改变car状态；否则，返回false
bool legalVisitSellCharger(CarRoute& car, const vector<Vehicle>& vehicle, const Node& sell,
	const vector<vector<int>>& distance, const vector<vector<int>>& time, const vector<Node>& charger)

{
	CarRoute curCar(car);

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

	//更新出发时间,出发时间为 货车在第一次到达商户的等待时间
	if (curCar.leaveTime == -1)
	{
		curCar.leaveTime = waitT;
	}
	else
	{
		curCar.waitTime += waitT;
	}

	curCar.curTime = arriveTime + waitT + 30 + time[sell.id][0];

	//cout<<curCar.curMileage<<" "<<curCar.curVolume<<" "<<curCar.curWeight<<" "<<curCar.curTime<<endl;

	if (!isConstraint(curCar, vehicle))
		return false;
	//特殊情况
	curCar.curMileage = curCar.curMileage - distance[sell.id][charger[closeChargerIndex].id] + distance[sell.id][0];
	curCar.route.push_back(sell.id);
	curCar.totalMileage = curCar.totalMileage + distance[lastIndex][sell.id]
		+ distance[sell.id][0] - distance[lastIndex][0];
	curCar.backTime = curCar.curTime;

	//访问充电站
	visitCharge(curCar, charger[closeChargerIndex], distance, time);

	car = curCar;
	return true;
}
