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

using namespace af;
Data data;

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

//void outState(const array& state)
//{
//	int n = cities.size();
//	for (int s0 = 0; s0 < n; ++s0)
//	{
//		for (int s1 = 0; s1 < n; ++s1)
//		{
//			int i = s0 * n + s1;
//			auto s = state.row(i).col(0);
//			std::cout << s;
//			std::cout << ",";
//		}
//		std::cout << std::endl;
//	}
//}

void run()
{
	int n = cities.size() * cities.size();
	setSeed(time(nullptr));
	array innerVal = NOISE * (randu(n, f64) - constant(0.5, n, f64));
	array result = activationFunc(innerVal);
	float progress = 0.0;
	const double step = 1.0 / (RECALL_TIME - 1.0);
	dim4 new_dims(cities.size(), cities.size());
	af_print(moddims(result, new_dims))
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
		if (i == RECALL_TIME / 2)
		{
			af_print(moddims(result, new_dims))
		}
	}
	std::cout << std::endl;
	af_print(moddims(result, new_dims))
}

int main(int argc, char* argv[])
{
	try
	{
		// Select a device and display arrayfire info
		int device = argc > 1 ? atoi(argv[1]) : 0;
		setDevice(device);
		info();
		data.load();
		//		af_print(data.weight_mtrx);
		run();
	}
	catch (exception& e)
	{
		fprintf(stderr, "%s\n", e.what());
		throw;
	}

	return 0;
}
