#pragma once
#ifndef _LOGISTICS_H_
#define _LOGISTICS_H_


#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
using namespace std;

const double MAX_DOUBLE = 100000000;
const double A = 0.4;
const double B = 1.2;
const double C = 1.1;
const int probability = 80;
const double GOBACK = 0.99;
const int SEED = 1;


class Vehicle {
public:
	int id;
	string name;
	double volume;
	double weight;
	int driving_range;
	double charge_tm;
	double unit_trans_cost;
	int vechile_cost;
public:
	Vehicle() {}
	Vehicle(int i, string n, double v, double w, int r, double tm, double unit, int cost) :
		id(i), name(n), volume(v), weight(w), driving_range(r),
		charge_tm(tm), unit_trans_cost(unit), vechile_cost(cost) {}
};

class Node {
public:
	int id;
	int type;
	double lng;
	double lat;
	double weight;
	double volume;
	string first_receive_tm;
	string last_receive_tm;
	int first_int_tm;
	int last_int_tm;
	bool isServer;

};
class CarRoute {
public:
	int curTime;        //时间约束
	int curMileage;     //里程约束
	double curVolume;   //容量约束
	double curWeight;   //载重约束

	int cartype;
	vector<int> route;
	int leaveTime;
	int backTime;
	int waitTime;
	int chargeNum;
	int totalMileage;
public:
	CarRoute() : curTime(0), curMileage(0), curVolume(0), curWeight(0),
		cartype(2), leaveTime(-1), backTime(0), waitTime(0),
		chargeNum(0), totalMileage(0) {}
};

struct AnswerRecord {

	string trans_code;
	string vehicle_type;
	string dist_seq;
	string distribute_lea_tm;
	string distribute_arr_tm;
	int distance;
	double trans_cost;
	int charge_cost;
	double wait_cost;
	int fixed_use_cost;
	double total_cost;
	int charge_cnt;
};




extern vector<int> sellMatchCharger;  //下标对应sell的id，内容是 charger的下标



//函数声明
int changeTime(const string& str); //把时间换成int

bool lastTimeCompare(Node node1, Node node2);

vector<CarRoute> execute(vector<Node>& seller, const vector<Node>& charger, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle);

bool isAllserver(const vector<Node>& seller);

bool isConstraint(const CarRoute& curCar, const vector<Vehicle>& vehicle);

bool canVisit(CarRoute curCar, const vector<Vehicle>& vehicle, const Node& sell
	, const vector<vector<int>>& distance, const vector<vector<int>>& time);

double calWeight(const CarRoute& curCar, const vector<Vehicle>& vehicle, const Node& sell
	, const vector<vector<int>>& distance, const vector<vector<int>>& time);

void visitSeller(CarRoute& curCar, Node& sell,
	const vector<vector<int>>& distance, const vector<vector<int>>& time);

int nextVisitIndex(const CarRoute& curCar, const vector<Node>& seller,
	const vector<vector<int>>& distance, const vector<vector<int>>& time, const vector<Vehicle>& vehicle);

bool isGoback(const CarRoute& curCar, const vector<Vehicle>& vehicle);

int nextSellCharger(const CarRoute& curCar, const vector<Node>& seller, const vector<Node>& charger,
	const vector<vector<int>>& distance, const vector<vector<int>>& time, const vector<Vehicle>& vehicle);

bool bySellChargeCanVisit(CarRoute curCar, const vector<Vehicle>& vehicle, const Node& sell,
	const vector<vector<int>>& distance, const vector<vector<int>>& time, const vector<Node>& charger);

int findCharge(Node sell, const vector<vector<int>>& distance, const vector<Node>& charger);

void visitCharge(CarRoute& curCar, const Node& charger,
	const vector<vector<int>>& distance, const vector<vector<int>>& time);

vector<AnswerRecord> getAnswer(const vector<CarRoute>& result, const vector<Vehicle>& vehicle);

string toTransCode(size_t i);

string toDistSeq(vector<int> vec);

string toTime(int num);

vector<vector<Node>>  kMeans(const vector<Node>& seller, const int k);

int  findeCoreIndex(Node sell, const vector<Node>& coreSeller);

Node calNewCore(const vector<Node>& seller);

double distanceNode(Node node1, Node node2);

void changeVehicleType(vector<CarRoute>& result, const vector<vector<int>>& distance, const vector<Vehicle>& vehicle);

bool canChangeCarType(const CarRoute& cRoute, const vector<vector<int>>& distance, const vector<Vehicle>& vehicle);

bool lessVehicleType(CarRoute car1, CarRoute car2);

int nextRandomVisitIndex(const CarRoute& curCar, const vector<Node>& seller,
	const vector<vector<int>>& distance, const vector<vector<int>>& time, const vector<Vehicle>& vehicle);

double calRandomWeight(const CarRoute& curCar, const vector<Vehicle>& vehicle, const Node& sell
	, const vector<vector<int>>& distance, const vector<vector<int>>& time);

//更改货车的型号
void changeVehicleType2(vector<CarRoute>& result, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);

//货车的型号是否可以更改，如果可以直接更改，如果不可以，返回false
bool isChangeVehicleType(CarRoute& car, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger);












#endif // _LOGISTICS_H_


