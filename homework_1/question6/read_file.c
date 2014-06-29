/*

  $RCSfile$
  $Revision$
  $Author$
  $Date$
  $Log$

  from:
  http://www.programmingsimplified.com/c-program-read-file

 */

#include "read_file.h"

int main()
{
   char ch, file_name[75];
   //FILE *fp;
   int fd;
   int num_read;
 
   printf("Enter the name of file you wish to see\n");
   fgets(file_name, sizeof(file_name), stdin);
   int i;
   for (i = 0; i < sizeof(file_name); i++)
       {	
       printf("%c", file_name[i]);
       }
   fd = open(file_name, O_RDONLY);
   if (fd < 0)
     {
       perror("Error while opening the file.\n");
       exit(EXIT_FAILURE);
     }
   printf("The contents of %s file are :\n", file_name);

   for (;;)
     {
       num_read = read(fd, &ch, sizeof(ch));
       if (num_read < 0)
	 {
	   perror("Error while reading the file.\n");
	   exit(EXIT_FAILURE);
	 }
       if (num_read == 0)
	 {
	   break;
	 }
       write(STDOUT_FILENO, &ch, sizeof(ch));
     }
   return 0;
}
