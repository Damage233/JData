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

//����sellMatchCharger;�±��Ӧsell��id�������� charger���±�
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

//�������壬���棬���죬���ҽ������Ŵ��㷨
void inheritance(vector<CarRoute>& result, const vector<vector<int>>& distance,
	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList, const vector<Node>& charger)
{
	//�������壬���棬���죬���ҽ������Ŵ��㷨

	int c = CIRCAL_TIME;
	while (--c)
	{
		DWORD start_time = GetTickCount();
		for (size_t i = 0; i < result.size(); ++i)
		{
			for (size_t j = 0; j < result.size(); ++j)
				if (i != j)
					exchangeCarRoute2(result[i], result[j], distance, time, vehicle, nodeList,charger); //�������

		}
		for (size_t i = 0; i < result.size(); ++i)
		{
			for (size_t j = 0; j < result.size(); ++j)
				if (i != j)
					addOrDeleteCarRoute2(result[i], result[j], distance, time, vehicle, nodeList, charger); //�������

		}
		for (size_t i = 0; i < result.size(); ++i)
		{
			selfExchangeCarRoute(result[i], distance, time, vehicle, nodeList, charger); //���ҽ���
		}
		for (size_t i = 0; i < result.size(); ++i)
		{
			selfAddOrDeleteCarRoute(result[i], distance, time, vehicle, nodeList, charger); //���ҵ���
		}
		std::cout << "Iterator" << CIRCAL_TIME - c << ":" << calAllCost(result, vehicle) << endl;
		DWORD end_time = GetTickCount();
		cout <<"���ε�����ʱ��"<< (end_time - start_time) / 1000 << "s" << endl;

		//ÿ���� 2 �� �� д���ļ�
		if ((CIRCAL_TIME - c) % 5 == 0)
		{
			//vector<CarRoute> tem = result;

			//changeVehicleType(result, distance, vehicle);		//ת��������ͺ�
			changeVehicleType2(result, distance, time, vehicle, nodeList, charger);
			deleteEmptyCar(result);

			sort(result.begin(), result.end(), lessVehicleType);

			vector<AnswerRecord> answer;
			answer = getAnswer(result, vehicle);

			whrit_answer(answer);
		}

	}
	
}
	
////����Ⱥ�Ż��������Ŵ��㷨
//void inheritance2(vector<CarRoute>& result, const vector<vector<int>>& distance,
//	const vector<vector<int>>& time, const vector<Vehicle>& vehicle, const vector<Node>& nodeList)
//{
//	vector<vector<CarRoute>> population = initPopulation(result); //��Ⱥ��ʼ��
//	int cir = ITERATION_NUM;
//	while (--cir)
//	{
//		//���в��������桢���죩����һ������֤��Ч��
//		for(auto& pop : population)
//		for (size_t i = 0; i < pop.size(); ++i)
//		{
//			for (size_t j = 0; j < pop.size(); ++j)
//				if (i != j)
//					allOperationCarRoute(pop[i], pop[j], distance, time, vehicle, nodeList); 
//
//		}
//		//����
//		sort(population.begin(), population.end(), compare);
//
//		////����Ⱥ�䣬�����Ϻõ�һ�룬�úõ�������õ�
//		updatePopulation(population);
//
//		cout << "inheritance" << ITERATION_NUM - cir << ":" << calAllCost(population[0], vehicle) << endl;
//
//		//ÿ����10�� �� д���ļ�
//		if ((ITERATION_NUM - cir) % 10 == 0)
//		{
//			vector<CarRoute> tem = population[0];
//
//			changeVehicleType(tem, distance, vehicle);		//ת��������ͺ�
//			sort(tem.begin(), tem.end(), lessVehicleType);
//
//			vector<AnswerRecord> answer;
//			answer = getAnswer(tem, vehicle);
//
//			whrit_answer(answer);
//		}
//
//	}
//	// population[0] ��һ������С�ģ��������compareֻ�Ƚ��ܾ���
//	result = population[0];
//}


//���������ķ���
double calAllCost(const vector<CarRoute>& result, const vector<Vehicle>& vehicle)
{
	////������λС��
	//record.trans_cost = result[i].totalMileage * vehicle[result[i].cartype - 1].unit_trans_cost;

	//record.charge_cost = result[i].chargeNum * 50;
	////������λС��
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

//��������
double calOneCost(const CarRoute& car, const vector<Vehicle>& vehicle)
{
	////������λС��
	//record.trans_cost = result[i].totalMileage * vehicle[result[i].cartype - 1].unit_trans_cost;

	//record.charge_cost = result[i].chargeNum * 50;
	////������λС��
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

//���Ž��������򲻱䣬����������
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
			swap(temCar1.route[i], temCar2.route[j]);  //������
			 //isLegal ����trueʱ�����޸�temCar��״̬
			if (isLegal(temCar1, distance, time, vehicle, nodeList,charger) 
				&& isLegal(temCar2, distance, time, vehicle, nodeList,charger))
			{
				double cost1 = calOneCost(temCar1, vehicle);
				double cost2 = calOneCost(temCar2, vehicle);
				if (cost1 + cost2 < minCost) //Ч����
				{
					car1 = temCar1;
					car2 = temCar2;
					minCost = cost1 + cost2;
					return;
				}
				else  //����õĽ������ԭtemCar��״̬
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


//���Ž��棬���򲻱䣬���������
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
			//·��Ϊ�յ����
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
				(rand() % 100 > JUMP_P && cost1 + cost2 - minCost < ACCEPT * minCost)) //Ч����
			{
				car1 = temCar1;
				car2 = temCar2;
				minCost = cost1 + cost2;
				return;
			}
			else  //����õĽ������ԭtemCar��״̬
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


//�Ϸ����޸� �����Ϸ��Ͳ���
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

	//�ı�����ͺ�
	isChangeVehicleType(tempCar, distance, time, vehicle, nodeList, charger);

	car = tempCar;
	return true;

}

//�������ӻ�ɾ�������򲻱�
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

													   //isLegal ����trueʱ�����޸�temCar��״̬
			if (isLegal(temCar1, distance, time, vehicle, nodeList, charger)
				&& isLegal(temCar2, distance, time, vehicle, nodeList, charger))
			{
				double cost1 = calOneCost(temCar1, vehicle);
				double cost2 = calOneCost(temCar2, vehicle);
				if (cost1 + cost2 < minCost) //Ч����
				{
					car1 = temCar1;
					car2 = temCar2;
					minCost = cost1 + cost2;
					return;
				}
				else  //����õĽ������ԭtemCar��״̬
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

//������ӻ�ɾ�������򲻱�
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
			//��ֹ����·��Ϊ�յ����
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
				(rand() % 100 > JUMP_P && cost1 + cost2 - minCost < ACCEPT * minCost)) //Ч����
			{
				car1 = temCar1;
				car2 = temCar2;
				minCost = cost1 + cost2;
				return;
			}
			else  //����õĽ������ԭtemCar��״̬
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




//Ⱥ���ʼ��
vector<vector<CarRoute>> initPopulation(const vector<CarRoute>& result)
{
	vector<vector<CarRoute>> results;
	for (int i = 0; i < POPULATION_NUM; ++i)
	{
		results.push_back(result);
	}
	return results;
}


//����֮��ıȽϺ���
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


////����Ⱥ�䣬�����Ϻõ�һ�룬�úõ�������õ�
void updatePopulation(vector<vector<CarRoute>>& population)
{
	typedef vector<vector<CarRoute>>::iterator Iter;
	Iter iter1 = population.begin();
	Iter iter2 = population.begin() + population.size() / 2;
	for (Iter i = iter2; i != population.end(); ++i, ++iter1)
		*i = *iter1;
}


//���ҽ���
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
			//·��Ϊ�յ����
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
			if (cost1 < minCost) //Ч����
			{
				car1 = temCar1;
				minCost = cost1;
				return;
			}
			else  //����õĽ������ԭtemCar��״̬
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

//����ɾ�����ӣ�Ҳ�ǵ���λ��
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
			//��ֹ����·��Ϊ�յ����
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
			if (cost1 < minCost) //Ч����
			{
				car1 = temCar1;
				minCost = cost1;
				return;
			}
			else  //����õĽ������ԭtemCar��״̬
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

//���в����ŵ�һ�飬�ڽ�����֤
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
		//����
		for (size_t i = 0; i < num; ++i)
		{
			//·��Ϊ�յ����
			if (temCar1.route.size() == 1 || temCar2.route.size() == 1)
				break;
			int d1 = rand() % (temCar1.route.size() - 1) + 1;
			int d2 = rand() % (temCar2.route.size() - 1) + 1;
			swap(temCar1.route[d1], temCar2.route[d2]);
		}
		//����
		typedef vector<int>::iterator Iter;
		for (size_t i = 0; i < num; ++i)
		{
			//��ֹ����·��Ϊ�յ����
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
		//���ҽ���
		for (size_t i = 0; i < num; ++i)
		{
			//·��Ϊ�յ����
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
			//·��Ϊ�յ����
			if (temCar2.route.size() <= 2)
				break;
			int d1 = rand() % (temCar2.route.size() - 1) + 1;
			int d2 = rand() % (temCar2.route.size() - 1) + 1;
			if (d1 == d2)
				continue;
			swap(temCar2.route[d1], temCar2.route[d2]);
		}
		//���ұ���
		for (size_t i = 0; i < num; ++i)
		{
			//��ֹ����·��Ϊ�յ����
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
			//��ֹ����·��Ϊ�յ����
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

		//��֤������Ч��
		if (isLegal(temCar1, distance, time, vehicle, nodeList, charger) && isLegal(temCar2, distance, time, vehicle, nodeList, charger))
		{
			double cost1 = calOneCost(temCar1, vehicle);
			double cost2 = calOneCost(temCar2, vehicle);
			if (cost1 + cost2 <= minCost) //Ч����
			{
				car1 = temCar1;
				car2 = temCar2;
				minCost = cost1 + cost2;
				break;
			}
			else  //����õĽ������ԭtemCar��״̬
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


//����ֱ�ӷ��ʣ�����ʸı�car��״̬�����򣬷���fasle�����ı�car״̬
bool legalVisitSeller(CarRoute& car, Node& sell,
	const vector<vector<int>>& distance, const vector<vector<int>>& time, const vector<Vehicle>& vehicle)
{
	CarRoute curCar(car);
	//����sell�̻�
	sell.isServer = true;
	curCar.curVolume += sell.volume;
	curCar.curWeight += sell.weight;

	int lastIndex = curCar.route.back();
	curCar.curMileage = curCar.curMileage + distance[lastIndex][sell.id]
		+ distance[sell.id][0] - distance[lastIndex][0];

	int arriveTime = curCar.curTime - time[lastIndex][0] + time[lastIndex][sell.id];

	//ʱ�䴰Լ��
	if (arriveTime > sell.last_int_tm)
		return false;

	int waitT = sell.first_int_tm > arriveTime ?
		sell.first_int_tm - arriveTime : 0;
	//���³���ʱ��,����ʱ��Ϊ �����ڵ�һ�ε����̻��ĵȴ�ʱ��
	if (curCar.leaveTime == -1)
	{
		curCar.leaveTime = waitT;
	}
	else 
	{
		curCar.waitTime += waitT;
	}

	curCar.curTime = arriveTime + waitT + 30 + time[sell.id][0];
	//����ʱ��+�ȴ�+����+����

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

//��������ȷ���sell��ȥ���վ���򷵻�true���ı�car״̬�����򣬷���false
bool legalVisitSellCharger(CarRoute& car, const vector<Vehicle>& vehicle, const Node& sell,
	const vector<vector<int>>& distance, const vector<vector<int>>& time, const vector<Node>& charger)

{
	CarRoute curCar(car);

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

	//���³���ʱ��,����ʱ��Ϊ �����ڵ�һ�ε����̻��ĵȴ�ʱ��
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
	//�������
	curCar.curMileage = curCar.curMileage - distance[sell.id][charger[closeChargerIndex].id] + distance[sell.id][0];
	curCar.route.push_back(sell.id);
	curCar.totalMileage = curCar.totalMileage + distance[lastIndex][sell.id]
		+ distance[sell.id][0] - distance[lastIndex][0];
	curCar.backTime = curCar.curTime;

	//���ʳ��վ
	visitCharge(curCar, charger[closeChargerIndex], distance, time);

	car = curCar;
	return true;
}
