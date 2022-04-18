#include "flash_memory.h"

void clear_flash(uint32_t beginning_address, uint32_t ending_address)
{
	HAL_FLASH_Unlock();
	while (beginning_address < ending_address)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, beginning_address, 0xFFFFFFFF);
		beginning_address += 0x04;
	}
	HAL_FLASH_Lock();
}

/**
 * Write character buffer to flash starting at a specified address.
 */
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

void write_metadata_to_flash(uint32_t metadata, uint32_t address)
{
	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, metadata);
	HAL_FLASH_Lock();
}

void read_string_from_flash(char buffer[255], uint32_t metadata_address, uint32_t buffer_address)
{
	size_t len = *(uint32_t*)metadata_address; // dereference value at metadata address to extract value of metadata (length of buffer)
	char* text = (char*)buffer_address;
	strncat(buffer, text, len);
}
