#ifndef AUTOVERSION_H
#define AUTOVERSION_H
/* Note: to use integer defines as strings, use STR(), eg. STR(VER_REVISION) */
/**** Version ****/
#	define VER_MAJOR 2
#	define VER_MINOR 4
#	define VER_BUILD 2
	/** status values: 0=Alpha, 1=Beta, 2=RC, 3=Release, 4=Maintenance */
#	define VER_STATUS 1
#	define VER_STATUS_FULL "Beta"
#	define VER_STATUS_SHORT "b"
#	define VER_STATUS_GREEK "β"
#	define VER_REVISION 460
#	define VER_FULL "2.4.2 Beta"
#	define VER_SHORT "2.4b2"
#	define VER_SHORT_DOTS "2.4.2"
#	define VER_SHORT_GREEK "2.4β2"
#	define VER_RC_REVISION 2, 4, 2, 460
#	define VER_RC_STATUS 2, 4, 2, 1
/**** Subversion Information ****/
#	define VER_REVISION_URL "https://github.com/Snaptags/T-Clock.git"
#	define VER_REVISION_DATE "2017-05-08 14:16:23 +0000 (Mon, May 08 2017)"
#	define VER_REVISION_HASH "4addc82"
#	define VER_REVISION_TAG "v2.4.2#460-beta"
/**** Date/Time ****/
#	define VER_TIMESTAMP 1494261184
#	define VER_TIME_SEC 4
#	define VER_TIME_MIN 33
#	define VER_TIME_HOUR 16
#	define VER_TIME_DAY 8
#	define VER_TIME_MONTH 5
#	define VER_TIME_YEAR 2017
#	define VER_TIME_WDAY 1
#	define VER_TIME_YDAY 127
#	define VER_TIME_WDAY_SHORT "Mon"
#	define VER_TIME_WDAY_FULL "Monday"
#	define VER_TIME_MONTH_SHORT "May"
#	define VER_TIME_MONTH_FULL "May"
#	define VER_TIME "16:33:04"
#	define VER_DATE "2017-05-08"
#	define VER_DATE_LONG "Mon, May 08, 2017 16:33:04 UTC"
#	define VER_DATE_SHORT "2017-05-08 16:33:04 UTC"
#	define VER_DATE_ISO "2017-05-08T16:33:04Z"
/**** Helper 'functions' ****/
#	define VER_IsReleaseOrHigher() ( VER_STATUS >= 3 )
#	define VER_IsAlpha() ( VER_STATUS == 0 )
#	define VER_IsBeta() ( VER_STATUS == 1 )
#	define VER_IsRC() ( VER_STATUS == 2 )
#	define VER_IsRelease() ( VER_STATUS == 3 )
#	define VER_IsMaintenance() ( VER_STATUS == 4 )
#ifndef STR
#	define STR_(x) #x
#	define STR(x) STR_(x)
#endif
#ifndef L
#	define L_(x) L##x
#	define L(x) L_(x)
#endif
#endif
