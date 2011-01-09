/*
 * This file is part of QUdev.
 * Copyright 2011 Johannes Pfeiffer (johannes.obticeo.de)
 *
 * QUdev is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * QUdev is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QUdev. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QUDEVDECLARATIONS_H
#define QUDEVDECLARATIONS_H

#include <QtCore>
#include <QMetaType>

/**
 * All current supported udev event actions
 */
enum QUdevEventAction
{
    eDeviceAdd,
    eDeviceRemove,
    eDeviceChange,
    eDeviceOnline,
    eDeviceOffline,
    eDeviceUnknownAction,

};

/**
 * This class represents one single udev event
 */
struct QUdevDevice
{
    QString m_strSysfsPath;
    QString m_strDevPath;

    QString m_strSubsystem;
    QString m_strDeviceType;

    QString m_strVendorID;
    QString m_strProductID;
    QString m_strManufacturer;
    QString m_strProduct;
    QString m_strSerial;

};

/**
 * A single udev event monitored
 */
struct QUdevEvent
{
    /**
     * This is the udev action causing this event (add/remove/...)
     */
    QUdevEventAction m_ueAction;

    /**
     * This is the affected device
     */
    QUdevDevice m_udDev;

};
Q_DECLARE_METATYPE(QUdevEvent);

typedef QList<QUdevDevice> QUdevDeviceList;
typedef QSharedPointer<QList<QUdevDevice> > QUdevDeviceListPtr;

#endif // QUDEVDECLARATIONS_H
