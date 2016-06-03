# lighttpd-suphp
Combined with userdir functionality this module allows users to serve php scripts running with their privileges.

Benefits:
- Allows user scripts to modify files and run commands as the user himself (not www-data).
- Does not require files in user dir to be writable by www-data.
- Does not allow user scripts to change other user files, or the main site files.

I tried to make it secure by:
- Making sure only www-data group (i.e. lighttpd) can invoke suphp.
- Making sure SCRIPT_NAME is at least 5 chars (minimum is "/~U/s").
- Requiring script to be a regular file (not symlink, not drectory, not special).
- Requiring username to exist.
- Require group suphp to exist.
- Requiring user to be on the suphp group (allows suphp only to selected users).
- Require the script to be in the user's home.
- Require the script to be owned by the user.
- Require the script to be readable by the user.
- Require the script to be writable only by the user.

Possible threats (unckecked):
- suphp runs as suid root, which may lead to security breaches. Let me know if you find any.
- I trusted environment variables set by lighttpd, and expect php-cgi to trust them as well. Not sure how safe this is.
- Users with php privileges on the main site (as www-data, not via suphp) may try breaching by feeding suphp with bad environments.
- I never checked for .. in SCRIPT_FILENAME. I just hoped lighttpd clears .. out of it.

Setting up in ubuntu:
- Run install.sh
  - ./install.sh
- Enable userdir and suphp modules:
  - sudo lighttpd-enable-mod userdir
  - sudo lighttpd-enable-mod suphp
- Restart lighttpd:
  - sudo /etc/init.d/lighttpd restart
- Add authorized users to the suphp group:
  - sudo adduser USER suphp
