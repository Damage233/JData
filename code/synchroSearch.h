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


//��������

//��ʼ��vector<CarRoute> result
vector<CarRoute> initResult();

//ͬ������
vector<CarRoute> synchroSearch(vector<Node>& seller, const vector<Node>& charger, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle);

//seller����ıȽϺ���
bool timeCompare(const Node& node1, const Node& node2);

//������ȥ�����������
int whichCarVisitThisSell(vector<CarRoute>& result, const Node& sell, const vector<Node>& charger, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle);

//ɾ���յĻ���
void deleteEmptyCar(vector<CarRoute>& result);