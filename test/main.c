#include "config_test.h"
#include "preset_test.h"

int main(void) {
	return config_test_suites() || preset_test_suites();
}
