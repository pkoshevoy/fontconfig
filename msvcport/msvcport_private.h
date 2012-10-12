/* -*- Mode: c; tab-width: 8; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#ifndef FONTCONFIG_MSVCPORT_PRIVATE_H_
#define FONTCONFIG_MSVCPORT_PRIVATE_H_

#include <wchar.h>

#if defined(_MSC_VER) && !defined(S_ISDIR)
#define S_ISDIR(mode) (((mode) & _S_IFDIR) != 0)
#endif

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

    wchar_t * fc_utf8_to_utf16(const char * cstr);
    char * fc_utf16_to_utf8(const wchar_t * wstr);

    /* NOTE:
       all char * strings passed to or returned from these
       fc_msvcport_... functions are UTF-8 encoded
    */

    typedef void * DIR;

    /*
      this structure is layed out same as in mingw/include/dirent.h
      for binary compatibility with code built with mingw compiler
    */
    struct dirent
    {
        long int           d_ino;       /* always zero */
        unsigned short int d_reclen;    /* always zero */
        unsigned short int d_namlen;    /* name length */
        char               d_name[260]; /* name[FILENAME_MAX] */
    };

    extern void * fc_msvcport_opendir(const char * path);
    extern struct dirent * fc_msvcport_readdir(void * priv);
    extern int fc_msvcport_closedir(void * priv);

    extern int fc_msvcport_mkdir(const char * path, int mode);
    extern int fc_msvcport_rmdir(const char * path);

    enum
    {
        F_OK = 0, /* Existence only */
        W_OK = 2, /* Write permission */
        R_OK = 4  /* Read permission */
    };

    extern int fc_msvcport_access(const char * path, int perms);
    extern int fc_msvcport_chmod(const char * path, int mode);
    extern int fc_msvcport_rename(const char * fnOld, const char * fnNew);
    extern int fc_msvcport_unlink(const char * path);
    extern int fc_msvcport_open(const char * fn, int oflag, ...);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif


#endif /* FONTCONFIG_MSVCPORT_PRIVATE_H_ */
