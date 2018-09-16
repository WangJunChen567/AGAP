#ifndef AGAP_H
#define AGAP_H

#include <vector>
#include <list>
#include <memory>
#include <random>

struct solution {
	solution(size_t num_TRAs, size_t num_PORs) : m_pi(std::vector<std::vector<bool>>(num_TRAs,std::vector<bool>(num_PORs,false))), m_delays(num_TRAs, 0) {}
	std::vector<std::vector<bool>> m_pi; // [i=TRA][j=POR]
	std::vector<size_t> m_delays;
	size_t m_obj;
};

struct TRA {
	TRA(size_t index) : m_index(index) {}
	const size_t m_index;
	TRA* m_next = nullptr;
	TRA* m_last = nullptr;
};

class POR_to_TRAs {
private:
	TRA *m_begin = nullptr;
	TRA *m_end = nullptr;
public:
	POR_to_TRAs() {}

	//POR_to_TRAs(const POR_to_TRAs& rsh) {
	//	for (const TRA* i = rsh.begin(); i != nullptr; i = i->m_next)
	//		push_back(i->m_index);
	//}

	~POR_to_TRAs() {
		while (m_end != nullptr)
			erase(m_end);
	}
	TRA* push_back(size_t index) {
		TRA* new_node(new TRA(index));
		if (m_begin != nullptr) {
			new_node->m_last = m_end;
			m_end->m_next = new_node;
			m_end = new_node;
		}
		else {
			m_begin = new_node;
			m_end = new_node;
		}
		return new_node;	
	}
	void erase(TRA* node) {
		if (node != m_begin && node != m_end) {
			node->m_last->m_next = node->m_next;
			node->m_next->m_last = node->m_last;
		}
		else if (node == m_begin && node != m_end) {
			node->m_next->m_last = nullptr;
			m_begin = node->m_next;
		}
		else if (node == m_end && node != m_begin) {
			node->m_last->m_next = nullptr;
			m_end = node->m_last;
		}
		else {
			m_begin = nullptr;
			m_end = nullptr;
		}
		delete node;
	}
	TRA* begin() { return m_begin; }
	TRA* end() { return m_end; }
	const TRA* begin() const { return m_begin; }
	const TRA* end() const { return m_end; }
	bool empty() { return m_begin == nullptr; }
};

class problem {
public:
	problem() = default;
	void initialize();
	void initialize_solution(solution& sol);
	void evaluate(solution& sol);
	size_t num_TRAs() { return m_num_TRAs; }
	size_t num_PORs() { return m_num_PORs; }
private:
	std::vector<std::pair<size_t, size_t>> m_TRAs_arrive_leave;
	std::vector<std::vector<size_t>> m_PORs_to_TRAs;
	//std::vector<POR_to_TRAs> m_PORs_list;
	//std::vector<std::list<std::pair<size_t,TRA*>>> m_TRAs_in_PORs_list;
	size_t m_num_TRAs = 0, m_num_PORs = 0;
	std::default_random_engine m_random_generator;
};

class algorithm {
public:
	algorithm(size_t size_pop, size_t max_evaluations) : m_pop(size_pop), m_max_evaluations(max_evaluations) {};
	void initialize();
	void run();
	void evolve();
	void output_result();
private:
	bool terminating();
private:
	std::vector<std::unique_ptr<solution>> m_pop;
	std::vector<std::unique_ptr<solution>> m_best;
	size_t m_evaluations = 0;
	size_t m_max_evaluations;
};

#endif // !AGAP_H
