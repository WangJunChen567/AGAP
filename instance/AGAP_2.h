#ifndef AGAP_2_H
#define AGAP_2_H

#include "AGAP_1.h"

#define AGAP_2_CAST dynamic_cast<AGAP_2*>(global::ms_global->m_problem.get())

class AGAP_2 : public AGAP_1 {
public:
	AGAP_2(size_t num_objs = 2) : AGAP_1(num_objs) {}
	void initialize();
	void evaluate(solution& sol);
	void record_pass_info(const solution & sol, std::vector<size_t>& nums_passengers, std::vector<int>& process_times);
	bool same(const solution& s1, const solution& s2);
	bool dominate(const solution& s1, const solution& s2);
protected:
	std::vector<std::vector<int>> m_ticket_info; // [i][0]-num_pass, [i][1]-arr_tra, [i][2]-lea_tra, [i][3]-arr_type, [i][4]-lea_type
	std::vector<std::vector<std::vector<std::vector<size_t>>>> m_proce_time; // [i]-arr_type, [j]-lea_type, [k]-arr_ter, [l]-lea_ter  
};

class Alg_2 : public Alg_1 {
public:
	Alg_2(size_t size_pop, size_t max_evaluations) : Alg_1(size_pop, max_evaluations) {};
	void output_result();
};


#endif // !AGAP_2_H
