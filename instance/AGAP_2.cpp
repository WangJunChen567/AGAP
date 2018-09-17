#include "AGAP_2.h"
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
	infile.open("data/proce_time.csv");
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

	infile.open("data/pass_info.csv");
	while (std::getline(infile, line)) {
		std::vector<int> temp_row;
		std::stringstream row(line);
		std::string temp;
		while (row >> temp)
			temp_row.push_back(atoi(temp.c_str()));
		m_pass_info.push_back(temp_row);
	}
	infile.close();
}

void AGAP_2::evaluate(solution & sol) {
	AGAP_1::evaluate(sol);
	size_t total_proce_time = 0;
	for (auto& pass : m_pass_info) {
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
		size_t arr_ter = sol.m_gate_of_tran[pass[1]] < 28 ? 0 : 1;
		size_t lea_ter = sol.m_gate_of_tran[pass[2]] < 28 ? 0 : 1;
		total_proce_time += pass[0] * m_proce_time[pass[3]][pass[4]][arr_ter][lea_ter];
	}
	sol.m_objs[1] = total_proce_time;
}

bool AGAP_2::same(const solution & s1, const solution & s2) {
	if (s1.m_objs[1] == s2.m_objs[1] && s1.m_objs[0] == s2.m_objs[0])
		return true;
	else
		return false;
}

bool AGAP_2::dominate(const solution & s1, const solution & s2) {
	if (s1.m_objs[1] < s2.m_objs[1])
		return true;
	else if (s1.m_objs[1] == s2.m_objs[1] && s1.m_objs[0] < s2.m_objs[0])
		return true;
	else
		return false;
}

void Alg_2::output_result() {
	std::ofstream outfile("result/result_2.txt");
	outfile << "Total evaluations: " << m_evaluations << std::endl;
	outfile << "The best one's objectives:\t" << std::endl;
	outfile << "\t" << "total process time: " << m_best[0]->m_objs[1] << std::endl;
	outfile << "\t" << "the number of gates: " << m_best[0]->m_objs[0] << std::endl;
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
