#include <unistd.h>

int main()
{
    char *args[] = {"cat", (char *) NULL};
    execvp("cat", args);
    return 0;
}
