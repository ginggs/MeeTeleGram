/*
 * qtelegram.h
 *
 *  Created on: ۱۹ تیر ۱۳۹۴ ه‍.ش.
 *      Author: Hedayat Vatankhah <hedayat.fwd@gmail.com>.
 */

#ifndef QTELEGRAM_H_
#define QTELEGRAM_H_

#include <QObject>
#include <QString>
#include <QStringList>
#include <tgl.h>

/**
 *
 */
class qtelegram: public QObject
{
        Q_OBJECT;

    private:
        typedef void (*getvalues_callback)(tgl_state *tls, const char *string[],
            void *arg);

    public:
        qtelegram(int app_id, const char *app_hash, const char *app_ver,
            const char *conf_dir, const char *serverkey_path,
            QObject *parent = NULL);
        ~qtelegram();

        Q_INVOKABLE void login();

        Q_INVOKABLE void request_contact_list();

        Q_INVOKABLE void set_phone_number(QString number);
        Q_INVOKABLE void set_code(QString code);
        Q_INVOKABLE void set_current_pass(QString pass);
        Q_INVOKABLE void set_register_info(QString name, QString lastname);
        Q_INVOKABLE void set_new_password(QString pass);
        Q_INVOKABLE void set_cur_and_new_password(QString curp, QString newp);

    signals:
        void error(int error_code, const char *error);
        void logged_in();
        void contact_list_received(tgl_user *contacts[], int size);
        void contact_list_received(QStringList contacts);
        void phone_number_requested();
        void code_requested();
        void current_pass_requested();
        void register_info_requested();
        void new_password_requested();
        void cur_and_new_password_requested();

    private:
        static tgl_update_callback qtg_update_cb;
        const QString config_dir;

        struct tgl_state *tlstate;
        bool enable_events;
        int get_state_timer;

        getvalues_callback getvalues_cb;
        void *getvalues_arg;

        QString auth_key_filename();
        void read_auth_file();
        void write_auth_file();
        void empty_auth_file();
        void read_dc(int auth_file_fd, int id, unsigned ver);
        static void write_dc(struct tgl_dc *DC, void *extra);

        QString state_filename();
        void read_state_file();
        void write_state_file();

        void read_secret_chat_file();
        void write_secret_chat_file();
        void read_secret_chat(int fd, int v);

        void call_getvalue_callback(const char *values[]);

        void timerEvent(QTimerEvent *event);

        static void print_message_gw(tgl_state *tls, tgl_message *m);
        static void marked_read_upd(tgl_state *tls, int num, tgl_message *list[]);
        static void logprintf(const char *format, ...);
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
        static void get_values(tgl_state *tls, tgl_value_type type,
            const char *prompt, int num_values, getvalues_callback callback,
            void *arg);

        static void on_contact_list_updated(tgl_state *tls,
            void *qtptr, int success, int size, tgl_user *contacts[]);
};

#endif /* QTELEGRAM_H_ */
