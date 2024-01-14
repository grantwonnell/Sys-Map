# Sys Map

* Uses a function pointer argument for when it gets a new writeable directory
* The whole directory mapping function is 63 lines in itself
* The algorithm is a lot more efficient than normal recursive scanning
* Algorithm: Scan dirs -> return array of all dirs -> scan new array

# Usage
```
void Print(char *Path) {
    puts(Path);
}

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
```
