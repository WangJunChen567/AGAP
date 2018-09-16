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
	std::vector<size_t> m_obj;
};

class problem {
public:
	problem() = default;
	virtual void initialize() = 0;
	virtual void initialize_solution(solution& sol) = 0;
	virtual void evaluate(solution& sol) = 0;
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
