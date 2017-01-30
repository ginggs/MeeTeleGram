/*
 * qpeer_id.cpp
 *
 *  Created on: ۷ بهمن ۱۳۹۵ ه‍.ش.
 *      Author: Hedayat Vatankhah <hedayat.fwd@gmail.com>.
 */

#include "qpeerid.h"

#include <QDebug>
#include <tgl.h>

QPeerId::QPeerId(tgl_peer_id_t p_id, QObject *parent) :
        QObject(parent), m_id(p_id)
{
}

QPeerId::~QPeerId()
{
}

QPeerId::Type QPeerId::type() const
{
    switch (tgl_get_peer_type(m_id))
    {
        case TGL_PEER_UNKNOWN:
            return Unknown;
        case TGL_PEER_USER:
            return User;
        case TGL_PEER_CHAT:
            return Chat;
        case TGL_PEER_CHANNEL:
            return Channel;
        case TGL_PEER_GEO_CHAT:
            return GeoChat;
        case TGL_PEER_ENCR_CHAT:
            return EncryptedChat;
        case TGL_PEER_TEMP_ID:
            return TempId;
        case TGL_PEER_RANDOM_ID:
            return RandomId;
        default:
            qDebug() << "Unknwon peer type: " << tgl_get_peer_type(m_id);
            break;
    }
    return Unknown;
}

bool QPeerId::is_equal(QPeerId *id)
{
    return tgl_cmp_peer_id(this->id(), id->id()) == 0;
}
