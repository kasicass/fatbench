/*
http://libdill.org/

# aptitude install autoconf
# aptitude install libtool   # TOFIX: error: Libtool library used but 'LIBTOOL' is undefined

$ wget https://github.com/sustrik/libdill/archive/2.13.tar.gz
$ tar zxf 2.13.tar.gz
$ cd libdill
$ ./autogen.sh
$ ./configure
$ make

# make install
# ldconfig
*/

// gcc -ldill -o c-libdill-echo c-libdill-echo.c

#include <assert.h>
// #include <string.h>
#include <libdill.h>

coroutine void echo_worker(int s) {
	int rc;
	char ch;

	rc = brecv(s, &ch, 1, -1); // well, libdill is not suitable for echo-server
    assert(rc == 0);

    rc = bsend(s, &ch, 1, -1);
    assert(rc == 0);
}

int main(void) {
	/* This will take care of deallocating finished coroutines cleanly. */
	int rc;
	int workers = bundle();
	assert(workers >= 0);

	struct ipaddr addr;
	rc = ipaddr_local(&addr, NULL, 25000, 0);
	assert(rc == 0);
	int ls = tcp_listen(&addr, 10);
	assert(ls >= 0);
	while(1) {
		int s = tcp_accept(ls, NULL, -1);
		assert(s >= 0);
		rc = bundle_go(workers, echo_worker(s));
		assert(rc == 0);
	}
}

