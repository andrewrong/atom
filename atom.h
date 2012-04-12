#ifndef ATOM_INCLUDE
#define ATOM_INCLUDE
extern int atom_length(const char *str);
extern const char* atom_new(const char *str,int len);
extern const char* atom_string(const char *str);
extern const char* atom_int(long n);
extern void atom_free();
#endif
