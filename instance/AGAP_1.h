#include "../core/AGAP.h"
#include <random>

struct tran {
	tran(size_t index) : m_index(index) {}
	const size_t m_index;
	tran* m_next = nullptr;
	tran* m_last = nullptr;
};

class list_trans_in_gate {
private:
	tran * m_begin = nullptr;
	tran *m_end = nullptr;
public:
	list_trans_in_gate() {}

	~list_trans_in_gate() {
		while (m_end != nullptr)
			erase(m_end);
	}
	tran* push_back(size_t index) {
		tran* new_node(new tran(index));
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
	void erase(tran* node) {
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
	tran* begin() { return m_begin; }
	tran* end() { return m_end; }
	const tran* begin() const { return m_begin; }
	const tran* end() const { return m_end; }
	bool empty() { return m_begin == nullptr; }
};

#define AGAP_1_CAST dynamic_cast<AGAP_1*>(global::ms_global->m_problem.get())

class AGAP_1 : public problem {
public:
	AGAP_1(size_t num_objs = 1) : problem(num_objs) {}
	void initialize();
	void initialize_solution(solution& sol);
	void evaluate(solution& sol);
	bool same(const solution& s1, const solution& s2);
	bool dominate(const solution& s1, const solution& s2);
	size_t num_trans() { return m_num_trans; }
	size_t num_gates() { return m_num_gates; }
protected:
	std::vector<std::pair<size_t, size_t>> m_time_of_trans;
	std::vector<std::vector<size_t>> m_feasi_trans_of_gate;
	size_t m_num_trans = 0, m_num_gates = 0;
	std::default_random_engine m_random_generator;
};

class Alg_1 : public algorithm {
public:
	Alg_1(size_t size_pop, size_t max_evaluations) : algorithm(size_pop, max_evaluations) {};
	void initialize();
	void evolve();
	void output_result();
};
