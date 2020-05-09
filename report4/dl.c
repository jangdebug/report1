#include <sys/types.h> 
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<fcntl.h>
#include <time.h>
#include <math.h>

char type(mode_t);		//파일 타입
char *perm(mode_t);		//파일 허가권

/*	--- struct stat에 포함된 파일 정보 ---
struct stat {
     dev_t           st_dev;       		장치 파일의 위치 및 여부
     ino_t           st_ino;       		파일의 inode 번호
     mode_t        	 st_mode;     		파일의 모드
     nlink_t         st_nlink;			파일의 하드링크 수
     uid_t           st_uid;      		user ID
     gid_t           st_gid;     		group ID
     dev_t           st_rdev;    		장치 파일(inode)를 기술
     off_t           st_size;    		파일의 사이즈
     blksize_t       st_blksize;   		효율적인 I/O 파일 시스템 위한 블럭 사이즈
     blkcnt_t        st_blocks;   		파일에 할당된 블럭 수 
     time_t;         st_atime;    		마지막 접근 시간
     time_t;         st_mtime;   		마지막 수정 시간
     time_t          st_xtime;    		마지막 상태 변화 시간
};
*/

void ls_lblock(struct stat *st){printf("%5ld ", st->st_blocks);}	
void ls_ltype(struct stat *st){ printf("%c%s ", type(st->st_mode), perm(st->st_mode)); }
void ls_llink(struct stat *st){ printf("%ld ", st->st_nlink); }
void ls_lugid(struct stat *st){ printf("%s %s ", getpwuid(st->st_uid)->pw_name, getgrgid(st->st_gid)->gr_name); }
void ls_luid(struct stat *st){ printf("%s ", getpwuid(st->st_uid)->pw_name); }
void ls_lsize(struct stat *st){ printf("%5ld ", st->st_size); }
void ls_ltime(struct stat *st){ printf("%.12s ", ctime(&st->st_mtime)+4); }
void ls_lnode(struct stat *st){printf("%17ld ",  st->st_ino);}
void ls_lhuman(struct stat *st)
{
	double i = (st->st_size);
	if (i >= pow(10, 9)){			//math.h 함수 - pow(a,b) : a^b
		i /= pow(10, 9);
		printf("%4.1f", i);
		printf("G ");
	}
	else if(i >= pow(10, 6)){
		i /= pow(10, 6);
		printf("%4.1f", i);
		printf("M ");
	}
	else if(i >= pow(10, 3)){
		i /= pow(10, 3);
		printf("%4.1f", i);
		printf("K ");
	}
	else
		ls_lsize(st);
}


//	참조 블로그 : https://big-sun.tistory.com/27		(옵션 종류) //
void Is_option(struct dirent d, struct stat st, char * option)
{																		// a : 숨김 파일 포함
	if((strcmp(option, "-al") == 0) || (strcmp(option, "-la") == 0))	// l 옵션		
	{
		ls_ltype(&st);		// 각 파일의 모드
		ls_llink(&st);		// 링크수
		ls_lugid(&st);		// 소유자, 그룹
		ls_lsize(&st);		// 크기(바이트)
		ls_ltime(&st);		// 최종 수정 시간
	}
	else if ((strcmp(option, "-ai")==0) || (strcmp(option, "-ia")==0))	// i옵션 
	{
		ls_lnode(&st);		// inode(고유 번호) 출력
	}
	else if (strcmp(option,"-alh")==0)									// h 옵션 
		{
		ls_ltype(&st); 
		ls_llink(&st);
		ls_lugid(&st);
		ls_lhuman(&st);		// 바이트가 아닌 K, M, G 단위를 사용하여 파일 크기를 사람이 보기 좋게 표시
		ls_ltime(&st);
	}
	else if ((strcmp(option, "-ao")==0) || (strcmp(option, "-oa")==0))	// o 옵션 
	{
		ls_ltype(&st); 
		ls_llink(&st);
		ls_luid(&st);		// 그룹 제외 소유만 출력
		ls_lsize(&st);
		ls_ltime(&st);
	}		
}

// 	참고 블로그 : https://go-it.tistory.com/4 *	//

/* 디렉토리 내용을 자세히 리스트한다. */
/* a옵션을 무조건 포함하는 코드 */

int main(int argc, char *argv[]) 		//argc : main 함수에 전달된 인자의 개수
										//**argv = *argv[] : 가변적인 개수의 문자열 (전해지는 데이터가 모두 문자열로 전해짐)
{
 	char * dir = (char *)malloc(sizeof(char) * 1024);
	memset(dir, 0, 1024);		//void * memset (void  ptr, int value, size_t num);
								//(시작 포인터, 메모리에 넣을 값, 넣고자 하는 메모리 크기)
								//특정 범위에 있는 연속된 메모리에 값을 지정하고 싶을 때 사용
	
	DIR *dp = NULL;				//디렉토리	
 	struct dirent *d = NULL;	//파일	

								/*	참고 블로그 : https://sosal.kr/114
								struct dirent{
   	 							long d_ino;                            아이노드
  								off_t d_off;                             dirent 의 offset
    							unsigned short d_reclen;           d_name 의 길이
   								char d_name [NAME_MAX+1];   파일 이름(없다면 NULL로 종료) 
								}
								*/	
			
 	struct stat st;		
  	char path[BUFSIZ+1];	
	
	getcwd(dir, 1024);			//char * getcwd(char *buf, size_t size);
								//현재 작업 디렉토리의 이름 size만큼 길이로 buf에 복사
								//size_t : 해당 시스템에서 어떤 객체나 값이 포함할 수 있는 최대 크기의 데이터를 표현하는 타입
								//		   unsigned 형으로 나타낸다.
					
	if ((dp = opendir(dir)) == NULL)	// 디렉토리 열기 
		perror(dir);					// 오류 메세지 출력	
	
	while ((d = readdir(dp)) != NULL) {				// 디렉토리 내의 각 파일에 대해 
		sprintf(path, "%s/%s", dir, d->d_name); 	// 파일 경로명 만들기 
		if (lstat(path, &st) < 0) 					// 파일 상태 정보 가져오기  
			perror(path);							// 오류 메세지 출력	
		if(argc>1)		
			Is_option(*d, st, argv[1]);				//argv[0] = dl	/ argv[1] = option			
		printf("%s\n", d->d_name);			
	}
	
	free(dir);
	closedir(dp);
	exit(0);
}

/* 파일 타입을 리턴 */
char type(mode_t mode) {		

	if (S_ISREG(mode)) 
		return('-');	// 일반 파일
	if (S_ISDIR(mode)) 
		return('d');	// 디렉토리
	if (S_ISCHR(mode)) 
		return('c');	// 문자 특수 파일
	if (S_ISBLK(mode)) 
		return('b');	// 블록 특수 파일
	if (S_ISLNK(mode)) 
		return('l');	// 기호 링크
	if (S_ISFIFO(mode)) 
		return('p');	// 선입선출(FIFO) 특수 파일
	if (S_ISSOCK(mode)) 
		return('s');	// 로컬 소켓
}

/* 파일 허가권을 리턴 */
char* perm(mode_t mode) {				
	int i;
	static char perms[10]; 

	strcpy(perms, "---------");

	for (i=0; i < 3; i++) {
		if (mode & (S_IREAD >> i*3)) 
			perms[i*3] = 'r';				// 읽기 권한
		if (mode & (S_IWRITE >> i*3)) 
			perms[i*3+1] = 'w';				// 쓰기 권한
		if (mode & (S_IEXEC >> i*3)) 
			perms[i*3+2] = 'x';				// 접근 권한
	}
	return(perms);
}
