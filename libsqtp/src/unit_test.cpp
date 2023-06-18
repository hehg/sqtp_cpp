#include "unit_test.h"

unit_test*unit_test::instance() {
	static unit_test s;
	return &s;
}