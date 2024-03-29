/*
 * Josh Deare
 * dearej@onid.orst.edu
 * Homewrk 3
 * CS344-400
 */

#include "myar.h"

#define _POSIX_SOURCE

// converts a non terminated char array with data length
// of num_elements to an int of base specified
int char_array_to_int(char* arr, int num_elements, int base)
{
	char* copy_arr = (char*)malloc(sizeof(char) * (num_elements+1));
	memcpy(copy_arr, arr, sizeof(char) * num_elements);
	copy_arr[num_elements] = '\0';

	return strtol(copy_arr, NULL, base);
}

// referenced how to memcpy a 2d array here:
// http://stackoverflow.com/questions/16896209/how-to-memcpy-the-two-dimensional-array-in-c
char** get_named_files(char **argv, int option_index)
{
//	char** files = (char**)malloc(num_file_args * sizeof(char*));
//
//	// this parses all argv from the end of '-' options and
//	// values to the end of argv
//	for(int i = option_index; i < argc; i++)
//	{
//		files[i - option_index] = (char*)malloc(sizeof(argv[i]));
//		memcpy(files[i - option_index], argv[i], sizeof(argv[i]));
//	}

	return argv + (sizeof(char) * option_index);
}

// Returns a \0 terminated string of
// the ar_name field in the header
char* get_ar_name(struct ar_hdr* header)
{
	int i;

	// Find how many chars are in the file name
	for(i = 0; i < 16; i++)
	{
		if(header->ar_name[i] == '/')
		{
			break;
		}
	}

	// i+1 is used to ensure room for a \0 at the end
	char* name = (char*)malloc(sizeof(char) * (i+1));

	// copy all the text data of the name to name
	memcpy(name, header->ar_name, sizeof(char) * i);

	// \0 terminate the char* to make it a string
	name[i] = '\0';

	return name;
}

int get_ar_date(struct ar_hdr* header)
{
	int i;

	// Find how many chars are in the file name
	for(i = 0; i < 12; i++)
	{
		if(header->ar_date[i] == ' ')
		{
			break;
		}
	}

	return char_array_to_int(header->ar_date, i, 10);
}

int get_ar_uid(struct ar_hdr* header)
{
	int i;

	// Find how many chars are in the file name
	for(i = 0; i < 6; i++)
	{
		if(header->ar_uid[i] == ' ')
		{
			break;
		}
	}

	return char_array_to_int(header->ar_uid, i, 10);
}

int get_ar_gid(struct ar_hdr* header)
{
	int i;

	// Find how many chars are in the file name
	for(i = 0; i < 6; i++)
	{
		if(header->ar_gid[i] == ' ')
		{
			break;
		}
	}

	return char_array_to_int(header->ar_gid, i, 10);
}

int get_ar_mode(struct ar_hdr* header)
{
	int i;

	// Find how many chars are in the file name
	for(i = 0; i < 8; i++)
	{
		if(header->ar_mode[i] == ' ')
		{
			break;
		}
	}

	return char_array_to_int(header->ar_mode, i, 8);
}

int get_ar_size(struct ar_hdr* header)
{
	int i;

	// Find how many chars are in the file name
	for(i = 0; i < 8; i++)
	{
		if(header->ar_size[i] == ' ')
		{
			break;
		}
	}

	return char_array_to_int(header->ar_size, i, 10);
}

// Referenced from:
// http://stackoverflow.com/questions/10323060/printing-file-permissions-like-ls-l-using-stat2-in-c
char* get_ar_mode_string(struct ar_hdr* header)
{
	char* mode_str = (char*)malloc(sizeof(char) * 9);
	
	int mode = get_ar_mode(header);
	
	// set a pointer offset for writing
	int length = 0;

	// if the mask matches then write the corresponding char
	// otherwise write the dash
	length += sprintf(length+mode_str, (mode & S_IRUSR) ? "r" : "-");
    	length += sprintf(length+mode_str, (mode & S_IWUSR) ? "w" : "-");
        length += sprintf(length+mode_str, (mode & S_IXUSR) ? "x" : "-");
	length += sprintf(length+mode_str, (mode & S_IRGRP) ? "r" : "-");
	length += sprintf(length+mode_str, (mode & S_IWGRP) ? "w" : "-");
	length += sprintf(length+mode_str, (mode & S_IXGRP) ? "x" : "-");
	length += sprintf(length+mode_str, (mode & S_IROTH) ? "r" : "-");
	length += sprintf(length+mode_str, (mode & S_IWOTH) ? "w" : "-");
	length += sprintf(length+mode_str, (mode & S_IXOTH) ? "x" : "-");

	return mode_str;

}

// get the ar_date string formatted
char* get_ar_date_string(struct ar_hdr* header)
{
	time_t date_int = (time_t)get_ar_date(header);
	struct tm* time_st = localtime(&date_int);

	char* time_str = (char*)malloc(sizeof(char) * 20);
	
	if(strftime(time_str, 20, "%b %d %H:%M %Y", time_st) == 0)
	{
		perror("Could not format date properly");
		exit(1);
	}
	return time_str;
}




// assuming we are seeked to a header
// otherwise, return NULL
//struct ar_hdr* read_header(int fd)
//{
//	struct ar_hdr* header = (struct ar_hdr*)malloc(sizeof(struct ar_hdr));
//
//	// Read header char_array, and exit if we don't
//	// read 60 bytes of data
//	if(read(fd, header, 60) < 0)
//	{
//		perror("Improper archive file.");
//		exit(1);
//	}
//
//	return header;
//}

// assuming we are seeked to a header
// otherwise, return NULL
int read_header(int fd, struct ar_hdr* header)
{
	int read_val = read(fd, header, 60);

	// Read header char_array, and exit if we don't
	// read 60 bytes of data
	if(read_val == -1)
	{
		perror("Improper archive file.");
		exit(1);
	}

	return read_val;
}


// assuming fd is seeked to a member
// we seek to the start of the next header
// we return the output of lseek, 
//
// note: this is how we handle the odd/even issue
// on a skip
int skip_body(int fd, struct ar_hdr* header)
{
	int num_bytes = get_ar_size(header);
	
	// account for \n pad for odd sizes
	if((num_bytes % 2) != 0)
	{
		num_bytes++;
	}

	int ret = lseek(fd, num_bytes, SEEK_CUR);

	if(ret == -1)
	{
		perror("Problem seeking over body");
		exit(1);
	}

	return ret;
}


// This is called everytime we open an
// archive for reading
int confirm_armag(int fd)
{
	char* buffer = (char*)malloc(sizeof(char) * SARMAG);

	int ret = read(fd, buffer, SARMAG);

	if(ret == -1)
	{
		perror("Not a valid archive file");
		exit(1);
	}
	else if(ret == 0)
	{
		printf("Empty Archive File.");
		exit(0);
	}

	return ! strcmp(buffer, ARMAG);

}

// easier way to call the stat of a file
// since we will always be using fstat
// and inserting into the buffer
struct stat* get_stat(int fd)
{
	struct stat* ret_val = (struct stat*)malloc(sizeof(struct stat));

	if(fstat(fd, ret_val) == -1)
	{
		perror("Problem getting stat on file: ");
		exit(1);
	}

	return ret_val;

}



// given a file_name, return an ar_hdr struct
struct ar_hdr* file_to_ar_hdr(char* file_name)
{
	// initialize structs
	struct stat* file_stat = (struct stat*)malloc(sizeof(struct stat));
	struct ar_hdr* header = (struct ar_hdr*)malloc(sizeof(struct ar_hdr));

	// populate and error check stat
	if(stat(file_name, file_stat) == -1)
	{
		perror("Problem getting stat of file:");
		exit(1);
	}

	// get the file basename
	char* file_base = (char*)malloc(sizeof(char) * 16);
	snprintf(file_base, 16, "%s/", basename(file_name));
	
	// populate the ar_hdr struct
	// note: we have to increase length to n+1
	// because snprintf reserves the last byte for \n
	// and we leave the ARFMAG for writing last without
	// snprintf
	snprintf(header->ar_name, 17, "%-16s", file_base);
	snprintf(header->ar_date, 13, "%-12ld", file_stat->st_mtime);
	snprintf(header->ar_uid, 7, "%-6i", file_stat->st_uid);
	snprintf(header->ar_gid, 7, "%-6i", file_stat->st_gid);
	snprintf(header->ar_mode, 9, "%-8o", file_stat->st_mode);
	snprintf(header->ar_size, 11, "%-10ld", file_stat->st_size);

	// write the ARFMAG
	memcpy(header->ar_fmag, ARFMAG, sizeof(char) * 2);
	
	return header;
}

// pass a header, char** and *num_files
// returns the matching char, removes it from files
// and decrements num_files by 1
char* pop_from_files(struct ar_hdr* header, char** files, int* num_files)
{
	char* name = get_ar_name(header);
	for(int i = 0; i < *num_files; i++)
	{
		if(strcmp(name, files[i]) == 0)
		{
			char* temp = files[i];
			for(int j = i; j < *num_files - 1; j++)
			{
				files[j] = files[j+1];
			}
			*num_files = *num_files - 1;
			return temp;
		}
	}
	return NULL;
}

int body_to_buffer(int fd_ar, char* buffer, struct ar_hdr* header)
{
	int read_val = read(fd_ar, buffer, get_ar_size(header));
	if(read_val != get_ar_size(header))
	{
		perror("Error reading body");
		exit(1);
	}
	return read_val;
}
void write_body_to_file(int fd_in, int fd_out, struct ar_hdr* header)
{
	char* buffer = (char*)malloc(get_ar_size(header));
	body_to_buffer(fd_in, buffer, header);
	if(write(fd_out, buffer, get_ar_size(header)) == -1)
	{
		perror("Error moving archive body to another file");
		exit(1);
	}
	
}

// Modify proper mode and time to file
// as well as UID and GID
// we assume the ar_name in header is a file
// in the pwd
void mod_mode_and_time(struct ar_hdr* header)
{
	char* filename = get_ar_name(header);

	// update UID and GID
	if(chown(filename, get_ar_uid(header), get_ar_gid(header)) == -1)
	{
		perror("Problem changing file UID and GID");
		exit(1);
	}

	// update mode
	if(chmod(filename, get_ar_mode(header)) == -1)
	{
		perror("Problem changing file perms");
		exit(1);
	}

	// update time
	struct utimbuf* times = (struct utimbuf*)malloc(sizeof(struct utimbuf));
	times->actime = get_ar_date(header);
	time_t now;
	time(&now);
	times->modtime = now;
	utime(filename, times);
}

void extract_member(int fd_ar, struct ar_hdr* header)
{
	// overwriting file as per @154
	int fd_out = open(get_ar_name(header), O_WRONLY | O_TRUNC | O_CREAT);
	if(fd_out == -1)
	{
		perror("Fault initializing file descriptor");
		exit(1);
	}
	write_body_to_file(fd_ar, fd_out, header);

	// remember to handle odd/even
	if((get_ar_size(header) % 2) != 0)
	{
		if(lseek(fd_ar, 1, SEEK_CUR) == -1)
		{
			perror("Problem tracking over odd byte");
			exit(1);
		}
	}

	if(close(fd_out) == -1)
	{
		perror("Problem closing output file");
		exit(1);
	}

	// restoring permissions and time
	mod_mode_and_time(header);

}


// fd_ar must be seeked to a correct position
// to write a file_in
void write_to_archive(int fd_ar, char* file_in)
{
	struct ar_hdr* header = file_to_ar_hdr(file_in);

	if(write(fd_ar, header, sizeof(struct ar_hdr)) != sizeof(struct ar_hdr))
	{
		perror("Problem writing header");
		exit(1);
	}
	// open the file to be written to the archive
	int fd_in = open(file_in, O_RDONLY);

	// initialize the buffer
	char* buffer = (char*)malloc(BLOCK_SIZE);

	while(read(fd_in, buffer, BLOCK_SIZE) == BLOCK_SIZE)
	{
		if(write(fd_ar, buffer, BLOCK_SIZE) != BLOCK_SIZE)
		{
			perror("Error writing file to archive.");
			exit(1);
		}
	}

	if((get_ar_size(header) % 2) != 0)
	{
		if(write(fd_ar, "\n", sizeof(char)) != sizeof(char))
		{
			perror("Error writing newline pad to odd size file");
			exit(1);
		}
	}
	
	if(close(fd_in) == -1)
	{
		perror("Problem closing input file");
		exit(1);
	}
}


void print_concise(char* archive_name)
{
	int fd = open(archive_name, O_RDONLY);

	if(! confirm_armag(fd))
	{
		perror("The given file is not a valid archive");
		exit(1);
	}

	struct ar_hdr* header = (struct ar_hdr*)malloc(sizeof(struct ar_hdr));

	while(read_header(fd, header))
	{
		printf("%s\n", get_ar_name(header));
		skip_body(fd, header);
	}

	if(close(fd) == -1)
	{
		perror("Couldn't close archive");
		exit(1);
	}
}


void print_verbose(char* archive_name)
{
	int fd = open(archive_name, O_RDONLY);

	if(! confirm_armag(fd))
	{
		perror("The given file is not a valid archive");
		exit(1);
	}

	struct ar_hdr* header = (struct ar_hdr*)malloc(sizeof(struct ar_hdr));

	while(read_header(fd, header)) {
		printf("%s %d/%d %6i %s %s\n",
				get_ar_mode_string(header),
				get_ar_uid(header),
				get_ar_gid(header),
				get_ar_size(header),
				get_ar_date_string(header),
				get_ar_name(header));
		skip_body(fd, header);
	} 
	
	if(close(fd) == -1)
	{
		perror("Couldn't close archive");
		exit(1);
	}
}

void quick_append_named(char* archive, char** files, int num_files)
{
	int fd;
	fd = open(archive, O_WRONLY | O_APPEND | O_EXCL | O_CREAT, 0666);
	
	// handle the case of file existing
	if(fd == -1)
	{
		fd = open(archive, O_WRONLY | O_APPEND);
		if(fd == -1)
		{
			perror("Archive not opening properly");
			exit(1);
		}
	}
	else if(write(fd, ARMAG, SARMAG) != SARMAG)
	{
		perror("Archive not allowing use to write ARMAG");
		exit(1);
	}

	for(int i = 0; i < num_files; i++)
	{
		write_to_archive(fd, files[i]);
	}

	if(close(fd) == -1)
	{
		perror("Problem closing archive file");
		exit(1);
	}
	
}

// overwrites existing files as
// referenced from @154
void extract_members(char* archive, char**files, int num_files)
{
	int fd = open(archive, O_RDONLY);
	if(fd == -1)
	{
		perror("Problem opening archive");
		exit(1);
	}
	
	if(! confirm_armag(fd))
	{
		perror("The given file is not a valid archive");
		exit(1);
	}

	struct ar_hdr* header = (struct ar_hdr*)malloc(sizeof(struct ar_hdr));

	while(read_header(fd, header))
	{
		if(num_files == 0)
		{
			extract_member(fd, header);
		}
		else
		{
			char* temp = pop_from_files(header, files, &num_files);
			if(temp == NULL)
			{
				skip_body(fd, header);
			}
			else
			{
				extract_member(fd, header);
				// if that pop zeroed us
				// then break or else
				// we will output the rest 
				// of all the files to the
				// end of the archive
				if(num_files == 0)
				{
					break;
				}
			}
		}
	}


	if(close(fd) == -1)
	{
		perror("Couldn't close archive");
		exit(1);
	}
}

void delete_members(char* archive, char** files, int num_files)
{
	// cleanly handle no passed files
	if(num_files == 0)
	{
		return;
	}

	int fd_ar = open(archive, O_RDONLY);
	if(fd_ar == -1)
	{
		perror("Problem opening archive");
		exit(1);
	}
	struct stat* ar_stat = (struct stat*)malloc(sizeof(struct stat));
	if(fstat(fd_ar, ar_stat) == -1)
	{
		perror("Problem statting archive");
		exit(1);
	}
	
	// use calloc so we know we have \0 at all unused spots when we
	// write to the new file
	char* buffer = (char*)calloc(ar_stat->st_size, sizeof(char));
	int buffer_pos = 0;
	if((buffer_pos += read(fd_ar, buffer+buffer_pos, SARMAG)) == -1)
	{
		perror("Problem reading ARMAG");
		exit(1);
	}

	struct ar_hdr * header = (struct ar_hdr*)malloc(sizeof(struct ar_hdr));
	while(read_header(fd_ar, header))
	{
		char* temp = pop_from_files(header, files, &num_files);
		// Prep the member if not marked for deletion
		if(temp == NULL)
		{
			memcpy(buffer+buffer_pos, header, sizeof(struct ar_hdr));
			buffer_pos += sizeof(struct ar_hdr);
			buffer_pos += body_to_buffer(fd_ar, buffer+buffer_pos, header);
			// handle odd-even items
			if((get_ar_size(header) % 2) != 0)
			{
				buffer[buffer_pos] = '\n';
				buffer_pos += sizeof(char);
			}
		}
		else
		{
			skip_body(fd_ar, header);
		}
	}
	if(close(fd_ar) == -1)
	{
		perror("Problem closing old archive");
		exit(1);
	}
	if(unlink(archive) == -1)
	{
		perror("Problem deleting old archive");
		exit(1);
	}

	int fd_out = open(archive, O_WRONLY | O_CREAT, 0666);
	if(fd_out == -1)
	{
		perror("Problem creating new archive");
		exit(1);
	}
	if(write(fd_out, buffer, buffer_pos) != buffer_pos)
	{
		perror("Problem writing data to new archive");
		exit(1);
	}
	if(close(fd_out) == -1)
	{
		perror("Problem closing new archive");
		exit(1);
	}
}

void append_all(char* archive)
{
	int fd;
	fd = open(archive, O_WRONLY | O_APPEND | O_EXCL | O_CREAT, 0666);
	
	// handle the case of file existing
	if(fd == -1)
	{
		fd = open(archive, O_WRONLY | O_APPEND);
		if(fd == -1)
		{
			perror("Archive not opening properly");
			exit(1);
		}
	}
	else if(write(fd, ARMAG, SARMAG) != SARMAG)
	{
		perror("Archive not allowing use to write ARMAG");
		exit(1);
	}

	// DIR usage referenced from:
	// http://stackoverflow.com/questions/6251019/listing-only-regular-files-problem-with-stat
	DIR* dir = NULL;
	struct dirent* element = NULL;
	// open current dir
	dir = opendir("./");
	if(dir == NULL)
	{
		perror("Problem opening DIR");
		exit(1);
	}
	/*
	 *  Definition of a regular File!
	 *  a regular file is one we would normally open in an editor
	 *  or perhaps a binary file. Basically, it's a not special type
	 *
	 *  It is not a directory, symbolic link, named pipe, socket,
	 *  device file, nor a door.
	 *
	 *  Reference:
	 *  http://en.wikipedia.org/wiki/Unix_file_types
	 */
	struct stat* stat_st = (struct stat*)malloc(sizeof(struct stat));
	
	while((element = readdir(dir)))
	{
		if(stat(element->d_name, stat_st) == -1)
		{
			perror("Problem statting the element");
			exit(1);
		}
		if(S_ISREG(stat_st->st_mode))
		{
			if(strcmp(archive, element->d_name))
			{
				write_to_archive(fd, element->d_name);
			}
		}
	}
}

int main(int argc, char **argv)
{

	if(argc < 2)
	{
  		printf("Usage: myar [-qxd archive_file] [-tvAw] [file1[...]]");
		return 1;
	}

	int c;
	int num_file_args;
	// reference for parsing multiple arguments on the 
	// same option:
	// http://stackoverflow.com/questions/15466782/parsing-command-line-options-with-multiple-arguments-getopt
	while((c = getopt(argc, argv, "q:x:t:v:d:A:w:")) != -1)
	{
		switch(c)
		{
			case 'q':
				num_file_args = argc - optind;
				quick_append_named(
						optarg,
						get_named_files(argv,
							optind),
						num_file_args);
				break;

			case 'x':
				num_file_args = argc - optind;
				extract_members(
						optarg,
						get_named_files(
							argv, 
							optind), 
						num_file_args);
				break;

			case 't':
				print_concise(optarg);
				break;

			case 'v':
				print_verbose(optarg);
				break;

			case 'd':
				num_file_args = argc - optind;
				delete_members(
						optarg,
						get_named_files(
							argv,
							optind),
						num_file_args);
				break;
			case 'A':
				append_all(optarg);
				break;
			case 'w':
				printf("Extra Credit:");
				break;
			case '?':
				printf("Usage: myar [-qxd archive_file] [-tvAw] [file1[...]]");
				break;
			default:
				printf("Usage: myar [-qxd archive_file] [-tvAw] [file1[...]]");
				break;

		}
	}
}
