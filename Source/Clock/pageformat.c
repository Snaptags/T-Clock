/*-------------------------------------------
  pageformat.c
  "Format" page of properties
                       KAZUBON 1997-1998
---------------------------------------------*/

#include "tclock.h"

static void OnInit(HWND hDlg);
static void OnApply(HWND hDlg);
static void OnLocale(HWND hDlg);
static void On12Hour(HWND hDlg);
static void OnCustom(HWND hDlg, BOOL bmouse);
static void OnFormatCheck(HWND hDlg, WORD id);

static int m_ilang;  // language code. ex) 0x411 - Japanese
static int m_idate;  // 0: mm/dd/yy 1: dd/mm/yy 2: yy/mm/dd
static char* m_pCustomFormat = NULL;
static char m_sMon[10];  //

static char m_bDayOfWeekIsLast;   // yy/mm/dd ddd
static char m_bTimeMarkerIsFirst; // AM/PM hh:nn:ss

static char m_sep_date[4]; // date seperator such as / .
static char m_sep_time[4]; // time seperator such as : .


static __inline void SendPSChanged(HWND hDlg)
{
	g_bApplyClock = TRUE;
	g_bApplyTaskbar = TRUE;
	SendMessage(GetParent(hDlg), PSM_CHANGED, (WPARAM)(hDlg), 0);
}
/*------------------------------------------------
   Dialog Procedure for the "Format" page
--------------------------------------------------*/
INT_PTR CALLBACK PageFormatProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message) {
	case WM_INITDIALOG:
		OnInit(hDlg);
		return TRUE;
	case WM_DESTROY:
		if(m_pCustomFormat) {
			free(m_pCustomFormat);
			m_pCustomFormat = NULL;
		}
		break;
	case WM_COMMAND: {
			WORD id, code;
			id = LOWORD(wParam); code = HIWORD(wParam);
			if(id == IDC_LOCALE && code == CBN_SELCHANGE)
				OnLocale(hDlg);
			// format textbox
			else if(id == IDC_FORMAT && code == EN_CHANGE)
				SendPSChanged(hDlg);
			// "Custumize Format"
			else if(id == IDC_CUSTOM)
				OnCustom(hDlg, TRUE);
			// "12H"
			else if(id == IDC_12HOUR)
				On12Hour(hDlg);
			// "year" -- "Internet Time"
			else if(IDC_YEAR4 <= id && id <= IDC_AMPM)
				OnFormatCheck(hDlg, id);
			else if(id == IDC_AMSYMBOL && code == CBN_SELCHANGE)
				SendPSChanged(hDlg);
			else if(id == IDC_PMSYMBOL && code == CBN_SELCHANGE)
				SendPSChanged(hDlg);
			else if(id == IDC_ZERO)
				SendPSChanged(hDlg);
			return TRUE;
		}
	case WM_NOTIFY:
		switch(((NMHDR*)lParam)->code) {
		case PSN_APPLY:
			OnApply(hDlg);
			break;
		} return TRUE;
	}
	return FALSE;
}

char* entrydate[] = { "Year4", "Year", "Month", "MonthS", "Day", "Weekday",
					  "Hour", "Minute", "Second", "Kaigyo", "InternetTime",
					  "AMPM", "Hour12", "HourZero", "Custom",
					};
#define ENTRY(id) entrydate[(id)-IDC_YEAR4]

/*------------------------------------------------
  Initialize Locale Infomation
--------------------------------------------------*/
void InitLocale(HWND hwnd)
{
	int i;
	const int aLangDayOfWeekIsLast[]={LANG_JAPANESE,LANG_KOREAN,0};
	
	if(hwnd) {
		int sel = (int)CBGetCurSel(hwnd, IDC_LOCALE);
		m_ilang = (int)CBGetItemData(hwnd, IDC_LOCALE, sel);
	} else {
		m_ilang = GetMyRegLong("Format", "Locale", GetUserDefaultLangID())&0x00ff;
	}
	GetLocaleInfo(m_ilang,LOCALE_SDATE,m_sep_date,sizeof(m_sep_date));
	GetLocaleInfo(m_ilang,LOCALE_STIME,m_sep_time,sizeof(m_sep_time));
	GetLocaleInfo(m_ilang, LOCALE_ITIMEMARKPOSN|LOCALE_RETURN_NUMBER, (LPSTR)&i, sizeof(i));
	m_bTimeMarkerIsFirst=(char)i;
	GetLocaleInfo(m_ilang, LOCALE_IDATE|LOCALE_RETURN_NUMBER, (LPSTR)&m_idate, sizeof(m_idate));
	GetLocaleInfo(m_ilang, LOCALE_SABBREVDAYNAME1, m_sMon, sizeof(m_sMon));
	
	m_bDayOfWeekIsLast = 0;
	for(i=0; aLangDayOfWeekIsLast[i]; ++i) {
		if((m_ilang&0x00ff) == aLangDayOfWeekIsLast[i]) {
			m_bDayOfWeekIsLast = 1; break;
		}
	}
}

static HWND m_hwndPage;
/*------------------------------------------------
  for EnumSystemLocales function
--------------------------------------------------*/
BOOL CALLBACK EnumLocalesProc(LPTSTR lpLocaleString)
{
	char str[80];
	int x, index;
	
	x = atox(lpLocaleString);
	if(GetLocaleInfo(x, LOCALE_SLANGUAGE, str, sizeof(str)) > 0)
		index = (int)CBAddString(m_hwndPage, IDC_LOCALE, str);
	else
		index = (int)CBAddString(m_hwndPage, IDC_LOCALE, lpLocaleString);
	CBSetItemData(m_hwndPage, IDC_LOCALE, index, x);
	return TRUE;
}

/*------------------------------------------------
  Initialize the "Format" page
--------------------------------------------------*/
void OnInit(HWND hDlg)
{
	const char* AM[]={"AM","am","A","a"," ",};
	const char* PM[]={"PM","pm","P","p"," ",};
	const size_t AMPMs=sizeof(AM)/sizeof(AM[0]);
	HFONT hfont;
	char fmt[MAX_BUFF];
	int i, count;
	int ilang;
	char ampm_user[TNY_BUFF];
	char ampm_locale[TNY_BUFF];
	
	m_hwndPage = hDlg;
	
	hfont = (HFONT)GetStockObject(SYSTEM_FIXED_FONT);
	if(hfont)
		SendDlgItemMessage(hDlg, IDC_FORMAT, WM_SETFONT, (WPARAM)hfont, 0);
		
	// Fill and select the "Locale" combobox
	EnumSystemLocales(EnumLocalesProc, LCID_INSTALLED);
	CBSetCurSel(hDlg, IDC_LOCALE, 0);
	ilang = GetMyRegLong("Format", "Locale", GetUserDefaultLangID());
	count = (int)CBGetCount(hDlg, IDC_LOCALE);
	for(i = 0; i < count; i++) {
		int x;
		x = (int)CBGetItemData(hDlg, IDC_LOCALE, i);
		if(x == ilang) {
			CBSetCurSel(hDlg, IDC_LOCALE, i); break;
		}
	}
	
	InitLocale(hDlg);
	
	// "year" -- "second"
	for(i = IDC_YEAR4; i <= IDC_SECOND; i++) {
		CheckDlgButton(hDlg, i,
					   GetMyRegLong("Format", ENTRY(i), TRUE));
	}
	
	if(IsDlgButtonChecked(hDlg, IDC_YEAR))
		CheckRadioButton(hDlg, IDC_YEAR4, IDC_YEAR, IDC_YEAR);
	if(IsDlgButtonChecked(hDlg, IDC_YEAR4))
		CheckRadioButton(hDlg, IDC_YEAR4, IDC_YEAR, IDC_YEAR4);
		
	if(IsDlgButtonChecked(hDlg, IDC_MONTH))
		CheckRadioButton(hDlg, IDC_MONTH, IDC_MONTHS, IDC_MONTH);
	if(IsDlgButtonChecked(hDlg, IDC_MONTHS))
		CheckRadioButton(hDlg, IDC_MONTH, IDC_MONTHS, IDC_MONTHS);
	
	// "Internet Time" -- "Customize format"
	for(i = IDC_KAIGYO; i <= IDC_CUSTOM; i++) {
		CheckDlgButton(hDlg, i,
					   GetMyRegLong("Format", ENTRY(i), FALSE));
	}
	
	GetMyRegStr("Format", "Format", fmt, 1024, "");
	SetDlgItemText(hDlg, IDC_FORMAT, fmt);
	
	m_pCustomFormat = malloc(MAX_BUFF);
	if(m_pCustomFormat)
		GetMyRegStr("Format", "CustomFormat", m_pCustomFormat, MAX_BUFF, "");
	
	// "AM Symbol" and "PM Symbol"
	CBResetContent(hDlg, IDC_AMSYMBOL);
	GetMyRegStr("Format", "AMsymbol", ampm_user, sizeof(ampm_user), "");
	if(*ampm_user)
		CBAddString(hDlg, IDC_AMSYMBOL, ampm_user);
	if(GetLocaleInfo(ilang, LOCALE_S1159, ampm_locale, sizeof(ampm_locale)) && strcmp(ampm_user,ampm_locale))
		CBAddString(hDlg,IDC_AMSYMBOL,ampm_locale);
	else
		*ampm_locale='\0';
	for(i=0; i<AMPMs; ++i){
		if(strcmp(ampm_locale,AM[i]) && strcmp(ampm_user,AM[i]))
			CBAddString(hDlg,IDC_AMSYMBOL,AM[i]);
	}
	CBSetCurSel(hDlg, IDC_AMSYMBOL, 0);
	
	CBResetContent(hDlg, IDC_PMSYMBOL);
	GetMyRegStr("Format", "PMsymbol", ampm_user, sizeof(ampm_user), "");
	if(*ampm_user)
		CBAddString(hDlg, IDC_PMSYMBOL, ampm_user);
	if(GetLocaleInfo(ilang, LOCALE_S2359, ampm_locale, sizeof(ampm_locale)) && strcmp(ampm_user,ampm_locale))
		CBAddString(hDlg,IDC_PMSYMBOL,ampm_locale);
	else
		*ampm_locale='\0';
	for(i=0; i<AMPMs; ++i){
		if(strcmp(ampm_locale,PM[i]) && strcmp(ampm_user,PM[i]))
			CBAddString(hDlg,IDC_PMSYMBOL,PM[i]);
	}
	CBSetCurSel(hDlg, IDC_PMSYMBOL, 0);
	
	On12Hour(hDlg);
	OnCustom(hDlg, FALSE);
}

//================================================================================================
//---------------------------------------------------------------------------+++--> "Apply" button:
void OnApply(HWND hDlg)   //----------------------------------------------------------------+++-->
{
	char s[1024];
	int i;
	
	SetMyRegLong("Format", "Locale",
				 (DWORD)CBGetItemData(hDlg, IDC_LOCALE, CBGetCurSel(hDlg, IDC_LOCALE)));
				 
	for(i = IDC_YEAR4; i <= IDC_CUSTOM; i++) {
		SetMyRegLong("Format", ENTRY(i), IsDlgButtonChecked(hDlg, i));
	}
	
	GetDlgItemText(hDlg, IDC_AMSYMBOL, s, 1024);
	SetMyRegStr("Format", "AMsymbol", s);
	GetDlgItemText(hDlg, IDC_PMSYMBOL, s, 1024);
	SetMyRegStr("Format", "PMsymbol", s);
	
	GetDlgItemText(hDlg, IDC_FORMAT, s, 1024);
	SetMyRegStr("Format", "Format", s);
	
	if(m_pCustomFormat) {
		if(IsDlgButtonChecked(hDlg, IDC_CUSTOM)) {
			strcpy(m_pCustomFormat, s);
			SetMyRegStr("Format", "CustomFormat", m_pCustomFormat);
		}
	}
}
//================================================================================================
//-------------------------------------------+++--> When User's Location (Locale ComboBox) Changes:
void OnLocale(HWND hDlg)   //---------------------------------------------------------------+++-->
{
	InitLocale(hDlg);
	OnCustom(hDlg, FALSE);
}
//================================================================================================
//-----------------------------------+++--> Handler for Enable/Disable "Customize format" CheckBox:
void OnCustom(HWND hDlg, BOOL bmouse)   //--------------------------------------------------+++-->
{
	BOOL b;
	int i;
	
	b = IsDlgButtonChecked(hDlg, IDC_CUSTOM);
	EnableDlgItem(hDlg, IDC_FORMAT, b);
	
	for(i = IDC_YEAR4; i <= IDC_AMPM; i++)
		EnableDlgItem(hDlg, i, !b);
	
	if(m_pCustomFormat && bmouse) {
		if(b) {
			if(m_pCustomFormat[0])
				SetDlgItemText(hDlg, IDC_FORMAT, m_pCustomFormat);
		} else {
			GetDlgItemText(hDlg, IDC_FORMAT, m_pCustomFormat, 1024);
		}
	}
	
	if(!b) OnFormatCheck(hDlg, 0);
	SendPSChanged(hDlg);
}
//================================================================================================
//-----------------------------------------+++--> Toggle Display Between 12 & 24 Hour Time Formats:
void On12Hour(HWND hDlg)   //---------------------------------------------------------------+++-->
{
	BOOL b;
	
	b = IsDlgButtonChecked(hDlg, IDC_12HOUR);
	if(!b) {
		CheckDlgButton(hDlg, IDC_AMPM, 0);
		if(!IsDlgButtonChecked(hDlg, IDC_CUSTOM)) OnFormatCheck(hDlg, 0);
	}
	SendPSChanged(hDlg);
}
/*------------------------------------------------
  When clicked "year" -- "am/pm"
--------------------------------------------------*/

#define CHECKS(a) checks[(a)-IDC_YEAR4]

void OnFormatCheck(HWND hDlg, WORD id)
{
	char s[1024];
	int checks[15];
	int i;
	
	for(i = IDC_YEAR4; i <= IDC_AMPM; i++) {
		CHECKS(i) = IsDlgButtonChecked(hDlg, i);
	}
	
	if(id == IDC_YEAR4 || id == IDC_YEAR) {
		if(id == IDC_YEAR4 && CHECKS(IDC_YEAR4)) {
			CheckRadioButton(hDlg, IDC_YEAR4, IDC_YEAR, IDC_YEAR4);
			CHECKS(IDC_YEAR) = FALSE;
		}
		if(id == IDC_YEAR && CHECKS(IDC_YEAR)) {
			CheckRadioButton(hDlg, IDC_YEAR4, IDC_YEAR, IDC_YEAR);
			CHECKS(IDC_YEAR4) = FALSE;
		}
	}
	
	if(id == IDC_MONTH || id == IDC_MONTHS) {
		if(id == IDC_MONTH && CHECKS(IDC_MONTH)) {
			CheckRadioButton(hDlg, IDC_MONTH, IDC_MONTHS, IDC_MONTH);
			CHECKS(IDC_MONTHS) = FALSE;
		}
		if(id == IDC_MONTHS && CHECKS(IDC_MONTHS)) {
			CheckRadioButton(hDlg, IDC_MONTH, IDC_MONTHS, IDC_MONTHS);
			CHECKS(IDC_MONTH) = FALSE;
		}
	}
	
	if(id == IDC_AMPM) {
		CheckDlgButton(hDlg, IDC_12HOUR, 1);
		On12Hour(hDlg);
	}
	
	CreateFormat(s, checks);
	SetDlgItemText(hDlg, IDC_FORMAT, s);
	SendPSChanged(hDlg);
}

/*------------------------------------------------
  Initialize a format string. Called from main.c
--------------------------------------------------*/
void InitFormat()
{
	char format[LRG_BUFF];
	int iter, checks[15];
	
	if(GetMyRegLong("Format", ENTRY(IDC_CUSTOM), FALSE))
		return;
	InitLocale(NULL);
	
	for(iter=IDC_YEAR4; iter<=IDC_SECOND; ++iter) {
		CHECKS(iter) = GetMyRegLong("Format", ENTRY(iter), TRUE);
	}
	
	if(CHECKS(IDC_YEAR))  CHECKS(IDC_YEAR4) = FALSE;
	if(CHECKS(IDC_YEAR4)) CHECKS(IDC_YEAR) = FALSE;
	
	if(CHECKS(IDC_MONTH))  CHECKS(IDC_MONTHS) = FALSE;
	if(CHECKS(IDC_MONTHS)) CHECKS(IDC_MONTH) = FALSE;
	
	for(iter=IDC_KAIGYO; iter<=IDC_CUSTOM; ++iter) {
		CHECKS(iter) = GetMyRegLong("Format", ENTRY(iter), FALSE);
	}
	
	CreateFormat(format, checks);
	SetMyRegStr("Format", "Format", format);
}

/*--------------------------------------------------
=============== Create a format string automatically
--------------------------------------------------*/
void CreateFormat(char* dst, int* checks)
{
	BOOL bdate = FALSE, btime = FALSE;
	int i;
	
	for(i = IDC_YEAR4; i <= IDC_WEEKDAY; i++) {
		if(CHECKS(i)) {
			bdate = TRUE;
			break;
		}
	}
	
	for(i = IDC_HOUR; i <= IDC_AMPM; i++) {
		if(CHECKS(i)) {
			btime = TRUE;
			break;
		}
	}
	
	dst[0] = 0;
	
	if(!m_bDayOfWeekIsLast && CHECKS(IDC_WEEKDAY)) {
		strcat(dst, "ddd");
		for(i = IDC_YEAR4; i <= IDC_DAY; i++) {
			if(CHECKS(i)) {
				if((m_ilang & 0x00ff) == LANG_CHINESE) strcat(dst, " ");
				else if(m_sMon[0] && m_sMon[ strlen(m_sMon) - 1 ] == '.')
					strcat(dst, " ");
				else strcat(dst, ", ");
				break;
			}
		}
	}
	
	switch(m_idate){
	case 0: // m/d/y
		if(CHECKS(IDC_MONTH) || CHECKS(IDC_MONTHS)) {
			if(CHECKS(IDC_MONTH)) strcat(dst, "mm");
			if(CHECKS(IDC_MONTHS)) strcat(dst, "mmm");
			if(CHECKS(IDC_DAY) || CHECKS(IDC_YEAR4) || CHECKS(IDC_YEAR)) {
				if(CHECKS(IDC_MONTH)) strcat(dst, m_sep_date);
				else strcat(dst, " ");
			}
		}
		if(CHECKS(IDC_DAY)) {
			strcat(dst, "dd");
			if(CHECKS(IDC_YEAR4) || CHECKS(IDC_YEAR)) {
				if(CHECKS(IDC_MONTH)) strcat(dst, m_sep_date);
				else strcat(dst, ", ");
			}
		}
		if(CHECKS(IDC_YEAR4)) strcat(dst, "yyyy");
		if(CHECKS(IDC_YEAR)) strcat(dst, "yy");
		break;
	case 1: // d/m/y
		if(CHECKS(IDC_DAY)) {
			strcat(dst, "dd");
			if(CHECKS(IDC_MONTH) || CHECKS(IDC_MONTHS)) {
				if(CHECKS(IDC_MONTH)) strcat(dst, m_sep_date);
				else strcat(dst, " ");
			} else if(CHECKS(IDC_YEAR4) || CHECKS(IDC_YEAR)) strcat(dst, m_sep_date);
		}
		if(CHECKS(IDC_MONTH) || CHECKS(IDC_MONTHS)) {
			if(CHECKS(IDC_MONTH)) strcat(dst, "mm");
			if(CHECKS(IDC_MONTHS)) strcat(dst, "mmm");
			if(CHECKS(IDC_YEAR4) || CHECKS(IDC_YEAR)) {
				if(CHECKS(IDC_MONTH)) strcat(dst, m_sep_date);
				else strcat(dst, " ");
			}
		}
		if(CHECKS(IDC_YEAR4)) strcat(dst, "yyyy");
		if(CHECKS(IDC_YEAR)) strcat(dst, "yy");
		break;
	default:  // y/m/d
		if(CHECKS(IDC_YEAR4) || CHECKS(IDC_YEAR)) {
			if(CHECKS(IDC_YEAR4)) strcat(dst, "yyyy");
			if(CHECKS(IDC_YEAR)) strcat(dst, "yy");
			if(CHECKS(IDC_MONTH) || CHECKS(IDC_MONTHS)
			   || CHECKS(IDC_DAY)) {
				if(CHECKS(IDC_MONTHS)) strcat(dst, " ");
				else strcat(dst, m_sep_date);
			}
		}
		if(CHECKS(IDC_MONTH) || CHECKS(IDC_MONTHS)) {
			if(CHECKS(IDC_MONTH)) strcat(dst, "mm");
			if(CHECKS(IDC_MONTHS)) strcat(dst, "mmm");
			if(CHECKS(IDC_DAY)) {
				if(CHECKS(IDC_MONTHS)) strcat(dst, " ");
				else strcat(dst, m_sep_date);
			}
		}
		if(CHECKS(IDC_DAY)) strcat(dst, "dd");
	}
	
	if(m_bDayOfWeekIsLast && CHECKS(IDC_WEEKDAY)) {
		for(i = IDC_YEAR4; i <= IDC_DAY; i++) {
			if(CHECKS(i)) { strcat(dst, " "); break; }
		}
		strcat(dst, "ddd");
	}
	
	if(bdate && btime) {
		if(CHECKS(IDC_KAIGYO)) strcat(dst, "\\n");
		else {
			if(m_idate < 2 && CHECKS(IDC_MONTHS) &&
			   (CHECKS(IDC_YEAR4) || CHECKS(IDC_YEAR)))
				strcat(dst, " ");
			strcat(dst, " ");
		}
	}
	
	if(m_bTimeMarkerIsFirst && CHECKS(IDC_AMPM)) {
		strcat(dst, "tt");
		if(CHECKS(IDC_HOUR) || CHECKS(IDC_MINUTE) ||
		   CHECKS(IDC_SECOND) || CHECKS(IDC_INTERNETTIME))
			strcat(dst, " ");
	}
	
	if(CHECKS(IDC_HOUR)) {
		strcat(dst, "hh");
		if(CHECKS(IDC_MINUTE) || CHECKS(IDC_SECOND)) strcat(dst, m_sep_time);
		else if(CHECKS(IDC_INTERNETTIME) ||
				(!m_bTimeMarkerIsFirst && CHECKS(IDC_AMPM))) strcat(dst, " ");
	}
	if(CHECKS(IDC_MINUTE)) {
		strcat(dst, "nn");
		if(CHECKS(IDC_SECOND)) strcat(dst, m_sep_time);
		else if(CHECKS(IDC_INTERNETTIME) ||
				(!m_bTimeMarkerIsFirst && CHECKS(IDC_AMPM))) strcat(dst, " ");
	}
	if(CHECKS(IDC_SECOND)) {
		strcat(dst, "ss");
		if(CHECKS(IDC_INTERNETTIME) ||
		   (!m_bTimeMarkerIsFirst && CHECKS(IDC_AMPM))) strcat(dst, " ");
	}
	
	if(!m_bTimeMarkerIsFirst && CHECKS(IDC_AMPM)) {
		strcat(dst, "tt");
		if(CHECKS(IDC_INTERNETTIME)) strcat(dst, " ");
	}
	
	if(CHECKS(IDC_INTERNETTIME)) strcat(dst, "@@@");
}
