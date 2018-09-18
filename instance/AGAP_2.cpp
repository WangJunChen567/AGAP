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
	//size_t num_used_gates(0);
	//for (size_t gate_of_puck : sol.m_gate_of_tran)
	//	if (gate_of_puck != -1)
	//		num_assigned_pucks++;
	//for (auto& pucks_in_gate : sol.m_pucks_in_gate)
	//	if (!pucks_in_gate.empty())
	//		num_used_gates++;
	double mean_proce_time = 0;
	size_t num_succes_pass = 0;
	for (auto& ticket : m_ticket_info) {
		if (sol.m_gate_of_puck[ticket[1]] == -1 || sol.m_gate_of_puck[ticket[2]] == -1)
			continue;
		if (ticket[1] == -1 && ticket[2] == -1)
			throw("error @ AGAP_2::evaluate");
		else if (ticket[1] == -1) {
			ticket[1] = ticket[2];
			ticket[3] = ticket[4];
		}
		else if (ticket[2] == -1) {
			ticket[2] = ticket[1];
			ticket[4] = ticket[3];
		}
		size_t arr_ter = sol.m_gate_of_puck[ticket[1]] < 28 ? 0 : 1;
		size_t lea_ter = sol.m_gate_of_puck[ticket[2]] < 28 ? 0 : 1;
		mean_proce_time += ticket[0] * m_proce_time[ticket[3]][ticket[4]][arr_ter][lea_ter];
		num_succes_pass += ticket[0];
	}
	mean_proce_time /= num_succes_pass;
	sol.m_objs[0] = num_succes_pass;
	sol.m_objs[1] = mean_proce_time;
	//sol.m_objs[2] = num_used_gates;
}

void AGAP_2::record_pass_info(const solution & sol, std::vector<size_t>& nums_passengers, std::vector<int>& process_times) {
	nums_passengers.clear();
	process_times.clear();
	for (auto& ticket : m_ticket_info) {
		nums_passengers.push_back(ticket[0]);
		if (sol.m_gate_of_puck[ticket[1]] == -1 || sol.m_gate_of_puck[ticket[2]] == -1) {
			process_times.push_back(-1);
			continue;
		}
		if (ticket[1] == -1 && ticket[2] == -1)
			throw("error @ AGAP_2::evaluate");
		else if (ticket[1] == -1) {
			ticket[1] = ticket[2];
			ticket[3] = ticket[4];
		}
		else if (ticket[2] == -1) {
			ticket[2] = ticket[1];
			ticket[4] = ticket[3];
		}
		size_t arr_ter = sol.m_gate_of_puck[ticket[1]] < 28 ? 0 : 1;
		size_t lea_ter = sol.m_gate_of_puck[ticket[2]] < 28 ? 0 : 1;
		process_times.push_back(m_proce_time[ticket[3]][ticket[4]][arr_ter][lea_ter]);
	}
}

bool AGAP_2::same(const solution & s1, const solution & s2) {
	if (s1.m_objs[0] == s2.m_objs[0] && s1.m_objs[1] == s2.m_objs[1])
		return true;
	else if (!AGAP_2::dominate(s1, s2) && !AGAP_2::dominate(s2, s1))
		return true;
	else
		return false;
}

bool AGAP_2::dominate(const solution & s1, const solution & s2) {
	if (s1.m_objs[0] > s2.m_objs[0] && s1.m_objs[1] < s2.m_objs[1])
		return true;
	else if (s1.m_objs[0] == s2.m_objs[0] && s1.m_objs[1] < s2.m_objs[1])
		return true;
	else if (s1.m_objs[0] > s2.m_objs[0] && s1.m_objs[1] == s2.m_objs[1])
		return true;
	else
		return false;
}

void Alg_2::output_result() {
	std::stringstream temp_outfile;

	//temp_outfile << "obj1,obj2" << std::endl;
	//for (auto iter = m_best.begin(); iter != m_best.end(); ++iter) {
	//	temp_outfile << iter->m_objs[0] << "," << iter->m_objs[1] << std::endl;
	//}

	size_t choose_one = 5;
	auto iter = m_best.begin();
	for (size_t i = 0; i < choose_one; i++)
		iter++;
	temp_outfile << "the number of transferred passengers: " << (*iter)->m_objs[0] << std::endl;
	temp_outfile << "the average process time: " << (*iter)->m_objs[1] << std::endl;
	temp_outfile << std::endl;

	std::vector<double> usage_rates;
	AGAP_1_CAST->record_gate_info(**iter, usage_rates);
	temp_outfile << "Pucks in each gate: " << std::endl;
	for (size_t j = 0; j < (*iter)->m_pucks_in_gate.size(); j++) {
		temp_outfile << "Gate" << j << "(" << std::fixed << std::setprecision(4) << usage_rates[j] << "): ";
		for (size_t tra : (*iter)->m_pucks_in_gate[j])
			temp_outfile << "tra" << tra << " ";
		temp_outfile << std::endl;
	}
	temp_outfile << std::endl;

	temp_outfile << "The process time of each passenger:" << std::endl;
	std::vector<size_t> nums_passengers;
	std::vector<int> process_times;
	AGAP_2_CAST->record_pass_info(**iter, nums_passengers, process_times);
	for (size_t i = 0; i < nums_passengers.size(); i++) {
		for (size_t j = 0; j < nums_passengers[i]; j++) {
			temp_outfile << process_times[i] << std::endl;
		}
	}

	std::ofstream outfile("result/result_2.txt");
	outfile << temp_outfile.str();
}
