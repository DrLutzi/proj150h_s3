#ifndef ERRORSHANDLER_HPP
#define ERRORSHANDLER_HPP

#include <QtGlobal>

#define ERROR_UNKNOWN do {qFatal("An unknown critical error occured.");} while(0)
#define ERROR(message) do {qFatal(message);} while(0)

#define WARNING_UNKNOWN do {qWarning("An unknown error occured.");} while(0)
#define WARNING(message) do {qWarning(message);} while(0)

#endif // ERRORSHANDLER_HPP
