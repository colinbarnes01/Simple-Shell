#include <stdio.h>
#include <dirent.h>

void list_cur_dir(char *buffer[]);

int main()
{
	/*
	DIR *dstream;
	struct dirent *dirent;

	dstream = opendir(".");
	
	if (dstream)
	{
		while ((dirent = readdir(dstream)) != NULL)
		{
			printf("%s\n", dirent->d_name);
		}
	}	
	*/
	char *buffer[100];
	list_cur_dir(buffer);
	

}

// returns a list of the files in the current working directory
// MAKE IT RETURN INT LATER
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
