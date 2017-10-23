#include "Data.h"
#include <af/random.h>
#include <af/data.h>
#include <iostream>


double kronecker_delta(int i, int j)
{
	if (j < 0)
	{
		j = cities.size() - 1;
	}
	else if (j > cities.size() - 1)
	{
		j = 0;
	}
	if (i == j)
	{
		return 1.0;
	}
	return 0.0;
}

double distance(int x, int y)
{
	return sqrt(
		(cities[x][0] - cities[y][0]) * (cities[x][0] - cities[y][0]) + (cities[x][1] - cities[y][1]) * (cities[x][1] - cities
			[y][1]));
}

array calc_weight_mtrx()
{
	int n = cities.size() * cities.size();
	array tmp = constant(0.0, n, n, f64);
	for (int s0 = 0; s0 < n; ++s0)
	{
		int x = s0 / cities.size();
		int i = s0 % cities.size();
		for (int s1 = 0; s1 < n; ++s1)
		{
			int y = s1 / cities.size();
			int j = s1 % cities.size();
			double dxy = distance(x, y);

			tmp(s0, s1) = -A * kronecker_delta(x, y) * (1.0 - kronecker_delta(i, j)) - B * kronecker_delta(i, j) * (1.0 -
				kronecker_delta(x, y)) - C - D * dxy * (kronecker_delta(j, i - 1) + kronecker_delta(j, i + 1));
		}
	}
	return tmp;
}

array calc_biases()
{
	int n = cities.size() * cities.size();
	array tmp = constant(C * N, n, f64);
	return tmp;
}

Data::Data()
{
}

void Data::load()
{
	weight_mtrx = calc_weight_mtrx();
	biases = calc_biases();
}
