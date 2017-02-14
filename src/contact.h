/*
 * contact.h
 *
 *  Created on: ۲۳ بهمن ۱۳۹۵ ه‍.ش.
 *      Author: Hedayat Vatankhah <hedayat.fwd@gmail.com>.
 */

#ifndef SRC_CONTACT_H_
#define SRC_CONTACT_H_

#include <QString>
#include <tgl-layout.h>

/**
 *
 */
class contact
{
    public:
        enum name_format_t {
            FIRST_LAST,
            LAST_FIRST
        };

    public:
        contact(tgl_user *user);
        ~contact();

        QString name() const;
        QString phone_number() const { return user->phone; }
        QString contact_group() const;
        tgl_peer_id_t peer_id() const { return user->id; }

        void set_name_format(name_format_t new_format)
        {
            name_format = new_format;
        }

        bool operator<(const contact &r) const;

    private:
        tgl_user *user;
        name_format_t name_format;
};

#endif /* SRC_CONTACT_H_ */
