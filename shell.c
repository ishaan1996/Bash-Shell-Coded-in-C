#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#define BUF_SIZE 1024
void parse_it(char *l, char **argv)
{
    while(*l != '\0')
    {
        while(*l == ' ' || *l == '\t' || *l == '\n')
            *l++ = '\0';

        *argv++ = l;

        while( *l != '\0' && *l != ' ' && *l != '\t' && *l != '\n')
            *l++;
    }
    *argv = '\0';

}

int check_redirect(char **argv)
{   
    const char in[] = ">";
    const char ina[] = ">>";
    const char out[] = "<";

    while(*argv != '\0')
    {
        if(strcmp(*argv, in) == 0 || strcmp(*argv, ina) == 0)
            return 1;
        else if(strcmp(*argv, out) == 0)
            return 2;
        *argv++;
    }

    return 0;
}
int pos(char **argv)
{
    const char in[] = ">";
    const char ina[] = ">>";
    const char out[] = "<";
    int count = 0;

    while(*argv != '\0')
    {
       if(strcmp(*argv, in) == 0 || strcmp(*argv, ina) == 0)
           return count;
       else if(strcmp(*argv, out) == 0)
           return count;

       count++;
       *argv++;
    }
}    
int main(int argc, char *argv[])
{
 char command[BUF_SIZE];
 int pid, status;
 char buffer[BUF_SIZE];

    while(1){	
	printf(">>");

        fgets(command, sizeof(command), stdin);

        if(strlen(command) > 0)
            command[strlen(command) - 1] = '\0';
   
	pid = fork();
	if (pid!=0) {
		waitpid(pid, &status, WUNTRACED);
		
	}
	else {
                
		char *argv[BUF_SIZE];
                parse_it(command, argv);

                if(check_redirect(argv) == 0)
        	    execvp(command, argv);
                else if(check_redirect(argv) == 1)
                {
                    int output;
                    const char h[] = ">";
                    int get_pos = pos(argv);
                    if(strcmp(argv[get_pos], h) == 0)
                        output = open(argv[get_pos + 1], O_WRONLY| O_CREAT, S_IRWXU);
                    else
                    {
                        if(access(argv[get_pos + 1], F_OK) != -1)
                            output = open(argv[get_pos + 1], O_WRONLY| O_APPEND);
                        else
                            output = open(argv[get_pos + 1], O_WRONLY| O_CREAT, S_IRWXU);
                    }
                    dup2(output, 1);
                    argv[get_pos] = '\0';
                    execvp(command, argv);
                    close(output);
                }
                else if(check_redirect(argv) == 2)
                {
                    int get_pos = pos(argv);
                    int input = open(argv[get_pos + 1], O_RDONLY);
                    dup2(input, 0);
                    argv[get_pos] = '\0';
                    execvp(command, argv);
                    close(input);
                }      
		break;
	}
   }
}
