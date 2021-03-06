/*
 * contacts_model.h
 *
 *  Created on: ۲۳ بهمن ۱۳۹۵ ه‍.ش.
 *      Author: Hedayat Vatankhah <hedayat.fwd@gmail.com>.
 */

#ifndef SRC_CONTACTS_MODEL_H_
#define SRC_CONTACTS_MODEL_H_

#include <map>
#include <QAbstractListModel>
#include <QList>
#include <QVariant>
#include <QVariantMap>
#include "contact.h"

/**
 *
 */
class contacts_model: public QAbstractListModel
{
        Q_OBJECT
        Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

    public:
        enum role_t {
            Name = Qt::UserRole + 1,
//            FirstName = Qt::UserRole + 1,
//            LastName,
            PhoneNumber,
            ContactGroup,
            PeerID,
            ProfilePicture
        };

    public:
        contacts_model(QObject *parent = 0);
        virtual ~contacts_model();

        void add_contact(const contact &c);

        Qt::ItemFlags flags(const QModelIndex &index) const;
        int rowCount(const QModelIndex &parent = QModelIndex()) const;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        bool setData(const QModelIndex &index, const QVariant &value, int role =
                Qt::EditRole);

        Q_INVOKABLE QVariant getProperty(int idx, QString prop);

        void print();

    signals:
        void countChanged();

    private:
        QList<contact> m_contacts;
        std::map<QByteArray, role_t> prop_map;
};

#endif /* SRC_CONTACTS_MODEL_H_ */
