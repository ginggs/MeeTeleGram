/**
 * qtconnection.cpp
 *
 *  Created on: ۱۸ تیر ۱۳۹۴ ه‍.ش.
 *      Author: Hedayat Vatankhah <hedayat.fwd@gmail.com>.
 */

#include <QObject>
#include <tgl.h>

struct connection {
        tgl_dc *dc;
        tgl_session *session;
};


int write_out (struct connection *c, const void *_data, int len) {
    qDebug(__PRETTY_FUNCTION__);
//  struct tgl_state *TLS = c->TLS;
//  const unsigned char *data = _data;
//  if (!len) { return 0; }
//  assert (len > 0);
  int x = 0;
//  while (len) {
//    if (c->out_tail->end - c->out_tail->wptr >= len) {
//      memcpy (c->out_tail->wptr, data, len);
//      c->out_tail->wptr += len;
//      c->out_bytes += len;
//      return x + len;
//    } else {
//      int y = c->out_tail->end - c->out_tail->wptr;
//      assert (y < len);
//      memcpy (c->out_tail->wptr, data, y);
//      x += y;
//      len -= y;
//      data += y;
////      struct connection_buffer *b = new_connection_buffer (1 << 20);
////      c->out_tail->next = b;
////      b->next = 0;
////      c->out_tail = b;
//      c->out_bytes += y;
//    }
//  }
  return x;
}

int read_in (struct connection *c, void *_data, int len) {
    qDebug(__PRETTY_FUNCTION__);
//  unsigned char *data = _data;
//  if (!len) { return 0; }
//  assert (len > 0);
//  if (len > c->in_bytes) {
//    len = c->in_bytes;
//  }
  int x = 0;
//  while (len) {
//    int y = c->in_head->wptr - c->in_head->rptr;
//    if (y > len) {
//      memcpy (data, c->in_head->rptr, len);
//      c->in_head->rptr += len;
//      c->in_bytes -= len;
//      return x + len;
//    } else {
//      memcpy (data, c->in_head->rptr, y);
//      c->in_bytes -= y;
//      x += y;
//      data += y;
//      len -= y;
//      void *old = c->in_head;
//      c->in_head = c->in_head->next;
//      if (!c->in_head) {
//        c->in_tail = 0;
//      }
////      delete_connection_buffer (old);
//    }
//  }
  return x;
}

int read_in_lookup (struct connection *c, void *_data, int len) {
    qDebug(__PRETTY_FUNCTION__);
    // read but not change read pointer
//  unsigned char *data = _data;
//  if (!len || !c->in_bytes) { return 0; }
//  assert (len > 0);
//  if (len > c->in_bytes) {
//    len = c->in_bytes;
//  }
  int x = 0;
//  struct connection_buffer *b = c->in_head;
//  while (len) {
//    int y = b->wptr - b->rptr;
//    if (y >= len) {
//      memcpy (data, b->rptr, len);
//      return x + len;
//    } else {
//      memcpy (data, b->rptr, y);
//      x += y;
//      data += y;
//      len -= y;
//      b = b->next;
//    }
//  }
  return x;
}

static void incr_out_packet_num (struct connection *c) {
    qDebug(__PRETTY_FUNCTION__);
//  c->out_packet_num ++;
}

static void conn_free (struct connection *c) {
    qDebug(__PRETTY_FUNCTION__);
//  if (c->ip) { tfree_str (c->ip); }
//  if (c->fail_ev) { event_free (c->fail_ev); }
//  if (c->read_ev) { event_free (c->read_ev); }
//  if (c->write_ev) { event_free (c->write_ev); }

//  struct connection_buffer *b = c->out_head;
//  while (b) {
//    struct connection_buffer *d = b;
//    b = b->next;
//    delete_connection_buffer (d);
//  }
//  b = c->in_head;
//  while (b) {
//    struct connection_buffer *d = b;
//    b = b->next;
//    delete_connection_buffer (d);
//  }

//  if (c->fd >= 0) { Connections[c->fd] = 0; close (c->fd); }
//  tfree (c, sizeof (*c));
}

static struct tgl_dc *get_dc (struct connection *c) {
        qDebug(__PRETTY_FUNCTION__);
  return c->dc;
}

static struct tgl_session *get_session (struct connection *c) {
        qDebug(__PRETTY_FUNCTION__);
  return c->session;
}

void flush_out (struct connection *c) {
    qDebug(__PRETTY_FUNCTION__);
}

struct connection *create_connection (struct tgl_state *TLS, const char *host, int port, struct tgl_session *session, struct tgl_dc *dc, struct mtproto_methods *methods) {
        qDebug(__PRETTY_FUNCTION__);
//  struct connection *c = talloc0 (sizeof (*c));
//  c->TLS = TLS;
//  c->ip = tstrdup (host);
//  c->port = port;
//
//  int fd = my_connect (c, c->ip);
//  if (fd < 0) {
//    vlogprintf (E_ERROR, "Can not connect to %s:%d %m\n", host, port);
//    tfree (c, sizeof (*c));
//    return 0;
//  }
//
//  c->fd = fd;
//  c->state = conn_connecting;
//  c->last_receive_time = tglt_get_double_time ();
//  c->flags = 0;
//  assert (!Connections[fd]);
//  Connections[fd] = c;
//
//  c->ping_ev = evtimer_new (TLS->ev_base, ping_alarm, c);
//  c->fail_ev = evtimer_new (TLS->ev_base, fail_alarm, c);
//  c->write_ev = event_new (TLS->ev_base, c->fd, EV_WRITE, conn_try_write, c);
//
//  struct timeval tv = {5, 0};
//  c->read_ev = event_new (TLS->ev_base, c->fd, EV_READ | EV_PERSIST, conn_try_read, c);
//  event_add (c->read_ev, &tv);
//
//  start_ping_timer (c);
//
//  c->dc = dc;
//  c->session = session;
//  c->methods = methods;
//
//  char byte = 0xef;
//  assert (tgln_write_out (c, &byte, 1) == 1);
//  tgln_flush_out (c);
//
//  return c;
        return NULL;
}

tgl_net_methods conn_methods = {
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
