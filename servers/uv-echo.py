# https://github.com/koehlma/uv
#
# wget https://github.com/koehlma/uv/archive/v0.1.0.tar.gz
# pypy setup.py install
# copy [uv/handles] => installed dir

import uv


def on_shutdown(request, _):
	request.stream.close()


def on_read(stream, status, data):
	if status is not uv.StatusCodes.SUCCESS: stream.close()
	if data: stream.write(data)


def on_connection(server, _):
	connection = server.accept()
	connection.read_start(on_read=on_read)


def on_quit(sigint, _):
	sigint.loop.close_all_handles()


def main():
	loop = uv.Loop.get_current()

	server = uv.TCP()
	server.bind(('0.0.0.0', 25000))
	server.listen(20, on_connection=on_connection)

	sigint = uv.Signal()
	sigint.start(uv.Signals.SIGINT, on_signal=on_quit)

	loop.run()


if __name__ == '__main__':
	main()

