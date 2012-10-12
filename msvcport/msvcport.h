/* -*- Mode: c; tab-width: 8; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#ifndef FONTCONFIG_MSVCPORT_H_
#define FONTCONFIG_MSVCPORT_H_

#include "msvcport_private.h"

#define FcOpendir(x) fc_msvcport_opendir(x)
#define FcReaddir(x) fc_msvcport_readdir(x)
#define FcClosedir(x) fc_msvcport_closedir(x)

#define FcMkdir(x, y) fc_msvcport_mkdir(x, y)
#define FcRmdir(x) fc_msvcport_rmdir(x)

#define FcAccess(x, y) fc_msvcport_access(x, y)
#define FcChmod(x, y) fc_msvcport_chmod(x, y)
#define FcUnlink(x) fc_msvcport_unlink(x)
#define FcOpen(x, y, ...) fc_msvcport_open(x, y, __VA_ARGS__)


#endif /* FONTCONFIG_MSVCPORT_H_ */
