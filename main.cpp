#include <exception>
using std::exception;

#include <iostream>
using std::cout;
using std::endl;

#include <limits>
using std::numeric_limits;

#include <memory>
using std::shared_ptr;

#include <thread>
using std::thread;

#include <vector>
using std::vector;

#include <boost/asio/buffer.hpp>
using boost::asio::mutable_buffer;

#include <boost/asio/buffered_stream.hpp>
using boost::asio::buffered_stream;

#include <boost/asio/io_service.hpp>
using boost::asio::io_service;

#include <boost/asio/ip/tcp.hpp>
using boost::asio::ip::tcp;

#include <boost/asio/spawn.hpp>
using boost::asio::spawn;

#include <boost/asio/yield.hpp>
using boost::asio::yield_context;

#include <boost/beast/core.hpp>
namespace beast = boost::beast;

#include <boost/beast/http.hpp>
namespace http = beast::http;


int main() {
	vector<thread> pool ((vector<thread>::size_type)(thread::hardware_concurrency()));
	io_service io;
	tcp::acceptor acceptor (io);
	tcp::endpoint endpoint (tcp::v6(), 8910);

	acceptor.open(endpoint.protocol());
	acceptor.set_option(tcp::acceptor::reuse_address(true));
	acceptor.bind(endpoint);
	acceptor.listen(numeric_limits<int>::max());

	spawn(acceptor.get_io_context(), [&acceptor](yield_context yc) {
		for (;;) {
			shared_ptr<tcp::socket> peer (new tcp::socket(acceptor.get_io_context()));
			acceptor.async_accept(*peer, yc);

			spawn(acceptor.get_io_context(), [peer](yield_context yc) {
				try {
					{
						auto remote (peer->remote_endpoint());
						cout << "I has conn: [" << remote.address().to_string() << "]:" << remote.port() << endl;
					}

					buffered_stream<decltype(*peer)> iobuf (*peer);

					iobuf.async_fill(yc);

					if (iobuf.in_avail() > 0) {
						char first_char;

						{
							mutable_buffer first_char_buf (&first_char, 1);
							iobuf.peek(first_char_buf);
						}

						if ('0' <= first_char && first_char <= '9') {
							cout << "I has JSON-RPC!" << endl;
						} else {
							beast::flat_buffer buf;

							for (;;) {
								http::request<http::string_body> req;

								http::async_read(iobuf, buf, req, yc);

								cout << "I has req: '" << req.body() << '\'' << endl;

								http::response<http::string_body> res;

								res.result(http::status::internal_server_error);
								res.set(http::field::content_type, "text/plain");
								res.set(http::field::content_length, "36");
								res.body() = "I like onions. They make people cry.";

								http::async_write(iobuf, res, yc);
								iobuf.async_flush(yc);

								cout << "I has res." << endl;
							}
						}
					}

					peer->shutdown(peer->shutdown_both);
				} catch (const exception& e) {
					cout << "I has exception: " << e.what() << endl;
				}
			});
		}
	});

	for (auto& thrd : pool) {
		thrd = thread([&io](){ io.run(); });
	}

	for (auto& thrd : pool) {
		thrd.join();
	}
}
