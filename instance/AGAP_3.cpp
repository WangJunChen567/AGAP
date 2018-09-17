#include "AGAP_3.h"
#include <fstream>
#include <string>
#include <sstream>

void AGAP_3::initialize() {
	
	AGAP_2::initialize();

	std::ifstream infile;
	std::string line;

	infile.open("data/gate_info.csv");
	std::getline(infile, line);
	std::stringstream row(line);
	std::string temp;
	while (row >> temp)
		m_gate_info.push_back(atoi(temp.c_str()));
	infile.close();

	infile.open("data/walk_time.csv");
	while (std::getline(infile, line)) {
		std::vector<size_t> temp_row;
		std::stringstream row(line);
		std::string temp;
		while (row >> temp)
			temp_row.push_back(atoi(temp.c_str()));
		m_walk_time.push_back(temp_row);
	}
	infile.close();

	m_metro_times.resize(2);
	for (auto& arr_type : m_metro_times) {
		arr_type.resize(2);
		for (auto& lea_type : arr_type) {
			lea_type.resize(2);
			for (auto& arr_ter : lea_type)
				arr_ter.resize(2);
		}
	}
	infile.open("data/metro_times.csv");
	size_t i = 0;
	while (std::getline(infile, line)) {
		std::stringstream row(line);
		std::string temp;
		size_t j = 0;
		while (row >> temp) {
			m_metro_times[i / 2][j / 2][i % 2][j % 2] = atoi(temp.c_str());
			j++;
		}
		i++;
	}
	infile.close();
}

void AGAP_3::evaluate(solution & sol) {
	AGAP_1::evaluate(sol);
	double total_transfer_tension = 0;
	for (auto& pass : m_pass_info) {
		double connection_time = m_time_of_trans[pass[2]].second - m_time_of_trans[pass[1]].first;
		double transfer_time = 0;
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
		transfer_time += m_proce_time[pass[3]][pass[4]][arr_ter][lea_ter];
		transfer_time += 8./5 * m_metro_times[pass[3]][pass[4]][arr_ter][lea_ter];
		transfer_time += m_walk_time[m_gate_info[sol.m_gate_of_tran[pass[1]]]][m_gate_info[sol.m_gate_of_tran[pass[2]]]];
		total_transfer_tension += pass[0] * transfer_time / connection_time;
	}
	sol.m_objs[1] = total_transfer_tension;
}

void Alg_3::output_result() {
	std::ofstream outfile("result/result_3.txt");
	outfile << "Total evaluations: " << m_evaluations << std::endl;
	outfile << "The best one's objectives:\t" << std::endl;
	outfile << "\t" << "total transfer tension: " << m_best[0]->m_objs[1] << std::endl;
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
