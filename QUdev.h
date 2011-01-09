/*
 * This file is part of QUdev - a simple libudev wrapper for Qt.
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

#ifndef QUDEV_H
#define QUDEV_H

#include <QObject>
#include <QMetaType>

#include "QUdev_global.h"
#include "QUdevDeclarations.h"

class QUdevPrivate;

/**
 * Public class used for retrieving udev events with qt signals
 *
 * This also implements a minimalistic interface to the libudev
 * enumeration interface.
 */
class QUDEVSHARED_EXPORT QUdev : public QObject
{
    Q_OBJECT

public:

    /**
     * Default constructor
     */
    explicit QUdev(QObject *parent = 0);

    /**
     * Default destructor
     */
    ~QUdev();

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

Q_SIGNALS:

    /**
     * Emitted if an event matching the registered rules was found
     */
    void newUDevEvent(QUdevEvent);

private:

    /**
     * Internal implementation
     */
    QUdevPrivate* d_ptr;
    Q_DECLARE_PRIVATE(QUdev);

};

#endif // QUDEV_H
