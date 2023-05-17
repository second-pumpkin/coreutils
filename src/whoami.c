/*------------------------------------------------------------ 
A less bloated version of GNU Whoami - Print current user
Author: Logan Goins
------------------------------------------------------------*/
#include <unistd.h>
#include <pwd.h>

// Alternative strlen() function found in the FreeBSD codebase because string.h and stdlib.h are bloat.
size_t strlen(const char *str)
{
    const char *s;
    for (s = str; *s; ++s);
    return(s - str);
}

int main(){
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (pw){
	char* user = pw->pw_name;
	write(1, user, strlen(user)); // Using write() because printf() is bloat
	write(1, "\n", 1);
	return 0;
    }
    else{
	char* err = "Could not get user\n";
	write(1, err, strlen(err));
	return 1;
    }

}
