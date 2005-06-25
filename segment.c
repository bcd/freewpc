
#include <wpc.h>
#include <sys/task.h>

typedef uint8_t segaddr_t;
typedef uint8_t segbits_t;


void seg_set (segaddr_t sa, segbits_t bits)
{
}

segbits_t seg_translate_char (char c)
{
}

void seg_write_bcd (bcd_t bcd)
{
}

void seg_write_uint8 (uint8_t u8)
{
}

void seg_write_digit (uint8_t digit)
{
}

void seg_write_string (segaddr_t sa, const char *s)
{
}

void seg_erase (segaddr_t sa, uint8_t len)
{
}

