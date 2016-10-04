#include "test-runner_Main.h"

BEGIN_TEST_RUNNER_NAMESPACE

bool run()
{
    return false;
}

END_TEST_RUNNER_NAMESPACE

int main()
{
    return TEST_RUNNER_NAMESPACE::run() ? 0 : 1;
}
