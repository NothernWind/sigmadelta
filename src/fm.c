/*!
 --------------------------------------------------------------------
 \file		fmgr
 \author	Navrotski Mikalai
 \version	0.0.0
 \date
 \brief
 --------------------------------------------------------------------
 */
#include "fm.h"

// Variables --------------------------------------------------------
char fm_header[38];
char FM_List[18][257];
char FM_Path[1024];
int FM_Cursor_Pos;
int FM_Objects;
int path_lenght;

int FM_Folders;
int FM_Files;
int FM_Window_Offset;

t_gpio_in_state FM_SD_State = {0, 0};
FATFS FM_Fatfs[_VOLUMES];
char FM_FS_Mounted = 0;

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void FM_Print_Main_Border(void)
{
	int i, x, y, scrpos;
	
	diplay_fgcolor.c = 0xFFFF;
	display_bgcolor.c = 0;
	
	display_print_char(0, 0, 0xc9);
	display_print_char(39, 0, 0xBB);
	display_print_char(39, 19, 0xBC);
	display_print_char(0, 19, 0xC8);
	
	for (x = 0; x < 38; x++) {
		display_print_char(x + 1, 0, 0xcD);
		display_print_char(x + 1, 19, 0xcD);
	}
	
	for (y = 0; y < 18; y++) {
		display_print_char(0, y + 1, 0xBA);
	}
	
	display_print_char(39, 1, 0xBA);
	display_print_char(39, 2, 0x18);
	
	for (y = 0; y < 14; y++) {
		display_print_char(39, y + 3, 0xB0);
	}
	
	display_print_char(39, 17, 0x19);
	display_print_char(39, 18, 0xBA);

	diplay_fgcolor.c = 0;
	display_bgcolor.c = 0xFFFF;
	
	y = strlen(fm_header);
	x = 20 - (y / 2);
	
	display_print_char(x - 1, 0, ' ');
	
	for (i = 0; fm_header[i]; i++) {
		display_print_char(i + x, 0, fm_header[i]);
	}
	
	display_print_char(i + x, 0, ' ');
	
	diplay_fgcolor.c = 0xFFFF;
	display_bgcolor.c = 0;
	scrpos = (((FM_Window_Offset + FM_Cursor_Pos)* 14 )/ FM_Objects);
	display_print_char(39, 3 + scrpos, 0xDB);
	
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void FM_Print_List(void)
{
	int x, y;
	char f = 0;
	
	diplay_fgcolor.c = 0xFFFF;
	display_bgcolor.c = 0;
	
	for (y = 0; y < 18; y++) {
		for (x = 0; x < 38; x++) {
			if (FM_List[y][x + 1] == 0) f = 1;
			
			if (FM_List[y][0] == 0) {
				diplay_fgcolor.c = 0xFFFF;	
			} else {
				diplay_fgcolor.rgb.red = 0x00;
				diplay_fgcolor.rgb.green = 0x3f;
				diplay_fgcolor.rgb.blue = 0x00;
			}
			
			if (y == FM_Cursor_Pos) {
				display_bgcolor.rgb.red = 0x00;
				display_bgcolor.rgb.green = 0x1f;
				display_bgcolor.rgb.blue = 0x0f;
			} else {
				display_bgcolor.c = 0;
			}
			
			display_print_char(x + 1, y + 1, (f == 0) ? FM_List[y][x + 1] : ' ');
			
		}
		f = 0;
	}
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
FRESULT FM_Scan_Folder(void)
{
	FRESULT res;
	FILINFO finf;
	DIR dir;
	char *fn;
	int i = 0;
	int j = 0;
#if _USE_LFN
	static char lfn[_MAX_LFN + 1];
	
	finf.lfname = lfn;
	finf.lfsize = sizeof(lfn);
#endif
	
	memset(&FM_List[0][0], 0, 18 * 257);
	
	FM_Files = 0;
	
	res = f_opendir(&dir, FM_Path);
	
	if (res != FR_OK) return res;
	
	FM_Folders = 0;
	FM_Files = 0;
	
	while(1) {// Посчитаем файлы и каталоги
		res = f_readdir(&dir, &finf);
		if (res != FR_OK || finf.fname[0] == 0) break;
		if (finf.fname[0] == '.') continue;
		
		if (finf.fattrib & AM_DIR) 
			FM_Folders++;
		else FM_Files++;
		
	}

	FM_Objects = FM_Files + FM_Folders;
	
	if (FM_Folders > FM_Window_Offset) {
	
		res = f_readdir(&dir, 0);
		if (res != FR_OK) return res;
		
		if (FM_Window_Offset > 0) {
			j = FM_Window_Offset;
		
			while(1) {
				res = f_readdir(&dir, &finf);
				if (res != FR_OK || finf.fname[0] == 0) break;
				if (finf.fname[0] == '.') continue;
				if (finf.fattrib & AM_DIR) {
					j--;
					if (j == 0 ) break;
				}
			}
		}
		
		while(1) { // Сначала прочитаем все папки
			res = f_readdir(&dir, &finf);
			if (res != FR_OK || finf.fname[0] == 0) break;
			if (finf.fname[0] == '.') continue;
		
#if _USE_LFN
			fn = *finf.lfname ? finf.lfname : finf.fname;
#else
			fn = finf.fname;
#endif
			if (finf.fattrib & AM_DIR) {
				FM_List[i][0] = 0;
				sprintf(&FM_List[i][1], "%s", fn);
				i++;
				if (i >= 18) break;
			}
		}
	}
	
	if (i >= 18) return res;
	
	res = f_readdir(&dir, 0);
	
	if (res != FR_OK) return res;
	
	if (FM_Folders < FM_Window_Offset) {
		j = FM_Window_Offset - FM_Folders;
	
		while(1) {
			res = f_readdir(&dir, &finf);
			if (res != FR_OK || finf.fname[0] == 0) break;
			if (finf.fname[0] == '.') continue;
			if ((finf.fattrib & AM_DIR) == 0) {
				j--;
				if (j == 0 ) break;
			}
		}	
	}
	
	while(1) { // А теперь вычитаем все файлы
		res = f_readdir(&dir, &finf);
		if (res != FR_OK || finf.fname[0] == 0) break;
		if (finf.fname[0] == '.') continue;
		
#if _USE_LFN
		fn = *finf.lfname ? finf.lfname : finf.fname;
#else
		fn = finf.fname;
#endif	
		if ((finf.fattrib & AM_DIR) == 0) {
			FM_List[i][0] = 1;
			sprintf(&FM_List[i][1], "%s", fn);
			i++;
			if (i >= 18) break;
		}
	}
	
	return res;
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void FM_Open_Folder(void)
{
	char * tmp;
	FRESULT res;
	
	tmp = strrchr(FM_Path, '/');
	if (tmp == 0) {
		sprintf(fm_header, "%s", FM_Path);
	} else {
		tmp++;
		if (strlen(tmp) >= 36) {
			strncpy(fm_header, tmp, 33);
			sprintf(&fm_header[33], "...");
		} else {
			sprintf(fm_header, "%s", tmp);
		}
	}
	
	res = FM_Scan_Folder();
	if (res != 0) {
		msg_ff_error(res);
		//SD_Get_Error(diskio_sd_init_error, res);
		//printf("Drive not mounted\n");
		return;
	}

	path_lenght = strlen(FM_Path);

	FM_Print_Main_Border();
	FM_Print_List();
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void FM_Check_SD(void)
{
	unsigned int res;
	
	FM_Cursor_Pos = 0;
	FM_Window_Offset = 0;
	
	GPIO_Debounce(GPIOB, 0, &FM_SD_State);
	
	if (FM_SD_State.event == 0) return;
	FM_SD_State.event = 0;
	
	if (FM_SD_State.state == 0) {
		msg_sd_ejected();
		FM_FS_Mounted = 0;
		return;
	}

	printf("SD-card Inserted\n");
	printf("Mount file system\n");
	
	res = f_mount(&FM_Fatfs[0], "0", 1);
	
	if (res != 0) {
		msg_ff_error(res);
		return;
	}
	
	sprintf(FM_Path, "0:");
	
	FM_FS_Mounted = 1;
	
	printf("Success\n\n");
	printf("Scan files in root folder\n");
	
	FM_Open_Folder();
	
	FM_Cursor_Pos = 0;
	
	while (1) {
		char *tmp;
		
		// Открыть папку или файл
		if (enter_button.pressed == 1) {
			enter_button.pressed = 0;
			// Если это каталог
			if (FM_List[FM_Cursor_Pos][0] == 0) {
				
				path_lenght = strlen(FM_Path);
				
				// Добавляем к пути имя каталога
				sprintf(&FM_Path[path_lenght], "/%s", &FM_List[FM_Cursor_Pos][1]);

				FM_Cursor_Pos = 0;
				
				FM_Open_Folder();
				
			} else { // Это файл
				// проверяем что за файл
				if (strstr(&FM_List[FM_Cursor_Pos][1], ".wav") != 0 ||
					strstr(&FM_List[FM_Cursor_Pos][1], ".WAV") != 0) {
					sprintf(&FM_Path[path_lenght], "/%s", &FM_List[FM_Cursor_Pos][1]);
					
					display_clear();
					WAV_Play(FM_Path);
					
					tmp = strrchr(FM_Path, '/');
					memset(tmp, 0, strlen(tmp));
					
					FM_Print_Main_Border();
					FM_Print_List();
				}
				
				if (strstr(&FM_List[FM_Cursor_Pos][1], ".mp3") != 0 || 
					strstr(&FM_List[FM_Cursor_Pos][1], ".MP3")) {
					sprintf(&FM_Path[path_lenght], "/%s", &FM_List[FM_Cursor_Pos][1]);
					
					display_clear();
					MP3_Play(FM_Path);
					
					tmp = strrchr(FM_Path, '/');
					memset(tmp, 0, strlen(tmp));
					
					FM_Print_Main_Border();
					FM_Print_List();
				}
			}
		}

		// Переместить курсор вверх по списку
		if (up_button.pressed == 1 || up_button.hold == 1) {
			up_button.pressed = 0; 
			up_button.hold = 0;
			
			if (FM_Cursor_Pos > 0) {
				FM_Cursor_Pos--;
				FM_Print_List();
			} else {
				if (FM_Window_Offset > 0) {
					FM_Window_Offset--;
					FM_Open_Folder();
				}
			}
		}

		// Переместить курсор вниз по списку
		if (down_button.pressed == 1 || down_button.hold == 1) {
			down_button.pressed = 0; 
			down_button.hold = 0;	
			
			if (FM_Cursor_Pos < 17 && FM_Cursor_Pos < FM_Objects - 1) {
				FM_Cursor_Pos++;
				FM_Print_List();
			} else {
				if (FM_Window_Offset < FM_Objects - 18) {
					FM_Window_Offset++;
					FM_Open_Folder();
				}
			}	
		}

		// Выход из папки
		if (exit_button.pressed == 1) {
			exit_button.pressed = 0;
			tmp = strrchr(FM_Path, '/');
			memset(tmp, 0, strlen(tmp));
			
			FM_Cursor_Pos = 0;	
			FM_Open_Folder();
		}
		
		GPIO_Debounce(GPIOB, 0, &FM_SD_State);
		if (FM_SD_State.event == 1) {
			FM_SD_State.event = 0;
			if (FM_SD_State.state == 0) {
				msg_sd_ejected();
				FM_FS_Mounted = 0;
				return;
			}
		}
	}
}

/*!
 --------------------------------------------------------------------
 \brief
 
 --------------------------------------------------------------------
 */
void SD_Get_Error(int sd_error, unsigned int ff_error)
{
	printf("[FS] ");
	switch(ff_error) {
		case FR_OK:
			/* (0) Succeeded */
			break;
		case FR_DISK_ERR:
			printf("A hard error occurred in the low level disk I/O layer\n");
			break;
		case FR_INT_ERR:
			printf("Assertion failed\n");
			break;
		case FR_NOT_READY:
			printf("The physical drive cannot work\n");
			break;
		case FR_NO_FILE:
			printf("Could not find the file\n");
			break;
		case FR_NO_PATH:
			printf("Could not find the path\n");
			break;
		case FR_INVALID_NAME:
			printf("The path name format is invalid\n");
			break;
		case FR_DENIED:
			printf("Access denied due to prohibited access or directory full\n");
			break;
		case FR_EXIST:
			printf("Access denied due to prohibited access\n");
			break;
		case FR_INVALID_OBJECT:
			printf("The file/directory object is invalid\n");
			break;
		case FR_WRITE_PROTECTED:
			printf("The physical drive is write protected\n");
			break;
		case FR_INVALID_DRIVE:
			printf("The logical drive number is invalid\n");
			break;
		case FR_NOT_ENABLED:
			printf("The volume has no work area\n");
			break;
		case FR_NO_FILESYSTEM:
			printf("There is no valid FAT volume\n");
			break;
		case FR_MKFS_ABORTED:
			printf("The f_mkfs() aborted due to any parameter error\n");
			break;
		case FR_TIMEOUT:
			printf("Could not get a grant to access the volume within defined period\n");
			break;
		case FR_LOCKED:
			printf("The operation is rejected according to the file sharing policy\n");
			break;
		case FR_NOT_ENOUGH_CORE:
			printf("LFN working buffer could not be allocated\n");
			break;
		case FR_TOO_MANY_OPEN_FILES:
			printf("Number of open files > _FS_SHARE\n");
			break;
		case FR_INVALID_PARAMETER:
			printf("Given parameter is invalid\n");
			break;
		default:break;
	}

	if (sd_error != 0) {
		printf("[SD Initialization Error]\n");
	
		switch(sd_error) {
			case 0: 
				break;
			case 1: 
				printf("Reset error;\n");
				break;
			case 2:
				printf("Check version error\n");
				printf("R7 Respunse: 0x%08X\n", SD_R7_Response);
				break;
			case 3: 
			case 4: 
				printf("Read OCR Register error\n");
				printf("OCR Register: 0x%08X\n", SD_OCR_Register);
				break;
			case 5: 
				printf("Non-compatible voltage range or check pattern errorCard\n");
				printf("OCR Register: 0x%08X\n", SD_OCR_Register);
				break;
			case 6: 
				printf("Initialization timeout error\n");
				break;
			case 7: 
				printf("Initialization unknown error\n");
				break;
			case 8: 
			case 9:
				printf("Read OCR Register error\n");
				printf("OCR Register: 0x%08X\n", SD_OCR_Register);
				break;
			case 10: 
				printf("Non-compatible voltage range or check pattern errorCard\n");
				printf("OCR Register: 0x%08X\n", SD_OCR_Register);
				break;
			case 11:
				printf("SDSC Cards is temporary not supported\n");
				//printf("Set block lenght error\n");
				break;
			case 12:
				printf("Read CSD Register error\n");
				break;
			case 13:
				printf("Read CID Register error\n");
				break;
			default: 
				printf("Unknown error\n");
				break;
		}
	}
	
	if (SD_ErrorCode != 0) {
		printf("[SD Error]\n");
		
		switch(SD_ErrorCode) {
			case 0:
				break;
			case 1:
				printf("SD_TIMEOUT_ERROR\n"); 
				break;
			case 2:
				printf("SD_BAD_VOLTAGE\n");
				break;
			case 3:
				printf("SD_NOT_INITIALIZED\n");
				break;
			case 4:
				printf("SD_READ_ERROR\n");
				break;
			
			case 20: 
				printf("SD_UNKNOWN_ERROR\n");
				break;
			default:
				printf("Unknown error code: %d\n", SD_ErrorCode);
				break;
		}
		
		printf("R1 Respunse: 0x%02X\n", SD_R1_Response);
	}
}


