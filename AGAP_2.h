#ifndef AGAP_2_H
#define AGAP_2_H

#include "AGAP_1.h"

#define AGAP_2_CAST dynamic_cast<AGAP_2*>(global::ms_global->m_problem.get())

class AGAP_2 : public AGAP_1 {
public:
	AGAP_2() = default;
	void initialize();
	void evaluate(solution& sol);
protected:
	std::vector<std::vector<size_t>> m_Pans_to_Proc_times;
};

class Alg_2 : public Alg_1 {
public:
	Alg_2(size_t size_pop, size_t max_evaluations) : Alg_1(size_pop, max_evaluations) {};
	void initialize();
	void output_result();
};


#endif // !AGAP_2_H
