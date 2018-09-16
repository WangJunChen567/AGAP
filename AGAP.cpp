#include "AGAP.h"
#include "global.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <numeric>

void algorithm::run() {
	while (!terminating()) {
		evolve();
	}
}

bool algorithm::terminating() {
	if (m_evaluations >= m_max_evaluations)
		return true;
	else
		return false;
}
