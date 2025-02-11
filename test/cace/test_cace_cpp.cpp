/*
 * Copyright (c) 2011-2024 The Johns Hopkins University Applied Physics
 * Laboratory LLC.
 *
 * This file is part of the Delay-Tolerant Networking Management
 * Architecture (DTNMA) Tools package.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** @file
 * This just verifies the ability for a C++11 executable to link with the
 * cace library.
 */

#include <cace/ari.h>
#include <cace/util/defs.h>
#include <iostream>

int main(int argc _U_, char *argv[] _U_)
{
    cace_ari_t value;
    cace_ari_init(&value);

    cace_ari_deinit(&value);
    return 0;
}
