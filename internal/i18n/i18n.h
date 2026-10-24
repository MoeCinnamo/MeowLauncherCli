#ifndef I18N_H
#define I18N_H

/*
 * Initialize localization.
 *
 * Selects the active language archive: the system locale (the LANG
 * environment variable) wins when the language was compiled in; otherwise
 * the built-in English archive is used. When only one language was compiled
 * in at build time, that language is used unconditionally.
 * Must be called before any I18n() lookup.
 */
void I18n_Init(void);

/*
 * Return the localized string for a dotted path like "help.tagline".
 *
 * Looks the key up in the active archive; on miss, falls back to the
 * English archive. The returned pointer is stable until the next call
 * (single-threaded CLI).
 */
const char *I18n(const char *path);

#endif /* I18N_H */
