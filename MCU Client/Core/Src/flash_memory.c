#include "flash_memory.h"
#include "addresses.h"

void clear_flash(uint32_t write_address)
{
	const size_t n_addresses = 4;

	uint32_t addresses[4] = {
			CLIPBOARD1_BEGIN_ADDRESS,
			CLIPBOARD2_BEGIN_ADDRESS,
			CLIPBOARD3_BEGIN_ADDRESS,
			CLIPBOARD4_BEGIN_ADDRESS
	};

	char copy_buffers[4][255] = {{0}, {0}, {0}, {0}};
	size_t buffer_lengths[4] = {0, 0, 0, 0};

	for (size_t i = 0; i < n_addresses; ++i)
	{
		if (addresses[i] == write_address)
			continue;
		else // note: metadata address is always 4 bytes before the begin address
			read_string_from_flash(copy_buffers[i], &buffer_lengths[i], addresses[i] - 0x04, addresses[i]);
	}

	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
	FLASH_Erase_Sector(FLASH_SECTOR_7, VOLTAGE_RANGE_3);
	HAL_FLASH_Lock();
	// after erasing flash, copy the other addresses (besides the one we're about to write to via copy)
	for (size_t i = 0; i < n_addresses; ++i)
	{
		if (addresses[i] == write_address)
			continue;
		else
		{
			write_metadata_to_flash(buffer_lengths[i], addresses[i] - 0x04);
			write_string_to_flash(copy_buffers[i], addresses[i]);
		}
	}
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

void read_string_from_flash(char buffer[256], size_t* buffer_len, uint32_t metadata_address, uint32_t buffer_address)
{
	// if block is empty, do nothing (otherwise it hangs forever/crashes)
	if (*(uint32_t*)buffer_address == 0xFFFFFFFF)
		return;

	*buffer_len = *(uint32_t*)metadata_address; // dereference value at metadata address to extract value of metadata (length of buffer)
	char* text = (char*)buffer_address;
	strncat(buffer, text, *buffer_len);
}
