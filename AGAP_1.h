#include "AGAP.h"

struct TRA {
	TRA(size_t index) : m_index(index) {}
	const size_t m_index;
	TRA* m_next = nullptr;
	TRA* m_last = nullptr;
};

class POR_to_TRAs {
private:
	TRA * m_begin = nullptr;
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

#define AGAP_1_CAST dynamic_cast<AGAP_1*>(global::ms_global->m_problem.get())

class AGAP_1 : public problem {
public:
	AGAP_1() = default;
	void initialize();
	void initialize_solution(solution& sol);
	void evaluate(solution& sol);
	size_t num_TRAs() { return m_num_TRAs; }
	size_t num_PORs() { return m_num_PORs; }
protected:
	std::vector<std::pair<size_t, size_t>> m_TRAs_arrive_leave;
	std::vector<std::vector<size_t>> m_PORs_to_TRAs;
	size_t m_num_TRAs = 0, m_num_PORs = 0;
	std::default_random_engine m_random_generator;
};

class Alg_1 : public algorithm {
public:
	Alg_1(size_t size_pop, size_t max_evaluations) : algorithm(size_pop, max_evaluations) {};
	void initialize();
	void evolve();
	void output_result();
};
