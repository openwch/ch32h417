/*---------------------------------------------------------------------------/
/  FatFs Functional Configurations
/---------------------------------------------------------------------------*/

#define FFCONF_DEF	86631	/* Revision ID */

/*---------------------------------------------------------------------------/
/ Function Configurations
/---------------------------------------------------------------------------*/

#define FF_FS_READONLY	0
/* This option switches read-only configuration. (0:Read/Write or 1:Read-only) */

#define FF_FS_MINIMIZE	0
/* This option defines minimization level to remove some basic API functions. */

#define FF_USE_FIND		0
/* This option switches filtered directory read functions. (0:Disable, 1:Enable 2:Enable with matching altname[] too) */

#define FF_USE_MKFS		1
/* This option switches f_mkfs() function. (0:Disable or 1:Enable) */

#define FF_USE_FASTSEEK	0
/* This option switches fast seek function. (0:Disable or 1:Enable) */

#define FF_USE_EXPAND	0
/* This option switches f_expand function. (0:Disable or 1:Enable) */

#define FF_USE_CHMOD	0
/* This option switches attribute manipulation functions. (0:Disable or 1:Enable) */

#define FF_USE_LABEL	0
/* This option switches volume label functions. (0:Disable or 1:Enable) */

#define FF_USE_FORWARD	0
/* This option switches f_forward() function. (0:Disable or 1:Enable) */

#define FF_USE_STRFUNC	0
#define FF_PRINT_LLI	0
#define FF_PRINT_FLOAT	0
#define FF_STRF_ENCODE	0
/* FF_USE_STRFUNC switches string functions. (0:Disable, 1:Enable, 2:Enable with LF-CRLF conversion) */

/*---------------------------------------------------------------------------/
/ Locale and Namespace Configurations
/---------------------------------------------------------------------------*/

#define FF_CODE_PAGE	936
/* This option specifies the OEM code page to be used on the target system.
/  936 - Simplified Chinese (DBCS) */

#define FF_USE_LFN		0
#define FF_MAX_LFN		255
/* The FF_USE_LFN switches the support for LFN (long file name). (0:Disable) */

#define FF_LFN_UNICODE	0
/* This option switches the character encoding on the API when LFN is enabled. */

#define FF_LFN_BUF		255
#define FF_SFN_BUF		12
/* This set of options defines size of file name members in the FILINFO structure. */

#define FF_FS_RPATH		0
/* This option configures support for relative path. (0:Disable) */

/*---------------------------------------------------------------------------/
/ Drive/Volume Configurations
/---------------------------------------------------------------------------*/

#define FF_VOLUMES		1
/* Number of volumes (logical drives) to be used. (1-10) */

#define FF_STR_VOLUME_ID	0
#define FF_VOLUME_STRS		"SD"
/* FF_STR_VOLUME_ID switches support for volume ID in arbitrary strings. */

#define FF_MULTI_PARTITION	0
/* This option switches support for multiple volumes on the physical drive. */

#define FF_MIN_SS		512
#define FF_MAX_SS		512
/* This set of options configures the range of sector size to be supported. */

#define FF_LBA64		0
/* This option switches support for 64-bit LBA. (0:Disable or 1:Enable) */

#define FF_MIN_GPT		0x10000000
/* Minimum number of sectors to switch GPT as partitioning format. */

#define FF_USE_TRIM		0
/* This option switches support for ATA-TRIM. (0:Disable or 1:Enable) */

/*---------------------------------------------------------------------------/
/ System Configurations
/---------------------------------------------------------------------------*/

#define FF_FS_TINY		0
/* This option switches tiny buffer configuration. (0:Normal or 1:Tiny) */

#define FF_FS_EXFAT		0
/* This option switches support for exFAT filesystem. (0:Disable or 1:Enable) */

#define FF_FS_NORTC		0
#define FF_NORTC_MON	1
#define FF_NORTC_MDAY	1
#define FF_NORTC_YEAR	2020
/* The option FF_FS_NORTC switches timestamp functiton. */

#define FF_FS_NOFSINFO	0
/* If you need to know correct free space on the FAT32 volume, set bit 0 of this option. */

#define FF_FS_LOCK		0
/* The option FF_FS_LOCK switches file lock function. */

/* #include <somertos.h>	// O/S definitions */
#define FF_FS_REENTRANT	0
#define FF_FS_TIMEOUT	1000
#define FF_SYNC_t		HANDLE
/* The option FF_FS_REENTRANT switches the re-entrancy (thread safe) of the FatFs module. */

/*--- End of configuration options ---*/
