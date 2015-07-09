/**
 * qtconnection.cpp
 *
 *  Created on: ۱۸ تیر ۱۳۹۴ ه‍.ش.
 *      Author: Hedayat Vatankhah <hedayat.fwd@gmail.com>.
 */

#include <QObject>
#include <QTcpSocket>
#include <cassert>
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

tgl_net_methods conn_methods =
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
        int write(const void *_data, int len);
        int read(void *_data, int len);
        void flush() { socket.flush(); }

    public:
        tgl_state *tl_state;
        tgl_session *session;
        tgl_dc *dc;

    private:
        QTcpSocket socket;
        mtproto_methods *methods;

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
        void fail()
        {
            int new_port = rotate_port();
            QByteArray addr = socket.peerName().toUtf8();
            connect_to_server(addr.data(), new_port);
        }

        int rotate_port()
        {
            switch (socket.peerPort())
            {
                case 443:
                    return 80;
                case 80:
                    return 25;
                case 25:
                    return 443;
            }
        }

        void try_rpc_read();
};

connection::connection(tgl_state *tls, tgl_session *s, tgl_dc *dc,
    mtproto_methods *mp, QObject *parent) :
        QObject(parent), tl_state(tls), socket(this), session(s), dc(dc),
        methods(mp)
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
    socket.connectToHost(host, port);
    // todo implement pinging the server
//  c->last_receive_time = tglt_get_double_time ();
//  c->ping_ev = evtimer_new (TLS->ev_base, ping_alarm, c);
//  start_ping_timer (c);

    // 5 seconds for inactivity check (failed connection)
}

int connection::write(const void *_data, int len)
{
    qDebug(__PRETTY_FUNCTION__);
    // buffer data & write
    return len; // num written
}

int connection::read(void *_data, int len)
{
    qDebug(__PRETTY_FUNCTION__);
    // read data from buffer to _data
    return 0;//num read
}

void connection::connected()
{
    qDebug(__PRETTY_FUNCTION__);
    char byte = 0xef;
    assert(socket.write(&byte, 1) == 1);
    socket.flush();

    methods->ready(tl_state, this);
}

void connection::disconnected()
{
    qDebug(__PRETTY_FUNCTION__);
}

void connection::error(QAbstractSocket::SocketError socketError)
{
    qDebug(__PRETTY_FUNCTION__);
}

void connection::host_found()
{
    qDebug(__PRETTY_FUNCTION__);
}

void connection::state_changed(QAbstractSocket::SocketState socketState)
{
    qDebug(__PRETTY_FUNCTION__);
}

void connection::about_to_close()
{
    qDebug(__PRETTY_FUNCTION__);
}

void connection::bytes_written(qint64 bytes)
{
    qDebug(__PRETTY_FUNCTION__);
    // write remaining data?!
}

void connection::read_channel_finished()
{
    qDebug(__PRETTY_FUNCTION__);
}

void connection::ready_read()
{
    qDebug(__PRETTY_FUNCTION__);
    // restart ping timer
    // read into buffer
    // if error, fail_connection()
    // x = readnum
    int x = 0;
    if (x)
        try_rpc_read();
}

void connection::try_rpc_read()
{
    qDebug(__PRETTY_FUNCTION__);
//  while (1) {
//    if (c->in_bytes < 1) { return; }
//    unsigned len = 0;
//    unsigned t = 0;
//    assert (tgln_read_in_lookup (c, &len, 1) == 1);
//    if (len >= 1 && len <= 0x7e) {
//      if (c->in_bytes < (int)(1 + 4 * len)) { return; }
//    } else {
//      if (c->in_bytes < 4) { return; }
//      assert (tgln_read_in_lookup (c, &len, 4) == 4);
//      len = (len >> 8);
//      if (c->in_bytes < (int)(4 + 4 * len)) { return; }
//      len = 0x7f;
//    }
//
//    if (len >= 1 && len <= 0x7e) {
//      assert (tgln_read_in (c, &t, 1) == 1);
//      assert (t == len);
//      assert (len >= 1);
//    } else {
//      assert (len == 0x7f);
//      assert (tgln_read_in (c, &len, 4) == 4);
//      len = (len >> 8);
//      assert (len >= 1);
//    }
//    len *= 4;
//    int op;
//    assert (tgln_read_in_lookup (c, &op, 4) == 4);
//    if (c->methods->execute (TLS, c, op, len) < 0) {
//      return;
//    }
//  }
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
    qDebug(__PRETTY_FUNCTION__);
}

static tgl_dc *get_dc(connection *c)
{
    return c->dc;
}

static tgl_session *get_session(connection *c)
{
    return c->session;
}

static void flush_out(connection *c)
{
    qDebug(__PRETTY_FUNCTION__);
    c->flush();
}

static void conn_free(connection *c)
{
    qDebug(__PRETTY_FUNCTION__);
    delete c;
}


#include "qtconnection.moc"
