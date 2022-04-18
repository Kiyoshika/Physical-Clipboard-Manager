#ifndef FLASH_MEMORY_H
#define FLASH_MEMORY_H

#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "main.h"

/**
 * Clear N bytes of data starting at a specified address.
 */
void clear_flash(uint32_t beginning_address, uint32_t ending_address);

/**
 * Write character buffer to flash starting at a specified address.
 */
void write_string_to_flash(const char* buffer, uint32_t address);

void write_metadata_to_flash(uint32_t metadata, uint32_t address);

void read_string_from_flash(char buffer[255], uint32_t metadata_address, uint32_t buffer_address);
#endif
