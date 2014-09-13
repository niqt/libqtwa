#ifndef LIBQTWA_H
#define LIBQTWA_H

#include <QtCore/QtGlobal>

#include "connection.h"

#if !defined(LIBQTWA)
#  if defined(LIBQTWA_LIBRARY)
#      define LIBQTWA Q_DECL_EXPORT
#  else
#      define LIBQTWA Q_DECL_IMPORT
#  endif //LIBQTWA_LIBRARY
#endif //LIBQTWA

#endif //LIBQTWA_H
