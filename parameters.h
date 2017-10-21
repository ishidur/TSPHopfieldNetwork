#pragma once

#define RECALL_TIME 1000
#define DELTA_T 0.01
#define	NOISE 0.01
#define U0 0.02
#define A 400.0
#define B 400.0
#define D 200.0

const std::vector<std::vector<double>> cities = {
	{ 0.5, 0.9 },
	{ 0.2, 0.8 },
	{ 0.4, 0.5 },
	{ 0.15, 0.25 },
	{ 0.25, 0.1 },
	{ 0.6, 0.3 },
	{ 0.65, 0.3 },
	{ 0.9, 0.35 },
	{ 0.7, 0.5 },
	{ 0.9, 0.7 }
};