#include "TelegramWindow.h"
#include "ui_TelegramWindow.h"

#include <assert.h>
#include <unistd.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>

#include <tgl.h>
#include <tgl-binlog.h>

#define TELEGRAM_N9_APP_HASH "923618563c7c9e07496c4aebb6924bfb"
#define TELEGRAM_N9_APP_ID 44097
#define TELEGRAM_N9_VERSION "1.0.0"
// TEST: :443
// Product: :443
#define TELEGRAM_N9_TEST_SERVER "149.154.167.40"
#define TELEGRAM_N9_PROD_SERVER "149.154.167.50"


#define DC_SERIALIZED_MAGIC 0x868aa81d
#define STATE_FILE_MAGIC 0x28949a93
#define SECRET_CHAT_FILE_MAGIC 0x37a1988a

extern tgl_update_callback upd_cb;
extern tgl_net_methods conn_methods;
extern tgl_timer_methods my_timers;

bool binlog_read = false;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
const char *get_auth_key_filename()
{
    return "AUTH_FILE";
}

void empty_auth_file(tgl_state *tlstate)
{
    qDebug(__PRETTY_FUNCTION__);
    if (tlstate->test_mode)
    {
        bl_do_dc_option(tlstate, 1, "", 0, TELEGRAM_N9_TEST_SERVER,
            strlen(TELEGRAM_N9_TEST_SERVER), 443);
        bl_do_set_working_dc(tlstate, 0); //todo(test_code)
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
        bl_do_dc_option(tlstate, 1, "", 0, TELEGRAM_N9_PROD_SERVER,
            strlen(TELEGRAM_N9_PROD_SERVER), 443);
        bl_do_set_working_dc(tlstate, 1); //todo(test_code)
//        bl_do_dc_option(tlstate, 1, "", 0, TG_SERVER_1, strlen(TG_SERVER_1), 443);
//        bl_do_dc_option(tlstate, 2, "", 0, TG_SERVER_2, strlen(TG_SERVER_2), 443);
//        bl_do_dc_option(tlstate, 3, "", 0, TG_SERVER_3, strlen(TG_SERVER_3), 443);
//        bl_do_dc_option(tlstate, 4, "", 0, TG_SERVER_4, strlen(TG_SERVER_4), 443);
//        bl_do_dc_option(tlstate, 5, "", 0, TG_SERVER_5, strlen(TG_SERVER_5), 443);
//        bl_do_set_working_dc(tlstate, TG_SERVER_DEFAULT);
    }
}

void read_dc(tgl_state *tlstate, int auth_file_fd, int id, unsigned ver)
{
    qDebug(__PRETTY_FUNCTION__);
  int port = 0;
  assert (read (auth_file_fd, &port, 4) == 4);
  int l = 0;
  assert (read (auth_file_fd, &l, 4) == 4);
  assert (l >= 0 && l < 100);
  char ip[100];
  assert (read (auth_file_fd, ip, l) == l);
  ip[l] = 0;

  long long auth_key_id;
  static unsigned char auth_key[256];
  assert (read (auth_file_fd, &auth_key_id, 8) == 8);
  assert (read (auth_file_fd, auth_key, 256) == 256);

  //bl_do_add_dc (id, ip, l, port, auth_key_id, auth_key);
  bl_do_dc_option (tlstate, id, "DC", 2, ip, l, port);
  bl_do_set_auth_key (tlstate, id, auth_key);
  bl_do_dc_signed (tlstate, id);
}

void read_auth_file(tgl_state *tlstate)
{
    qDebug(__PRETTY_FUNCTION__);
  int auth_file_fd = open (get_auth_key_filename(), O_CREAT | O_RDWR, 0600);
  if (auth_file_fd < 0) {
    empty_auth_file (tlstate);
    return;
  }
  assert (auth_file_fd >= 0);
  unsigned x;
  unsigned m;
  if (read (auth_file_fd, &m, 4) < 4 || (m != DC_SERIALIZED_MAGIC)) {
    close (auth_file_fd);
    empty_auth_file (tlstate);
    return;
  }
  assert (read (auth_file_fd, &x, 4) == 4);
  assert (x > 0);
  int dc_working_num;
  assert (read (auth_file_fd, &dc_working_num, 4) == 4);

  int i;
  for (i = 0; i <= (int)x; i++) {
    int y;
    assert (read (auth_file_fd, &y, 4) == 4);
    if (y) {
      read_dc (tlstate, auth_file_fd, i, m);
    }
  }
  bl_do_set_working_dc (tlstate, dc_working_num);
  int our_id;
  int l = read (auth_file_fd, &our_id, 4);
  if (l < 4) {
    assert (!l);
  }
  if (our_id) {
    bl_do_set_our_id (tlstate, our_id);
  }
  close (auth_file_fd);
}

void write_dc (struct tgl_dc *DC, void *extra) {
    qDebug(__PRETTY_FUNCTION__);
    int auth_file_fd = *(int *)extra;
    if (!DC) {
      int x = 0;
      assert (write (auth_file_fd, &x, 4) == 4);
      return;
    } else {
      int x = 1;
      assert (write (auth_file_fd, &x, 4) == 4);
    }

    assert (DC->flags & TGLDCF_LOGGED_IN);

    assert (write (auth_file_fd, &DC->options[0]->port, 4) == 4);
    int l = strlen (DC->options[0]->ip);
    assert (write (auth_file_fd, &l, 4) == 4);
    assert (write (auth_file_fd, DC->options[0]->ip, l) == l);
    assert (write (auth_file_fd, &DC->auth_key_id, 8) == 8);
    assert (write (auth_file_fd, DC->auth_key, 256) == 256);
}

void write_auth_file (tgl_state *tlstate) {
    qDebug(__PRETTY_FUNCTION__);
  int auth_file_fd = open (get_auth_key_filename(), O_CREAT | O_RDWR, 0600);
  assert (auth_file_fd >= 0);
  int x = DC_SERIALIZED_MAGIC;
  assert (write (auth_file_fd, &x, 4) == 4);
  assert (write (auth_file_fd, &tlstate->max_dc_num, 4) == 4);
  assert (write (auth_file_fd, &tlstate->dc_working_num, 4) == 4);

  tgl_dc_iterator_ex (tlstate, write_dc, &auth_file_fd);

  assert (write (auth_file_fd, &tlstate->our_id, 4) == 4);
  close (auth_file_fd);
}

const char *get_state_filename (void) {
  return "STATE_FILE";
}

void read_state_file (tgl_state *tlstate) {
    qDebug(__PRETTY_FUNCTION__);
  int state_file_fd = open (get_state_filename (), O_CREAT | O_RDWR, 0600);
  if (state_file_fd < 0) {
    return;
  }
  int version, magic;
  if (read (state_file_fd, &magic, 4) < 4) { close (state_file_fd); return; }
  if (magic != (int)STATE_FILE_MAGIC) { close (state_file_fd); return; }
  if (read (state_file_fd, &version, 4) < 4) { close (state_file_fd); return; }
  assert (version >= 0);
  int x[4];
  if (read (state_file_fd, x, 16) < 16) {
    close (state_file_fd);
    return;
  }
  int pts = x[0];
  int qts = x[1];
  int seq = x[2];
  int date = x[3];
  close (state_file_fd);
  bl_do_set_seq (tlstate, seq);
  bl_do_set_pts (tlstate, pts);
  bl_do_set_qts (tlstate, qts);
  bl_do_set_date (tlstate, date);
}


void write_state_file (tgl_state *tlstate) {
    qDebug(__PRETTY_FUNCTION__);
  static int wseq;
  static int wpts;
  static int wqts;
  static int wdate;
  if (wseq >= tlstate->seq && wpts >= tlstate->pts && wqts >= tlstate->qts && wdate >= tlstate->date) { return; }
  wseq = tlstate->seq; wpts = tlstate->pts; wqts = tlstate->qts; wdate = tlstate->date;
  int state_file_fd = open (get_state_filename (), O_CREAT | O_RDWR, 0600);
  if (state_file_fd < 0) {
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
  assert (write (state_file_fd, x, 24) == 24);
  close (state_file_fd);
}

void read_secret_chat (int fd, int v) {
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

void read_secret_chat_file (void) {
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

void write_secret_chat_file()
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

/////////////////////////////////////////////////////////////////////////////
TelegramWindow::TelegramWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::TelegramWindow)
{
    ui->setupUi(this);

    tgl_allocator = &tgl_allocator_debug; // todo(test_code) change to release
    tlstate = tgl_state_alloc();
    tgl_set_test_mode(tlstate); // todo(test_code) remove for production
    tgl_set_binlog_mode (tlstate, 0);
    tgl_incr_verbosity (tlstate);
    tgl_incr_verbosity (tlstate);
    tgl_incr_verbosity (tlstate);
    tgl_incr_verbosity (tlstate);
    tgl_incr_verbosity (tlstate);

    tgl_set_rsa_key(tlstate, "/etc/telegram-n9/server.pub");
    tgl_set_rsa_key(tlstate, "tg-server.pub");
    tgl_set_callback(tlstate, &upd_cb);
    tgl_set_ev_base(tlstate, this);
    tgl_set_net_methods(tlstate, &conn_methods);
    tgl_set_timer_methods(tlstate, &my_timers);
    assert(tlstate->timer_methods);
    tgl_set_download_directory(tlstate, "downloads/");
    tgl_register_app_id(tlstate, TELEGRAM_N9_APP_ID, TELEGRAM_N9_APP_HASH);
    tgl_set_app_version(tlstate, "Telegram-N9 " TELEGRAM_N9_VERSION);
//    if (ipv6_enabled)
//    {
//        tgl_enable_ipv6 (tlstate);
//    }
//    if (disable_link_preview)
//    {
//        tgl_disable_link_preview (tlstate);
//    }
    tgl_init(tlstate);

    read_auth_file(tlstate);
    read_state_file(tlstate);
    read_secret_chat_file();

    binlog_read = true;

//    if (reset_authorization) {
//      tgl_peer_t *P = tgl_peer_get (tlstate, TGL_MK_USER (tlstate->our_id));
//      if (P && P->user.phone && reset_authorization == 1) {
//        set_default_username (P->user.phone);
//      }
//      bl_do_reset_authorization (tlstate);
//    }

    tgl_login (tlstate);

    // net loop!
//    while (true)
//    {
//        if (safe_quit && !tlstate->active_queries) {
//          printf ("All done. Exit\n");
//          do_halt (0);
//          safe_quit = 0;
//        }
//        if (time (0) - last_get_state > 3600) {
//          tgl_do_lookup_state (tlstate);
//          last_get_state = time (0);
//        }

//        write_state_file ();
//        if (unknown_user_list_pos) {
//          int i;
//          for (i = 0; i < unknown_user_list_pos; i++) {
//            tgl_do_get_user_info (tlstate, TGL_MK_USER (unknown_user_list[i]), 0, 0, 0);
//          }
//          unknown_user_list_pos = 0;
//        }
//    }
}

TelegramWindow::~TelegramWindow()
{
    delete ui;
}

void TelegramWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type())
    {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}
