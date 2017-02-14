/*
 * contact.cpp
 *
 *  Created on: ۲۳ بهمن ۱۳۹۵ ه‍.ش.
 *      Author: Hedayat Vatankhah <hedayat.fwd@gmail.com>.
 */

#include <cassert>
#include <src/contact.h>
#include <tgl.h>

contact::contact(tgl_user *user): user(user), name_format(FIRST_LAST)
{
    assert(user);
}

contact::~contact()
{
}

QString contact::name() const
{
    QString name;
//    if (!U)
//    {
//        name = QString("user#%1").arg(tgl_get_peer_id(id));
//        // todo add tgl_get_peer_id(id) to unknown user list (to retrieve info)
//        // using tgl_do_get_user_info (TLS, TGL_MK_USER (unknown_user_list[i]), 0, 0, 0);
//    }
    if ((user->flags & TGLUF_DELETED))
        name = QString("user#%1 [deleted]").arg(tgl_get_peer_id(user->id));
    else if (!(user->flags & TGLUF_CREATED))
        name = QString("user#%1").arg(tgl_get_peer_id(user->id));
    else
    {
        if (!user->first_name || !strlen(user->first_name))
            name = QString::fromUtf8(user->last_name);
        else if (!user->last_name || !strlen(user->last_name))
            name = QString::fromUtf8(user->first_name);
        else
        {
            QString fn = QString::fromUtf8(user->first_name);
            QString ln = QString::fromUtf8(user->last_name);
            switch (name_format)
            {
                case FIRST_LAST:
                    name = fn + ' ' + ln;
                    break;
                case LAST_FIRST:
                    name = ln + ' ' + fn;
                    break;
            }
        }
    }
//        if (!(U->flags & TGLUF_CONTACT))
//            name = "[non-contact] " + name;
    return name;
}

QString contact::contact_group() const
{
    return name()[0].toUpper();
}

bool contact::operator <(const contact &r) const
{
    return name().compare(r.name(), Qt::CaseInsensitive) < 0;
}
