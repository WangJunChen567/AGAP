#include "AGAP_1.h"
#include "../core/global.h"
#include <list>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <numeric>
#include <algorithm>

void AGAP_1::initialize() {
	std::ifstream infile;
	std::string line;

	infile.open("data/arrive_and_leave_time_of_each_puck.csv");
	while (std::getline(infile, line)) {
		std::stringstream row(line);
		std::string temp;
		row >> temp;
		size_t arrive = atoi(temp.c_str());
		row >> temp;
		size_t leave = atoi(temp.c_str());
		m_time_of_puck.push_back({ arrive,leave });
		m_num_pucks++;
	}
	infile.close();

	infile.open("data/feasible_pucks_of_each_gate.csv");
	while (std::getline(infile, line)) {
		std::vector<size_t> temp_pucks;
		std::stringstream row(line);
		std::string temp;
		while (row >> temp)
			temp_pucks.push_back(atoi(temp.c_str()));
		m_feasi_pucks_of_gate.push_back(temp_pucks);
		m_num_gates++;
	}
	infile.close();
}

void AGAP_1::initialize_solution(solution & sol) {
	std::vector<list_pucks_in_gate> list_of_gate(m_num_gates);
	std::vector<std::list<std::pair<size_t, puck*>>> nodes_of_pucks(m_num_pucks);
	for (size_t i = 0; i < m_num_gates; i++)
		for (size_t pucks : m_feasi_pucks_of_gate[i])
			nodes_of_pucks[pucks].push_back({ i,list_of_gate[i].push_back(pucks) });

	std::vector<size_t> gates_rand_seque(m_num_gates);
	std::iota(gates_rand_seque.begin(), gates_rand_seque.end(), 0);
	std::random_shuffle(gates_rand_seque.begin(), gates_rand_seque.end());
	for (size_t gate_chosen : gates_rand_seque) {
		size_t earliset_next = 0;
		while (!list_of_gate[gate_chosen].empty()) {
			std::vector<size_t> feasible_pucks;
			size_t earliest_feasible = 100000;
			for (puck* i = list_of_gate[gate_chosen].begin(); i != nullptr; i = i->m_next)
				if (m_time_of_puck[i->m_index].first >= earliset_next) {
					feasible_pucks.push_back(i->m_index);
					if (m_time_of_puck[i->m_index].first < earliest_feasible)
						earliest_feasible = m_time_of_puck[i->m_index].first;
				}
			if (feasible_pucks.empty())
				break;
			std::vector<size_t> candidates;
			for (puck* i = list_of_gate[gate_chosen].begin(); i != nullptr; i = i->m_next)
				if ((m_time_of_puck[i->m_index].first - earliest_feasible) < 6)
					candidates.push_back(i->m_index);
			size_t pucks_chosen;
			if (candidates.size() == 1)
				pucks_chosen = candidates[0];
			else {
				std::uniform_int_distribution<int> distribution(0, candidates.size() - 1);
				pucks_chosen = candidates[distribution(m_random_generator)];
			}
			sol.m_pucks_in_gate[gate_chosen].push_back(pucks_chosen);
			sol.m_gate_of_puck[pucks_chosen] = gate_chosen;
			for (auto& puck : nodes_of_pucks[pucks_chosen]) {
				list_of_gate[puck.first].erase(puck.second);
				puck.second = nullptr;
			}
			earliset_next = m_time_of_puck[pucks_chosen].second + 9;
		}
	}
}

void AGAP_1::evaluate(solution & sol) {
	size_t num_assigned_pucks(0);
	size_t num_used_gates(0);
	for (size_t gate_of_puck : sol.m_gate_of_puck) 
		if (gate_of_puck != -1)
			num_assigned_pucks++;
	for (auto& pucks_in_gate : sol.m_pucks_in_gate)
		if (!pucks_in_gate.empty())
			num_used_gates++;
	sol.m_objs[0] = num_assigned_pucks;
	sol.m_objs[1] = num_used_gates;
}

void AGAP_1::record_gate_info(const solution & sol, std::vector<double>& usage_rates) {
	usage_rates.clear();
	for (auto& pucks_in_gate : sol.m_pucks_in_gate) {
		double usage_rate = 0;
		for (size_t puck : pucks_in_gate) {
			size_t arrive, leave;
			if (m_time_of_puck[puck].first < 288)
				arrive = 288;
			else
				arrive = m_time_of_puck[puck].first;
			if (m_time_of_puck[puck].second > 576)
				leave = 576;
			else
				leave = m_time_of_puck[puck].second;
			usage_rate += leave - arrive;
		}
		usage_rate /= 288;
		usage_rates.push_back(usage_rate);
	}
}

bool AGAP_1::same(const solution & s1, const solution & s2) {
	if (s1.m_objs[0] == s2.m_objs[0] && s1.m_objs[1] == s2.m_objs[1])
		return true;
	else
		return false;
}

bool AGAP_1::dominate(const solution & s1, const solution & s2) {
	if (s1.m_objs[0] > s2.m_objs[0])
		return true;
	else if (s1.m_objs[0] == s2.m_objs[0] && s1.m_objs[1] < s2.m_objs[1])
		return true;
	else
		return false;
}

void Alg_1::initialize() {
	// Initialize population
	for (auto& i : m_pop) {
		i = std::move(std::unique_ptr<solution>(new solution(AGAP_1_CAST->num_objs(), AGAP_1_CAST->num_pucks(), AGAP_1_CAST->num_gates())));
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
		i = std::move(std::unique_ptr<solution>(new solution(AGAP_1_CAST->num_objs(), AGAP_1_CAST->num_pucks(), AGAP_1_CAST->num_gates())));
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
	std::stringstream temp_outfile;

	temp_outfile << "The total number of evaluations: " << m_evaluations << std::endl;
	temp_outfile << "The best one's objectives:\t" << std::endl;
	temp_outfile << "\t" << "the number of assigned pucks: " << m_best[0]->m_objs[0] << std::endl;
	temp_outfile << "\t" << "the number of used gates: " << m_best[0]->m_objs[1] << std::endl;
	temp_outfile << std::endl;

	for (size_t idx = 0; idx < m_best.size(); ++idx) {
		std::vector<double> usage_rates;
		AGAP_1_CAST->record_gate_info(*m_best[idx], usage_rates);
		temp_outfile << "The best solution " << idx + 1 << ": pucks in each gate: " << std::endl;
		for (size_t j = 0; j < m_best[idx]->m_pucks_in_gate.size(); j++) {
			temp_outfile << "Gate" << j << "(" << std::fixed << std::setprecision(4) << usage_rates[j] << "): ";
			for (size_t tra : m_best[idx]->m_pucks_in_gate[j])
				temp_outfile << "tra" << tra << " ";
			temp_outfile << std::endl;
		}
		temp_outfile << std::endl;
	}

	std::ofstream outfile("result/result_1.txt");
	outfile << temp_outfile.str();
}