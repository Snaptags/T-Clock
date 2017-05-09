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
#	define VER_REVISION 463
#	define VER_FULL "2.4.2 Beta"
#	define VER_SHORT "2.4b2"
#	define VER_SHORT_DOTS "2.4.2"
#	define VER_SHORT_GREEK "2.4β2"
#	define VER_RC_REVISION 2, 4, 2, 463
#	define VER_RC_STATUS 2, 4, 2, 1
/**** Subversion Information ****/
#	define VER_REVISION_URL "https://github.com/Snaptags/T-Clock.git"
#	define VER_REVISION_DATE "2017-05-09 07:41:06 +0000 (Tue, May 09 2017)"
#	define VER_REVISION_HASH "6e70f66"
#	define VER_REVISION_TAG "v2.4.2#463-beta"
/**** Date/Time ****/
#	define VER_TIMESTAMP 1494316041
#	define VER_TIME_SEC 21
#	define VER_TIME_MIN 47
#	define VER_TIME_HOUR 7
#	define VER_TIME_DAY 9
#	define VER_TIME_MONTH 5
#	define VER_TIME_YEAR 2017
#	define VER_TIME_WDAY 2
#	define VER_TIME_YDAY 128
#	define VER_TIME_WDAY_SHORT "Tue"
#	define VER_TIME_WDAY_FULL "Tuesday"
#	define VER_TIME_MONTH_SHORT "May"
#	define VER_TIME_MONTH_FULL "May"
#	define VER_TIME "07:47:21"
#	define VER_DATE "2017-05-09"
#	define VER_DATE_LONG "Tue, May 09, 2017 07:47:21 UTC"
#	define VER_DATE_SHORT "2017-05-09 07:47:21 UTC"
#	define VER_DATE_ISO "2017-05-09T07:47:21Z"
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
