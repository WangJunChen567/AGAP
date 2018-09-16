#ifndef GLOBAL_H
#define GLOBAL_H

#include "AGAP.h"

struct global {
	std::unique_ptr<problem> m_problem;
	std::unique_ptr<algorithm> m_algorithm;

	static std::unique_ptr<global> ms_global;
};

#endif // !GLOBAL_H
