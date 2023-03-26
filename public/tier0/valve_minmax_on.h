//========= Copyright Valve Corporation, All rights reserved. ============//
#if !defined(POSIX)
#ifndef min
	#define MIN(a,b)  (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
	#define MAX(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#endif
