#ifndef TARGET_COM_GLOBAL_H
#define TARGET_COM_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef TARGET_COM_LIB
# define TARGET_COM_EXPORT Q_DECL_EXPORT
#else
# define TARGET_COM_EXPORT Q_DECL_IMPORT
#endif

#endif // TARGET_COM_GLOBAL_H
