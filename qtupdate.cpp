/**
 * qtupdate.cpp
 *
 *  Created on: ۱۹ تیر ۱۳۹۴ ه‍.ش.
 *      Author: Hedayat Vatankhah <hedayat.fwd@gmail.com>.
 */

#include <cassert>
#include <cstdio>
#include <ctime>
#include <unistd.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>
#include <QObject>
#include <QInputDialog>
#include <tgl.h>

extern bool binlog_read;

void write_auth_file(tgl_state *tlstate);
void write_secret_chat_file();

static void print_message_gw(tgl_state *tls, tgl_message *m);
static void marked_read_upd(tgl_state *tls, int num, tgl_message *list[]);
static void logprintf(const char *format, ...);
static void do_get_string(tgl_state *tls, const char *prompt, int flags,
    void (*cb)(tgl_state *, const char *, void *), void *arg);
static void on_login(tgl_state *tls);
static void on_started(tgl_state *tls);
static void type_notification_upd(tgl_state *tls, tgl_user *U,
    enum tgl_typing_status status);
static void type_in_chat_notification_upd(tgl_state *tls, tgl_user *U,
    tgl_chat *C, enum tgl_typing_status status);
static void user_update_gw(tgl_state *tls, tgl_user *U, unsigned flags);
static void chat_update_gw(tgl_state *tls, tgl_chat *U, unsigned flags);
static void secret_chat_update_gw(tgl_state *tls, tgl_secret_chat *U,
    unsigned flags);
static void our_id_gw(tgl_state *tls, int id);
static void user_status_upd(tgl_state *tls, tgl_user *U);
static void get_values(tgl_state *tls, tgl_value_type type, int num_values,
    void (*callback)(tgl_state *tls, const char *string[], void *arg), void *arg);

tgl_update_callback upd_cb =
{
    print_message_gw, // new_msg
    marked_read_upd,
    logprintf,
    do_get_string,
    on_login,
    on_started,
    type_notification_upd,
    type_in_chat_notification_upd,
    //void (*type_in_secret_chat_notification)(tgl_state *tls, tgl_secret_chat *E);
    0,
    //void (*status_notification)(tgl_state *tls, tgl_user *U);
    0,
    //void (*user_registered)(tgl_state *tls, tgl_user *U);
    0,
    //void (*user_activated)(tgl_state *tls, tgl_user *U);
    0,
    //void (*new_authorization)(tgl_state *tls, const char *device, const char *location);
    0,
    chat_update_gw,
    user_update_gw,
    secret_chat_update_gw,
    print_message_gw, // msg_receive
    our_id_gw,
    //void (*notification)(tgl_state *tls, const char *type, const char *message);
    0,
    user_status_upd,
    //char *(*create_print_name) (tgl_state *tls, tgl_peer_id_t id, const char *a1, const char *a2, const char *a3, const char *a4);
    0,
    //  void (*get_values)(struct tgl_state *TLS, enum tgl_value_type type, int num_values, int (*callback)(struct tgl_state *TLS, const char *string[], void *arg), void *arg);
    get_values
};

static void print_chat_name(tgl_peer_id_t id, tgl_peer_t *C)
{
    if (!C)
        printf("chat#%d", tgl_get_peer_id(id));
    else
        printf("%s", C->chat.title);
}

static void print_encr_chat_name(tgl_peer_id_t id, tgl_peer_t *C)
{
    if (!C)
        printf("encr_chat#%d", tgl_get_peer_id(id));
    else
        printf("%s", C->print_name);
}

static void print_user_name(tgl_peer_id_t id, tgl_peer_t *U)
{
    if (!U)
    {
        printf("user#%d", tgl_get_peer_id(id));
        int i;
        int ok = 1;
        // todo add tgl_get_peer_id(id) to unknown user list (to retrieve info)
        // using tgl_do_get_user_info (TLS, TGL_MK_USER (unknown_user_list[i]), 0, 0, 0);
    }
    else
    {
        if (U->flags & (TGLUF_SELF | TGLUF_CONTACT))
        {
            printf("self|contact ");
        }
        if ((U->flags & TGLUF_DELETED))
        {
            printf("deleted user#%d", tgl_get_peer_id(id));
        }
        else if (!(U->flags & TGLUF_CREATED))
        {
            printf("user#%d", tgl_get_peer_id(id));
        }
//        else if (use_ids)
//        {
//            printf("user#%d", tgl_get_peer_id(id));
//        }
        else if (!U->user.first_name || !strlen(U->user.first_name))
        {
            printf("%s", U->user.last_name);
        }
        else if (!U->user.last_name || !strlen(U->user.last_name))
        {
            printf("%s", U->user.first_name);
        }
        else
        {
            printf("%s %s", U->user.first_name, U->user.last_name);
        }
    }
}

static void print_typing(tgl_typing_status status)
{
    switch (status)
    {
        case tgl_typing_none:
            printf("doing nothing");
            break;
        case tgl_typing_typing:
            printf("typing");
            break;
        case tgl_typing_cancel:
            printf("deleting typed message");
            break;
        case tgl_typing_record_video:
            printf("recording video");
            break;
        case tgl_typing_upload_video:
            printf("uploading video");
            break;
        case tgl_typing_record_audio:
            printf("recording audio");
            break;
        case tgl_typing_upload_audio:
            printf("uploading audio");
            break;
        case tgl_typing_upload_photo:
            printf("uploading photo");
            break;
        case tgl_typing_upload_document:
            printf("uploading document");
            break;
        case tgl_typing_geo:
            printf("choosing location");
            break;
        case tgl_typing_choose_contact:
            printf("choosing contact");
            break;
    }
}

static void print_service_message(tgl_state *tls, tgl_message *M)
{
    assert(M);
    // ?! can be shown M->id;
    // time_t M->date;
    if (tgl_get_peer_type(M->to_id) == TGL_PEER_CHAT)
    {
        print_chat_name(M->to_id, tgl_peer_get(tls, M->to_id));
    }
    else
    {
        assert(tgl_get_peer_type (M->to_id) == TGL_PEER_ENCR_CHAT);
        print_encr_chat_name(M->to_id, tgl_peer_get(tls, M->to_id));
    }
    print_user_name(M->from_id, tgl_peer_get(tls, M->from_id));

    switch (M->action.type)
    {
        case tgl_message_action_none:
            printf("\n");
            break;
        case tgl_message_action_geo_chat_create:
            printf("Created geo chat\n");
            break;
        case tgl_message_action_geo_chat_checkin:
            printf("Checkin in geochat\n");
            break;
        case tgl_message_action_chat_create:
            printf(" created chat %s. %d users\n", M->action.title,
                M->action.user_num);
            break;
        case tgl_message_action_chat_edit_title:
            printf(" changed title to %s\n", M->action.new_title);
            break;
        case tgl_message_action_chat_edit_photo:
            printf(" changed photo\n");
            break;
        case tgl_message_action_chat_delete_photo:
            printf(" deleted photo\n");
            break;
        case tgl_message_action_chat_add_user:
            printf(" added user ");
            print_user_name(tgl_set_peer_id(TGL_PEER_USER, M->action.user),
                tgl_peer_get(tls,
                    tgl_set_peer_id(TGL_PEER_USER, M->action.user)));
            printf("\n");
            break;
        case tgl_message_action_chat_add_user_by_link:
            printf(" added by link from ");
            print_user_name(tgl_set_peer_id(TGL_PEER_USER, M->action.user),
                tgl_peer_get(tls,
                    tgl_set_peer_id(TGL_PEER_USER, M->action.user)));
            printf("\n");
            break;
        case tgl_message_action_chat_delete_user:
            printf(" deleted user ");
            print_user_name(tgl_set_peer_id(TGL_PEER_USER, M->action.user),
                tgl_peer_get(tls,
                    tgl_set_peer_id(TGL_PEER_USER, M->action.user)));
            printf("\n");
            break;
        case tgl_message_action_set_message_ttl:
            printf(" set ttl to %d seconds. Unsupported yet\n",
                M->action.ttl);
            break;
        case tgl_message_action_read_messages:
            printf(" %d messages marked read\n", M->action.read_cnt);
            break;
        case tgl_message_action_delete_messages:
            printf(" %d messages deleted\n", M->action.delete_cnt);
            break;
        case tgl_message_action_screenshot_messages:
            printf(" %d messages screenshoted\n",
                M->action.screenshot_cnt);
            break;
        case tgl_message_action_flush_history:
            printf(" cleared history\n");
            break;
        case tgl_message_action_resend:
            printf(" resend query\n");
            break;
        case tgl_message_action_notify_layer:
            printf(" updated layer to %d\n", M->action.layer);
            break;
        case tgl_message_action_typing:
            printf(" is ");
            print_typing(M->action.typing);
            break;
        case tgl_message_action_noop:
            printf(" noop\n");
            break;
        case tgl_message_action_request_key:
            printf(" request rekey #%016llx\n", M->action.exchange_id);
            break;
        case tgl_message_action_accept_key:
            printf(" accept rekey #%016llx\n", M->action.exchange_id);
            break;
        case tgl_message_action_commit_key:
            printf(" commit rekey #%016llx\n", M->action.exchange_id);
            break;
        case tgl_message_action_abort_key:
            printf(" abort rekey #%016llx\n", M->action.exchange_id);
            break;
    }
}

static void print_media(tgl_message_media *M)
{
    assert(M);
    switch (M->type)
    {
        case tgl_message_media_none:
            return;
        case tgl_message_media_photo:
            if (!M->photo)
            {
                printf("[photo bad]");
            }
            else if (M->photo->caption && strlen(M->photo->caption))
            {
                printf("[photo %s]", M->photo->caption);
            }
            else
            {
                printf("[photo]");
            }
            if (M->caption)
            {
                printf(" %s", M->caption);
            }
            return;
        case tgl_message_media_document:
            printf("[");
            assert(M->document);
            if (M->document->flags & TGLDF_IMAGE)
            {
                printf("image");
            }
            else if (M->document->flags & TGLDF_AUDIO)
            {
                printf("audio");
            }
            else if (M->document->flags & TGLDF_VIDEO)
            {
                printf("video");
            }
            else if (M->document->flags & TGLDF_STICKER)
            {
                printf("sticker");
            }
            else
            {
                printf("document");
            }

            if (M->document->caption && strlen(M->document->caption))
            {
                printf(" %s:", M->document->caption);
            }
            else
            {
                printf(":");
            }

            if (M->document->mime_type)
            {
                printf(" type=%s", M->document->mime_type);
            }

            if (M->document->w && M->document->h)
            {
                printf(" size=%dx%d", M->document->w, M->document->h);
            }

            if (M->document->duration)
            {
                printf(" duration=%d", M->document->duration);
            }

            printf(" size=");
            if (M->document->size < (1 << 10))
            {
                printf("%dB", M->document->size);
            }
            else if (M->document->size < (1 << 20))
            {
                printf("%dKiB", M->document->size >> 10);
            }
            else if (M->document->size < (1 << 30))
            {
                printf("%dMiB", M->document->size >> 20);
            }
            else
            {
                printf("%dGiB", M->document->size >> 30);
            }

            printf("]");

            if (M->caption)
            {
                printf(" %s", M->caption);
            }

            return;
        case tgl_message_media_document_encr:
            printf("[");
            if (M->encr_document->flags & TGLDF_IMAGE)
            {
                printf("image");
            }
            else if (M->encr_document->flags & TGLDF_AUDIO)
            {
                printf("audio");
            }
            else if (M->encr_document->flags & TGLDF_VIDEO)
            {
                printf("video");
            }
            else if (M->encr_document->flags & TGLDF_STICKER)
            {
                printf("sticker");
            }
            else
            {
                printf("document");
            }

            if (M->encr_document->caption && strlen(M->encr_document->caption))
            {
                printf(" %s:", M->encr_document->caption);
            }
            else
            {
                printf(":");
            }

            if (M->encr_document->mime_type)
            {
                printf(" type=%s", M->encr_document->mime_type);
            }

            if (M->encr_document->w && M->encr_document->h)
            {
                printf(" size=%dx%d", M->encr_document->w,
                    M->encr_document->h);
            }

            if (M->encr_document->duration)
            {
                printf(" duration=%d", M->encr_document->duration);
            }

            printf(" size=");
            if (M->encr_document->size < (1 << 10))
            {
                printf("%dB", M->encr_document->size);
            }
            else if (M->encr_document->size < (1 << 20))
            {
                printf("%dKiB", M->encr_document->size >> 10);
            }
            else if (M->encr_document->size < (1 << 30))
            {
                printf("%dMiB", M->encr_document->size >> 20);
            }
            else
            {
                printf("%dGiB", M->encr_document->size >> 30);
            }

            printf("]");

            return;
        case tgl_message_media_geo:
            printf("[geo https://maps.google.com/?q=%.6lf,%.6lf]",
                M->geo.latitude, M->geo.longitude);
            return;
        case tgl_message_media_contact:
            printf("[contact] ");
            printf("%s %s ", M->first_name, M->last_name);
            printf("%s", M->phone);
            return;
        case tgl_message_media_unsupported:
            printf("[unsupported]");
            return;
        case tgl_message_media_webpage:
            printf("[webpage:");
            assert(M->webpage);
            if (M->webpage->url)
            {
                printf(" url:'%s'", M->webpage->url);
            }
            if (M->webpage->title)
            {
                printf(" title:'%s'", M->webpage->title);
            }
            if (M->webpage->description)
            {
                printf(" description:'%s'", M->webpage->description);
            }
            if (M->webpage->author)
            {
                printf(" author:'%s'", M->webpage->author);
            }
            printf("]");
            break;
        case tgl_message_media_venue:
            printf("[geo https://maps.google.com/?q=%.6lf,%.6lf",
                M->venue.geo.latitude, M->venue.geo.longitude);

            if (M->venue.title)
            {
                printf(" title:'%s'", M->venue.title);
            }

            if (M->venue.address)
            {
                printf(" address:'%s'", M->venue.address);
            }
            if (M->venue.provider)
            {
                printf(" provider:'%s'", M->venue.provider);
            }
            if (M->venue.venue_id)
            {
                printf(" id:'%s'", M->venue.venue_id);
            }

            printf("]");
            return;

        default:
            printf("x = %d\n", M->type);
            assert(0);
    }
}

static void print_message(tgl_state *tls, tgl_message *M)
{
    assert(M);
    qDebug(__PRETTY_FUNCTION__);
    if (M->flags & (TGLMF_EMPTY | TGLMF_DELETED))
    {
        return;
    }
    if (!(M->flags & TGLMF_CREATED)) { return; }
    if (M->flags & TGLMF_SERVICE)
    {
        print_service_message(tls, M);
        return;
    }
    if (!tgl_get_peer_type(M->to_id))
    {
        logprintf("Bad msg\n");
        return;
    }

//                printf("%lld ", M->id);
//            print_date(ev, M->date);
    if (tgl_get_peer_type(M->to_id) == TGL_PEER_USER)
    {
        if (M->flags & TGLMF_OUT)
            print_user_name(M->to_id, tgl_peer_get(tls, M->to_id));
        else
            print_user_name(M->from_id, tgl_peer_get(tls, M->from_id));
    }
    else if (tgl_get_peer_type(M->to_id) == TGL_PEER_ENCR_CHAT)
    {
        tgl_peer_t *P = tgl_peer_get(tls, M->to_id);
        assert(P);
        printf(" %s", P->print_name);
    }
    else
    {
        assert(tgl_get_peer_type (M->to_id) == TGL_PEER_CHAT);
        print_chat_name(M->to_id, tgl_peer_get(tls, M->to_id));
        printf(" ");
        print_user_name(M->from_id, tgl_peer_get(tls, M->from_id));
        if ((tgl_get_peer_type(M->from_id) == TGL_PEER_USER)
                && (tgl_get_peer_id(M->from_id) == tls->our_id))
            printf("MINE?");
    }
    if (M->flags & TGLMF_OUT)
    {
        if (M->flags & TGLMF_UNREAD)
            printf(" <<< ");
        else
            printf(" ««« ");
    }
    else
    {
        if (M->flags & TGLMF_UNREAD)
            printf(" >>> ");
        else
            printf(" »»» ");
    }

    if (tgl_get_peer_type(M->fwd_from_id) == TGL_PEER_USER)
    {
        printf("[fwd from ");
        print_user_name(M->fwd_from_id, tgl_peer_get(tls, M->fwd_from_id));
        printf("] ");
    }
    if (M->reply_id)
    {
        printf("[reply to %d] ", M->reply_id);
    }
    if (M->flags & TGLMF_MENTION)
    {
        printf("[mention] ");
    }
    if (M->message && strlen(M->message))
    {
        printf("%s", M->message);
    }
    if (M->media.type != tgl_message_media_none)
    {
        if (M->message && strlen(M->message))
        {
            printf(" ");
        }
        print_media(&M->media);
    }
    printf("\n");
}

static void print_message_gw(tgl_state *tls, tgl_message *m)
{
    qDebug(__PRETTY_FUNCTION__);
    if (!binlog_read)
    {
        qDebug("\t!binlog_read, returning");
        return;
    }
    if (tgl_get_peer_type(m->to_id) == TGL_PEER_ENCR_CHAT)
    {
        write_secret_chat_file();
    }
    print_message(tls, m);
}

static void print_read_list(tgl_state *TLS, int num, tgl_message *list[])
{
    int i;
    for (i = 0; i < num; i++)
        if (list[i])
        {
            tgl_peer_id_t to_id;
            if (tgl_get_peer_type(list[i]->to_id) == TGL_PEER_USER
                    && tgl_get_peer_id(list[i]->to_id) == TLS->our_id)
            {
                to_id = list[i]->from_id;
            }
            else
            {
                to_id = list[i]->to_id;
            }
            int j;
            int c1 = 0;
            int c2 = 0;
            for (j = i; j < num; j++)
                if (list[j])
                {
                    tgl_peer_id_t end_id;
                    if (tgl_get_peer_type(list[j]->to_id) == TGL_PEER_USER
                            && tgl_get_peer_id(list[j]->to_id) == TLS->our_id)
                    {
                        end_id = list[j]->from_id;
                    }
                    else
                    {
                        end_id = list[j]->to_id;
                    }
                    if (!tgl_cmp_peer_id(to_id, end_id))
                    {
                        if (list[j]->flags & TGLMF_OUT)
                        {
                            c1++;
                        }
                        else
                        {
                            c2++;
                        }
                        list[j] = 0;
                    }
                }

            assert(c1 + c2 > 0);
            switch (tgl_get_peer_type(to_id))
            {
                case TGL_PEER_USER:
                    printf("User ");
                    print_user_name(to_id, tgl_peer_get(TLS, to_id));
                    break;
                case TGL_PEER_CHAT:
                    printf("Chat ");
                    print_chat_name(to_id, tgl_peer_get(TLS, to_id));
                    break;
                case TGL_PEER_ENCR_CHAT:
                    printf("Secret chat ");
                    print_encr_chat_name(to_id, tgl_peer_get(TLS, to_id));
                    break;
                default:
                    assert(0);
            }
            printf(" marked read %d outbox and %d inbox messages\n", c1, c2);
        }
}

static void marked_read_upd(tgl_state *tls, int num, tgl_message *list[])
{
    qDebug(__PRETTY_FUNCTION__);
    if (!binlog_read)
    {
        qDebug("\t!binlog_read, returning");
        return;
    }
//  if (log_level < 1) { return; }
//
    print_read_list(tls, num, list);
}

static void logprintf(const char *format, ...)
{
    int x = 0;
    printf(" *** ");
    va_list ap;
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);
}

static void do_get_string(tgl_state *tls, const char *prompt, int flags,
    void (*cb)(tgl_state *, const char *, void *), void *arg)
{
    qDebug(__PRETTY_FUNCTION__);
  // flags & 1: don't echo user input!
    QString input = QInputDialog::getText(NULL, "Input!", prompt,
        flags & 1 ? QLineEdit::Password : QLineEdit::Normal);
    QByteArray raw = input.toUtf8();
    cb(tls, raw.constData(), arg);
}

static void on_login(tgl_state *tls)
{
    qDebug(__PRETTY_FUNCTION__);
    write_auth_file(tls);
}

static void on_started(tgl_state *tls)
{
    qDebug(__PRETTY_FUNCTION__);
//  if (wait_dialog_list) { // boolean, user option
//    wait_dialog_list = 0;
//    tgl_do_get_dialog_list (tls, 100, 0, dlist_cb, 0);
//    return;
//  }
}

static void type_notification_upd(tgl_state *tls, tgl_user *U,
    enum tgl_typing_status status)
{
    qDebug(__PRETTY_FUNCTION__);
    printf("User ");
    print_user_name(U->id, (tgl_peer_t *)U);
    printf(" is ");
    print_typing(status);
    printf("\n");
}

static void type_in_chat_notification_upd(tgl_state *tls, tgl_user *U,
    tgl_chat *C, enum tgl_typing_status status)
{
    qDebug(__PRETTY_FUNCTION__);
    printf("User ");
    print_user_name(U->id, (tgl_peer_t *) U);
    printf(" is ");
    print_typing(status);
    printf(" in chat ");
    print_chat_name(C->id, (tgl_peer_t *) C);
    printf("\n");
}

static void print_peer_updates(int flags)
{
    if (flags & TGL_UPDATE_PHONE)
    {
        printf(" phone");
    }
    if (flags & TGL_UPDATE_CONTACT)
    {
        printf(" contact");
    }
    if (flags & TGL_UPDATE_PHOTO)
    {
        printf(" photo");
    }
    if (flags & TGL_UPDATE_BLOCKED)
    {
        printf(" blocked");
    }
    if (flags & TGL_UPDATE_REAL_NAME)
    {
        printf(" name");
    }
    if (flags & TGL_UPDATE_NAME)
    {
        printf(" contact_name");
    }
    if (flags & TGL_UPDATE_REQUESTED)
    {
        printf(" status");
    }
    if (flags & TGL_UPDATE_WORKING)
    {
        printf(" status");
    }
    if (flags & TGL_UPDATE_FLAGS)
    {
        printf(" flags");
    }
    if (flags & TGL_UPDATE_TITLE)
    {
        printf(" title");
    }
    if (flags & TGL_UPDATE_ADMIN)
    {
        printf(" admin");
    }
    if (flags & TGL_UPDATE_MEMBERS)
    {
        printf(" members");
    }
    if (flags & TGL_UPDATE_ACCESS_HASH)
    {
        printf(" access_hash");
    }
    if (flags & TGL_UPDATE_USERNAME)
    {
        printf(" username");
    }
}

static void user_update_gw(tgl_state *tls, tgl_user *U, unsigned flags)
{
    qDebug(__PRETTY_FUNCTION__);

    if (!binlog_read)
    {
        qDebug("binlog not read!");
        return;
    }
    if (!(flags & TGL_UPDATE_CREATED))
    {
        printf("User ");
        print_user_name(U->id, (tgl_peer_t *) U);
        if (!(flags & TGL_UPDATE_DELETED))
        {
            printf(" updated");
            print_peer_updates(flags);
        }
        else
        {
            printf(" deleted");
        }
        printf("\n");
    }
}

static void chat_update_gw(tgl_state *tls, tgl_chat *U, unsigned flags)
{
    qDebug(__PRETTY_FUNCTION__);

    if (!binlog_read)
    {
        qDebug("binlog not read!");
        return;
    }

    if (!(flags & TGL_UPDATE_CREATED))
    {
        printf("Chat ");
        print_chat_name(U->id, (tgl_peer_t *) U);
        if (!(flags & TGL_UPDATE_DELETED))
        {
            printf(" updated");
            print_peer_updates(flags);
        }
        else
        {
            printf(" deleted");
        }
        printf("\n");
    }
}

static void secret_chat_update_gw(tgl_state *tls, tgl_secret_chat *U,
    unsigned flags)
{
    qDebug(__PRETTY_FUNCTION__);

    if ((flags & TGL_UPDATE_WORKING) || (flags & TGL_UPDATE_DELETED))
    {
        write_secret_chat_file();
    }

    if (!binlog_read)
    {
        return;
    }

//    if ((flags & TGL_UPDATE_REQUESTED) && !disable_auto_accept)
//    {
//        tgl_do_accept_encr_chat_request(tls, U, print_encr_chat_success_gw, 0);
//    }
//
    if (!(flags & TGL_UPDATE_CREATED))
    {
        printf("Secret chat ");
        print_encr_chat_name(U->id, (tgl_peer_t *) U);
        if (!(flags & TGL_UPDATE_DELETED))
        {
            printf(" updated");
            print_peer_updates(flags);
        }
        else
        {
            printf(" deleted");
        }
        printf("\n");
    }
}

static void our_id_gw(tgl_state *tls, int id)
{
    qDebug(__PRETTY_FUNCTION__);
}

static void print_date_full(long t)
{
    struct tm *tm = localtime((time_t *) &t);
    printf("[%04d/%02d/%02d %02d:%02d:%02d]", tm->tm_year + 1900,
        tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
}

static void print_user_status(tgl_user_status *S)
{
    if (S->online > 0)
    {
        printf("online (was online ");
        print_date_full(S->when);
        printf(")");
    }
    else
    {
        if (S->online == 0)
        {
            printf("offline");
        }
        else if (S->online == -1)
        {
            printf("offline (was online ");
            print_date_full(S->when);
            printf(")");
        }
        else if (S->online == -2)
        {
            printf("offline (was online recently)");
        }
        else if (S->online == -3)
        {
            printf("offline (was online last week)");
        }
        else if (S->online == -4)
        {
            printf("offline (was online last month)");
        }
    }
}

static void user_status_upd(tgl_state *tls, tgl_user *U)
{
    qDebug(__PRETTY_FUNCTION__);
    if (!binlog_read)
    {
        return;
    }
    printf("User ");
    print_user_name(U->id, (tgl_peer_t *) U);
    printf(" ");
    print_user_status(&U->status);
    printf("\n");
}

static void get_values(tgl_state *tls, tgl_value_type type, int num_values,
    void (*callback)(tgl_state *tls, const char *string[], void *arg), void *arg)
{
    const char *vs[num_values];

    switch (type)
    {
        case tgl_phone_number:
        {
            QString input = QInputDialog::getText(NULL, "yesss!",
                "Give phone number");
            QByteArray raw = input.toUtf8();
            vs[0] = raw.constData();
            break;
        }
        default:
            qDebug("Unknown data requested!");
            return;
    }
    callback(tls, vs, arg);
}
