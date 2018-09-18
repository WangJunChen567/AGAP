#ifndef AGAP_H
#define AGAP_H

#include <vector>
#include <memory>

struct solution {
	solution(size_t num_objs, size_t num_pucks, size_t num_gates) : m_objs(num_objs), m_gate_of_puck(num_pucks, -1), m_pucks_in_gate(num_gates), m_delays(num_pucks, 0) {}
	std::vector<std::vector<size_t>> m_pucks_in_gate;
	std::vector<int> m_gate_of_puck;
	std::vector<size_t> m_delays;
	std::vector<double> m_objs;
};

class problem {
public:
	problem(size_t num_objs) : m_num_objs(num_objs) {}
	virtual void initialize() = 0;
	virtual void initialize_solution(solution& sol) = 0;
	virtual void evaluate(solution& sol) = 0;
	virtual bool same(const solution& s1, const solution& s2) = 0;
	virtual bool dominate(const solution& s1, const solution& s2) = 0;
	size_t num_objs() { return m_num_objs; }
protected:
	size_t m_num_objs;
};

class algorithm {
public:
	algorithm(size_t size_pop, size_t max_evaluations) : m_pop(size_pop), m_max_evaluations(max_evaluations) {};
	virtual void initialize() = 0;
	void run();
	virtual void evolve() = 0;
	virtual void output_result() = 0;
private:
	bool terminating();
protected:
	std::vector<std::unique_ptr<solution>> m_pop;
	std::vector<std::unique_ptr<solution>> m_best;
	size_t m_evaluations = 0;
	size_t m_max_evaluations;
};

#endif // !AGAP_H
