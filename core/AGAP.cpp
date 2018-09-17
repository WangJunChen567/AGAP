#include "AGAP.h"

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
