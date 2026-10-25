#ifndef JSON_H
#define JSON_H

/*
 * Minimal JSON file IO + string walker used by internal modules.
 *
 * The walker handles the tiny subset this project embeds/writes: flat
 * objects and quoted string values. It is deliberately not a full parser.
 */

/* Read the whole file into a malloc'd buffer. Returns NULL on error. */
char *Json_Load(const char *path);

/* Write buf to path, creating parent directories recursively.
 * Returns 0 on success, nonzero on error. */
int Json_Save(const char *path, const char *buf);

/* Locate the object named "key" inside a flat JSON document. On success,
 * sets *end to just past the matching closing brace and returns the
 * pointer to the opening '{'. Returns NULL when not found or malformed. */
const char *Json_FindObject(const char *json, const char *key,
                            const char **end);

/* Within an object bounded by [start, end), locate "key":"value" and set
 * *vlen to the value length. Escaped quotes and backslashes (\", \\)
 * inside the value are skipped, so the whole value is returned including
 * its escape sequences. Returns pointer to the opening quote of the
 * value, or NULL on miss. */
const char *Json_FindValue(const char *start, const char *end,
                           const char *key, size_t *vlen);

/* Return a malloc'd JSON-escaped copy of s per RFC 8259 string rules:
 * " -> \", \ -> \\, control chars -> \b \f \n \r \t or \u00XX; bytes
 * >= 0x20 (including UTF-8 continuation bytes) pass through unchanged.
 * Returns NULL on allocation failure; caller must free(). */
char *Json_Escape(const char *s);

#endif
