#ifndef ATOM_INCLUDE
#define ATOM_INCLUDE
extern int atom_length(const char *str);
extern const char* atom_new(const char *str,int len,int flag);
extern const char* atom_string(const char *str);
extern const char* atom_int(long n);
extern void atom_reset();
extern void atom_free(const char *str);

extern void atom_vload(const char* str,...);
extern void atom_aload(const char* str[]);

extern const char* atom_add(const char* str,int len);
#endif
