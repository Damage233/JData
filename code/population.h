#pragma once

#include "logistics.h"
#include "read_file.h"

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <Windows.h>

using namespace std;

//种群中个体的数量
const int POPULATION_NUM = 10;

//每代执行的最多次数
const int REPEAT_NUM = 20;

//遗传代数
const int ITERATION_NUM = 500000;

//迭代次数
const int CIRCAL_TIME = 20000;

//执行最多多少次变化，再验证序列的有效性
const int CHANGE_NUM = 5;

//跳出局部最优大概率
const double JUMP_P = 95;

//可接受变差的范围
const double ACCEPT = 0.01;

extern vector<int> sellMatchCharger;  //下标对应sell的id，内容是 charger的下标




//函数声明

bool isLegal(CarRoute& car, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//单个个体，交叉，变异，自我交换的遗传算法
void inheritance(vector<CarRoute>& result, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//以种群优化迭代，遗传算法
void inheritance2(vector<CarRoute>& result, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//遍历增加或删减，否则不变
void addOrDeleteCarRoute(CarRoute& car1, CarRoute& car2, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//随机增加或删减，否则不变
void addOrDeleteCarRoute2(CarRoute& car1, CarRoute& car2, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//择优交叉，否则不变，遍历单交叉
void exchangeCarRoute(CarRoute& car1, CarRoute& car2, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//择优交叉，否则不变，随机单交叉
void exchangeCarRoute2(CarRoute& car1, CarRoute& car2, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//群落初始化
vector<vector<CarRoute>> initPopulation(const vector<CarRoute>& result);

//单个方案的费用
double calAllCost(const vector<CarRoute>& result, const vector<Vehicle>& vehicle);

//单车费用
double calOneCost(const CarRoute& car, const vector<Vehicle>& vehicle);

//个体之间的比较函数
bool compare(const vector<CarRoute>& a, const vector<CarRoute>& b);

////更新群落，保留较好的一半，用好的替代不好的
void updatePopulation(vector<vector<CarRoute>>& population);

//自我交换
void selfExchangeCarRoute(CarRoute& car1, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//自我删除增加，也是调整位置
void selfAddOrDeleteCarRoute(CarRoute& car1, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//所有操作放到一块，在进行验证
void allOperationCarRoute(CarRoute& car1, CarRoute& car2, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//计算sellMatchCharger;  
//下标对应sell的id，内容是 charger的下标
void calSellChargerMatch(vector<int> &vec, const vector<vector<int>>& distance,
	const vector<Node>& charger, const vector<Node>& seller);

//可以直接访问，则访问改变car的状态；否则，返回fasle，不改变car状态
bool legalVisitSeller(CarRoute& car, Node& sell,
	const vector<vector<int>>& distance, const vector<vector<int>>& time, const vector<Vehicle>& vehicle);

//如果可以先访问sell在去充电站，则返回true，改变car状态；否则，返回false
bool legalVisitSellCharger(CarRoute& car, const vector<Vehicle>& vehicle, const Node& sell,
	const vector<vector<int>>& distance, const vector<vector<int>>& time, const vector<Node>& charger);
