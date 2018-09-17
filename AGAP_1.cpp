#include "AGAP_1.h"
#include "global.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <numeric>

void AGAP_1::initialize() {
	std::ifstream infile;
	std::string line;

	infile.open("./trans_time.csv");
	while (std::getline(infile, line)) {
		std::stringstream row(line);
		std::string temp;
		row >> temp;
		size_t arrive = atoi(temp.c_str());
		row >> temp;
		size_t leave = atoi(temp.c_str());
		m_time_of_trans.push_back({ arrive,leave });
		m_num_trans++;
	}
	infile.close();

	infile.open("./gates_to_trans.csv");
	while (std::getline(infile, line)) {
		std::vector<size_t> temp_trans;
		std::stringstream row(line);
		std::string temp;
		while (row >> temp)
			temp_trans.push_back(atoi(temp.c_str()));
		m_feasi_trans_of_gate.push_back(temp_trans);
		m_num_gates++;
	}
	infile.close();
}

void AGAP_1::initialize_solution(solution & sol) {
	std::vector<list_trans_in_gate> list_of_gate(m_num_gates);
	std::vector<std::list<std::pair<size_t, tran*>>> nodes_of_trans(m_num_trans);
	for (size_t i = 0; i < m_num_gates; i++)
		for (size_t trans : m_feasi_trans_of_gate[i])
			nodes_of_trans[trans].push_back({ i,list_of_gate[i].push_back(trans) });

	std::vector<size_t> gates_rand_seque(m_num_gates);
	std::iota(gates_rand_seque.begin(), gates_rand_seque.end(), 0);
	std::random_shuffle(gates_rand_seque.begin(), gates_rand_seque.end());
	for (size_t gate_chosen : gates_rand_seque) {
		size_t earliset_next = 0;
		while (!list_of_gate[gate_chosen].empty()) {
			std::vector<size_t> feasible_trans;
			size_t earliest_feasible = 100000;
			for (tran* i = list_of_gate[gate_chosen].begin(); i != nullptr; i = i->m_next)
				if (m_time_of_trans[i->m_index].first >= earliset_next) {
					feasible_trans.push_back(i->m_index);
					if (m_time_of_trans[i->m_index].first < earliest_feasible)
						earliest_feasible = m_time_of_trans[i->m_index].first;
				}
			if (feasible_trans.empty())
				break;
			std::vector<size_t> candidates;
			for (tran* i = list_of_gate[gate_chosen].begin(); i != nullptr; i = i->m_next)
				if ((m_time_of_trans[i->m_index].first - earliest_feasible) < 6)
					candidates.push_back(i->m_index);
			size_t trans_chosen;
			if (candidates.size() == 1)
				trans_chosen = candidates[0];
			else {
				std::uniform_int_distribution<int> distribution(0, candidates.size() - 1);
				trans_chosen = candidates[distribution(m_random_generator)];
			}
			sol.m_trans_in_gate[gate_chosen].push_back(trans_chosen);
			sol.m_gate_of_tran[trans_chosen] = gate_chosen;
			for (auto& tran : nodes_of_trans[trans_chosen]) {
				list_of_gate[tran.first].erase(tran.second);
				tran.second = nullptr;
			}
			earliset_next = m_time_of_trans[trans_chosen].second + 9;
		}
	}
}

void AGAP_1::evaluate(solution & sol) {
	size_t num_gates_used(0);
	for (const auto& gate : sol.m_trans_in_gate)
		if(!gate.empty())
			num_gates_used++;
	sol.m_objs[0] = num_gates_used;
}

bool AGAP_1::same(const solution & s1, const solution & s2) {
	if (s1.m_objs[0] == s2.m_objs[0])
		return true;
	else 
		return false;
}

bool AGAP_1::dominate(const solution & s1, const solution & s2) {
	if (s1.m_objs[0] < s2.m_objs[0])
		return true;
	else
		return false;
}

void Alg_1::initialize() {
	// Initialize population
	for (auto& i : m_pop) {
		i = std::move(std::unique_ptr<solution>(new solution(AGAP_1_CAST->num_objs(), AGAP_1_CAST->num_trans(), AGAP_1_CAST->num_gates())));
		global::ms_global->m_problem->initialize_solution(*i);
		global::ms_global->m_problem->evaluate(*i);
		m_evaluations++;
	}
	// Find the best solution
	m_best.push_back(std::unique_ptr<solution>(new solution(*m_pop[0])));
	for (size_t i = 1; i < m_pop.size(); i++) {
		if (global::ms_global->m_problem->same(*m_pop[i],*m_best[0]))
			m_best.push_back(std::unique_ptr<solution>(new solution(*m_pop[i])));
		else if (global::ms_global->m_problem->dominate(*m_pop[i], *m_best[0])) {
			m_best.clear();
			m_best.push_back(std::unique_ptr<solution>(new solution(*m_pop[i])));
		}
	}
}

void Alg_1::evolve() {
	// Reinitialize population
	for (auto& i : m_pop) {
		i = std::move(std::unique_ptr<solution>(new solution(AGAP_1_CAST->num_objs(), AGAP_1_CAST->num_trans(), AGAP_1_CAST->num_gates())));
		global::ms_global->m_problem->initialize_solution(*i);
		global::ms_global->m_problem->evaluate(*i);
		m_evaluations++;
	}
	// Find the best solution
	for (size_t i = 0; i < m_pop.size(); i++) {
		if (global::ms_global->m_problem->same(*m_pop[i], *m_best[0]))
			m_best.push_back(std::unique_ptr<solution>(new solution(*m_pop[i])));
		else if (global::ms_global->m_problem->dominate(*m_pop[i], *m_best[0])) {
			m_best.clear();
			m_best.push_back(std::unique_ptr<solution>(new solution(*m_pop[i])));
		}
	}
	std::cout << "Evaluations:" << m_evaluations << std::endl;
}

void Alg_1::output_result() {
	std::ofstream outfile("./result_1.txt");
	outfile << "Total evaluations: " << m_evaluations << std::endl;
	outfile << "The minimum number of gates: " << m_best[0]->m_objs[0] << std::endl;
	outfile << std::endl;

	for (auto& best : m_best) {
		for (size_t j = 0; j < best->m_trans_in_gate.size(); j++) {
			outfile << "Gate" << j << ": ";
			for (size_t tra : best->m_trans_in_gate[j])
				outfile << "tra" << tra << " ";
			outfile << std::endl;
		}
		outfile << std::endl;
	}
}