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

#ifndef QUDEVIMPL_H
#define QUDEVIMPL_H

#include <QObject>
#include <libudev.h>

#include "QUdevDeclarations.h"

class QUdev;

/**
 * Internal QUdev implementation
 *
 * This class can enumerate current available devices based on given subsystem and devicetype information.
 * It can also be used to monitor for multiple subsystem/devicetype combination to be notified if any of
 * these devices produces an udev event.
 */
class QUdevPrivate : public QThread
{
    public:

        /**
         * Default constructor
         */
        explicit QUdevPrivate(QUdev *parent);

        /**
         * Default destructor
         *
         * This will unload the udev handle.
         */
        ~QUdevPrivate();

        /**
         * Assignment operator
         *
         * This will use the udev handle from Other and releasing the current handle from this instance
         * if present.
         */
        QUdevPrivate &operator=(const QUdevPrivate& Other);

        /**
         * Get all devices currently present in the system for the given parameters
         *
         * Example usage:\n
         * - to get all usb disks call with getUDevDevicesForSubsystem(QString("block"), QString("disk"), QString("usb"), QString("usb_device"))\n
         * - to get all usb partitions call with getUDevDevicesForSubsystem(QString("block"), QString("partition"), QString("usb"), QString("usb_device"))\n
         *
         * NOTE: leaving the strDeviceType empty will return both disks and partitions
         *
         * @param strSubSystem The desired subsystem for the device (for example: block, char, scsi, ...)
         * @param strDeviceType The desired devicetype (for example: disk,partition, ...).
         *        With an empty string this parameter is ignored
         * @param strParentSubSystem Any of the parent for the resulting devices must have a parent from this subsystem (useful to restrict returned devices to usb, ieee1394, ...)
         *        With an empty string this parameter is ignored
         * @param strParentDeviceType The device type for the parent
         *        With an empty string this parameter is ignored
         *
         * @return The list with all devices matching the given constraints
         */
        QUdevDeviceList getUDevDevicesForSubsystem(const QString &strSubSystem, const QString &strDeviceType, const QString &strParentSubSystem, const QString &strParentDeviceType);

        /**
         * Add a new monitor rule to the list of monitored udev devices.
         *
         * Every received udev event is checked if the source device is matching the specified parameters.
         *
         * @param strSubSystem The desired subsystem for the new monitor rule
         * @param strDeviceType The desired devicetype
         *        With an empty string this parameter is ignored
         * @param strParentSubSystem Any of the parent for the  the new monitor rule mut have this subsystem
         *        With an empty string this parameter is ignored
         * @param strParentDeviceType The device type for the parent
         *        With an empty string this parameter is ignored
         *
         * @return True if the rule could be added to the monitoring framework. False if the parameters are invalid or such a rule is already present
         */
        bool addNewMonitorRule(const QString &strSubSystem, const QString &strDeviceType, const QString &strParentSubSystem, const QString &strParentDeviceType);

        /**
         * Remove an existing monitor rule from the list of monitored udev devices.
         *
         * @param strSubSystem The desired subsystem for the monitor rule to be removed
         * @param strDeviceType The desired devicetype
         * @param strParentSubSystem The parent subsystem
         * @param strParentDeviceType The device type for the parent
         *
         * @return True if the rule could be removed from the monitoring framework. False if such a rule could not be found in the current monitor list
         */
        bool removeMonitorRule(const QString &strSubSystem, const QString &strDeviceType, const QString &strParentSubSystem, const QString &strParentDeviceType);

    private:

        virtual void run();

        /**
         * Translate the udev action strings to our internal enumeration members
         */
        QUdevEventAction getQUdevEventActionFromUdevAction(const QString &strUdevAction) const;

        /**
         * This entry defines one rule for events we want to be notified about
         */
        typedef struct QUdevInternalWatcherEntry
        {
            /**
             * This is the subsystem we are monitoring
             */
            QString m_strSubsystem;
            /**
             * This is the devicetype we are monitoring
             */
            QString m_strDeviceType;
            /**
             * The parent subsystem
             */
            QString m_strParentSubSystem;
            /**
             * The parent devicetype
             */
            QString m_strParentDeviceType;

            QUdevInternalWatcherEntry(const QString &strSubSystem, const QString &strDeviceType, const QString &strParentSubSystem, const QString &strParentDeviceType)
              : m_strSubsystem(strSubSystem),
                m_strDeviceType(strDeviceType),
                m_strParentSubSystem(strParentSubSystem),
                m_strParentDeviceType(strParentDeviceType)
            {

            }

            /**
             * Comparison operator to identify duplicated monitor entries
             */
            bool operator==(const QUdevInternalWatcherEntry &Other)
            {
                bool bSame = true;
                if(this != &Other)
                {
                    bSame &= (m_strSubsystem == Other.m_strSubsystem);
                    bSame &= (m_strDeviceType == Other.m_strDeviceType);
                    bSame &= (m_strParentSubSystem == Other.m_strParentSubSystem);
                    bSame &= (m_strParentDeviceType == Other.m_strParentDeviceType);
                }
                return bSame;
            }

        } QUdevInternalWatcherEntry;

        /**
         * Handle to the udev library
         */
        struct udev *m_pUdev;

        /**
         * Handle to the udev monitor interface
         */
        struct udev_monitor* m_pMon;

        /**
         * All rules for device events we are currently monitoring
         */
        QList<QUdevInternalWatcherEntry> m_lMonitorEntries;

        /**
         * Map from udev action strings to the QUdevEventAction enumeration
         */
        QMap<QString, QUdevEventAction> m_mUdevActions;

        /**
         * Access to parent for signal emitting and stuff
         */
        QUdev * const q_ptr;
        Q_DECLARE_PUBLIC(QUdev);

        /**
         * Thread safe monitor event list handling
         */
        QMutex m_Mutex;

        /**
         * Hold the status of the monitoring status
         */
        bool m_bMonitoringActive;
};

#endif // QUDEVIMPL_H
