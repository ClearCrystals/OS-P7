#include "ext2_fs.h"
#include "read_ext2.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

/*
PART 1 DONE...
Current Issue: The program compiles but it prints "read_super_block: Not a Ext2 filesystem". After
looking at the read_ext2.c file, I put print statements for the super.s_magic and EX2_SUPER_MAGIC. The former is 0 
and the latter prints 61267 so need help figuring that out.
*/

void writeJPG(int fd, int inodeNumber, char *outputDir) {

    char file[256];
    snprintf(file, sizeof(file), "%s/file-%d.jpg", outputDir, inodeNumber);
    int out_fd = open(file, O_CREAT | O_WRONLY, 0644);
    if(out_fd < 0) {
        fprintf(stderr, "OUTPUT FILE OPENING ERROR %s\n", file);
        return;
    }

    //retrieve inode of the file
    struct ext2_inode inode;
    struct ext2_group_desc group;
    read_inode(fd, locate_inode_table(0, &group), inodeNumber, &inode);

    //check if inode is a regular file
    if(!(S_ISREG(inode.i_mode))) {
        return;
    }

    //check if file is a JPG file
    char buffer[1024];
    int blockNumber = inode.i_block[0];
    pread(fd, buffer, 1024, BLOCK_OFFSET(blockNumber));
    //Below given on canvas
    int is_jpg = 0; 
    if (buffer[0] == (char)0xff &&
        buffer[1] == (char)0xd8 &&
        buffer[2] == (char)0xff &&
        (buffer[3] == (char)0xe0 ||
        buffer[3] == (char)0xe1 ||
        buffer[3] == (char)0xe8)) 
    {
        is_jpg = 1;
    }
    if(is_jpg == 1) {
        write(out_fd, buffer, inode.i_size);
    }
    close(out_fd);
}

int main(int argc, char **argv) 
{
    if (argc != 3) 
    {
        printf("expected usage: ./runscan inputfile outputfile\n");
        exit(0);
    }

    /* This is some boilerplate code to help you get started, feel free to modify
       as needed! */

    int fd;
    fd = open(argv[1], O_RDONLY);    /* open disk image */

    ext2_read_init(fd);

    struct ext2_super_block super;
    struct ext2_group_desc group;

    // example read first the super-block and group-descriptor
    read_super_block(fd, 0, &super);
    read_group_desc(fd, 0, &group);

    //scan inodes from table
    for(int i = 0; i < (int) super.s_inodes_count; i++) {
        writeJPG(fd, i + 1, argv[2]);
    }
    //close disk image
    close(fd);
    
    return 0;
}
