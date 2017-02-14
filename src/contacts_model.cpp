/*
 * contacts_model.cpp
 *
 *  Created on: ۲۳ بهمن ۱۳۹۵ ه‍.ش.
 *      Author: Hedayat Vatankhah <hedayat.fwd@gmail.com>.
 */

#include <algorithm>
#include <QDebug>
#include <src/contacts_model.h>
#include <src/qpeerid.h>

contacts_model::contacts_model(QObject *parent): QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
//    roles[role::FirstName] = "first_name";
//    roles[role::LastName] = "last_name";
    roles[role_t::Name] = "name";
    roles[role_t::PhoneNumber] = "phone_number";
    roles[role_t::ContactGroup] = "contact_group";
    roles[role_t::PeerID] = "peer_id";
    roles[role_t::ProfilePicture] = "profile_pic";
    setRoleNames(roles);
}

contacts_model::~contacts_model()
{
}

void contacts_model::add_contact(const contact &c)
{
    qDebug(__PRETTY_FUNCTION__);
    QList<contact>::iterator pos = std::upper_bound(m_contacts.begin(),
        m_contacts.end(), c);
    int idx = pos - m_contacts.begin();
    qDebug() << "Insert new contact at pos: " << idx;
    beginInsertRows(QModelIndex(), idx, idx);
    m_contacts.insert(pos, c);
    endInsertRows();
}

Qt::ItemFlags contacts_model::flags(const QModelIndex &index) const
{
    qDebug(__PRETTY_FUNCTION__);
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

int contacts_model::rowCount(const QModelIndex &/*parent*/) const
{
    qDebug(__PRETTY_FUNCTION__);
    return m_contacts.count();
}

QVariant contacts_model::data(const QModelIndex &index, int role) const
{
    qDebug(__PRETTY_FUNCTION__);
    if (index.row() < 0 || index.row() > m_contacts.count())
        return QVariant();

    const contact &c = m_contacts[index.row()];
    switch (role)
    {
        case role_t::Name:
            return c.name();
            break;
        case role_t::PhoneNumber:
            return c.phone_number();
            break;
        case role_t::ContactGroup:
            return c.contact_group();
            break;
        case role_t::PeerID:
            return QPeerId::mk_var(c.peer_id());
            break;
    }
    return QVariant();
}

bool contacts_model::setData(const QModelIndex &index, const QVariant &value,
    int role)
{
    qDebug(__PRETTY_FUNCTION__);
    // todo
    emit dataChanged(index, index);
    return false;
}

QHash<int, QByteArray> contacts_model::roleNames() const
{
    qDebug(__PRETTY_FUNCTION__);
    QHash<int, QByteArray> roles;
    roles[role_t::Name] = "name";
    roles[role_t::PhoneNumber] = "phone_number";
    roles[role_t::ContactGroup] = "contact_group";
    roles[role_t::PeerID] = "peer_id";
    roles[role_t::ProfilePicture] = "profile_pic";
    return roles;
}

void contacts_model::print()
{
    for (int i = 0; i < m_contacts.size(); ++i)
    {
        qDebug() << "Contact: " << m_contacts[i].contact_group() << ' '
                << m_contacts[i].name() << ' ' << m_contacts[i].phone_number();
    }
}
