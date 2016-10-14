#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>

int parse(char *user_input_buffer, char *argv[]);
int is_abs_pathname(char argv[]);
int is_internal_cmd(char *argv[]);
int handle_internal_cmd(char *argv[]);
void list_cur_dir(char *buffer[]);
int is_io_redirect(char *argv[], int argc, int *in, int *out);
int handle_io_redirect(char *argv[], int *in_index, int *out_index);


extern char **environ;


int main(int argc, char *argv[])
{

	if (argv[1])
	{
		//printf("argv[1]: %s\n", argv[1]);
		int	newstdin = open(argv[1], O_RDONLY);
		close(0);
		dup(newstdin);
		close(newstdin);
	}

	char user_input_buffer[100] = "";

	while( 1 )
	{
		
		/* GET THE CURRENT WORKING DIRECTORY */
		char cwd_buffer[100];
		size_t cwd_size = 100;
		char *cwd = getcwd(cwd_buffer, cwd_size);
		//printf("Current Working Directory: %s\n", cwd);
		

		// PRINT PROMPT TO USER AND GET USER STRING
		printf("%smyshell>>", cwd);
		if ( fgets(user_input_buffer, 100, stdin) == NULL )
		{
			exit(0);
		}
		
		
		char user_input_buffer2[100] = "";
		int a1;
		int buff_len = strlen(user_input_buffer);
		
		// REMOVE THE TRAILING NEWLINE CHAR FROM USER STRING
		for (a1 = 0; a1 < buff_len - 1; a1++)
		{
			user_input_buffer2[a1] = user_input_buffer[a1];
		}
		int buff_len2 = strlen(user_input_buffer2);
		
	
		// PARSE USER STRING INTO ARRAY OF C STRINGS
		int argc;
		char *argv[100];
		
		argc = parse(user_input_buffer2, argv);

		
		if ( !strcmp(argv[0], "exit") )
			exit(0);
			
		
		/* HANDLE INTERNAL COMMANDS */
		if (is_internal_cmd(argv))
		{
			handle_internal_cmd(argv);
		}
	
		/* HANDLE ALL OTHER COMMANDS */
		else
		{
			
			// CREATE ABSOLUTE PATHNAME FOR USER COMMAND
			char *abs_pathname;
		
			if ( is_relative_pathname(argv[0]) )
			{
				abs_pathname = argv[0];
			}
		
			// CREATE A NEW PROCESS AND EXECUTE USER COMMAND (IN THE CURRENT WORKING DIRECTORY ONLY)
			pid_t pid = fork();
			if  (pid < 0) 
			{
				printf("Error while trying to fork");
			}
			else if (pid == 0)		// CHILD PROCESS EXECUTING
			{	
				int x = 0;
				int y = 0;
				int *in_index = &x;
				int *out_index = &y;
				
				if ( is_io_redirect(argv, argc, in_index, out_index) > 0 )		// std input char '<' encountered
				{
					handle_io_redirect(argv, in_index, out_index);
				}				

				char *child_argv[] = {abs_pathname, NULL };

				if ( (execvp(argv[0], argv)) < 0 )
				{
					printf("Error while executing command: The specified executable does not exist\n");
					exit(1);
				}
			}
			else if (pid > 0)	// PARENT PROCESS EXECUTING
			{
				int wait_flag = 1;
				int a;
				for (a = 0; a < argc; a++)
				{
					if ( !strcmp(argv[a], "&") )
						wait_flag = 0;	
				}
				if ( wait_flag )
				{
					int status = 0;
					waitpid(pid, &status, 0);
					printf("child exited with status %d\n", status);
				}
			}
		}
		
	
	}

return 0;
}



/* HELPER FUNCTIONS */

/* Parse the user string from the command line into an array of c strings */
int parse(char *user_input_buffer, char *argv[])
{

	char *token = strtok(user_input_buffer, " ");
	argv[0] = token;
	int arg_count = 0;
   
    while( token != NULL ) 
    {
    	arg_count++;   	  	
    	token = strtok(NULL, " ");
    	argv[arg_count] = token;
    }
    argv[arg_count] = NULL;
    
return arg_count;
}


/* Returns 1 if given a name that begins with a "/" "./" or "../" */
int is_relative_pathname(char argv[])
{
	if (argv[0] == '/')
	{
		return 1;
	}
	else if (argv[0] == '.') 
	{
		if (argv[1] == '/') 
		{
			return 1;
		}
		else if (argv[1] == '.')
		{
			if (argv[2] == '/')
			{
				return 1;
			}
		}
	}
	return 0;
}

/* Handle several internal shell commands */
int handle_internal_cmd(char *argv[])
{
	if ( !strcmp(argv[0], "cd") )		// working
	{
		my_chdir(argv);
	}
	else if ( !strcmp(argv[0], "clr") )		// working
	{
		system("clear");
	}
	else if ( !strcmp(argv[0], "dir") )		// working
	{
		char *dir_buffer[100];
		list_cur_dir(dir_buffer);
	}
	else if ( !strcmp(argv[0], "environ") )		//working
	{
		char *path = getenv("PATH");
		printf("PATH : %s\n", path);
	}	
	else if ( !strcmp(argv[0], "echo") )	// semi-working but does not work for spaces
	{
		int i = 1;
		while (argv[i])
		{
			printf("%s", argv[i]);
			i++;
		}
		printf("\n");
	}
	else if ( !strcmp(argv[0], "help") )	// NOT WORKING
	{
		printf("++ Display Manual -More Here ++\n");
	}
		
	else if ( !strcmp(argv[0], "pause") )	// NOT WORKING
	{
		printf("pause\n");
	}
		
	else if ( !strcmp(argv[0], "quit") )	// NOT WORKING
	{
		exit(0);
	}
	
	else if ( !strcmp(argv[0], "pwd") )		// works
	{
		//printf("pwd\n");
		char cwd_buffer[100];
		size_t cwd_size = 100;
		char *cwd = getcwd(cwd_buffer, cwd_size);
		printf("%s\n", cwd);
	}
	
	return 0;
}

/* return 1 if argv[0] is an internal shell command and 0 otherwhise */
int is_internal_cmd(char *argv[])
{	
	if ( !strcmp(argv[0], "cd") )
		return 1;
	else if ( !strcmp(argv[0], "clr") )
		return 1;
	else if ( !strcmp(argv[0], "dir") )
		return 1;
	else if ( !strcmp(argv[0], "environ") )
		return 1;
	else if ( !strcmp(argv[0], "echo") )
		return 1;
	else if ( !strcmp(argv[0], "help") )
		return 1;
	else if ( !strcmp(argv[0], "pause") )
		return 1;
	else if ( !strcmp(argv[0], "quit") )
		return 1;
	else if ( !strcmp(argv[0], "pwd") )
		return 1;
	
	return 0;
}

/* List all of the contents of the current working directory */
void list_cur_dir(char *buffer[])
{
	DIR *dstream;
	struct dirent *dirent;

	dstream = opendir(".");
	
	if (dstream)
	{
		int i = 0;
		while ((dirent = readdir(dstream)) != NULL)
		{
			buffer[i] = dirent->d_name;
			printf("%s\n", buffer[i]);
			i++;
		}
	}	
}

/* 
Change the current working directory.  If directory argument not present, report
current directory.  If target directory does not exist, report an error to the user. 
*/
int my_chdir(char *argv[])
{
	if ( !strcmp(argv[0], "" ) )
		;
	else
	{
		int err = chdir(argv[1]);
		if (err < 0)
			printf("Error while executing cd command: Specified directory does not exist\n");
	}
		
	return 0;
}

/* RETURNS
	1 if <
	2 if >
	if both:
	3 if < before >
	4 if > before <
*/
int is_io_redirect(char *argv[], int argc, int *in, int *out)
{
	//printf("in is_io_redirect!\n");
	int in_flag = 0;
	int out_flag = 0;
	int in_index;
	int out_index;
	int i;
	for (i = 0; i < argc; i++)
	{
		if ( !strcmp(argv[i], "<") )
		{
			in_flag = 1;
			in_index = i;
			*in = i;
		}
		else if ( !strcmp(argv[i], ">") )
		{
			out_flag = 1;
			out_index = i;
			*out = i;
			//printf("out: %d\n", *out);
		}
	}
	
	if (in_flag && out_flag)
	{
		int order;
		if (in_index > out_index)
			return 3;
		else
			return 4;
	}
	
	else if (in_flag)
		return 1;
	else if (out_flag)
		//printf("in is_io_redirect! > returning \n");
		return 2;
	
	return 0;
}

	
int handle_io_redirect(char *argv[], int *in_index, int *out_index)
{
	int err_flag = 0;

	printf("in_index in handle_io: %d\n", *in_index);
	printf("out_index in handle_io: %d\n", *out_index);
	
	if (*in_index)
	{
		printf("in in_index!\n");
		int	newstdin = open(argv[(*in_index) + 1], O_RDONLY);
		close(0);
		dup(newstdin);
		close(newstdin);
	}
	if (*out_index)
	{
		int newstdout = open(argv[(*out_index) + 1], O_WRONLY|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO);
		close(1);
		dup(newstdout);
		close(newstdout);
	}
	
	
	return err_flag;
}





