/* -*- Mode: c; tab-width: 8; c-basic-offset: 4; indent-tabs-mode: nil -*- */
#if defined(_WIN32)

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#ifndef _FILE_OFFSET_BITS
#define _FILE_OFFSET_BITS 64
#endif

#include <windows.h>
#include <io.h>
#include <winnls.h>
#include <wchar.h>
#include <share.h>

#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "msvcport_private.h"

wchar_t *
fc_utf8_to_utf16(const char * cstr)
{
    int sz = MultiByteToWideChar(CP_UTF8, 0, cstr, -1, NULL, 0);
    wchar_t * wstr = (wchar_t *)malloc(sz * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, cstr, -1, wstr, sz);
    return wstr;
}

char *
fc_utf16_to_utf8(const wchar_t * wstr)
{
    int sz = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char * cstr = (char *)malloc(sz);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, cstr, sz, NULL, NULL);
    return cstr;
}


struct TDir
{
    struct dirent         dirent_;
    intptr_t              handle_;
    struct _wfinddata64_t dfound_;
    int                   nfound_;
    wchar_t *             wquery_;
};

static void
TDirInit(struct TDir * d)
{
    memset(d, 0, sizeof(struct TDir));
}

static int
TDirClose(struct TDir * d)
{
    if (d->handle_)
    {
        int err = _findclose(d->handle_);
        memset(&d->dirent_, 0, sizeof(d->dirent_));
        d->nfound_ = 0;
        free(d->wquery_);
        d->wquery_ = NULL;
        return err;
    }

    return 0;
}

static int
TDirOpen(struct TDir * d, const char * path)
{
    size_t pathLen = strlen(path);
    size_t queryLen = pathLen;

    char * query = (char *)malloc(pathLen + 3);
    memcpy(query, path, queryLen);

    if (!pathLen || !strchr("\\/", path[pathLen - 1]))
    {
        query[queryLen++] = '/';
    }

    query[queryLen++] = '*';
    query[queryLen] = 0;

    TDirClose(d);
    d->wquery_ = fc_utf8_to_utf16(query);
    free(query);
    query = NULL;

    if (!d->wquery_)
    {
        return 0;
    }

    d->handle_ = _wfindfirst64(d->wquery_, &d->dfound_);
    if (d->handle_)
    {
        return 1;
    }

    return 0;
}

static void
TDirLoad(struct TDir * d)
{
    static const size_t nameMax = sizeof(d->dirent_.d_name);
    char * name = fc_utf16_to_utf8(d->dfound_.name);
    size_t nameLen = strlen(name);
    size_t copyLen = nameLen < nameMax ? nameLen : nameMax;
    strncpy_s(d->dirent_.d_name, nameMax, name, copyLen);
    free(name);
}

static int
TDirNext(struct TDir * d)
{
    if (!d->handle_ || d->nfound_ < 0)
    {
        return 0;
    }

    TDirLoad(d);

    if (_wfindnext64(d->handle_, &d->dfound_) == -1)
    {
        d->nfound_ = -1;
    }
    else
    {
        d->nfound_ ++;
    }

    return 1;
}


void *
fc_msvcport_opendir(const char * path)
{
    struct TDir * dir = (struct TDir *)malloc(sizeof(struct TDir));
    TDirInit(dir);

    if (TDirOpen(dir, path))
    {
        return dir;
    }

    TDirClose(dir);
    free(dir);
    return NULL;
}

int
fc_msvcport_closedir(void * priv)
{
    struct TDir * dir = (struct TDir *)priv;
    if (!dir)
    {
        return -1;
    }

    TDirClose(dir);
    free(dir);
    return 0;
}

struct dirent *
fc_msvcport_readdir(void * priv)
{
    struct TDir * dir = (struct TDir *)priv;
    if (dir && TDirNext(dir))
    {
        return &(dir->dirent_);
    }

    return NULL;
}

int
fc_msvcport_mkdir(const char * path, int mode)
{
    wchar_t * tmp = fc_utf8_to_utf16(path);
    int r = _wmkdir(tmp);
    free(tmp);
    return r;
}

int
fc_msvcport_rmdir(const char * path)
{
    wchar_t * tmp = fc_utf8_to_utf16(path);
    int r = _wrmdir(tmp);
    free(tmp);
    return r;
}

int
fc_msvcport_access(const char * path, int perms)
{
    /* X_OK (01) permission causes fatal error in win32 _access(..),
       therefore it must be excluded from the permission bitmask: */
    int wperms = perms & ~1;

    wchar_t * tmp = fc_utf8_to_utf16(path);
    int r = _waccess(tmp, wperms);
    free(tmp);
    return r;
}

int
fc_msvcport_chmod(const char * path, int unixPerms)
{
    wchar_t * tmp = fc_utf8_to_utf16(path);
    int permissions = 0;
    int ret = 0;

    if (unixPerms & 0444)
    {
        permissions |= _S_IREAD;
    }

    if (unixPerms & 0222)
    {
        permissions |= _S_IWRITE;
    }

    ret = _wchmod(tmp, permissions);
    free(tmp);
    return ret;
}

int
fc_msvcport_rename(const char * fnOld, const char * fnNew)
{
    wchar_t * wold = fc_utf8_to_utf16(fnOld);
    wchar_t * wnew = fc_utf8_to_utf16(fnNew);

    int ret = _wrename(wold, wnew);

    free(wold);
    free(wnew);
    return ret;
}

int
fc_msvcport_unlink(const char * path)
{
    wchar_t * tmp = fc_utf8_to_utf16(path);
    int r = _wunlink(tmp);
    free(tmp);
    return r;
}

int
fc_msvcport_open(const char * filenameUtf8, int accessMode, ...)
{
    int waccessMode = accessMode | O_BINARY;

    wchar_t * wname = fc_utf8_to_utf16(filenameUtf8);
    int fd = -1;
    int sh = accessMode & (_O_RDWR | _O_WRONLY) ? _SH_DENYWR : _SH_DENYNO;
    int permissions = _S_IREAD;
    errno_t err = 0;

    if ((accessMode & _O_CREAT) != 0)
    {
        int unixPerms = 0644;
        va_list ap;
        va_start(ap, accessMode);
        unixPerms = va_arg(ap, int);

        permissions = 0;
        if (unixPerms & 0444)
        {
            permissions |= _S_IREAD;
        }

        if (unixPerms & 0222)
        {
            permissions |= _S_IWRITE;
        }

        if (unixPerms & 0111)
        {
            permissions |= _S_IEXEC;
        }

        va_end(ap);
    }

    err = _wsopen_s(&fd, wname, waccessMode, sh, permissions);
    free(wname);

    return fd;
}


#endif /* _WIN32 */
