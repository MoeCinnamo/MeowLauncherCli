/*
 * JSON string escaping: Json_Escape.
 *
 * Produces the body of a JSON string (without the surrounding quotes)
 * per RFC 8259: double quotes and backslashes are escaped, control
 * characters use the short \b \f \n \r \t forms or \u00XX, and bytes
 * >= 0x20 (including UTF-8 continuation bytes) pass through unchanged.
 */

#include <stdlib.h>

#include "internal/json/json.h"

/* Return the number of bytes the JSON-escaped form of s will occupy. */
static size_t json_escape_len(const char *s)
{
    size_t n = 0;
    unsigned char c;

    while ((c = (unsigned char)*s++) != '\0') {
        switch (c) {
        case '"':
        case '\\':
            n += 2;
            break;
        case '\b':
        case '\f':
        case '\n':
        case '\r':
        case '\t':
            n += 2;
            break;
        default:
            n += (c < 0x20) ? 6 : 1;
            break;
        }
    }
    return n;
}

/* Return a malloc'd JSON-escaped copy of s. Returns NULL on allocation
 * failure; the caller must free() the result. */
char *Json_Escape(const char *s)
{
    static const char hex[] = "0123456789abcdef";
    size_t n = json_escape_len(s);
    char *out = malloc(n + 1);
    char *p = out;

    if (out == NULL) {
        return NULL;
    }
    while (*s != '\0') {
        unsigned char c = (unsigned char)*s++;
        switch (c) {
        case '"':
            *p++ = '\\';
            *p++ = '"';
            break;
        case '\\':
            *p++ = '\\';
            *p++ = '\\';
            break;
        case '\b':
            *p++ = '\\';
            *p++ = 'b';
            break;
        case '\f':
            *p++ = '\\';
            *p++ = 'f';
            break;
        case '\n':
            *p++ = '\\';
            *p++ = 'n';
            break;
        case '\r':
            *p++ = '\\';
            *p++ = 'r';
            break;
        case '\t':
            *p++ = '\\';
            *p++ = 't';
            break;
        default:
            if (c < 0x20) {
                *p++ = '\\';
                *p++ = 'u';
                *p++ = '0';
                *p++ = '0';
                *p++ = hex[(c >> 4) & 0xf];
                *p++ = hex[c & 0xf];
            } else {
                *p++ = (char)c;
            }
            break;
        }
    }
    *p = '\0';
    return out;
}
