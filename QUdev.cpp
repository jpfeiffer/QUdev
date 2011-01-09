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

#include "QUdev.h"
#include "QUdev_private.h"

QUdev::QUdev(QObject *parent /*= 0*/)
 : QObject(parent),
   d_ptr(new QUdevPrivate(this))
{

}

QUdev::~QUdev()
{
    Q_D(QUdev);
    delete d;
}

QUdevDeviceList QUdev::getUDevDevicesForSubsystem(const QString &strSubSystem, const QString &strDeviceType, const QString &strParentSubSystem, const QString &strParentDeviceType)
{
    Q_D(QUdev);
    return d->getUDevDevicesForSubsystem(strSubSystem, strDeviceType, strParentSubSystem, strParentDeviceType);
}

bool QUdev::addNewMonitorRule(const QString &strSubSystem, const QString &strDeviceType, const QString &strParentSubSystem, const QString &strParentDeviceType)
{
    Q_D(QUdev);
    return d->addNewMonitorRule(strSubSystem, strDeviceType, strParentSubSystem, strParentDeviceType);
}

bool QUdev::removeMonitorRule(const QString &strSubSystem, const QString &strDeviceType, const QString &strParentSubSystem, const QString &strParentDeviceType)
{
    Q_D(QUdev);
    return d->removeMonitorRule(strSubSystem, strDeviceType, strParentSubSystem, strParentDeviceType);
}
