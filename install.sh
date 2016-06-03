#! /bin/bash
[ -x "/usr/bin/php-cgi" ] || {
	echo "You need an executable /usr/bin/php-cgi to use lighttpd-suphp."
	exit 1
}

# For ubuntu
gcc lighttpd-suphp.c -o lighttpd-suphp \
&& sudo install -m u=rwxs,g=rx,o= -o root -g www-data lighttpd-suphp /usr/sbin/lighttpd-suphp \
&& sudo install -m u=rw,go=r -o root -g root 10-suphp.conf /etc/lighttpd/conf-available/10-suphp.conf \
&& sudo addgroup --system suphp \
&& echo "Installed successfully." || echo "Failed to install."

# For fedora
# gcc lighttpd-suphp.c -o lighttpd-suphp \
# && sudo install -m u=rwxs,g=rx,o= -o root -g lighttpd lighttpd-suphp /usr/sbin/lighttpd-suphp \
# && sudo install -m u=rw,go=r -o root -g root 10-suphp.conf /etc/lighttpd/conf.d/suphp.conf \
# && echo "Installed successfully." || echo "Failed to install."


