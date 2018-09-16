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

	infile.open("./TRAs_arrive_leave.csv");
	while (std::getline(infile, line)) {
		std::stringstream row(line);
		std::string temp;
		row >> temp;
		size_t arrive = atoi(temp.c_str());
		row >> temp;
		size_t leave = atoi(temp.c_str());
		m_TRAs_arrive_leave.push_back({ arrive,leave });
		m_num_TRAs++;
	}
	infile.close();

	infile.open("./PORs_to_TRAs.csv");
	while (std::getline(infile, line)) {
		std::vector<size_t> temp_TRAs;
		std::stringstream row(line);
		std::string temp;
		while (row >> temp)
			temp_TRAs.push_back(atoi(temp.c_str()));
		m_PORs_to_TRAs.push_back(temp_TRAs);
		m_num_PORs++;
	}
	infile.close();
}

void AGAP_1::initialize_solution(solution & sol) {
	std::vector<POR_to_TRAs> PORs_list(m_num_PORs);
	std::vector<std::list<std::pair<size_t, TRA*>>> TRAs_in_PORs_list(m_num_TRAs);
	for (size_t i = 0; i < m_num_PORs; i++)
		for (size_t TRA : m_PORs_to_TRAs[i])
			TRAs_in_PORs_list[TRA].push_back({ i,PORs_list[i].push_back(TRA) });

	std::vector<size_t> sequence(m_num_PORs);
	std::iota(sequence.begin(), sequence.end(), 0);
	std::random_shuffle(sequence.begin(), sequence.end());
	for (size_t cur_POR : sequence) {
		size_t earliset_next = 0;
		while (!PORs_list[cur_POR].empty()) {
			std::vector<size_t> feasible_TRAs;
			size_t earliest_feasible = 100000;
			for (TRA* i = PORs_list[cur_POR].begin(); i != nullptr; i = i->m_next)
				if (m_TRAs_arrive_leave[i->m_index].first >= earliset_next) {
					feasible_TRAs.push_back(i->m_index);
					if (m_TRAs_arrive_leave[i->m_index].first < earliest_feasible)
						earliest_feasible = m_TRAs_arrive_leave[i->m_index].first;
				}
			if (feasible_TRAs.empty())
				break;
			std::vector<size_t> candidates;
			for (TRA* i = PORs_list[cur_POR].begin(); i != nullptr; i = i->m_next)
				if ((m_TRAs_arrive_leave[i->m_index].first - earliest_feasible) < 4)
					candidates.push_back(i->m_index);
			size_t choose;
			if (candidates.size() == 1)
				choose = candidates[0];
			else {
				std::uniform_int_distribution<int> distribution(0, candidates.size() - 1);
				choose = candidates[distribution(m_random_generator)];
			}
			sol.m_pi[choose][cur_POR] = true;
			for (auto& TRA : TRAs_in_PORs_list[choose]) {
				PORs_list[TRA.first].erase(TRA.second);
				TRA.second = nullptr;
			}
			earliset_next = m_TRAs_arrive_leave[choose].second + 9;
		}
	}
}

void AGAP_1::evaluate(solution & sol) {
	size_t num_PORs_used(0);
	for (size_t j = 0; j < m_num_PORs; j++) {
		bool used(false);
		for (size_t i = 0; i < m_num_TRAs; i++) {
			if (sol.m_pi[i][j]) {
				used = true;
				break;
			}
		}
		if (used)
			num_PORs_used++;
	}
	sol.m_obj[0] = num_PORs_used;
}

void Alg_1::initialize() {
	// Initialize population
	for (auto& i : m_pop) {
		i = std::move(std::unique_ptr<solution>(new solution(AGAP_1_CAST->num_TRAs(), AGAP_1_CAST->num_PORs())));
		global::ms_global->m_problem->initialize_solution(*i);
		global::ms_global->m_problem->evaluate(*i);
		m_evaluations++;
	}
	// Find the best solution
	m_best.push_back(std::unique_ptr<solution>(new solution(*m_pop[0])));
	for (size_t i = 1; i < m_pop.size(); i++) {
		if (m_pop[i]->m_obj == m_best[0]->m_obj)
			m_best.push_back(std::unique_ptr<solution>(new solution(*m_pop[i])));
		else if (m_pop[i]->m_obj < m_best[0]->m_obj) {
			m_best.clear();
			m_best.push_back(std::unique_ptr<solution>(new solution(*m_pop[i])));
		}
	}
}

void Alg_1::evolve() {
	// Reinitialize population
	for (auto& i : m_pop) {
		i = std::move(std::unique_ptr<solution>(new solution(AGAP_1_CAST->num_TRAs(), AGAP_1_CAST->num_PORs())));
		global::ms_global->m_problem->initialize_solution(*i);
		global::ms_global->m_problem->evaluate(*i);
		m_evaluations++;
	}
	// Find the best solution
	for (size_t i = 0; i < m_pop.size(); i++) {
		if (m_pop[i]->m_obj == m_best[0]->m_obj)
			m_best.push_back(std::unique_ptr<solution>(new solution(*m_pop[i])));
		else if (m_pop[i]->m_obj < m_best[0]->m_obj) {
			m_best.clear();
			m_best.push_back(std::unique_ptr<solution>(new solution(*m_pop[i])));
		}
	}
	std::cout << "Evaluations:" << m_evaluations << "\t" << "Number of gates: " << m_best[0]->m_obj[0] << std::endl;
}

void Alg_1::output_result() {
	std::ofstream outfile("./result.txt");
	outfile << "Total evaluations: " << m_evaluations << std::endl;
	outfile << "The minimum number of gates: " << m_best[0]->m_obj[0] << std::endl;
	outfile << std::endl;

	for (auto& best : m_best) {
		for (size_t j = 0; j < AGAP_1_CAST->num_PORs(); j++) {
			outfile << "Port" << j << ": ";
			for (size_t i = 0; i < AGAP_1_CAST->num_TRAs(); i++) {
				if (best->m_pi[i][j])
					outfile << "tra" << i << " ";
			}
			outfile << std::endl;
		}
		outfile << std::endl;
	}

}