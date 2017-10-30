/*******************************************************
 * Copyright (c) 2014, ArrayFire
 * All rights reserved.
 *
 * This file is distributed under 3-clause BSD license.
 * The complete license agreement can be obtained at:
 * http://arrayfire.com/licenses/BSD-3-Clause
 ********************************************************/

#include <arrayfire.h>
#include <cstdio>
#include <cstdlib>
#include "parameters.h"
#include "Data.h"
#include <iomanip>
#include <imagehlp.h>
#include <ctime>
#pragma comment(lib, "imagehlp.lib")>
#include <fstream>
#include <iostream>
#include  <algorithm>

using namespace af;
Data data;

double city_dist(int x, int y)
{
	return sqrt(
		(cities[x][0] - cities[y][0]) * (cities[x][0] - cities[y][0]) + (cities[x][1] - cities[y][1]) * (cities[x][1] - cities
			[y][1]));
}

double calcRouteLength(std::vector<int> const& path)
{
	double result = city_dist(path[0], path[path.size() - 1]);
	for (int i = 1; i < path.size(); ++i)
	{
		result += city_dist(path[i], path[i - 1]);
	}
	return result;
}

void print(const std::vector<int>& v)
{
	std::for_each(v.begin(), v.end(), [](int x)
	{
		std::cout << x << " ";
	});
	std::cout << std::endl;
}

void allPath(std::ostream& ofs = std::cout)
{
	std::vector<int> x = {0,1,2,3,4,5,6,7,8,9};
	int nx = x.size();
	do
	{
		ofs << calcRouteLength(x) << ",";
	}
	while (next_permutation(x.begin(), x.end()));
}

array activationFunc(const array& inputs)
{
	return sigmoid(inputs / U0);
}

array calcDeltaU(const array& state, const array& innerVal)
{
	const double tau = 1.0;
	array delta = (-innerVal / tau + matmul(data.weight_mtrx, state) + data.biases) * DELTA_T;
	return delta;
}

void outState(const array& state, std::ostream& out = std::cout)
{
	int n = cities.size();
	for (int s0 = 0; s0 < n; ++s0)
	{
		for (int s1 = 0; s1 < n; ++s1)
		{
			int i = s0 * n + s1;
			double s = sum<double>(state(i));
			out << s;
			if (s1 != n - 1)
			{
				out << ",";
			}
		}
		out << std::endl;
	}
}

void run(std::ostream& ofs = std::cout)
{
	int n = cities.size() * cities.size();
	setSeed(time(nullptr));
	array innerVal = NOISE * (randu(n, f64) - constant(0.5, n, f64));
	array result = activationFunc(innerVal);
	float progress = 0.0;
	const double step = 1.0 / (RECALL_TIME - 1.0);
	dim4 new_dims(cities.size(), cities.size());
	//	af_print(moddims(result, new_dims))
	for (int i = 0; i < RECALL_TIME; ++i)
	{
		//update innerVal
		innerVal += calcDeltaU(result, innerVal);
		//update state from innerVal
		result = activationFunc(innerVal);
		int barWidth = 70;
		std::cout << "[";
		int pos = barWidth * progress;
		for (int i = 0; i < barWidth; ++i)
		{
			if (i < pos) std::cout << "=";
			else if (i == pos) std::cout << ">";
			else std::cout << " ";
		}
		std::cout << "] " << int(progress * 100.0) << " %\r";
		std::cout.flush();
		progress += step;
		//		if (i == RECALL_TIME / 2)
		//		{
		//			af_print(moddims(result, new_dims))
		//			outState(result, ofs);
		//		}
	}
	std::cout << std::endl;
	std::vector<int> route;
	for (int i = 0; i < cities.size(); ++i)
	{
		int pos = -1;
		double maxval = 0.0;
		for (int x = 0; x < cities.size(); ++x)
		{
			int index = x * cities.size() + i;
			double val = sum<double>(result(index));
			if (val > maxval)
			{
				maxval = val;
				pos = x;
			}
		}
		if (pos == -1)
		{
			break;
		}
		route.push_back(pos);
	}
	outState(result, ofs);
	for (int i = 0; i < cities.size(); ++i)
	{
		ofs << ",";
	}
	if (route.size() == cities.size())
	{
		double length = calcRouteLength(route);
		print(route);
		std::cout << std::endl;
		ofs << length << std::endl;
	}
	else
	{
		ofs << "invailed" << std::endl;
	}
}


int main(int argc, char* argv[])
{
	try
	{
		// Select a device and display arrayfire info
		int device = argc > 1 ? atoi(argv[1]) : 0;
		setDevice(device);
		info();
		//		std::string filename = "path";
		//		time_t epoch_time;
		//		epoch_time = time(nullptr);
		//		filename += "-" + std::to_string(epoch_time);
		//		filename += ".csv";
		//		std::ofstream ofs(filename);
		//		allPath(ofs);
		data.load();
		//		af_print(data.weight_mtrx);
		std::string filename = "result";
		time_t epoch_time;
		epoch_time = time(nullptr);
		filename += "-" + std::to_string(epoch_time);
		filename += ".csv";
		std::ofstream ofs(filename);
		for (int i = 0; i < TRIAL; ++i)
		{
			run(ofs);
			ofs << std::endl;
		}
	}
	catch (exception& e)
	{
		fprintf(stderr, "%s\n", e.what());
		throw;
	}

	return 0;
}
