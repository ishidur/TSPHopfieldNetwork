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

array reluFunc(const array& inputs)
{
	array result = inputs + 0.5;
	result(result > 1.0) = 1.0;
	result(result < 0.0) = 0.0;
	return result;
}

array reluFunc1(const array& inputs)
{
	array result = inputs;
	result(result > 1.0) = 1.0;
	result(result < 0.0) = 0.0;
	return result;
}

array calcDeltaU(const array& state)
{
	array delta = (matmul(data.weight_mtrx, state) + data.biases) * DELTA_T;
	return delta;
}

void run()
{
	int n = cities.size() * cities.size();
	array innerVal = NOISE * (randu(n, f64) - constant(0.5, n, f64));
	array result = activationFunc(innerVal);
	float progress = 0.0;
	const double step = 1.0 / (RECALL_TIME - 1.0);
	for (int i = 0; i < RECALL_TIME; ++i)
	{
		//update innerVal
		innerVal += calcDeltaU(result);
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
	}
	std::cout << std::endl;

	dim4 new_dims(cities.size(), cities.size());
	af_print(moddims(result, new_dims));
//	array sdf = range(cities.size() * cities.size());
//	af_print(moddims(sdf, new_dims));
}

int main(int argc, char* argv[])
{
	try
	{
		// Select a device and display arrayfire info
		int device = argc > 1 ? atoi(argv[1]) : 0;
		setDevice(device);
		info();
		//		array asdf = { 1.0, 0.2, -0.5, -0.2, 0.9 };
		data.load();
		//		af_print(data.weight_mtrx)		;
		run();
	}
	catch (exception& e)
	{
		fprintf(stderr, "%s\n", e.what());
		throw;
	}

	return 0;
}
