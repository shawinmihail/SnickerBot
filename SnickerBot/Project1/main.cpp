#include <stdio.h>
#include <windows.h>
#include <string>
#include <iostream>
#include <assert.h>

class Url{

public:

	Url(std::string& fullPath){
		setFullPath(fullPath);
		parseShortPath();
		parseHost();
	}

	const std::string fullPath() const{
		return _fullPath;
	}

	const std::string shortPath() const{
		return _shortPath;
	}

	const std::string host() const{
		return _host;
	}

private:

	std::string _fullPath;
	std::string _shortPath;
	std::string _host;

	void setFullPath(std::string& fullPath){
		_fullPath = fullPath;
	}

	void parseShortPath(){
		std::string shortPath;
		int startIndex = fullPath().find("//") + 2;
		if (startIndex < 2) {
			startIndex = 0;
		}
		for (size_t i = startIndex; i < fullPath().length(); i++){
			shortPath += fullPath()[i];
		}
		if (shortPath[shortPath.length()-1] == '/'){
			shortPath.erase(shortPath.end()-1);
		}
		_shortPath = shortPath;
	}

	void parseHost(){
		std::string host;
		int index = fullPath().find("www.");
		if (index > 0){
			int startIndex = fullPath().find("www.") + 4;
			int finishIndex = fullPath().find("/", startIndex);
			for (int i = startIndex; i < finishIndex; i++){
				host += fullPath()[i];
			}
			_host = host;
		}
		else{
			int startIndex = fullPath().find("//") + 2;
			if (startIndex < 2){
				startIndex = 0;
			}
			int finishIndex = fullPath().find("/", startIndex);
			if (finishIndex < 0){
				finishIndex = fullPath().length();
			}
			for (int i = startIndex; i < finishIndex; i++){
				host += fullPath()[i];
			}
			_host = host;
		}
	}

};

class Query{

public:

	Query(){
	}

	const std::string& body() const {
		return _body;
	}

	void setBody(std::string& body) {
		_body = body;
	}

private:

	std::string _body;
};

class QueryGetPage : public Query{

public:

	QueryGetPage(const Url& url) : Query(){
		std::string query;

		std::cout << url.shortPath() << std::endl;
		std::cout << url.host();
		std::cin.get();

		query += "GET /";
		query += url.shortPath();
		query += " HTTP/1.0\nHost: ";
		query += url.host();
		query += "\nUser-agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)";
		query += "\nAccept: */*\n\n";
		setBody(query);

		//query += "GET /";
		//query += "vk.com";
		//query += " HTTP/1.0\nHost: ";
		//query += "vk.com";
		//query += "\nUser-agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)";
		//query += "\nAccept: */*\n\n";
		//setBody(query);
	}
};


class Proxy {

public:

	Proxy(){
		WSAStartup(0x0101, &_WSAData);
		_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}

	bool OpenUrl(const Url& url) {

		int port = 80;

		_hostent = gethostbyname(url.host().c_str());

		struct sockaddr_in ssin;
		memset((char *)&ssin, 0, sizeof(ssin));
		ssin.sin_family = AF_INET;
		ssin.sin_addr.S_un.S_un_b.s_b1 = _hostent->h_addr[0];
		ssin.sin_addr.S_un.S_un_b.s_b2 = _hostent->h_addr[1];
		ssin.sin_addr.S_un.S_un_b.s_b3 = _hostent->h_addr[2];
		ssin.sin_addr.S_un.S_un_b.s_b4 = _hostent->h_addr[3];
		ssin.sin_port = htons(port);

		printf("Connecting to %d.%d.%d.%d...\n", (unsigned char)_hostent->h_addr[0],
			(unsigned char)_hostent->h_addr[1],
			(unsigned char)_hostent->h_addr[2],
			(unsigned char)_hostent->h_addr[3]);

		connect(_socket, (sockaddr *)&ssin, sizeof(ssin));

		printf("connected!\n\n");
		return true;
	}

	bool sendQuery(const Query& query) const{
		int result = send(_socket, query.body().c_str(), query.body().size(), 0);
		return true;
	}

	std::string receiveResponde() const{
		std::string responde;
		char buff;
		while (true){
			if (recv(_socket, &buff, 1, 0) == 0){
				break;
			}
			responde += buff;
		}
		return responde;
	}

private:

	SOCKET _socket;
	struct hostent* _hostent;
	WSADATA _WSAData;

};

void main(){
	Proxy proxy = Proxy();
	Url url = Url(std::string("derevyagino.ru"));
	proxy.OpenUrl(url);
	QueryGetPage query = QueryGetPage(url);
	proxy.sendQuery(query);
	std::cout << proxy.receiveResponde();
	std::cin.get();
}