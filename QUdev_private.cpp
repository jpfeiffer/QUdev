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

#include "QUdev_private.h"
#include "QUdev.h"

QUdevPrivate::QUdevPrivate(QUdev *parent)
  : m_pUdev(0),
    q_ptr(parent),
    m_bMonitoringActive(false)
{

    qRegisterMetaType<QUdevEvent>("QUdevEvent");

    //fill the action map
    m_mUdevActions[QString("add")] = eDeviceAdd;
    m_mUdevActions[QString("remove")] = eDeviceRemove;
    m_mUdevActions[QString("change")] = eDeviceChange;
    m_mUdevActions[QString("online")] = eDeviceOnline;
    m_mUdevActions[QString("offline")] = eDeviceOffline;

    //create the udev object
    m_pUdev = udev_new();
    //set up a udev monitor object
    m_pMon = udev_monitor_new_from_netlink(m_pUdev, "udev");

    Q_ASSERT(m_pUdev);
    Q_ASSERT(m_pMon);

    udev_monitor_enable_receiving(m_pMon);
}

QUdevPrivate::~QUdevPrivate()
{
    {
        QMutexLocker l(&m_Mutex);
        //stop the monitoring thread and wait for it
        m_bMonitoringActive = false;
        Q_UNUSED(l);
    }
    wait();

    //release the udev objects
    if(m_pUdev) udev_unref(m_pUdev);
    if(m_pMon) udev_monitor_unref(m_pMon);
}

QUdevPrivate &QUdevPrivate::operator=(const QUdevPrivate& Other)
{
    if(this != &Other)
    {
        if(m_pUdev) udev_unref(m_pUdev);
        m_pUdev = Other.m_pUdev;
        m_lMonitorEntries = Other.m_lMonitorEntries;
    }
    return *this;
}

QUdevDeviceList QUdevPrivate::getUDevDevicesForSubsystem(const QString &strSubSystem, const QString &strDeviceType, const QString &strParentSubSystem, const QString &strParentDeviceType)
{
    struct udev_enumerate *enumerate = udev_enumerate_new(m_pUdev);
    struct udev_list_entry *devices = 0;
    struct udev_list_entry *dev_list_entry = 0;
    struct udev_device *dev = 0;

    QList<QUdevDevice> lDevices;

    if(strSubSystem.isEmpty()) return lDevices;

    //get subsystem enumerator
    udev_enumerate_add_match_subsystem(enumerate, strSubSystem.toUtf8().constData());
    //perform sysfs scanning
    udev_enumerate_scan_devices(enumerate);
    devices = udev_enumerate_get_list_entry(enumerate);

    //iterate over all devices in the enumeration list
    udev_list_entry_foreach(dev_list_entry, devices)
    {
        QUdevDevice udDev;

        //create udev device for the sysfs path returned
        udDev.m_strSysfsPath = QString::fromLatin1(udev_list_entry_get_name(dev_list_entry));
        dev = udev_device_new_from_syspath(m_pUdev, udDev.m_strSysfsPath.toLatin1().constData());

        //filter the correct device types, ignored if empty device type is specified
        if(strDeviceType.isEmpty() || (QString::fromLatin1(udev_device_get_devtype(dev)) == strDeviceType))
        {
            //get the path inside /dev
            udDev.m_strDevPath = QString::fromLatin1(udev_device_get_devnode(dev));
            udDev.m_strSubsystem = strSubSystem;
            udDev.m_strDeviceType = strDeviceType;

            //if the caller wants a specific parent subsystem/devtype query the sysfs tree here
            if(!strParentSubSystem.isEmpty() && !strParentDeviceType.isEmpty())
            {
                /*
                 * retrieve the parent device with the subsystem/devtype pair of m_strParentSubSystem/m_strParentDeviceType.
                 *
                 * udev_device_get_parent_with_subsystem_devtype() will walk up the complete tree if needed
                 */
                dev = udev_device_get_parent_with_subsystem_devtype(dev, strParentSubSystem.toLatin1().constData(), strParentDeviceType.toLatin1().constData());
            }

            if(dev)
            {
                //fill the device information
                udDev.m_strVendorID = QString::fromLatin1(udev_device_get_sysattr_value(dev,"idVendor"));
                udDev.m_strProductID = QString::fromLatin1(udev_device_get_sysattr_value(dev, "idProduct"));
                udDev.m_strManufacturer = QString::fromLatin1(udev_device_get_sysattr_value(dev,"manufacturer"));
                udDev.m_strProduct = QString::fromLatin1(udev_device_get_sysattr_value(dev,"product"));
                udDev.m_strSerial = QString::fromLatin1(udev_device_get_sysattr_value(dev, "serial"));
                lDevices.append(udDev);
            }

            udev_device_unref(dev);
        }
    }
    //drop our reference to the enumeration interface
    udev_enumerate_unref(enumerate);

    return lDevices;
}

bool QUdevPrivate::addNewMonitorRule(const QString &strSubSystem, const QString &strDeviceType, const QString &strParentSubSystem, const QString &strParentDeviceType)
{
    QUdevInternalWatcherEntry iwe(strSubSystem, strDeviceType, strParentSubSystem, strParentDeviceType);
    QMutexLocker l(&m_Mutex);

    //filter duplicated rules
    if(m_lMonitorEntries.contains(iwe)) return false;

    m_lMonitorEntries.append(iwe);

    //clear all filter from the monitor interface
    udev_monitor_filter_remove(m_pMon);

    if(false == m_lMonitorEntries.empty())
    {
        foreach(QUdevInternalWatcherEntry iwe, m_lMonitorEntries)
        {
            udev_monitor_filter_add_match_subsystem_devtype(m_pMon, iwe.m_strSubsystem.toLatin1().constData(), iwe.m_strDeviceType.toLatin1().constData());
        }

        m_bMonitoringActive = true;
        if(false==this->isRunning()) this->start();
    }

    Q_UNUSED(l);
    return true;
}

bool QUdevPrivate::removeMonitorRule(const QString &strSubSystem, const QString &strDeviceType, const QString &strParentSubSystem, const QString &strParentDeviceType)
{
    QUdevInternalWatcherEntry iwe(strSubSystem, strDeviceType, strParentSubSystem, strParentDeviceType);
    QMutexLocker l(&m_Mutex);

    //rule must be present
    if(false == m_lMonitorEntries.contains(iwe)) return false;

    //remove the first instance of the given monitoring rule
    m_lMonitorEntries.removeOne(iwe);

    //clear all filter from the monitor interface
    udev_monitor_filter_remove(m_pMon);

    if(false == m_lMonitorEntries.empty())
    {
        foreach(QUdevInternalWatcherEntry iwe, m_lMonitorEntries)
        {
            udev_monitor_filter_add_match_subsystem_devtype(m_pMon, iwe.m_strSubsystem.toLatin1().constData(), iwe.m_strDeviceType.toLatin1().constData());
        }

        m_bMonitoringActive = true;
        if(false==this->isRunning()) this->start();
    }

    Q_UNUSED(l);
    return true;
}

void QUdevPrivate::run()
{
    qDebug() << QString("QUdevPrivate::run() monitoring thread started");

    //libudev can provide us with a file descriptor usable with select()
    int fd = udev_monitor_get_fd(m_pMon);

    while(m_bMonitoringActive)
    {
        fd_set fds;
        struct timeval tv;
        int ret;

        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        ret = select(fd+1, &fds, NULL, NULL, &tv);

        //data available
        if (ret > 0 && FD_ISSET(fd, &fds))
        {
            struct udev_device* parent_dev = 0;
            //should not block due to select()
            struct udev_device* dev = udev_monitor_receive_device(m_pMon);

            if (dev)
            {
                QMutexLocker l(&m_Mutex);
                foreach(QUdevInternalWatcherEntry iwe, m_lMonitorEntries)
                {
                    bool bMatch = true;
                    bMatch &= (iwe.m_strSubsystem == QString::fromLatin1(udev_device_get_subsystem(dev)));
                    bMatch &= (iwe.m_strDeviceType == QString::fromLatin1(udev_device_get_devtype(dev)));

                    //subsystem and devicetype match, check if parent matching is requested
                    if(bMatch)
                    {
                        //if the monitor entry wants a specific parent subsystem/devtype query the sysfs tree here
                        if(!iwe.m_strParentSubSystem.isEmpty() && !iwe.m_strParentDeviceType.isEmpty())
                        {
                            /*
                             * udev_device_get_parent_with_subsystem_devtype() will walk up the complete tree if needed
                             * to find any parent with the requested subsystem/devtype combination
                             *
                             * We only care if any device can be found up the tree or not
                             */
                            parent_dev = udev_device_get_parent_with_subsystem_devtype(dev, iwe.m_strParentSubSystem.toLatin1().constData(), iwe.m_strParentDeviceType.toLatin1().constData());
                            bMatch &= (0 != parent_dev);
                        }
                    }

                    if(bMatch)
                    {
                        QUdevEvent e;

                        //fill the action
                        e.m_ueAction = getQUdevEventActionFromUdevAction(QString::fromLatin1(udev_device_get_action(dev)));

                        //fill the device information
                        e.m_udDev.m_strSubsystem = iwe.m_strSubsystem;
                        e.m_udDev.m_strDeviceType = iwe.m_strDeviceType;
                        e.m_udDev.m_strSysfsPath = QString::fromLatin1(udev_device_get_syspath(dev));
                        e.m_udDev.m_strDevPath = QString::fromLatin1(udev_device_get_devnode(dev));

                        //detailed information may come from the parent (if specified)
                        struct udev_device* detail_dev = (0!=parent_dev) ? parent_dev : dev;

                        e.m_udDev.m_strVendorID = QString::fromLatin1(udev_device_get_sysattr_value(detail_dev,"idVendor"));
                        e.m_udDev.m_strProductID = QString::fromLatin1(udev_device_get_sysattr_value(detail_dev, "idProduct"));
                        e.m_udDev.m_strManufacturer = QString::fromLatin1(udev_device_get_sysattr_value(detail_dev,"manufacturer"));
                        e.m_udDev.m_strProduct = QString::fromLatin1(udev_device_get_sysattr_value(detail_dev,"product"));
                        e.m_udDev.m_strSerial = QString::fromLatin1(udev_device_get_sysattr_value(detail_dev, "serial"));

                        Q_Q(QUdev);
                        emit q->newUDevEvent(e);
                    }
                }
                Q_UNUSED(l);
                udev_device_unref(dev);
                //NOTE: parent_dev needs NOT to be unreferenced, see libudev documentation for details
            }
        }
        msleep(500);
    }
}

QUdevEventAction QUdevPrivate::getQUdevEventActionFromUdevAction(const QString &strUdevAction) const
{
    if(m_mUdevActions.contains(strUdevAction))
    {
        return m_mUdevActions[strUdevAction];
    }
    else
    {
        return eDeviceUnknownAction;
    }
}
