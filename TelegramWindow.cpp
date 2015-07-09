#include "TelegramWindow.h"
#include "ui_TelegramWindow.h"

#include <assert.h>
#include <cstdio>
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

extern tgl_net_methods conn_methods;
extern tgl_timer_methods my_timers;

struct tgl_state *TLS;
bool binlog_read = false;

/////////////////////////////////////////////////////////////////////////////
const char *get_auth_key_filename()
{
    return "AUTH_FILE";
}
void write_secret_chat_file(void)
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
//  tgl_peer_iterator_ex (TLS, write_secret_chat, y);
//
//  lseek (secret_chat_fd, 8, SEEK_SET);
//  assert (write (secret_chat_fd, &y[1], 4) == 4);
//  close (secret_chat_fd);
}

void print_message (struct tgl_message *M) {
  assert (M);
    qDebug(__PRETTY_FUNCTION__);
//  if (M->flags & (TGLMF_EMPTY | TGLMF_DELETED)) {
//    return;
//  }
//  if (!(M->flags & TGLMF_CREATED)) { return; }
//  if (M->flags & TGLMF_SERVICE) {
//    print_service_message (ev, M);
//    return;
//  }
//  if (!tgl_get_peer_type(M->to_id)) {
//    logprintf ("Bad msg\n");
//    return;
//  }
//
//  last_from_id = M->from_id;
//  last_to_id = M->to_id;
//
//  //print_start ();
//  if (tgl_get_peer_type (M->to_id) == TGL_PEER_USER) {
//    if (M->flags & TGLMF_OUT) {
//      mpush_color (ev, COLOR_GREEN);
//      if (msg_num_mode) {
//        mprintf (ev, "%lld ", M->id);
//      }
//      print_date (ev, M->date);
//      mpop_color (ev);
//      mprintf (ev, " ");
//      print_user_name (ev, M->to_id, tgl_peer_get (TLS, M->to_id));
//      mpush_color (ev, COLOR_GREEN);
//      if (M->flags & TGLMF_UNREAD) {
//        mprintf (ev, " <<< ");
//      } else {
//        mprintf (ev, " ««« ");
//      }
//    } else {
//      mpush_color (ev, COLOR_BLUE);
//      if (msg_num_mode) {
//        mprintf (ev, "%lld ", M->id);
//      }
//      print_date (ev, M->date);
//      mpop_color (ev);
//      mprintf (ev, " ");
//      print_user_name (ev, M->from_id, tgl_peer_get (TLS, M->from_id));
//      mpush_color (ev, COLOR_BLUE);
//      if (M->flags & TGLMF_UNREAD) {
//        mprintf (ev, " >>> ");
//      } else {
//        mprintf (ev, " »»» ");
//      }
//    }
//  } else if (tgl_get_peer_type (M->to_id) == TGL_PEER_ENCR_CHAT) {
//    tgl_peer_t *P = tgl_peer_get (TLS, M->to_id);
//    assert (P);
//    if (M->flags & TGLMF_UNREAD) {
//      mpush_color (ev, COLOR_GREEN);
//      if (msg_num_mode) {
//        mprintf (ev, "%lld ", M->id);
//      }
//      print_date (ev, M->date);
//      mprintf (ev, " ");
//      mpush_color (ev, COLOR_CYAN);
//      mprintf (ev, " %s", P->print_name);
//      mpop_color (ev);
//      if (M->flags & TGLMF_UNREAD) {
//        mprintf (ev, " <<< ");
//      } else {
//        mprintf (ev, " ««« ");
//      }
//    } else {
//      mpush_color (ev, COLOR_BLUE);
//      if (msg_num_mode) {
//        mprintf (ev, "%lld ", M->id);
//      }
//      print_date (ev, M->date);
//      mpush_color (ev, COLOR_CYAN);
//      mprintf (ev, " %s", P->print_name);
//      mpop_color (ev);
//      if (M->flags & TGLMF_UNREAD) {
//        mprintf (ev, " >>> ");
//      } else {
//        mprintf (ev, " »»» ");
//      }
//    }
//  } else {
//    assert (tgl_get_peer_type (M->to_id) == TGL_PEER_CHAT);
//    mpush_color (ev, COLOR_MAGENTA);
//    if (msg_num_mode) {
//      mprintf (ev, "%lld ", M->id);
//    }
//    print_date (ev, M->date);
//    mpop_color (ev);
//    mprintf (ev, " ");
//    print_chat_name (ev, M->to_id, tgl_peer_get (TLS, M->to_id));
//    mprintf (ev, " ");
//    print_user_name (ev, M->from_id, tgl_peer_get (TLS, M->from_id));
//    if ((tgl_get_peer_type (M->from_id) == TGL_PEER_USER) && (tgl_get_peer_id (M->from_id) == TLS->our_id)) {
//      mpush_color (ev, COLOR_GREEN);
//    } else {
//      mpush_color (ev, COLOR_BLUE);
//    }
//    if (M->flags & TGLMF_UNREAD) {
//      mprintf (ev, " >>> ");
//    } else {
//      mprintf (ev, " »»» ");
//    }
//  }
//  if (tgl_get_peer_type (M->fwd_from_id) == TGL_PEER_USER) {
//    mprintf (ev, "[fwd from ");
//    print_user_name (ev, M->fwd_from_id, tgl_peer_get (TLS, M->fwd_from_id));
//    mprintf (ev, "] ");
//  }
//  if (M->reply_id) {
//    mprintf (ev, "[reply to %d] ", M->reply_id);
//  }
//  if (M->flags & TGLMF_MENTION) {
//    mprintf (ev, "[mention] ");
//  }
//  if (M->message && strlen (M->message)) {
//    mprintf (ev, "%s", M->message);
//  }
//  if (M->media.type != tgl_message_media_none) {
//    if (M->message && strlen (M->message)) {
//      mprintf (ev, " ");
//    }
//    print_media (ev, &M->media);
//  }
//  mpop_color (ev);
//  assert (!color_stack_pos);
//  mprintf (ev, "\n");
}

void print_message_gw (struct tgl_state *TLSR, struct tgl_message *M) {
  assert (TLSR == TLS);
  qDebug(__PRETTY_FUNCTION__);
  if (!binlog_read) { qDebug("\t!binlog_read, returning"); return; }
  if (tgl_get_peer_type (M->to_id) == TGL_PEER_ENCR_CHAT) {
    write_secret_chat_file ();
  }
    print_message (M);
}

void mark_read_upd (struct tgl_state *TLSR, int num, struct tgl_message *list[]) {
  assert (TLSR == TLS);
  qDebug(__PRETTY_FUNCTION__);
  if (!binlog_read) { qDebug("\t!binlog_read, returning"); return; }
//  if (log_level < 1) { return; }
//
//  if (unread_message_count + num <= MAX_UNREAD_MESSAGE_COUNT) {
//    memcpy (unread_message_list + unread_message_count, list, num * sizeof (void *));
//    unread_message_count += num;
//
//    if (!unread_message_event) {
//      unread_message_event = evtimer_new (TLS->ev_base, unread_message_alarm, 0);
//      static struct timeval ptimeout = { 1, 0};
//      event_add (unread_message_event, &ptimeout);
//    }
//  } else {
//    print_read_list (unread_message_count, unread_message_list);
//    print_read_list (num, list);
//    unread_message_count = 0;
//    if (unread_message_event) {
//      event_free (unread_message_event);
//      unread_message_event = 0;
//    }
//  }
}

void logprintf (const char *format, ...) {
  int x = 0;
  printf (" *** ");
  va_list ap;
  va_start (ap, format);
  vfprintf (stdout, format, ap);
  va_end (ap);
}

void do_get_string (struct tgl_state *TLS, const char *prompt, int flags, void (*cb)(struct tgl_state *, const char *, void *), void *arg) {
    qDebug(__PRETTY_FUNCTION__);
  printf ("GET STR: %s ", prompt);
//  one_string_prompt = strdup (prompt);
//  one_string_flags = flags;
//  read_one_string = 1;
//  on_string_cb = cb;
//  string_cb_arg = arg;
//  one_string_len = 0;
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

void write_auth_file (void) {
    qDebug(__PRETTY_FUNCTION__);
  int auth_file_fd = open (get_auth_key_filename(), O_CREAT | O_RDWR, 0600);
  assert (auth_file_fd >= 0);
  int x = DC_SERIALIZED_MAGIC;
  assert (write (auth_file_fd, &x, 4) == 4);
  assert (write (auth_file_fd, &TLS->max_dc_num, 4) == 4);
  assert (write (auth_file_fd, &TLS->dc_working_num, 4) == 4);

  tgl_dc_iterator_ex (TLS, write_dc, &auth_file_fd);

  assert (write (auth_file_fd, &TLS->our_id, 4) == 4);
  close (auth_file_fd);
}

void on_login (struct tgl_state *TLS) {
    qDebug(__PRETTY_FUNCTION__);
  write_auth_file ();
}

void on_started (struct tgl_state *TLS) {
    qDebug(__PRETTY_FUNCTION__);
//  if (wait_dialog_list) { // boolean, user option
//    wait_dialog_list = 0;
//    tgl_do_get_dialog_list (TLS, 100, 0, dlist_cb, 0);
//    return;
//  }
}

//void print_typing (struct in_ev *ev, enum tgl_typing_status status) {
//  switch (status) {
//  case tgl_typing_none:
//    mprintf (ev, "doing nothing");
//    break;
//  case tgl_typing_typing:
//    mprintf (ev, "typing");
//    break;
//  case tgl_typing_cancel:
//    mprintf (ev, "deleting typed message");
//    break;
//  case tgl_typing_record_video:
//    mprintf (ev, "recording video");
//    break;
//  case tgl_typing_upload_video:
//    mprintf (ev, "uploading video");
//    break;
//  case tgl_typing_record_audio:
//    mprintf (ev, "recording audio");
//    break;
//  case tgl_typing_upload_audio:
//    mprintf (ev, "uploading audio");
//    break;
//  case tgl_typing_upload_photo:
//    mprintf (ev, "uploading photo");
//    break;
//  case tgl_typing_upload_document:
//    mprintf (ev, "uploading document");
//    break;
//  case tgl_typing_geo:
//    mprintf (ev, "choosing location");
//    break;
//  case tgl_typing_choose_contact:
//    mprintf (ev, "choosing contact");
//    break;
//  }
//}

void type_notification_upd (struct tgl_state *TLSR, struct tgl_user *U, enum tgl_typing_status status) {
  assert (TLSR == TLS);
  qDebug(__PRETTY_FUNCTION__);
//  if (log_level < 2 || (disable_output && !notify_ev)) { return; }
//  if (enable_json) { return; }
//  struct in_ev *ev = notify_ev;
//  mprint_start (ev);
//  mpush_color (ev, COLOR_YELLOW);
//  mprintf (ev, "User ");
//  print_user_name (ev, U->id, (void *)U);
//  mprintf (ev, " is ");
//  print_typing (ev, status);
//  mprintf (ev, "\n");
//  mpop_color (ev);
//  mprint_end (ev);
}

void type_in_chat_notification_upd (struct tgl_state *TLSR, struct tgl_user *U, struct tgl_chat *C, enum tgl_typing_status status) {
  assert (TLSR == TLS);
  qDebug(__PRETTY_FUNCTION__);
//  if (log_level < 2 || (disable_output && !notify_ev)) { return; }
//  if (enable_json) { return; }
//  struct in_ev *ev = notify_ev;
//  mprint_start (ev);
//  mpush_color (ev, COLOR_YELLOW);
//  mprintf (ev, "User ");
//  print_user_name (ev, U->id, (void *)U);
//  mprintf (ev, " is ");
//  print_typing (ev, status);
//  mprintf (ev, " in chat ");
//  print_chat_name (ev, C->id, (void *)C);
//  mprintf (ev, "\n");
//  mpop_color (ev);
//  mprint_end (ev);
}


void user_update_gw (struct tgl_state *TLSR, struct tgl_user *U, unsigned flags) {
  assert (TLSR == TLS);
  qDebug(__PRETTY_FUNCTION__);

//  if (disable_output && !notify_ev) { return; }
//  if (!binlog_read) { return; }
//  struct in_ev *ev = notify_ev;
//
//  if (!(flags & TGL_UPDATE_CREATED)) {
//    mprint_start (ev);
//    if (!enable_json) {
//      mpush_color (ev, COLOR_YELLOW);
//      mprintf (ev, "User ");
//      print_user_name (ev, U->id, (void *)U);
//      if (!(flags & TGL_UPDATE_DELETED)) {
//        mprintf (ev, " updated");
//        print_peer_updates (ev, flags);
//      } else {
//        mprintf (ev, " deleted");
//      }
//      mprintf (ev, "\n");
//      mpop_color (ev);
//    } else {
//      json_peer_update (ev, (void *)U, flags);
//    }
//    mprint_end (ev);
//  }
}

//void print_peer_updates (struct in_ev *ev, int flags) {
//  if (flags & TGL_UPDATE_PHONE) {
//    mprintf (ev, " phone");
//  }
//  if (flags & TGL_UPDATE_CONTACT) {
//    mprintf (ev, " contact");
//  }
//  if (flags & TGL_UPDATE_PHOTO) {
//    mprintf (ev, " photo");
//  }
//  if (flags & TGL_UPDATE_BLOCKED) {
//    mprintf (ev, " blocked");
//  }
//  if (flags & TGL_UPDATE_REAL_NAME) {
//    mprintf (ev, " name");
//  }
//  if (flags & TGL_UPDATE_NAME) {
//    mprintf (ev, " contact_name");
//  }
//  if (flags & TGL_UPDATE_REQUESTED) {
//    mprintf (ev, " status");
//  }
//  if (flags & TGL_UPDATE_WORKING) {
//    mprintf (ev, " status");
//  }
//  if (flags & TGL_UPDATE_FLAGS) {
//    mprintf (ev, " flags");
//  }
//  if (flags & TGL_UPDATE_TITLE) {
//    mprintf (ev, " title");
//  }
//  if (flags & TGL_UPDATE_ADMIN) {
//    mprintf (ev, " admin");
//  }
//  if (flags & TGL_UPDATE_MEMBERS) {
//    mprintf (ev, " members");
//  }
//  if (flags & TGL_UPDATE_ACCESS_HASH) {
//    mprintf (ev, " access_hash");
//  }
//  if (flags & TGL_UPDATE_USERNAME) {
//    mprintf (ev, " username");
//  }
//}

void chat_update_gw (struct tgl_state *TLSR, struct tgl_chat *U, unsigned flags) {
  assert (TLSR == TLS);
  qDebug(__PRETTY_FUNCTION__);

//  if (disable_output && !notify_ev) { return; }
//  if (!binlog_read) { return; }
//  struct in_ev *ev = notify_ev;
//
//  if (!(flags & TGL_UPDATE_CREATED)) {
//    mprint_start (ev);
//    if (!enable_json) {
//      mpush_color (ev, COLOR_YELLOW);
//      mprintf (ev, "Chat ");
//      print_chat_name (ev, U->id, (void *)U);
//      if (!(flags & TGL_UPDATE_DELETED)) {
//        mprintf (ev, " updated");
//        print_peer_updates (ev, flags);
//      } else {
//        mprintf (ev, " deleted");
//      }
//      mprintf (ev, "\n");
//      mpop_color (ev);
//    } else {
//      json_peer_update (ev, (void *)U, flags);
//    }
//    mprint_end (ev);
//  }
}

void secret_chat_update_gw (struct tgl_state *TLSR, struct tgl_secret_chat *U, unsigned flags) {
  assert (TLSR == TLS);
  qDebug(__PRETTY_FUNCTION__);


//
//  if ((flags & TGL_UPDATE_WORKING) || (flags & TGL_UPDATE_DELETED)) {
//    write_secret_chat_file ();
//  }
//
//  if (!binlog_read) { return; }
//
//  if ((flags & TGL_UPDATE_REQUESTED) && !disable_auto_accept)  {
//    //tgl_do_accept_encr_chat_request (TLS, U, 0, 0);
//    tgl_do_accept_encr_chat_request (TLS, U, print_encr_chat_success_gw, 0);
//  }
//
//  if (disable_output && !notify_ev) { return; }
//  struct in_ev *ev = notify_ev;
//
//
//  if (!(flags & TGL_UPDATE_CREATED)) {
//    mprint_start (ev);
//    if (!enable_json) {
//      mpush_color (ev, COLOR_YELLOW);
//      mprintf (ev, "Secret chat ");
//      print_encr_chat_name (ev, U->id, (void *)U);
//      if (!(flags & TGL_UPDATE_DELETED)) {
//        mprintf (ev, " updated");
//        print_peer_updates (ev, flags);
//      } else {
//        mprintf (ev, " deleted");
//      }
//      mprintf (ev, "\n");
//      mpop_color (ev);
//    } else {
//      json_peer_update (ev, (void *)U, flags);
//    }
//    mprint_end (ev);
//  }
}

void our_id_gw (struct tgl_state *TLSR, int id) {
  assert (TLSR == TLS);
  qDebug(__PRETTY_FUNCTION__);
}

void user_status_upd (struct tgl_state *TLS, struct tgl_user *U) {
    qDebug(__PRETTY_FUNCTION__);
//  if (disable_output && !notify_ev) { return; }
//  if (!binlog_read) { return; }
//  if (log_level < 3) { return; }
//  struct in_ev *ev = notify_ev;
//  mprint_start (ev);
//    mpush_color (ev, COLOR_YELLOW);
//    mprintf (ev, "User ");
//    print_user_name(ev, U->id, (void *) U);
//    mprintf (ev, " ");
//    print_user_status(&U->status, ev);
//    mprintf (ev, "\n");
//    mpop_color (ev);
//  mprint_end (ev);
}

tgl_update_callback upd_cb =
{
    print_message_gw, // new_msg
    mark_read_upd,
    logprintf,
    do_get_string,
    on_login,
    on_started,
    type_notification_upd,
    type_in_chat_notification_upd,
    //void (*type_in_secret_chat_notification)(struct tgl_state *TLS, struct tgl_secret_chat *E);
    0,
    //void (*status_notification)(struct tgl_state *TLS, struct tgl_user *U);
    0,
    //void (*user_registered)(struct tgl_state *TLS, struct tgl_user *U);
    0,
    //void (*user_activated)(struct tgl_state *TLS, struct tgl_user *U);
    0,
    //void (*new_authorization)(struct tgl_state *TLS, const char *device, const char *location);
    0,
    chat_update_gw,
    user_update_gw,
    secret_chat_update_gw,
    print_message_gw, // msg_receive
    our_id_gw,
    //void (*notification)(struct tgl_state *TLS, const char *type, const char *message);
    0,
    user_status_upd,
    //char *(*create_print_name) (struct tgl_state *TLS, tgl_peer_id_t id, const char *a1, const char *a2, const char *a3, const char *a4);
    0
};

/////////////////////////////////////////////////////////////////////////////
void empty_auth_file(void)
{
    qDebug(__PRETTY_FUNCTION__);
    if (TLS->test_mode)
    {
        bl_do_dc_option(TLS, 1, "", 0, TELEGRAM_N9_TEST_SERVER,
            strlen(TELEGRAM_N9_TEST_SERVER), 443);
        bl_do_set_working_dc(TLS, 1); //todo(test_code)
//        bl_do_dc_option(TLS, 1, "", 0, TG_SERVER_TEST_1,
//            strlen(TG_SERVER_TEST_1), 443);
//        bl_do_dc_option(TLS, 2, "", 0, TG_SERVER_TEST_2,
//            strlen(TG_SERVER_TEST_2), 443);
//        bl_do_dc_option(TLS, 3, "", 0, TG_SERVER_TEST_3,
//            strlen(TG_SERVER_TEST_3), 443);
//        bl_do_set_working_dc(TLS, TG_SERVER_TEST_DEFAULT);
    }
    else
    {
        bl_do_dc_option(TLS, 1, "", 0, TG_SERVER_1, strlen(TG_SERVER_1), 443);
        bl_do_dc_option(TLS, 2, "", 0, TG_SERVER_2, strlen(TG_SERVER_2), 443);
        bl_do_dc_option(TLS, 3, "", 0, TG_SERVER_3, strlen(TG_SERVER_3), 443);
        bl_do_dc_option(TLS, 4, "", 0, TG_SERVER_4, strlen(TG_SERVER_4), 443);
        bl_do_dc_option(TLS, 5, "", 0, TG_SERVER_5, strlen(TG_SERVER_5), 443);
        bl_do_set_working_dc(TLS, TG_SERVER_DEFAULT);
    }
}

void read_dc (int auth_file_fd, int id, unsigned ver) {
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
  bl_do_dc_option (TLS, id, "DC", 2, ip, l, port);
  bl_do_set_auth_key (TLS, id, auth_key);
  bl_do_dc_signed (TLS, id);
}

void read_auth_file (void) {
    qDebug(__PRETTY_FUNCTION__);
  int auth_file_fd = open (get_auth_key_filename(), O_CREAT | O_RDWR, 0600);
  if (auth_file_fd < 0) {
    empty_auth_file ();
    return;
  }
  assert (auth_file_fd >= 0);
  unsigned x;
  unsigned m;
  if (read (auth_file_fd, &m, 4) < 4 || (m != DC_SERIALIZED_MAGIC)) {
    close (auth_file_fd);
    empty_auth_file ();
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
      read_dc (auth_file_fd, i, m);
    }
  }
  bl_do_set_working_dc (TLS, dc_working_num);
  int our_id;
  int l = read (auth_file_fd, &our_id, 4);
  if (l < 4) {
    assert (!l);
  }
  if (our_id) {
    bl_do_set_our_id (TLS, our_id);
  }
  close (auth_file_fd);
}

const char *get_state_filename (void) {
  return "STATE_FILE";
}

void read_state_file (void) {
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
  bl_do_set_seq (TLS, seq);
  bl_do_set_pts (TLS, pts);
  bl_do_set_qts (TLS, qts);
  bl_do_set_date (TLS, date);
}


void write_state_file (void) {
    qDebug(__PRETTY_FUNCTION__);
  static int wseq;
  static int wpts;
  static int wqts;
  static int wdate;
  if (wseq >= TLS->seq && wpts >= TLS->pts && wqts >= TLS->qts && wdate >= TLS->date) { return; }
  wseq = TLS->seq; wpts = TLS->pts; wqts = TLS->qts; wdate = TLS->date;
  int state_file_fd = open (get_state_filename (), O_CREAT | O_RDWR, 0600);
  if (state_file_fd < 0) {
    logprintf ("Can not write state file '%s': %m\n", get_state_filename ());
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
//  bl_do_encr_chat_new (TLS, id,
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
/////////////////////////////////////////////////////////////////////////////
TelegramWindow::TelegramWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::TelegramWindow)
{
    ui->setupUi(this);

    tgl_allocator = &tgl_allocator_debug; // todo(test_code) change to release
    TLS = tgl_state_alloc();
    tgl_set_test_mode(TLS); // todo(test_code) remove for production
    tgl_set_binlog_mode (TLS, 0);
    tgl_incr_verbosity (TLS);
    tgl_incr_verbosity (TLS);
    tgl_incr_verbosity (TLS);
    tgl_incr_verbosity (TLS);
    tgl_incr_verbosity (TLS);

    tgl_set_rsa_key(TLS, "/etc/telegram-n9/server.pub");
    tgl_set_rsa_key(TLS, "tg-server.pub");
    tgl_set_callback(TLS, &upd_cb);
    tgl_set_ev_base(TLS, this);
    tgl_set_net_methods(TLS, &conn_methods);
    tgl_set_timer_methods(TLS, &my_timers);
    assert(TLS->timer_methods);
    tgl_set_download_directory(TLS, "downloads/");
    tgl_register_app_id(TLS, TELEGRAM_N9_APP_ID, TELEGRAM_N9_APP_HASH);
    tgl_set_app_version(TLS, "Telegram-N9 " TELEGRAM_N9_VERSION);
//    if (ipv6_enabled)
//    {
//        tgl_enable_ipv6 (TLS);
//    }
//    if (disable_link_preview)
//    {
//        tgl_disable_link_preview (TLS);
//    }
    tgl_init(TLS);

    read_auth_file();
    read_state_file();
    read_secret_chat_file();

    binlog_read = true;

//    if (reset_authorization) {
//      tgl_peer_t *P = tgl_peer_get (TLS, TGL_MK_USER (TLS->our_id));
//      if (P && P->user.phone && reset_authorization == 1) {
//        set_default_username (P->user.phone);
//      }
//      bl_do_reset_authorization (TLS);
//    }

    tgl_login (TLS);

    // net loop!
//    while (true)
//    {
//        if (safe_quit && !TLS->active_queries) {
//          printf ("All done. Exit\n");
//          do_halt (0);
//          safe_quit = 0;
//        }
//        if (time (0) - last_get_state > 3600) {
//          tgl_do_lookup_state (TLS);
//          last_get_state = time (0);
//        }

//        write_state_file ();
//        if (unknown_user_list_pos) {
//          int i;
//          for (i = 0; i < unknown_user_list_pos; i++) {
//            tgl_do_get_user_info (TLS, TGL_MK_USER (unknown_user_list[i]), 0, 0, 0);
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
