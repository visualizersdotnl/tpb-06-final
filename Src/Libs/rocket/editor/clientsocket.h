#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QTcpSocket>
#include <QByteArray>

class TcpSocket {
public:
	explicit TcpSocket(QTcpSocket *socket) : socket(socket) {}

	bool connected() const
	{
		return socket != NULL;
	}

	virtual void disconnect()
	{
		if (connected())
			socket->close();
		socket = NULL;
	}

	virtual bool recv(char *buffer, int length)
	{
		if (!connected())
			return false;

		// wait for enough data to arrive
		while (socket->bytesAvailable() < int(length)) {
			if (!socket->waitForReadyRead(-1))
				break;
		}

		qint64 ret = socket->read(buffer, length);
		if (ret != int(length)) {
			TcpSocket::disconnect();
			return false;
		}
		return true;
	}

	virtual bool send(const char *buffer, size_t length, bool endOfMessage)
	{
		(void)endOfMessage;
		if (!connected())
			return false;
		int ret = socket->write(buffer, length);
		if (ret != int(length)) {
			TcpSocket::disconnect();
			return false;
		}
		return true;
	}

	virtual bool pollRead()
	{
		if (!connected())
			return false;
		return socket->bytesAvailable() > 0;
	}

	QTcpSocket *socket;
};

class WebSocket : public TcpSocket {
public:
	explicit WebSocket(QTcpSocket *socket) : TcpSocket(socket), firstFrame(true) {}

	bool recv(char *buffer, int length);
	bool send(const char *buffer, size_t length, bool endOfMessage)
	{
		return sendFrame(firstFrame ? 2 : 0, buffer, length, endOfMessage);
	}


	virtual void disconnect()
	{
		sendFrame(8, NULL, 0, true);
		TcpSocket::disconnect();
	}

	bool pollRead()
	{
		if (buf.length() > 0)
			return true;
		return TcpSocket::pollRead();
	}

	// helpers
	bool readFrame(QByteArray &buf);
	bool sendFrame(int opcode, const char *payloadData, size_t payloadLength, bool endOfMessage);
	static WebSocket *upgradeFromHttp(QTcpSocket *socket);

private:
	bool firstFrame;
	QByteArray buf;
};

class ClientSocket {
public:
	ClientSocket() : clientPaused(true), socket(NULL) {}

	bool connected() const
	{
		if (!socket)
			return false;
		return socket->connected();
	}

	void disconnect()
	{
		if (socket)
			socket->disconnect();
		clientTracks.clear();
	}

	bool recv(char *buffer, int length)
	{
		if (!socket)
			return false;
		return socket->recv(buffer, length);
	}

	bool send(const char *buffer, size_t length, bool endOfMessage)
	{
		if (!socket)
			return false;
		return socket->send(buffer, length, endOfMessage);
	}

	bool pollRead()
	{
		if (!connected())
			return false;
		return socket->pollRead();
	}

	void sendSetKeyCommand(const QString &trackName, const struct track_key &key);
	void sendDeleteKeyCommand(const QString &trackName, int row);
	void sendSetRowCommand(int row);
	void sendPauseCommand(bool pause);
	void sendSaveCommand();

	bool clientPaused;
	QMap<QString, size_t> clientTracks;
	TcpSocket *socket;
};

#endif // !defined(CLIENTSOCKET_H)
