#pragma once
#ifndef _READ_FILE_H_
#define _READ_FILE_H_


#include <iostream>
#include <fstream>
#include <vector>
#include "logistics.h"
using namespace std;

void read_node(vector<Node>& vec);
void read_distance(vector<vector<int>>& dis, vector<vector<int>>& time);
void whrit_answer(vector<AnswerRecord> answer);
void showVehicle(const Vehicle& vehicle);
void showNodeList(const vector<Node>& vec);
void showResult(const vector<CarRoute>& result);
















#endif // _READ_FILE_H_

