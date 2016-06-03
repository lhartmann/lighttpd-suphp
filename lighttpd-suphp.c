/*
  Lighttpd licensing information yet to be attached.

  Initially written Nov 2009 by Lucas V. Hartmann <lucas.hartmann@gmail.com>.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>

#define TEMP_MAX_LEN (1024)
#define SUPHP_GROUP_NAME "suphp"

int error(int r, char *msg) {
/*	puts("Content-type: text/plain\n\n");
	
	puts("Internal server error:\n");
	puts(msg);
	
	int fd = open("/tmp/suphp.log", O_CREAT|O_WRONLY|O_APPEND);
	if (fd>0) {
		write(fd, msg, strlen(msg+1));
		close(fd);
	} */
	
	return r;
}

int isUserInGroup(const char *user, char **group) {
	while (*group) {
		if (strcmp(user, *group++)==0) return 1;
	}
	return 0;
}

int main(void) {
	char username[TEMP_MAX_LEN+1];
	char *sn, *sfn, *userstart, *userend, *args[2];
	int sn_len, sfn_len, userlen;
	struct passwd *userpw;
	struct group *grp;
	struct stat sst;
	
	// Recover desired environment variables
	sn = getenv("SCRIPT_NAME");
	if (!sn) return error(__LINE__,"SCRIPT_NAME undefined.");
	sn_len = strlen(sn);
	
	sfn = getenv("SCRIPT_FILENAME");
	if (!sfn) return error(__LINE__,"SCRIPT_FILENAME undefined.");
	sfn_len = strlen(sfn);
	
	// Minimum name: "/~U/s"
	if (sn_len < 5  ) return error(__LINE__,"SCRIPT_NAME malformed.");
	if (sn[0] != '/') return error(__LINE__,"SCRIPT_NAME malformed.");
	if (sn[1] != '~') return error(__LINE__,"SCRIPT_NAME malformed.");
	
	// Recover status info for the file
	if (stat(sfn, &sst)) return error(__LINE__,"Script not found.");
	
	// Check if it is a regular file (not simlinks, dir, or other)
	if (!S_ISREG(sst.st_mode))
		return error(__LINE__,"Script is not a regular file.");
	
	// Look for the username
	userstart = sn + 2;
	userend   = strchr(sn+2, '/');
	if (!userend) return error(__LINE__,"Bad user name or URL.");

	// Copy to a new string
	userlen = userend - userstart;
	if (userlen > TEMP_MAX_LEN) return error(__LINE__,"Bad user name.");
	
	strncpy(username, userstart, userlen);
	username[userlen] = 0;
	
	// Require the user to exist
	userpw = getpwnam(username);
	if (!userpw) return error(__LINE__,"User not found.");
	
	// Require the suphp group to exist
	grp = getgrnam(SUPHP_GROUP_NAME);
	if (!grp) return error(__LINE__,"Missing " SUPHP_GROUP_NAME " group.");
	
	// Require the user to be in the suphp group
	if (!isUserInGroup(username, grp->gr_mem))
		return error(__LINE__,"User not allowed to use scripts.");
	
	// Require the script to be inside the user's home
	if (strncmp(sfn, userpw->pw_dir, strlen(userpw->pw_dir)))
		return error(__LINE__,"Invalid script directory.");
	
	// Require user to own the script
	if (userpw->pw_uid != sst.st_uid)
		return error(__LINE__,"Unsafe script.");
	
	// Require user read permission to the file
	if(!(sst.st_mode&S_IRUSR))
		return error(__LINE__,"Can not read script.");
	
	// Refuse scripts writable by group/other
	if(sst.st_mode & (S_IWGRP|S_IWOTH))
		return error(__LINE__,"Unsafe script.");
	
	// Change to the user
	if (setuid(userpw->pw_uid))
		return error(__LINE__,"User switching failed.");
	
	// Run the script
	args[0] = sfn;
	args[1] = 0;
	execv("/usr/bin/php-cgi", args);
	return 0;
}
