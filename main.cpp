#include "AGAP_3.h"
#include "global.h"

int main() {
	global::ms_global.reset(new global());
	global::ms_global->m_problem.reset(new AGAP_3());
	global::ms_global->m_problem->initialize();
	global::ms_global->m_algorithm.reset(new Alg_3(100,20000));
	global::ms_global->m_algorithm->initialize();
	global::ms_global->m_algorithm->run();
	global::ms_global->m_algorithm->output_result();
	return 0;
}

