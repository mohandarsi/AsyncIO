#pragma once

#if !defined(ASYNCFILEIO_API)

#ifdef ASYNCFILEIO_EXPORTS
#define ASYNCFILEIO_API __declspec(dllexport)
#else
#define ASYNCFILEIO_API __declspec(dllimport)
#endif
#endif
