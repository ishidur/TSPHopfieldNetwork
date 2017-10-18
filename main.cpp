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

void run()
{
	int n = cities.size() * cities.size();
	array innerVal = NOISE * (randu(n) - constant(0.5, n, f64));
	array result = activationFunc(innerVal);
	for (int i = 0; i < RECALL_TIME; ++i)
	{
		//update innerVal
		innerVal += calcDeltaU(result, innerVal);
		//update state from innerVal
		result = activationFunc(innerVal);
	}
	dim4 new_dims(cities.size(), cities.size());
	af_print(moddims(innerVal, new_dims))
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
