#include "AGAP.h"
#include "global.h"

int main() {
	global::ms_global.reset(new global());
	global::ms_global->m_problem.reset(new problem());
	global::ms_global->m_problem->initialize();
	global::ms_global->m_algorithm.reset(new algorithm(100,200000));
	global::ms_global->m_algorithm->initialize();
	global::ms_global->m_algorithm->run();
	global::ms_global->m_algorithm->output_result();
	return 0;
}

