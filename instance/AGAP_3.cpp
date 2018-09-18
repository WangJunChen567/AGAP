#include "AGAP_3.h"
#include "../core/global.h"
#include <fstream>
#include <string>
#include <sstream>

void AGAP_3::initialize() {
	
	AGAP_2::initialize();

	std::ifstream infile;
	std::string line;

	infile.open("data/area_type_of_each_gate.csv");
	std::getline(infile, line);
	std::stringstream row(line);
	std::string temp;
	while (row >> temp)
		m_gate_info.push_back(atoi(temp.c_str()));
	infile.close();

	infile.open("data/walk_time_by_gate_area_type.csv");
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
	infile.open("data/metro_times_by_puck_and_gate.csv");
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
	//size_t num_assigned_pucks(0);
	//size_t num_used_gates(0);
	//for (size_t gate_of_puck : sol.m_gate_of_tran)
	//	if (gate_of_puck != -1)
	//		num_assigned_pucks++;
	//for (auto& pucks_in_gate : sol.m_pucks_in_gate)
	//	if (!pucks_in_gate.empty())
	//		num_used_gates++;
	double mean_transfer_tension = 0;
	size_t num_succes_pass = 0;
	for (auto& pass : m_ticket_info) {
		if (sol.m_gate_of_puck[pass[1]] == -1 || sol.m_gate_of_puck[pass[2]] == -1)
			continue;
		double connection_time = m_time_of_puck[pass[2]].second - m_time_of_puck[pass[1]].first;
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
		size_t arr_ter = sol.m_gate_of_puck[pass[1]] < 28 ? 0 : 1;
		size_t lea_ter = sol.m_gate_of_puck[pass[2]] < 28 ? 0 : 1;
		transfer_time += m_proce_time[pass[3]][pass[4]][arr_ter][lea_ter];
		transfer_time += 8. / 5 * m_metro_times[pass[3]][pass[4]][arr_ter][lea_ter];
		transfer_time += m_walk_time[m_gate_info[sol.m_gate_of_puck[pass[1]]]][m_gate_info[sol.m_gate_of_puck[pass[2]]]];
		if (transfer_time <= connection_time) {
			mean_transfer_tension += pass[0] * transfer_time / connection_time;
			num_succes_pass += pass[0];
		}
	}
	mean_transfer_tension /= num_succes_pass;
	sol.m_objs[0] = num_succes_pass;
	sol.m_objs[1] = mean_transfer_tension;
	//sol.m_objs[2] = num_used_gates;
}

void AGAP_3::record_pass_info(const solution & sol, std::vector<size_t>& nums_passengers, std::vector<double>& transfer_times, std::vector<double>& transfer_tensions) {
	nums_passengers.clear();
	transfer_times.clear();
	transfer_tensions.clear();
	for (auto& ticket : m_ticket_info) {
		nums_passengers.push_back(ticket[0]);
		if (sol.m_gate_of_puck[ticket[1]] == -1 || sol.m_gate_of_puck[ticket[2]] == -1) {
			transfer_times.push_back(-1);
			transfer_tensions.push_back(-1);
			continue;
		}
		double connection_time = m_time_of_puck[ticket[2]].second - m_time_of_puck[ticket[1]].first;
		double transfer_time = 0;
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
		transfer_time += m_proce_time[ticket[3]][ticket[4]][arr_ter][lea_ter];
		transfer_time += 8. / 5 * m_metro_times[ticket[3]][ticket[4]][arr_ter][lea_ter];
		transfer_time += m_walk_time[m_gate_info[sol.m_gate_of_puck[ticket[1]]]][m_gate_info[sol.m_gate_of_puck[ticket[2]]]];
		if (transfer_time <= connection_time) {
			transfer_times.push_back(transfer_time);
			transfer_tensions.push_back(transfer_time / connection_time);
		}
		else {
			transfer_times.push_back(-1);
			transfer_tensions.push_back(-1);
		}
	}
}

void Alg_3::output_result() {
	std::stringstream temp_outfile;

	//temp_outfile << "obj1,obj2" << std::endl;
	//for (auto iter = m_best.begin(); iter != m_best.end(); ++iter) {
	//	temp_outfile << iter->m_objs[0] << "," << iter->m_objs[1] << std::endl;
	//}

	size_t choose_one = 0;
	auto iter = m_best.begin();
	for (size_t i = 0; i < choose_one; i++)
		iter++;
	temp_outfile << "the number of transferred passengers: " << (*iter)->m_objs[0] << std::endl;
	temp_outfile << "the average transfer tension of transferred passengers: " << (*iter)->m_objs[1] << std::endl;
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

	temp_outfile << "The transfer time and transfer tension of each passenger: " << std::endl;
	std::vector<size_t> nums_passengers;
	std::vector<double> transfer_times, transfer_tensions;
	AGAP_3_CAST->record_pass_info(**iter, nums_passengers, transfer_times, transfer_tensions);
	for (size_t i = 0; i < nums_passengers.size(); i++)	{
		for (size_t j = 0; j < nums_passengers[i]; j++)	{
			temp_outfile << transfer_times[i] << "\t" << transfer_tensions[i] << std::endl;
		}
	}

	std::ofstream outfile("result/result_3.txt");
	outfile << temp_outfile.str();
}
