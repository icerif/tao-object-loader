/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 Version 2.0.0, packaged on July 2010.

 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 *****************************************************************************/
//! \file glc_bsrep.cpp implementation for the GLC_BSRep class.

#include "glc_bsrep.h"
#include "../glc_fileformatexception.h"

// The binary rep suffix
const QString GLC_BSRep::m_Suffix("BSRep");

// The binary rep magic number
const QUuid GLC_BSRep::m_Uuid("{d6f97789-36a9-4c2e-b667-0e66c27f839f}");

// The binary rep version
const quint32 GLC_BSRep::m_Version= 101;


// Default constructor
GLC_BSRep::GLC_BSRep(const QString& fileName, bool useCompression)
: m_FileInfo()
, m_pFile(NULL)
, m_DataStream()
, m_UseCompression(useCompression)
, m_CompressionLevel(-1)
, m_VersionIsCompatible(false)
{
	setAbsoluteFileName(fileName);
	m_DataStream.setVersion(QDataStream::Qt_4_6);
	m_DataStream.setFloatingPointPrecision(QDataStream::SinglePrecision);
}

// Copy constructor
GLC_BSRep::GLC_BSRep(const GLC_BSRep& binaryRep)
: m_FileInfo(binaryRep.m_FileInfo)
, m_pFile(NULL)
, m_DataStream()
, m_UseCompression(binaryRep.m_UseCompression)
, m_CompressionLevel(binaryRep.m_CompressionLevel)
{
	m_DataStream.setVersion(QDataStream::Qt_4_6);
	m_DataStream.setFloatingPointPrecision(binaryRep.m_DataStream.floatingPointPrecision());
}

GLC_BSRep::~GLC_BSRep()
{
	delete m_pFile;
}

// Return true if the binary rep is up to date
bool GLC_BSRep::repIsUpToDate(const QDateTime& timeStamp)
{
	//qDebug() << "GLC_BSRep::repIsUpToDate";
	bool isUpToDate= false;
	if (open(QIODevice::ReadOnly))
	{
		if (headerIsOk())
		{
			isUpToDate= m_VersionIsCompatible && timeStampOk(timeStamp);
			isUpToDate= isUpToDate && close();
		}
		else
		{
			QString message(QString("GLC_BSRep::loadRep File not recognise ") + m_FileInfo.fileName());
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, m_FileInfo.fileName(), GLC_FileFormatException::WrongFileFormat);
			close();
			throw(fileFormatException);
		}
	}
	else
	{
		QString message(QString("GLC_BSRep::loadRep Enable to open the file ") + m_FileInfo.fileName());
		qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_FileInfo.fileName(), GLC_FileFormatException::FileNotFound);
		close();
		throw(fileFormatException);
	}

	if (!isUpToDate) qDebug() << "Rep is not up to date";
	return isUpToDate;
}

//////////////////////////////////////////////////////////////////////
// name Get Functions
//////////////////////////////////////////////////////////////////////
// Load the binary rep
GLC_3DRep GLC_BSRep::loadRep()
{
	//qDebug() << "GLC_BSRep::loadRep";
	GLC_3DRep loadedRep;

	if (open(QIODevice::ReadOnly))
	{
		if (headerIsOk())
		{
			timeStampOk(QDateTime());
			GLC_BoundingBox boundingBox;
			m_DataStream >> boundingBox;
			bool useCompression;
			m_DataStream >> useCompression;
			if (useCompression)
			{
				QByteArray CompresseBuffer;
				m_DataStream >> CompresseBuffer;
				QByteArray uncompressedBuffer= qUncompress(CompresseBuffer);
				CompresseBuffer.clear();
				QDataStream bufferStream(uncompressedBuffer);
				bufferStream >> loadedRep;
			}
			else
			{
				m_DataStream >> loadedRep;
			}

			if (!close())
			{
				QString message(QString("GLC_BSRep::loadRep An error occur when loading file ") + m_FileInfo.fileName());
				qDebug() << message;
				GLC_FileFormatException fileFormatException(message, m_FileInfo.fileName(), GLC_FileFormatException::WrongFileFormat);
				throw(fileFormatException);
			}
		}
		else
		{
			QString message(QString("GLC_BSRep::loadRep File not supported ") + m_FileInfo.fileName());
			qDebug() << message;
			GLC_FileFormatException fileFormatException(message, m_FileInfo.fileName(), GLC_FileFormatException::FileNotSupported);
			close();
			throw(fileFormatException);
		}
	}
	else
	{
		QString message(QString("GLC_BSRep::loadRep Enable to open the file ") + m_FileInfo.fileName());
		qDebug() << message;
		GLC_FileFormatException fileFormatException(message, m_FileInfo.fileName(), GLC_FileFormatException::FileNotFound);
		close();
		throw(fileFormatException);
	}


	return loadedRep;
}

// Return the bounding box of the binary representation
GLC_BoundingBox GLC_BSRep::boundingBox()
{
	GLC_BoundingBox boundingBox;

	if (open(QIODevice::ReadOnly))
	{
		if (headerIsOk())
		{
			timeStampOk(QDateTime());

			m_DataStream >> boundingBox;
		}
	}
	return boundingBox;
}

// Return bsrep suffix
QString GLC_BSRep::suffix()
{
	return m_Suffix;
}

//////////////////////////////////////////////////////////////////////
//name Set Functions
//////////////////////////////////////////////////////////////////////
// Set the binary representation file name
void GLC_BSRep::setAbsoluteFileName(const QString& fileName)
{
	m_FileInfo.setFile(fileName);
	if (m_FileInfo.suffix() != m_Suffix)
	{
		m_FileInfo.setFile(fileName + '.' + m_Suffix);
	}

}

// Save the GLC_3DRep in serialised binary
bool GLC_BSRep::save(const GLC_3DRep& rep)
{
	qDebug() << "GLC_BSRep::save";
	//! Check if the currentFileInfo is valid and writable
	bool saveOk= open(QIODevice::WriteOnly);
	if (saveOk)
	{
		writeHeader(rep.lastModified());

		// Representation Bounding Box
		m_DataStream << rep.boundingBox();

		// Compression usage
		m_DataStream << m_UseCompression;
		if (m_UseCompression)
		{
			QByteArray uncompressedBuffer;
			QDataStream bufferStream(&uncompressedBuffer, QIODevice::WriteOnly);
			bufferStream << rep;
			m_DataStream << qCompress(uncompressedBuffer, m_CompressionLevel);
		}
		else
		{
			// Binary representation geometry
			// Add the rep
			m_DataStream << rep;
		}

		// Close the file
		saveOk= close();
	}
	return saveOk;
}


// Open the file
bool GLC_BSRep::open(QIODevice::OpenMode mode)
{
	//qDebug() << "Open :" << m_FileInfo.fileName();
	bool openOk= m_FileInfo.exists();
	if (openOk || (mode == QIODevice::WriteOnly))
	{
		m_DataStream.setDevice(NULL);
		delete m_pFile;
		m_pFile= new QFile(m_FileInfo.filePath());
		openOk= m_pFile->open(mode);
		if (openOk)
		{
			m_DataStream.setDevice(m_pFile);
		}
	}
	else
	{
		qDebug() << "File info " << m_FileInfo.filePath() << " do not exists";
	}
	return openOk;
}

// Close the file
bool GLC_BSRep::close()
{
	Q_ASSERT(m_pFile != NULL);
	Q_ASSERT(m_DataStream.device() != NULL);
	bool closeOk= m_DataStream.status() == QDataStream::Ok;
	m_DataStream.setDevice(NULL);
	m_pFile->close();
	delete m_pFile;
	m_pFile= NULL;

	return closeOk;
}

// Write the header
void GLC_BSRep::writeHeader(const QDateTime& dateTime)
{
	Q_ASSERT(m_pFile != NULL);
	Q_ASSERT(m_DataStream.device() != NULL);
	Q_ASSERT(m_pFile->openMode() == QIODevice::WriteOnly);

	// Binary representation Header
	// Add the magic number
	m_DataStream << m_Uuid;
	// Add the version
	m_DataStream << m_Version;

	// Set the version of the data stream
	m_DataStream.setVersion(QDataStream::Qt_4_6);

	// Add the time stamp
	m_DataStream << dateTime;
}

// Check the header
bool GLC_BSRep::headerIsOk()
{
	Q_ASSERT(m_pFile != NULL);
	Q_ASSERT(m_DataStream.device() != NULL);
	Q_ASSERT(m_pFile->openMode() == QIODevice::ReadOnly);

	QUuid uuid;
	quint32 version;
	m_DataStream >> uuid;
	m_DataStream >> version;

	// Set the version of the data stream
	m_DataStream.setVersion(QDataStream::Qt_4_6);

	bool headerOk= (uuid == m_Uuid);
	m_VersionIsCompatible= (version == m_Version);

	return headerOk;
}

// Check the time Stamp
bool GLC_BSRep::timeStampOk(const QDateTime& timeStamp)
{
	Q_ASSERT(m_pFile != NULL);
	Q_ASSERT(m_DataStream.device() != NULL);
	Q_ASSERT(m_pFile->openMode() == QIODevice::ReadOnly);

	QDateTime dateTime;
	m_DataStream >> dateTime;

	bool timeStampOk= !timeStamp.isValid() || (dateTime == timeStamp);
	return timeStampOk;
}

