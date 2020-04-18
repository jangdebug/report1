#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "student.h"

/* 학생 정보를 입력받아 데이터베이스 파일에 저장한다. */
int main(int argc, char *argv[]){
	int fd,num,id;
	char c;
	struct student rec;

	if (argc < 2) {
		fprintf(stderr,  "사용법 : %s file\n", argv[0]);
		exit(1);
	}

	if ((fd = open(argv[1],O_WRONLY |O_CREAT, 0640))==-1) {
		perror(argv[1]);
		exit(2);
	}

	printf("%s\n%s\n%s\n%s\n%s\n%s","[메뉴 선택]", "1. db 생성",  "2. db 질의",  "3. db 갱신","0. 종료","번호 입력:"); 
	scanf("%d",&num);
	if(num==1){
		if ((fd = open(argv[1],O_WRONLY |O_CREAT, 0640))==-1) {
			perror(argv[1]);
			exit(2);
		}
		printf("%-9s %-8s %-4s\n", "학번",  "이름",  "점수"); 
		while (scanf("%d %s %d", &rec.id, rec.name, &rec.score) ==  3) {
			lseek(fd, (rec.id - START_ID) * sizeof(rec), SEEK_SET);
			write(fd, &rec, sizeof(rec) );
			printf("\n---정보 입력이 완료되었습니다.---\n\n");
		}
	}
	else if(num==2){
		if ((fd = open(argv[1], O_RDONLY)) == -1) {
			perror(argv[1]);
			exit(2);
		}
		do {
			printf("\n검색할 학생의 학번 입력:");
			if (scanf("%d", &id) == 1) {
				lseek(fd, (id-START_ID)*sizeof(rec), SEEK_SET);
				if ((read(fd, &rec, sizeof(rec)) > 0) && (rec.id != 0)) 
					printf("학번:%d\t 이름:%s\t 점수:%d\n", rec.id, rec.name, rec.score);
				else printf("레코드 %d 없음\n", id);
			}
			else printf("입력 오류"); 
			printf("계속하겠습니까?(Y/N)");
			scanf(" %c", &c);
		} while (c == 'Y');
	}
	else if(num==3){
		if ((fd = open(argv[1], O_RDWR)) == -1) {
			perror(argv[1]);
			exit(2);
		}
		do {
			printf("수정할 학생의 학번 입력: ");
			if (scanf("%d", &id) == 1) {
				lseek(fd,  (long) (id-START_ID)*sizeof(rec), SEEK_SET);
				if ((read(fd, &rec, sizeof(rec)) > 0) && (rec.id != 0)) {
					printf("학번:%8d\t 이름:%4s\t 점수:%4d\n",rec.id, rec.name, rec.score);
					printf("새로운 점수: ");
					scanf("%d", &rec.score);
					lseek(fd, (long) -sizeof(rec), SEEK_CUR);
					write(fd, &rec, sizeof(rec));
					printf("\n---수정이 완료되었습니다---\n");
				}
				else printf("레코드 %d 없음\n", id);
			}
			else printf("입력오류\n");
			printf("계속하겠습니까?(Y/N)");
			scanf(" %c",&c);
		} while (c == 'Y');
	}
	else if(num==0)
		printf("db를 종료합니다.\n");
	close(fd);
	exit(0);
}
