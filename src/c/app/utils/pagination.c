#include "pagination.h"

uint16_t page_start(uint16_t section, uint16_t section_size)
{
    return section * section_size;
}

uint16_t page_end(uint16_t section, uint16_t section_size, uint16_t total)
{
    uint16_t end = page_start(section, section_size) + section_size;
    return end < total ? end : total;
}

uint16_t page_count(uint16_t section, uint16_t section_size, uint16_t total)
{
    return page_end(section, section_size, total) - page_start(section, section_size);
}

bool has_next_section(uint16_t section, uint16_t section_size, uint16_t total)
{
    return page_end(section, section_size, total) < total;
}

bool has_prev_section(uint16_t section)
{
    return section > 0;
}
