#ifndef FLASH_MEMORY_H
#define FLASH_MEMORY_H

#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "main.h"

/**
 * Dump flash memory so we can write to it (again).
 * This needs the write address so we can copy the other addresses over after erasing and leave the write address empty as we are copying a new buffer into it.
 *
 * @param write_address The address we're writing into (copy all the others after erasing flash memory)
 */
void clear_flash(uint32_t write_address);

/**
 * Write character buffer to flash starting at a specified address.
 */
void write_string_to_flash(const char* buffer, uint32_t address);

void write_metadata_to_flash(uint32_t metadata, uint32_t address);

void read_string_from_flash(char buffer[256], size_t* buffer_len, uint32_t metadata_address, uint32_t buffer_address);
#endif
