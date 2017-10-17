#pragma once
#include <af/array.h>
#include "parameters.h"
using namespace af;

class Data
{
public:
	array weight_mtrx;
	array biases;
	Data();
	void load();
};
