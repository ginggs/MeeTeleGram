/*
 * qtelegram.cpp
 *
 *  Created on: ۱۹ تیر ۱۳۹۴ ه‍.ش.
 *      Author: Hedayat Vatankhah <hedayat.fwd@gmail.com>.
 */

#include "qtelegram.h"

#include <cassert>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <QDateTime>
#include <QTimerEvent>
#include <QDebug>
#include <QVariantMap>

#include <tgl.h>
#include <tgl-binlog.h>
#include <tgl-queries.h>

#include "qpeerid.h"

#define TELEGRAM_N9_TEST_SERVER "149.154.167.40"
#define TELEGRAM_N9_PROD_SERVER "149.154.167.50"

#define DC_SERIALIZED_MAGIC 0x868aa81d
#define STATE_FILE_MAGIC 0x28949a93
#define SECRET_CHAT_FILE_MAGIC 0x37a1988a

extern tgl_net_methods qtg_net_methods;
extern tgl_timer_methods qtg_timers;

qtelegram::qtelegram(int app_id, const char *app_hash, const char *app_ver,
    const char *conf_dir, const char *serverkey_path, QObject *parent) :
        QObject(parent), config_dir(conf_dir), tlstate(tgl_state_alloc()),
        enable_events(false), get_state_timer(0), getvalues_cb(0),
        getvalues_arg(0)
{
    if (!tlstate)
    {
        qDebug("ERROR: cannot allocate tgl state object");
        return;
    }

    tgl_allocator = &tgl_allocator_release;
    tgl_set_binlog_mode (tlstate, 0);

    tgl_set_verbosity(tlstate, 7);
//    tgl_set_test_mode(tlstate); // todo(test_code) remove for production

    tgl_set_rsa_key(tlstate, "tg-server.pub");
    tgl_set_rsa_key(tlstate, serverkey_path);
    tgl_set_callback(tlstate, &qtg_update_cb);
    tgl_set_ev_base(tlstate, this);
    tgl_set_net_methods(tlstate, &qtg_net_methods);
    tgl_set_timer_methods(tlstate, &qtg_timers);
    tgl_set_download_directory(tlstate,
        (config_dir + "/downloads/").toUtf8().constData());
    tgl_register_app_id(tlstate, app_id, app_hash);
    tgl_set_app_version(tlstate, app_ver);
//    if (ipv6_enabled)
//    {
//        tgl_enable_ipv6 (tlstate);
//    }
//    if (disable_link_preview)
//    {
//        tgl_disable_link_preview (tlstate);
//    }
    tgl_init(tlstate);
}

qtelegram::~qtelegram()
{
    write_state_file();
    free(tlstate);
}

bool qtelegram::is_our_id(QPeerId *id)
{
    qDebug(__PRETTY_FUNCTION__);
    return tgl_cmp_peer_id(id->id(), tlstate->our_id) == 0;
}

void qtelegram::login()
{
    qDebug(__PRETTY_FUNCTION__);
    read_auth_file();
    read_state_file();
    read_secret_chat_file();

    enable_events = true;

//    if (reset_authorization) {
//      tgl_peer_t *P = tgl_peer_get (tlstate, TGL_MK_USER (tlstate->our_id));
//      if (P && P->user.phone && reset_authorization == 1) {
//        set_default_username (P->user.phone);
//      }
//      bl_do_reset_authorization (tlstate);
//    }

    tgl_login (tlstate);

    get_state_timer = startTimer(3600 * 1000);
//    startTimer(0);
}

void qtelegram::request_contact_list()
{
    qDebug(__PRETTY_FUNCTION__);
    tgl_do_update_contact_list(tlstate, on_contact_list_updated, this);
}

void qtelegram::get_dialog_list(int offset)
{
    qDebug(__PRETTY_FUNCTION__);
    last_dialog = offset;
    tgl_do_get_dialog_list(tlstate, 10000, offset, on_dialog_list_received, this);
}

void qtelegram::load_messages(QPeerId *peer, int offset, int limit, bool offline)
{
    qDebug(__PRETTY_FUNCTION__);
    qDebug() << "Loading messages for peer: " << peer->id().peer_id
            << " from: " << offset << " limit: " << limit << " offline: "
            << offline;
    tgl_do_get_history(tlstate, peer->id(), offset, limit, offline,
        on_message_history, this);
}

//void qtelegram::print_success_gw(struct tgl_state *TLSR, void *extra, int success)
//{
//    if (!success) { print_fail (ev); return; }
//    else { print_success (ev); return; }
//}

void qtelegram::message_sent_status_gw(struct tgl_state *tls,
                void *extra, int success, struct tgl_message *msg)
{
    qDebug(__PRETTY_FUNCTION__);
    qtelegram *qtg = reinterpret_cast<qtelegram *>(extra);

    if (!success)
        emit qtg->error(tls->error_code, tls->error);

    qtg->write_secret_chat_file();
}

void qtelegram::send_msg(tgl_peer_id_t peer_id, QString text_message,
    unsigned long long flags, struct tl_ds_reply_markup *reply_markup)
{
    qDebug(__PRETTY_FUNCTION__);
    QByteArray msg = text_message.toUtf8();
    tgl_do_send_message(tlstate, peer_id, msg.constData(), msg.length(),
        flags/*TGL_SEND_MSG_FLAG_REPLY(reply_id) | disable_msg_preview | do_html*/,
        reply_markup, message_sent_status_gw, this);
}

void qtelegram::send_msg(QPeerId *peer, QString text_message)
{
    send_msg(peer->id(), text_message, 0, NULL);
}

void qtelegram::set_phone_number(QString number)
{
    QByteArray utf8 = number.toUtf8();
    const char *vs[] = { utf8.constData() };
    call_getvalue_callback(vs);
}

void qtelegram::set_code(QString code)
{
    QByteArray utf8 = code.toUtf8();
    const char *vs[] = { utf8.constData() };
    call_getvalue_callback(vs);
}

void qtelegram::set_current_pass(QString pass)
{
    QByteArray utf8 = pass.toUtf8();
    const char *vs[] = { utf8.constData() };
    call_getvalue_callback(vs);
}

void qtelegram::set_register_info(QString name, QString lastname)
{
    QByteArray utf8 = name.toUtf8();
    QByteArray utf8_2 = lastname.toUtf8();
    const char *vs[] = { "y", utf8.constData(), utf8_2.constData() };
    call_getvalue_callback(vs);
}

void qtelegram::set_new_password(QString pass)
{
    QByteArray utf8 = pass.toUtf8();
    const char *vs[] = { utf8.constData() };
    call_getvalue_callback(vs);
}

void qtelegram::set_cur_and_new_password(QString curp, QString newp)
{
    QByteArray utf8 = curp.toUtf8();
    QByteArray utf8_2 = newp.toUtf8();
    const char *vs[] = { utf8.constData(), utf8_2.constData() };
    call_getvalue_callback(vs);
}

QString qtelegram::get_str_from_console(QString msg)
{
    std::cout << msg.toUtf8().constData();
    std::string response;
    std::cin >> response;
    return QString::fromStdString(response);
}

QString qtelegram::auth_key_filename()
{
    return config_dir + "/auth_key";
}

void qtelegram::read_auth_file()
{
    qDebug(__PRETTY_FUNCTION__);
    int auth_file_fd = open(auth_key_filename().toUtf8().constData(), O_CREAT | O_RDWR, 0600);
    if (auth_file_fd < 0)
    {
        empty_auth_file();
        return;
    }
    assert(auth_file_fd >= 0);
    unsigned x;
    unsigned m;
    if (read(auth_file_fd, &m, 4) < 4 || (m != DC_SERIALIZED_MAGIC))
    {
        close(auth_file_fd);
        empty_auth_file();
        return;
    }
    assert(read(auth_file_fd, &x, 4) == 4);
    assert(x > 0);
    int dc_working_num;
    assert(read(auth_file_fd, &dc_working_num, 4) == 4);

    int i;
    for (i = 0; i <= (int) x; i++)
    {
        int y;
        assert(read(auth_file_fd, &y, 4) == 4);
        if (y)
        {
            read_dc(auth_file_fd, i, m);
        }
    }
    bl_do_set_working_dc(tlstate, dc_working_num);
    int our_id;
    int l = read(auth_file_fd, &our_id, 4);
    if (l < 4)
    {
        assert(!l);
    }
    if (our_id)
    {
        bl_do_set_our_id(tlstate, TGL_MK_USER(our_id));
    }
    close(auth_file_fd);
}

void qtelegram::write_auth_file()
{
    qDebug(__PRETTY_FUNCTION__);
    int auth_file_fd = open(auth_key_filename().toUtf8().constData(), O_CREAT | O_RDWR, 0600);
    assert(auth_file_fd >= 0);
    int x = DC_SERIALIZED_MAGIC;
    assert(write(auth_file_fd, &x, 4) == 4);
    assert(write(auth_file_fd, &tlstate->max_dc_num, 4) == 4);
    assert(write(auth_file_fd, &tlstate->dc_working_num, 4) == 4);

    tgl_dc_iterator_ex(tlstate, write_dc, &auth_file_fd);

    assert(write(auth_file_fd, &tlstate->our_id, 4) == 4);
    close(auth_file_fd);
}

void qtelegram::empty_auth_file()
{
    qDebug(__PRETTY_FUNCTION__);
    if (tlstate->test_mode)
    {
        bl_do_dc_option(tlstate, 0, TGL_DC_AUTO_ID, "default", strlen("default"), TELEGRAM_N9_TEST_SERVER,
            strlen(TELEGRAM_N9_TEST_SERVER), 443);
        bl_do_set_working_dc(tlstate, 0);
//        bl_do_dc_option(tlstate, 1, "", 0, TG_SERVER_TEST_1,
//            strlen(TG_SERVER_TEST_1), 443);
//        bl_do_dc_option(tlstate, 2, "", 0, TG_SERVER_TEST_2,
//            strlen(TG_SERVER_TEST_2), 443);
//        bl_do_dc_option(tlstate, 3, "", 0, TG_SERVER_TEST_3,
//            strlen(TG_SERVER_TEST_3), 443);
//        bl_do_set_working_dc(tlstate, TG_SERVER_TEST_DEFAULT);
    }
    else
    {
        bl_do_dc_option(tlstate, 0, TGL_DC_AUTO_ID, "default", strlen("default"), TELEGRAM_N9_PROD_SERVER,
            strlen(TELEGRAM_N9_PROD_SERVER), 443);
        bl_do_set_working_dc(tlstate, 0);
//        bl_do_dc_option(tlstate, 1, "", 0, TG_SERVER_1, strlen(TG_SERVER_1), 443);
//        bl_do_dc_option(tlstate, 2, "", 0, TG_SERVER_2, strlen(TG_SERVER_2), 443);
//        bl_do_dc_option(tlstate, 3, "", 0, TG_SERVER_3, strlen(TG_SERVER_3), 443);
//        bl_do_dc_option(tlstate, 4, "", 0, TG_SERVER_4, strlen(TG_SERVER_4), 443);
//        bl_do_dc_option(tlstate, 5, "", 0, TG_SERVER_5, strlen(TG_SERVER_5), 443);
//        bl_do_set_working_dc(tlstate, TG_SERVER_DEFAULT);
    }
}

void qtelegram::read_dc(int auth_file_fd, int id, unsigned ver)
{
    qDebug(__PRETTY_FUNCTION__);
    int port = 0;
    assert(read(auth_file_fd, &port, 4) == 4);
    int l = 0;
    assert(read(auth_file_fd, &l, 4) == 4);
    assert(l >= 0 && l < 100);
    char ip[100];
    assert(read(auth_file_fd, ip, l) == l);
    ip[l] = 0;

    long long auth_key_id;
    static unsigned char auth_key[256];
    assert(read(auth_file_fd, &auth_key_id, 8) == 8);
    assert(read(auth_file_fd, auth_key, 256) == 256);

    //bl_do_add_dc (id, ip, l, port, auth_key_id, auth_key);
    bl_do_dc_option(tlstate, 0, id, "DC", 2, ip, l, port);
    bl_do_set_auth_key(tlstate, id, auth_key);
    bl_do_dc_signed(tlstate, id);
}

void qtelegram::write_dc(struct tgl_dc *DC, void *extra)
{
    qDebug(__PRETTY_FUNCTION__);
    int auth_file_fd = *(int *) extra;
    if (!DC)
    {
        int x = 0;
        assert(write(auth_file_fd, &x, 4) == 4);
        return;
    }
    else
    {
        int x = 1;
        assert(write(auth_file_fd, &x, 4) == 4);
    }

    assert(DC->flags & TGLDCF_LOGGED_IN);

    assert(write(auth_file_fd, &DC->options[0]->port, 4) == 4);
    int l = strlen(DC->options[0]->ip);
    assert(write(auth_file_fd, &l, 4) == 4);
    assert(write(auth_file_fd, DC->options[0]->ip, l) == l);
    assert(write(auth_file_fd, &DC->auth_key_id, 8) == 8);
    assert(write(auth_file_fd, DC->auth_key, 256) == 256);
}

QString qtelegram::state_filename()
{
    return config_dir + "/tgl_state";
}

void qtelegram::read_state_file()
{
    qDebug(__PRETTY_FUNCTION__);
    int state_file_fd = open(state_filename().toUtf8().constData(), O_CREAT | O_RDWR, 0600);
    if (state_file_fd < 0)
    {
        return;
    }
    int version, magic;
    if (read(state_file_fd, &magic, 4) < 4)
    {
        close(state_file_fd);
        return;
    }
    if (magic != (int) STATE_FILE_MAGIC)
    {
        close(state_file_fd);
        return;
    }
    if (read(state_file_fd, &version, 4) < 4)
    {
        close(state_file_fd);
        return;
    }
    assert(version >= 0);
    int x[4];
    if (read(state_file_fd, x, 16) < 16)
    {
        close(state_file_fd);
        return;
    }
    int pts = x[0];
    int qts = x[1];
    int seq = x[2];
    int date = x[3];
    close(state_file_fd);
    bl_do_set_seq(tlstate, seq);
    bl_do_set_pts(tlstate, pts);
    bl_do_set_qts(tlstate, qts);
    bl_do_set_date(tlstate, date);
}

void qtelegram::write_state_file()
{
    qDebug(__PRETTY_FUNCTION__);
    static int wseq;
    static int wpts;
    static int wqts;
    static int wdate;
    if (wseq >= tlstate->seq && wpts >= tlstate->pts && wqts >= tlstate->qts
            && wdate >= tlstate->date)
    {
        return;
    }
    wseq = tlstate->seq;
    wpts = tlstate->pts;
    wqts = tlstate->qts;
    wdate = tlstate->date;
    int state_file_fd = open(state_filename().toUtf8().constData(), O_CREAT | O_RDWR, 0600);
    if (state_file_fd < 0)
    {
//    logprintf ("Can not write state file '%s': %m\n", get_state_filename ());
        exit(1);
    }
    int x[6];
    x[0] = STATE_FILE_MAGIC;
    x[1] = 0;
    x[2] = wpts;
    x[3] = wqts;
    x[4] = wseq;
    x[5] = wdate;
    assert(write(state_file_fd, x, 24) == 24);
    close(state_file_fd);
}

void qtelegram::read_secret_chat_file()
{
    qDebug(__PRETTY_FUNCTION__);
//  int secret_chat_fd = open (get_secret_chat_filename (), O_RDWR, 0600);
//  if (secret_chat_fd < 0) { return; }
//  //assert (secret_chat_fd >= 0);
//  int x;
//  if (read (secret_chat_fd, &x, 4) < 4) { close (secret_chat_fd); return; }
//  if (x != SECRET_CHAT_FILE_MAGIC) { close (secret_chat_fd); return; }
//  int v = 0;
//  assert (read (secret_chat_fd, &v, 4) == 4);
//  assert (v == 0 || v == 1 || v == 2); // version
//  assert (read (secret_chat_fd, &x, 4) == 4);
//  assert (x >= 0);
//  while (x --> 0) {
//    read_secret_chat (secret_chat_fd, v);
//  }
//  close (secret_chat_fd);
}

void qtelegram::write_secret_chat_file()
{
    qDebug(__PRETTY_FUNCTION__);
//  int secret_chat_fd = open (get_secret_chat_filename (), O_CREAT | O_RDWR, 0600);
//  assert (secret_chat_fd >= 0);
//  int x = SECRET_CHAT_FILE_MAGIC;
//  assert (write (secret_chat_fd, &x, 4) == 4);
//  x = 2;
//  assert (write (secret_chat_fd, &x, 4) == 4); // version
//  assert (write (secret_chat_fd, &x, 4) == 4); // num
//
//  int y[2];
//  y[0] = secret_chat_fd;
//  y[1] = 0;
//
//  tgl_peer_iterator_ex (tlstate, write_secret_chat, y);
//
//  lseek (secret_chat_fd, 8, SEEK_SET);
//  assert (write (secret_chat_fd, &y[1], 4) == 4);
//  close (secret_chat_fd);
}

void qtelegram::read_secret_chat(int fd, int v)
{
    qDebug(__PRETTY_FUNCTION__);
//  int id, l, user_id, admin_id, date, ttl, layer, state;
//  long long access_hash, key_fingerprint;
//  static char s[1000];
//  static unsigned char key[256];
//  static unsigned char sha[20];
//  assert (read (fd, &id, 4) == 4);
//  //assert (read (fd, &flags, 4) == 4);
//  assert (read (fd, &l, 4) == 4);
//  assert (l > 0 && l < 1000);
//  assert (read (fd, s, l) == l);
//  assert (read (fd, &user_id, 4) == 4);
//  assert (read (fd, &admin_id, 4) == 4);
//  assert (read (fd, &date, 4) == 4);
//  assert (read (fd, &ttl, 4) == 4);
//  assert (read (fd, &layer, 4) == 4);
//  assert (read (fd, &access_hash, 8) == 8);
//  assert (read (fd, &state, 4) == 4);
//  assert (read (fd, &key_fingerprint, 8) == 8);
//  assert (read (fd, &key, 256) == 256);
//  if (v >= 2) {
//    assert (read (fd, sha, 20) == 20);
//  } else {
//    SHA1 ((void *)key, 256, sha);
//  }
//  int in_seq_no = 0, out_seq_no = 0, last_in_seq_no = 0;
//  if (v >= 1) {
//    assert (read (fd, &in_seq_no, 4) == 4);
//    assert (read (fd, &last_in_seq_no, 4) == 4);
//    assert (read (fd, &out_seq_no, 4) == 4);
//  }
//
//  bl_do_encr_chat_new (tlstate, id,
//    &access_hash,
//    &date,
//    &admin_id,
//    &user_id,
//    key,
//    NULL,
//    sha,
//    &state,
//    &ttl,
//    &layer,
//    &in_seq_no,
//    &last_in_seq_no,
//    &out_seq_no,
//    &key_fingerprint,
//    TGLECF_CREATE | TGLECF_CREATED
//  );

}

void qtelegram::call_getvalue_callback(const char *values[])
{
    getvalues_callback getvalues_cb_cur = getvalues_cb;
    void *getvalues_arg_cur = getvalues_arg;

    // free variables for new callbacks, which might be called while we
    // call the callback function
    getvalues_cb = NULL;
    getvalues_arg = NULL;

    // the callback might call get_values again
    getvalues_cb_cur(tlstate, values, getvalues_arg_cur);
}

void qtelegram::timerEvent(QTimerEvent* event)
{
//    write_state_file();
//    if (unknown_user_list_pos)
//    {
//        int i;
//        for (i = 0; i < unknown_user_list_pos; i++)
//        {
//            tgl_do_get_user_info(tlstate, TGL_MK_USER(unknown_user_list[i]), 0,
//                0, 0);
//        }
//        unknown_user_list_pos = 0;
//    }
//    if (safe_quit && !tlstate->active_queries)
//    {
//        printf("All done. Exit\n");
//        do_halt(0);
//        safe_quit = 0;
//    }

    if (event->timerId() == get_state_timer)
    {
        qDebug("* hourly state lookup *");
        tgl_do_lookup_state(tlstate);
    }
}

//==============================================
// function callbacks
QString get_user_name(tgl_peer_id_t id, tgl_peer_t *U)
{
    QString name;
    if (!U)
    {
        name = QString("user#%1").arg(tgl_get_peer_id(id));
        // todo add tgl_get_peer_id(id) to unknown user list (to retrieve info)
        // using tgl_do_get_user_info (TLS, TGL_MK_USER (unknown_user_list[i]), 0, 0, 0);
    }
    else
    {
        if ((U->flags & TGLUF_DELETED))
            name = QString("user#%1 [deleted]").arg(tgl_get_peer_id(id));
        else if (!(U->flags & TGLUF_CREATED))
            name = QString("user#%1").arg(tgl_get_peer_id(id));
        else
        {
            if (!U->user.first_name || !strlen(U->user.first_name))
                name = QString::fromUtf8(U->user.last_name);
            else if (!U->user.last_name || !strlen(U->user.last_name))
                name = QString::fromUtf8(U->user.first_name);
            else
            {
                QString fn = QString::fromUtf8(U->user.first_name);
                QString ln = QString::fromUtf8(U->user.last_name);
                name = fn + ' ' + ln;
            }
        }
//        if (!(U->flags & TGLUF_CONTACT))
//            name = "[non-contact] " + name;
    }
    return name;
}

bool is_contact(tgl_peer_t *U)
{
    return U && (U->flags & TGLUF_CONTACT);
}

QString get_chat_name(tgl_peer_id_t id, tgl_peer_t *C)
{
    assert(tgl_get_peer_type (id) == TGL_PEER_CHAT);
    if (!C)
        return QString("chat#%1").arg(tgl_get_peer_id(id));
    else
        return QString::fromUtf8(C->chat.title);
}

QString get_channel_name(tgl_peer_id_t id, tgl_peer_t *C)
{
    assert(tgl_get_peer_type (id) == TGL_PEER_CHANNEL);
    if (!C)
        return QString("channel#%1").arg(tgl_get_peer_id(id));
    else
        return QString::fromUtf8(C->channel.title);
}

QString get_encr_chat_name(tgl_peer_id_t id, tgl_peer_t *C)
{
    assert(tgl_get_peer_type (id) == TGL_PEER_ENCR_CHAT);
    if (!C)
        return QString("encr_chat#%1").arg(tgl_get_peer_id(id));
    else
        return QString::fromUtf8(C->print_name);
}


void qtelegram::on_contact_list_updated(tgl_state *tls, void *callback_extra,
    int success, int size, tgl_user *contacts[])
{
    qDebug(__PRETTY_FUNCTION__);
    qtelegram *qtg = reinterpret_cast<qtelegram *>(callback_extra);

    QStringList user_names;
    printf("Contact list received: ");
    for (int i = size - 1; i >= 0; i--)
    {
        user_names << get_user_name(contacts[i]->id, (tgl_peer_t *) contacts[i]);
    }
    user_names.sort();

    if (!success)
        emit qtg->error(tls->error_code, tls->error);
    else
    {
        emit qtg->contact_list_received(user_names);
        emit qtg->contact_list_received(contacts, size);
    }
}

static inline QVariant mkidvar(const tgl_peer_id_t &id, QObject *parent = NULL)
{
    return QVariant::fromValue((QObject *)new QPeerId(id, parent));
}

void qtelegram::get_message(tgl_message *msg, QVariantMap &message)
{
    qDebug(__PRETTY_FUNCTION__);
    if (!msg)
        return;
//    if (!(M->flags & TGLMF_CREATED)) { qDebug("CREATED"); return; }
//    if (M->flags & TGLMF_SERVICE)
//    {
//        print_service_message(tls, M);
//        return;
//    }
    if (!(msg->flags & (TGLMF_EMPTY | TGLMF_DELETED))
        && (msg->flags & TGLMF_CREATED) && !(msg->flags & TGLMF_SERVICE))
    {
        if (!tgl_get_peer_type(msg->to_id))
        {
            qDebug("Bad msg\n");
            return;
        }
//        qDebug() << "Adding message" ;
        if (msg->message && strlen(msg->message))
            message.insert("message", QString::fromUtf8(msg->message));
        else if (msg->media.type != tgl_message_media_none)
            message.insert("message", "[media]");
//                print_media(&M->media);
    }
    message.insert("message_date", QDateTime::fromTime_t(msg->date));
    QVariant fromid = mkidvar(msg->from_id, this);
    QVariant toid = mkidvar(msg->to_id, this);
    message.insert("from_id", fromid);
    message.insert("to_id", toid);

    QVariant dialogid;
    switch (tgl_get_peer_type(msg->to_id))
    {
        case TGL_PEER_USER:
            if (msg->flags & TGLMF_OUT)
                dialogid = toid;
            else
                dialogid = fromid;
            break;
        case TGL_PEER_ENCR_CHAT:
            dialogid = toid;
//            tgl_peer_t *P = tgl_peer_get(tls, M->to_id);
//            assert(P);
//            printf(" %s", P->print_name);
            break;
        case TGL_PEER_CHAT:
            dialogid = toid;
            break;
        case TGL_PEER_CHANNEL:
            dialogid = toid;
//            if (tgl_get_peer_type (M->from_id) == TGL_PEER_USER)
//            {
//                printf (" ");
//                print_user_name (M->from_id, tgl_peer_get (tls, M->from_id));
//            }
            break;
        default:
            qDebug("Dialog ID not found!");
            break;
    }
    if (dialogid.isValid())
        message.insert("dialog_id", dialogid);
//    if (M->flags & TGLMF_OUT)
    message.insert("unread", static_cast<bool>(msg->flags & TGLMF_UNREAD));

    if (tgl_get_peer_type(msg->fwd_from_id) == TGL_PEER_USER)
    {
        message.insert("forward", true);
        message.insert("forward_from", mkidvar(msg->fwd_from_id));
    }
    if (msg->reply_id)
    {
        message.insert("reply", true);
        message.insert("reply_id", msg->reply_id);
    }
    message.insert("mention", static_cast<bool>(msg->flags & TGLMF_MENTION));
}

void qtelegram::on_dialog_list_received(tgl_state *tls, void *extra,
    int success, int size, tgl_peer_id_t peers[],
    tgl_message_id_t *last_msg_id[], int unread_count[])
{
    qDebug(__PRETTY_FUNCTION__);
    qtelegram *qtg = reinterpret_cast<qtelegram *>(extra);

    if (!success)
        emit qtg->error(tls->error_code, tls->error);

    qDebug() << "Dialogs received: from = " << qtg->last_dialog << " num =" << size;
    // using offset doesn't work as expected!
    // todo: check for duplicate dialogs!
//    if (size == 100)
//        qtg->get_dialog_list(qtg->last_dialog + size);
    for (int i = 0; i < size; ++i)
    {
        tgl_peer_t *UC = tgl_peer_get(tls, peers[i]);
        QVariantMap dlg;
        switch (tgl_get_peer_type(peers[i]))
        {
            case TGL_PEER_USER:
                qDebug() << "Peer type: User";
                dlg.insert("type", "user");
                dlg.insert("name", get_user_name(peers[i], UC));
                break;
            case TGL_PEER_CHAT:
                qDebug() << "Peer type: Chat";
                dlg.insert("type", "chat");
                dlg.insert("name", get_chat_name(peers[i], UC));
                break;
            case TGL_PEER_CHANNEL:
                qDebug() << "Peer type: Channel";
                dlg.insert("type", "channel");
                dlg.insert("name", get_channel_name(peers[i], UC));
                break;
            default:
                qDebug() << "PEER TYPE: " << tgl_get_peer_type(peers[i]);
                break;
        }
        QPeerId *idptr = new QPeerId(peers[i], qtg);
        dlg.insert("peer_id", QVariant::fromValue((QObject*)idptr));
        dlg.insert("unread_count", unread_count[i]);
        assert(last_msg_id[i] != NULL);
        tgl_message *msg = tgl_message_get(tls, last_msg_id[i]);
        if (msg)
            qtg->get_message(msg, dlg);
        dlg.insert("is_contact", is_contact(UC));

        emit qtg->dialog_received(dlg);
    }
}

void qtelegram::on_message_history(tgl_state *tls, void *extra, int success,
    int size, struct tgl_message *list[])
{
    qDebug(__PRETTY_FUNCTION__);
    qtelegram *qtg = reinterpret_cast<qtelegram *>(extra);

    if (!success)
        emit qtg->error(tls->error_code, tls->error);

    qDebug() << "Messages received: num =" << size;
    for (int i = 0; i < size; ++i)
    {
        QVariantMap message;
        qtg->get_message(list[i], message);
        emit qtg->message_received(message);
    }
    if (size > 0)
    {
        // todo is it needed?
//        if (tgl_cmp_peer_id(ML[0]->to_id, TLS->our_id))
//        {
//            tgl_do_messages_mark_read(TLS, ML[0]->to_id, ML[0]->server_id, 0, NULL,
//                NULL);
//        }
//        else
//        {
//            tgl_do_messages_mark_read(TLS, ML[0]->from_id, ML[0]->server_id, 0, NULL,
//                NULL);
//        }
    }
}
