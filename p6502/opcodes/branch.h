
#ifndef BRANCH_20121206_H_
#define BRANCH_20121206_H_

struct branch {
	typedef operation_branch memory_access;

protected:
	// branch conditions
	struct carry_clear    { bool operator()() const { return (P & C_MASK) == 0; } };
	struct carry_set      { bool operator()() const { return (P & C_MASK) != 0; } };
	struct zero_clear     { bool operator()() const { return (P & Z_MASK) == 0; } };
	struct zero_set       { bool operator()() const { return (P & Z_MASK) != 0; } };
	struct negative_clear { bool operator()() const { return (P & N_MASK) == 0; } };
	struct negative_set   { bool operator()() const { return (P & N_MASK) != 0; } };
	struct overflow_clear { bool operator()() const { return (P & V_MASK) == 0; } };
	struct overflow_set   { bool operator()() const { return (P & V_MASK) != 0; } };

protected:
	template <class Condition>
	bool do_branch(Condition condition) const {
		return condition();
	}
};

#endif

