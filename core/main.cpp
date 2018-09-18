#include "../instance/AGAP_3.h"
#include "global.h"

int main(int argc, char *argv[]) {
	global::ms_global.reset(new global());
	switch (atoi(argv[1])){
	case (1):
		global::ms_global->m_problem.reset(new AGAP_1());
		break;
	case (2):
		global::ms_global->m_problem.reset(new AGAP_2());
		break;
	case (3):
		global::ms_global->m_problem.reset(new AGAP_3());
		break;
	}
	global::ms_global->m_problem->initialize();
	switch (atoi(argv[1])) {
	case (1):
		global::ms_global->m_algorithm.reset(new Alg_1(100, atoi(argv[2])));
		break;
	case (2):
		global::ms_global->m_algorithm.reset(new Alg_2(100, atoi(argv[2])));
		break;
	case (3):
		global::ms_global->m_algorithm.reset(new Alg_3(100, atoi(argv[2])));
		break;
	}
	global::ms_global->m_algorithm->initialize();
	global::ms_global->m_algorithm->run();
	global::ms_global->m_algorithm->output_result();
	return 0;
}

