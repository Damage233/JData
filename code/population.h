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

//��Ⱥ�и��������
const int POPULATION_NUM = 10;

//ÿ��ִ�е�������
const int REPEAT_NUM = 20;

//�Ŵ�����
const int ITERATION_NUM = 500000;

//��������
const int CIRCAL_TIME = 20000;

//ִ�������ٴα仯������֤���е���Ч��
const int CHANGE_NUM = 5;

//�����ֲ����Ŵ����
const double JUMP_P = 95;

//�ɽ��ܱ��ķ�Χ
const double ACCEPT = 0.01;

extern vector<int> sellMatchCharger;  //�±��Ӧsell��id�������� charger���±�




//��������

bool isLegal(CarRoute& car, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//�������壬���棬���죬���ҽ������Ŵ��㷨
void inheritance(vector<CarRoute>& result, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//����Ⱥ�Ż��������Ŵ��㷨
void inheritance2(vector<CarRoute>& result, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//�������ӻ�ɾ�������򲻱�
void addOrDeleteCarRoute(CarRoute& car1, CarRoute& car2, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//������ӻ�ɾ�������򲻱�
void addOrDeleteCarRoute2(CarRoute& car1, CarRoute& car2, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//���Ž��棬���򲻱䣬����������
void exchangeCarRoute(CarRoute& car1, CarRoute& car2, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//���Ž��棬���򲻱䣬���������
void exchangeCarRoute2(CarRoute& car1, CarRoute& car2, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//Ⱥ���ʼ��
vector<vector<CarRoute>> initPopulation(const vector<CarRoute>& result);

//���������ķ���
double calAllCost(const vector<CarRoute>& result, const vector<Vehicle>& vehicle);

//��������
double calOneCost(const CarRoute& car, const vector<Vehicle>& vehicle);

//����֮��ıȽϺ���
bool compare(const vector<CarRoute>& a, const vector<CarRoute>& b);

////����Ⱥ�䣬�����Ϻõ�һ�룬�úõ�������õ�
void updatePopulation(vector<vector<CarRoute>>& population);

//���ҽ���
void selfExchangeCarRoute(CarRoute& car1, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//����ɾ�����ӣ�Ҳ�ǵ���λ��
void selfAddOrDeleteCarRoute(CarRoute& car1, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//���в����ŵ�һ�飬�ڽ�����֤
void allOperationCarRoute(CarRoute& car1, CarRoute& car2, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//����sellMatchCharger;  
//�±��Ӧsell��id�������� charger���±�
void calSellChargerMatch(vector<int> &vec, const vector<vector<int>>& distance,
	const vector<Node>& charger, const vector<Node>& seller);

//����ֱ�ӷ��ʣ�����ʸı�car��״̬�����򣬷���fasle�����ı�car״̬
bool legalVisitSeller(CarRoute& car, Node& sell,
	const vector<vector<int>>& distance, const vector<vector<int>>& time, const vector<Vehicle>& vehicle);

//��������ȷ���sell��ȥ���վ���򷵻�true���ı�car״̬�����򣬷���false
bool legalVisitSellCharger(CarRoute& car, const vector<Vehicle>& vehicle, const Node& sell,
	const vector<vector<int>>& distance, const vector<vector<int>>& time, const vector<Node>& charger);
