#ifndef FLASH_MEMORY_H
#define FLASH_MEMORY_H

#include <stdint.h>
#include <math.h>
#include <string.h>
#include "main.h"

void write_string_to_flash(const char* buffer, uint32_t address)
{
    char write_buffer[4];
    size_t buffer_size = strlen(buffer);
    size_t write_chunks = (int)ceil((float)buffer_size / 4.0f);
    size_t buffer_ptr_idx = 0;
    uint8_t address_increment = 0x04;

    HAL_FLASH_Unlock();
    for (size_t chunk = 0; chunk < write_chunks; ++chunk)
    {
        memcpy(write_buffer, &buffer[buffer_ptr_idx], 4);
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, *(uint32_t*)&write_buffer);
        address += address_increment;
        buffer_ptr_idx += 4;
    }
    HAL_FLASH_Lock();
}

// TODO: implement this
void read_string_from_flash()
{

}
#endif