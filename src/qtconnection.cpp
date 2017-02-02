/*
 * qtconnection.cpp
 *
 *  Created on: ۱۸ تیر ۱۳۹۴ ه‍.ش.
 *      Author: Hedayat Vatankhah <hedayat.fwd@gmail.com>.
 */

#include <cassert>
#include <QObject>
#include <QTcpSocket>
#include <QTimerEvent>
#include <tgl.h>

static connection *create_connection(tgl_state *tls, const char *host, int port,
    tgl_session *session, tgl_dc *dc, mtproto_methods *methods);
static int write_out (connection *c, const void *_data, int len);
static int read_in (connection *c, void *_data, int len);
static int read_in_lookup (connection *c, void *_data, int len);
static void incr_out_packet_num (connection *c);
static tgl_dc *get_dc(connection *c);
static tgl_session *get_session(connection *c);
static void flush_out(connection *c);
static void conn_free(connection *c);

tgl_net_methods qtg_net_methods =
{
    write_out,
    read_in,
    read_in_lookup,
    flush_out,
    incr_out_packet_num,
    conn_free,
    get_dc,
    get_session,
    create_connection
};


struct connection: public QObject
{
    private:
        Q_OBJECT;

    public:
        connection(tgl_state *tls, tgl_session *s, tgl_dc *dc,
            mtproto_methods *mp, QObject *parent = NULL);

        void connect_to_server(const char *host, int port);
        int write(const void *data, int len);
        int read(void *data, int len);
        void flush() { socket.flush(); }
        void stop();

        tgl_session *get_session() { return session; }
        tgl_dc *get_dc() { return dc; }

    private:
        tgl_state *tl_state;
        tgl_session *session;
        tgl_dc *dc;

    private:
        QTcpSocket socket;
        mtproto_methods *methods;
        QByteArray read_buffer;
        QByteArray write_buffer;
        int server_port;
        bool active;
        int reconnect_timer;

    private slots:
        void connected();
        void disconnected();
        void error(QAbstractSocket::SocketError socketError);
        void host_found();
//        void proxyAuthenticationRequired(const QNetworkProxy & proxy,
//            QAuthenticator * authenticator)
        void state_changed(QAbstractSocket::SocketState socketState);
        void about_to_close();
        void bytes_written(qint64 bytes);
        void read_channel_finished();
        void ready_read();

    private:
        void fail(bool do_rotate_port = true)
        {
            if (!active)
                return;
            methods->close(tl_state, this);
            int new_port = do_rotate_port ? rotate_port() : server_port;
            QByteArray addr = socket.peerName().toUtf8();
            connect_to_server(addr.data(), new_port);
        }

        int rotate_port()
        {
            switch (server_port)
            {
                case 443:
                    return 80;
                case 80:
                    return 25;
                case 25:
                    return 443;
            }
            return 443;
        }

        qint64 available_bytes();
        void try_rpc_read();
        int read_lookup(void *data, int len);
        void send_buffered_data();
        void timerEvent(QTimerEvent *event);
};

connection::connection(tgl_state *tls, tgl_session *s, tgl_dc *dc,
    mtproto_methods *mp, QObject *parent) :
        QObject(parent), tl_state(tls), session(s), dc(dc), socket(this),
        methods(mp), server_port(0), active(false), reconnect_timer(0)
{
    socket.setSocketOption(QAbstractSocket::LowDelayOption, 1);
    socket.setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    connect(&socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(&socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)),
        this, SLOT(error(QAbstractSocket::SocketError)));
    connect(&socket, SIGNAL(hostFound()), this, SLOT(host_found()));
    connect(&socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)),
        this, SLOT(state_changed(QAbstractSocket::SocketState)));

    connect(&socket, SIGNAL(aboutToClose()), this, SLOT(about_to_close()));
    connect(&socket, SIGNAL(bytesWritten(qint64)), this,
        SLOT(bytes_written(qint64)));
    connect(&socket, SIGNAL(readChannelFinished()), this,
        SLOT(read_channel_finished()));
    connect(&socket, SIGNAL(readyRead()), this, SLOT(ready_read()));
}

void connection::connect_to_server(const char *host, int port)
{
    qDebug(__PRETTY_FUNCTION__);
    active = true;
    qDebug() << "Connecting to: " << host << ':' << port;
    server_port = port;
    socket.connectToHost(host, port);
    // todo implement pinging the server
//  c->last_receive_time = tglt_get_double_time ();
//  c->ping_ev = evtimer_new (TLS->ev_base, ping_alarm, c);
//  start_ping_timer (c);

    // 5 seconds for inactivity check (failed connection)
}

int connection::write(const void *data, int len)
{
    // buffer data & write
    const char *data_bytes = reinterpret_cast<const char *>(data);
    if (socket.state() != QAbstractSocket::ConnectedState
            || !write_buffer.isEmpty())
    {
        qDebug() << "Not connected yet, adding " << len << " bytes to internal "
                "write buffer (size: " << write_buffer.length() << ")";
        write_buffer.append(data_bytes, len);
    }
    else
    {
        qint64 n = socket.write(data_bytes, len);
        if (n == -1)
        {
            qDebug() << "Network write error: " << socket.errorString();
            n = 0;
        }
        if (n < len)
            write_buffer.append(data_bytes + n, len - n);
        qDebug() << "Queue " << n << " bytes for sending, adding remaining "
                << len - n << " bytes to internal write buffer (size: "
                << write_buffer.length() << ")";
    }
    return len;
}

int connection::read(void *data, int len)
{
    if (len < read_buffer.size())
    {
        memcpy(data, read_buffer.data(), len);
        read_buffer.remove(0, len);
        return len;
    }
    else
    {
        memcpy(data, read_buffer.data(), read_buffer.size());
        int read = read_buffer.size();
        read_buffer.clear();

        if (len > read)
        {
            qDebug() << "NETWORK: not enough bytes in read buffer, reading from socket: "
                    << len - read;
            char *rem_data = reinterpret_cast<char *>(data) + read;
            read += socket.read(rem_data, len - read);
        }
        return read;
    }
}

void connection::stop()
{
    active = false;
    socket.close();
}

void connection::connected()
{
    char byte = 0xef;
    assert(socket.write(&byte, 1) == 1);
    socket.flush();

    methods->ready(tl_state, this);
}

void connection::disconnected()
{
    qDebug(__PRETTY_FUNCTION__);
    // reconnect
    fail();
}

void connection::error(QAbstractSocket::SocketError socketError)
{
    qDebug(__PRETTY_FUNCTION__);
    qDebug() << "error: " << socketError;
    if (socketError == QAbstractSocket::SocketTimeoutError)
        fail();
    else if (!reconnect_timer)
    {
        int pause_sec = 30;
        qDebug() << "Reconnecting in " << pause_sec << " seconds";
        reconnect_timer = startTimer(pause_sec * 1000);
        if (!reconnect_timer)
            qDebug("Cannot create a reconnection timer!");
    }
}

void connection::host_found()
{
    qDebug(__PRETTY_FUNCTION__);
}

void connection::state_changed(QAbstractSocket::SocketState socketState)
{
    qDebug(__PRETTY_FUNCTION__);
    qDebug() << "New sate: " << socketState;
    if (socketState == QAbstractSocket::ConnectedState)
        send_buffered_data();
}

void connection::about_to_close()
{
    qDebug(__PRETTY_FUNCTION__);
    flush();
}

void connection::bytes_written(qint64 bytes)
{
    qDebug() << bytes << " byte(s) sent.";
    if (active)
        send_buffered_data();
}

void connection::read_channel_finished()
{
    qDebug(__PRETTY_FUNCTION__);
}

void connection::ready_read()
{
    // restart ping timer
    int newbytes = socket.bytesAvailable();
    qDebug() << "New bytes received: " << newbytes;
    if (newbytes > 1024 * 1024)
        newbytes = 1024 * 1024;

    int cur_size = read_buffer.size();
    read_buffer.resize(cur_size + newbytes);
    char *buf = read_buffer.data() + cur_size;
    qint64 r = socket.read(buf, newbytes);
    assert(r == newbytes);
    // if error, fail_connection()
    if (r)
        try_rpc_read();
}

qint64 connection::available_bytes()
{
    return read_buffer.size() + socket.bytesAvailable();
}

void connection::try_rpc_read()
{
    while (true)
    {
        if (available_bytes() < 1) { return; }
        unsigned len = 0;
        unsigned t = 0;
        assert(read_lookup(&len, 1) == 1);
        if (len >= 1 && len <= 0x7e)
        {
            if (available_bytes() < (int) (1 + 4 * len)) return;
        }
        else
        {
            if (available_bytes() < 4) return;
            assert(read_lookup(&len, 4) == 4);
            len = (len >> 8);
            if (available_bytes() < (int) (4 + 4 * len)) return;
            len = 0x7f;
        }

        if (len >= 1 && len <= 0x7e)
        {
            assert(read(&t, 1) == 1);
            assert(t == len);
            assert(len >= 1);
        }
        else
        {
            assert(len == 0x7f);
            assert(read(&len, 4) == 4);
            len = (len >> 8);
            assert(len >= 1);
        }
        len *= 4;
        int op;
        assert(read_lookup(&op, 4) == 4);
        if (methods->execute(tl_state, this, op, len) < 0)
            break;
    }
}

int connection::read_lookup(void *data, int len)
{
    if (len > read_buffer.size())
    {
        int append_len = qMin(static_cast<qint64>(len - read_buffer.size()),
            socket.bytesAvailable());
        len = read_buffer.size() + append_len;
        read_buffer.resize(len);
        read_buffer.append(socket.read(append_len));
    }
    memcpy(data, read_buffer.constData(), len);
    return len;
}

void connection::send_buffered_data()
{
    if (write_buffer.isEmpty())
        return;

    qint64 n = socket.write(write_buffer.constData(), write_buffer.length());
    if (n == -1)
    {
        qDebug() << "Network write error: " << socket.errorString();
        return;
    }
    if (n == write_buffer.length())
        write_buffer.clear();
    else
        write_buffer.remove(0, n);
    qDebug() << "Queue " << n << " bytes from internal write buffer, "
             << write_buffer.length() << " bytes remain.";
}

void connection::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == reconnect_timer)
    {
        reconnect_timer = 0;
        fail(false);
    }
}

static connection *create_connection(tgl_state *tls, const char *host, int port,
    tgl_session *session, tgl_dc *dc, mtproto_methods *methods)
{
    qDebug(__PRETTY_FUNCTION__);
    connection *c = new connection(tls, session, dc, methods);
    c->connect_to_server(host, port);
    return c;
}

static int write_out(connection *c, const void *_data, int len)
{
    return c->write(_data, len);
}

static int read_in(connection *c, void *_data, int len)
{
    return c->read(_data, len);
}

static int read_in_lookup(connection *c, void *_data, int len)
{
    qDebug(__PRETTY_FUNCTION__);
    // read but not change read pointer
    return 0;
}

static void incr_out_packet_num(connection *c)
{
//    qDebug(__PRETTY_FUNCTION__);
}

static tgl_dc *get_dc(connection *c)
{
    return c->get_dc();
}

static tgl_session *get_session(connection *c)
{
    return c->get_session();
}

static void flush_out(connection *c)
{
    c->flush();
}

static void conn_free(connection *c)
{
    qDebug(__PRETTY_FUNCTION__);
    c->stop();
    c->deleteLater();
}


#include "qtconnection.moc"
