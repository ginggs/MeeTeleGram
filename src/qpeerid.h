/*
 * qpeer_id.h
 *
 *  Created on: ۷ بهمن ۱۳۹۵ ه‍.ش.
 *      Author: Hedayat Vatankhah <hedayat.fwd@gmail.com>.
 */

#ifndef SRC_QPEERID_H_
#define SRC_QPEERID_H_

#include <tgl-layout.h>
#include <QObject>
#include <QMetaType>

/**
 *
 */
class QPeerId: public QObject
{
        Q_OBJECT;
        Q_PROPERTY(Type type READ type);

    public:
        enum Type {
            Unknown,
            User,
            Chat,
            Channel,
            GeoChat,
            EncryptedChat,
            TempId,
            RandomId
        };
        Q_ENUMS(Type);

    public:
        QPeerId(tgl_peer_id_t p_id = tgl_peer_id_t(), QObject *parent = NULL);
        QPeerId(const QPeerId &p): QObject() { m_id = p.m_id; }

        virtual ~QPeerId();

        Q_INVOKABLE Type type() const;
        Q_INVOKABLE int getId() const { return m_id.peer_id; }

        tgl_peer_id_t id() const { return m_id; }

    private:
        tgl_peer_id_t m_id;
};

#endif /* SRC_QPEERID_H_ */
