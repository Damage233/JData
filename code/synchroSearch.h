#pragma once

#include "logistics.h"
#include "read_file.h"

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

const int INIT_CAR_NUM = 170;


//函数声明

//初始化vector<CarRoute> result
vector<CarRoute> initResult();

//同步搜索
vector<CarRoute> synchroSearch(vector<Node>& seller, const vector<Node>& charger, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle);

//seller排序的比较函数
bool timeCompare(const Node& node1, const Node& node2);

//哪辆车去服务这家商铺
int whichCarVisitThisSell(vector<CarRoute>& result, const Node& sell, const vector<Node>& charger, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle);

//删除空的货车
void deleteEmptyCar(vector<CarRoute>& result);