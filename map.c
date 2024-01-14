#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

#include <linux/types.h>

#include "map.h"
#include "bool.h"

BOOL UtilCanWrite(char *Path);
int UtilCharCount(const char *String, char c);

static void CleanupArr(char **arr, size_t len) {
	for(int i = 0; i < len; i++)
		free(arr[i]);

	free(arr);
}

static BOOL MapScanArr(dirctx_t *ctx) { /* Main function | returns boolean if it can continue or not */
	struct dirent *files;
	char path[PATH_MAX] = {0};
	char **array = calloc(1, sizeof(char *));

	ctx->new_array_size = 0;

	for(int i = 0; i < ctx->array_size; i++) {
		if(UtilCharCount(ctx->array[i], '/') >= ctx->config->dirs_limit && ctx->config->dirs_limit != -1 && (ctx->config->dirs_limit == 1 && ctx->ran == TRUE)) {
			free(array);
			CleanupArr(ctx->array, ctx->array_size);
			return FALSE;
		}

		if(!strcmp(ctx->array[i], "/proc/") || !strcmp(ctx->array[i], "/dev/fd"))
			continue;

		DIR *dir;
		if((dir = opendir(ctx->array[i])) == NULL)
			continue;

		while((files = readdir(dir))) {
			if(ctx->total_dirs_ran++ >= ctx->config->total_limit && ctx->config->total_limit != -1) {
				CleanupArr(array, ctx->new_array_size);

				if(ctx->ran == TRUE)
					CleanupArr(ctx->array, ctx->array_size);
					
				closedir(dir);
				return FALSE;
			}

			if(!strcmp(files->d_name, ".") || !strcmp(files->d_name, "..")) /* ignore */
				continue;

			strcpy(path, ctx->array[i]);
			
			if(strcmp(ctx->array[i], "/"))
				strcat(path, "/");

			strcat(path, files->d_name);

			if(files->d_type == DT_DIR) { /* is a directory */
				array = realloc(array, (ctx->new_array_size + 1) * sizeof(char *));
				array[ctx->new_array_size++] = strdup(path);
				
				if(UtilCanWrite(path))
					ctx->config->fptr(array[ctx->new_array_size - 1]);
			}
		}

		closedir(dir);
	}

	if(ctx->ran == TRUE)
		CleanupArr(ctx->array, ctx->array_size); /* All previous memory is freed */
	else
		ctx->ran = TRUE;

	ctx->array = array;
	ctx->array_size = ctx->new_array_size;

	return TRUE;
}

/* wrappers */
void MapScanAll(mapconfig_t *config) {
	char *Begin[] = {"/"};

	dirctx_t ctx = {
		.ran = FALSE,
		.array = Begin,
		.array_size = 1,
		.config = config,
	};

	BOOL ret;

	do {
		ret = MapScanArr(&ctx);
	} while(ret != FALSE);
}

void MapScanFromArr(mapconfig_t *config, char *Arr[], size_t Size) {
	dirctx_t ctx = {
		.ran = FALSE,
		.array = Arr,
		.array_size = Size,
		.config = config,
	};

	BOOL ret;

	do {
		ret = MapScanArr(&ctx);
	} while(ret != FALSE);
}

/* utils */
int UtilCharCount(const char *String, char c) {
	int count = 0;

	while(*String) {
		if(*String++ == c)
			count++;
	}

	return count;
}

BOOL UtilCanWrite(char *Path) {
	char path[PATH_MAX] = {0};

	strcpy(path, Path);
	strcat(path, "/");
	strcat(path, CREATE_PATH);

	int fd;
	if((fd = open(path, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1)
		return FALSE;

	close(fd);
	unlink(path);
	return TRUE;
}

void Print(char *Path) {
	puts(Path);
}

int main(void) {
	MapScanAll(&(mapconfig_t){
		.dirs_limit = 3, /* if a path has over 3 '/' it will stop running */
		.total_limit = 100, /* if 100 dirs were scanned it will stop running */
		.fptr = Print,
	});

	char *DirArr[] = {"/root", "/tmp", "/usr"};

	MapScanFromArr(&(mapconfig_t){
		.dirs_limit = 3, /* if a path has over 3 '/' it will stop running */
		.total_limit = 100, /* if 100 dirs were scanned it will stop running */
		.fptr = Print,
	}, DirArr, sizeof(DirArr)/sizeof(DirArr[0]));
}
