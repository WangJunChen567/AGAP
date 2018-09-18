#ifndef AGAP_3_H
#define AGAP_3_H

#include "AGAP_2.h"

#define AGAP_3_CAST dynamic_cast<AGAP_3*>(global::ms_global->m_problem.get())

class AGAP_3 : public AGAP_2 {
public:
	AGAP_3(size_t num_objs = 2) : AGAP_2(num_objs) {}
	void initialize();
	void evaluate(solution& sol);
	void record_pass_info(const solution & sol, std::vector<size_t>& nums_passengers, std::vector<double>& transfer_times, std::vector<double>& transfer_tensions);
protected:
	std::vector<size_t> m_gate_info;
	std::vector<std::vector<size_t>> m_walk_time;
	std::vector<std::vector<std::vector<std::vector<size_t>>>> m_metro_times; // [i]-arr_type, [j]-lea_type, [k]-arr_ter, [l]-lea_ter  
};

class Alg_3 : public Alg_2 {
public:
	Alg_3(size_t size_pop, size_t max_evaluations) : Alg_2(size_pop, max_evaluations) {};
	void output_result();
};

#endif // !AGAP_3_H

