#include "AGAP_2.h"
#include "../core/global.h"
#include <fstream>
#include <string>
#include <sstream>

void AGAP_2::initialize() {

	AGAP_1::initialize();

	std::ifstream infile;
	std::string line;

	m_proce_time.resize(2);
	for (auto& arr_type : m_proce_time) {
		arr_type.resize(2);
		for (auto& lea_type : arr_type) {
			lea_type.resize(2);
			for (auto& arr_ter : lea_type)
				arr_ter.resize(2);
		}
	}
	infile.open("data/proce_time_by_puck_and_gate.csv");
	size_t i = 0;
	while (std::getline(infile, line)) {
		std::stringstream row(line);
		std::string temp;
		size_t j = 0;
		while (row >> temp) {
			m_proce_time[i / 2][j / 2][i % 2][j % 2] = atoi(temp.c_str());
			j++;
		}
		i++;
	}
	infile.close();

	infile.open("data/ticket_info.csv");
	while (std::getline(infile, line)) {
		std::vector<int> temp_row;
		std::stringstream row(line);
		std::string temp;
		while (row >> temp)
			temp_row.push_back(atoi(temp.c_str()));
		m_ticket_info.push_back(temp_row);
	}
	infile.close();
}

void AGAP_2::evaluate(solution & sol) {
	//size_t num_assigned_pucks(0);
	size_t num_used_gates(0);
	//for (size_t gate_of_puck : sol.m_gate_of_tran)
	//	if (gate_of_puck != -1)
	//		num_assigned_pucks++;
	for (auto& pucks_in_gate : sol.m_pucks_in_gate)
		if (!pucks_in_gate.empty())
			num_used_gates++;
	double mean_proce_time = 0;
	size_t num_succes_pass = 0;
	for (auto& pass : m_ticket_info) {
		if (sol.m_gate_of_puck[pass[1]] == -1 || sol.m_gate_of_puck[pass[2]] == -1)
			continue;
		if (pass[1] == -1 && pass[2] == -1)
			throw("error @ AGAP_2::evaluate");
		else if (pass[1] == -1) {
			pass[1] = pass[2];
			pass[3] = pass[4];
		}
		else if (pass[2] == -1) {
			pass[2] = pass[1];
			pass[4] = pass[3];
		}
		size_t arr_ter = sol.m_gate_of_puck[pass[1]] < 28 ? 0 : 1;
		size_t lea_ter = sol.m_gate_of_puck[pass[2]] < 28 ? 0 : 1;
		mean_proce_time += pass[0] * m_proce_time[pass[3]][pass[4]][arr_ter][lea_ter];
		num_succes_pass += pass[0];
	}
	mean_proce_time /= num_succes_pass;
	sol.m_objs[0] = num_succes_pass;
	sol.m_objs[1] = mean_proce_time;
	sol.m_objs[2] = num_used_gates;
}

bool AGAP_2::same(const solution & s1, const solution & s2) {
	if (AGAP_1::same(s1, s2) && s1.m_objs[2] == s2.m_objs[2])
		return true;
	else
		return false;
}

bool AGAP_2::dominate(const solution & s1, const solution & s2) {
	if (s1.m_objs[0] > s2.m_objs[0])
		return true;
	else if (s1.m_objs[0] == s2.m_objs[0] && s1.m_objs[1] < s2.m_objs[1])
		return true;
	else if (s1.m_objs[0] == s2.m_objs[0] && s1.m_objs[1] == s2.m_objs[1] && s1.m_objs[2] < s2.m_objs[2])
		return true;
	else
		return false;
}

void Alg_2::output_result() {
	std::stringstream temp_outfile;

	temp_outfile << "The total number of evaluations: " << m_evaluations << std::endl;
	temp_outfile << "The best one's objectives:\t" << std::endl;
	temp_outfile << "\t" << "the number of transferred passengers: " << m_best[0]->m_objs[0] << std::endl;
	temp_outfile << "\t" << "the average process time: " << m_best[0]->m_objs[1] << std::endl;
	temp_outfile << "\t" << "the number of used gates: " << m_best[0]->m_objs[2] << std::endl;
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

	std::ofstream outfile("result/result_2.txt");
	outfile << temp_outfile.str();
}
