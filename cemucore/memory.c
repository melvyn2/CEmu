/*
 * Copyright (c) 2015-2021 CE Programming.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "memory.h"

#include <stdlib.h>

bool memory_init(memory_t *memory)
{
    if (memory && (memory->flash = calloc(0x400000, sizeof(uint8_t))))
    {
        if ((memory->ram = calloc(0x65800, sizeof(uint8_t))))
        {
#ifdef CEMUCORE_NODEBUG
            return true;
#else
            if ((memory->debug = calloc(0x1000000, sizeof(uint8_t))))
            {
                return true;
            }
            free(memory->ram);
#endif
        }
        free(memory->flash);
    }
    return false;
}

void memory_destroy(memory_t *memory)
{
#ifndef CEMUCORE_NODEBUG
    free(memory->debug);
#endif
    free(memory->ram);
    free(memory->flash);
}
