// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>

#include "test_helper.h"

#include "define_enum_2_test.h"
#include "define_struct_test.h"
#include "mu_eat_empty_args_test.h"

int main(void)
{
    int result;
    result = run_define_enum_2_test();
    POOR_MANS_ASSERT(result == 0);
    
    result = run_mu_eat_empty_args_test();
    POOR_MANS_ASSERT(result == 0);

    result = run_define_struct_tests();
    POOR_MANS_ASSERT(result == 0);

    return 0;
}

