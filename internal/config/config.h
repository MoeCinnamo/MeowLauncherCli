#ifndef CONFIG_H
#define CONFIG_H

int config_path(char *buf, size_t bufsz);
void config_read_str(const char *key, const char *def, char *out, size_t outsz);
int config_write_json(const char *obj);

int Config_Java_Set(const char *version);
int Config_Java_Show(void);
int Config_Memory_Set(int mb);
int Config_Memory_Show(void);
int Config_Window_Set(const char *mode, int w, int h);
int Config_Window_Show(void);
int Config_Java_Args_Set(const char *args);
int Config_Java_Args_Show(void);
int Config_Precmd_Set(const char *cmd);
int Config_Precmd_Show(void);
int Config_Postcmd_Set(const char *cmd);
int Config_Postcmd_Show(void);
int Config_Gfx_Set(const char *api);
int Config_Gfx_Show(void);
void Config_Help(void);

#endif
