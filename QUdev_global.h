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

#ifndef QUDEV_GLOBAL_H
#define QUDEV_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QUDEV_LIBRARY)
#  define QUDEVSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QUDEVSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QUDEV_GLOBAL_H
