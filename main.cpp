﻿/*******************************************************
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

double calc_route_length(std::vector<int> const& path)
{
	double result = city_dist(path[0], path[path.size() - 1]);
	for (int i = 1; i < path.size(); ++i) { result += city_dist(path[i], path[i - 1]); }
	return result;
}

void print(const std::vector<int>& v)
{
	std::for_each(v.begin(), v.end(), [](int x) { std::cout << x << " "; });
	std::cout << std::endl;
}

int current = -1;
int unique_number() { return ++current; }

void all_path(std::ostream& ofs = std::cout)
{
	std::vector<int> x(cities.size());
	generate(x.begin(), x.end(), unique_number);
	int nx = x.size();
	do { ofs << calc_route_length(x) << ","; }
	while (next_permutation(x.begin(), x.end()));
}

array activation_func(const array& inputs) { return sigmoid(inputs / U0); }

array calc_delta_u(const array& state, const array& innerVal)
{
	const double tau = 1.0;
	array delta = (-innerVal / tau + matmul(data.weight_mtrx, state) + data.biases) * DELTA_T;
	return delta;
}

void out_state(const array& state, std::ostream& out = std::cout)
{
	int n = cities.size();
	for (int s0 = 0; s0 < n; ++s0)
	{
		for (int s1 = 0; s1 < n; ++s1)
		{
			int i = s0 * n + s1;
			double s = sum<double>(state(i));
			out << s;
			if (s1 != n - 1) { out << ","; }
		}
		out << std::endl;
	}
}

bool validate_route(std::vector<int> const& route)
{
	if (route.size() != cities.size()) { return false; }
	for (int i = 0; i < cities.size(); ++i)
	{
		int n = std::count(route.begin(), route.end(), i);
		if (n != 1) { return false; }
	}
	return true;
}

void run(std::ostream& ofs = std::cout)
{
	int n = cities.size() * cities.size();
	setSeed(time(nullptr));
	array inner_val = NOISE * (randu(n, f64) - constant(0.5, n, f64));
	array result = activation_func(inner_val);
	float progress = 0.0;
	const double step = 1.0 / (RECALL_TIME - 1.0);
	dim4 new_dims(cities.size(), cities.size());
	//	af_print(moddims(result, new_dims))
	for (int i = 0; i < RECALL_TIME; ++i)
	{
		//update innerVal
		inner_val += calc_delta_u(result, inner_val);
		//update state from innerVal
		result = activation_func(inner_val);
		const int barWidth = 70;
		std::cout << "[";
		int pos = barWidth * progress;
		for (int i = 0; i < barWidth; ++i)
		{
			if (i < pos) std::cout << "=";
			else if (i == pos) std::cout << ">";
			else std::cout << " ";
		}
		progress += step;
		std::cout << "] " << int(progress * 100.0) << " %\r";
		std::cout.flush();
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
		if (pos == -1) { break; }
		route.push_back(pos);
	}
	out_state(result, ofs);

	for (int i = 0; i < cities.size(); ++i) { ofs << ","; }
	print(route);
	std::cout << std::endl;
	if (validate_route(route))
	{
		double length = calc_route_length(route);
		ofs << length << std::endl;
	}
	else
	{
		std::cout << "invailed" << std::endl;
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
		time_t epoch_time;
		epoch_time = time(nullptr);
		//		filename += "-" + std::to_string(epoch_time);
		//		filename += ".csv";
		//		std::ofstream ofs(filename);
		//		allPath(ofs);
		data.load();
		std::string filename1 = "result";
		//		time_t epoch_time;
		//		epoch_time = time(nullptr);
		filename1 += "-" + std::to_string(epoch_time);
		filename1 += ".csv";
		std::ofstream ofs1(filename1);
		af_print(data.weight_mtrx)		;
		//		for (int i = 0; i < TRIAL; ++i)
		//		{
		//			run(ofs1);
		//			ofs1 << std::endl;
		//		}
	}
	catch (exception& e)
	{
		fprintf(stderr, "%s\n", e.what());
		throw;
	}

	return 0;
}
