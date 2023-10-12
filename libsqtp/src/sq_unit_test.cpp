#include "sq_unit_test.h"

sq_unit_test*sq_unit_test::instance() {
	static sq_unit_test s;
	return &s;
}