/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 ifast (SMC-PVT) LTD, COMSATS Institute of Information Technology & Qatar University
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Ubaid ur Rahman (ubaid@ifast.pk)
 * 			Kashif Bilal (kashif@qu.edu.pk)
 * 			Aiman Erbad (aerbad@qu.edu.qa)
 * 			Samee U Khan (samee.khan@ndsu.edu)
 *
 *
 */

#ifndef STORAGE_H
#define	STORAGE_H

#include <string>
#include <iostream>
#include <stdint.h>
#include "ns3/nstime.h"
#include "ns3/attribute.h"
#include "ns3/attribute-helper.h"
#include "ns3/deprecated.h"

namespace ns3 {

/**
 * \brief Converts human readable storage units into bytes
 *
 * This class converts storage amount provided in human readable
 * format e.g. 5GB to its equivalent number of bytes. The code use of this class:
 * \code
 *
 * Storage x("5GB");
 *
 * \endcode
 *
 * The class can also calculate the amount of time it takes to
 * read and write some amount of data, given the read/write rate e.g.
 * \code
 * Storage x("5GB");
 * double time = x.CalculateDRWxTime(x, 50);
 * \endcode
 *
 * This class also supports the regular comparison operators \c <, \c >,
 * \c <=, \c >=, \c ==, and \c !=
 *
 *
 *     Bytes Reference
 *
 * | Unit       |   Symbol  |    bytes      |
 * | :----------|-----------|--------------:|
 * | byte       |   B       |    1          |
 * | kilobyte   |   KB      |   1024 = 2^10 |
 * | megabyte   |   MB      |   2^20        |
 * | gigabyte   |   GB      |   2^30        |
 * | terabyte   |   TB      |   2^40        |
 * | petabyte   |   PB      |   2^50        |
 * | exabyte    |   EB      |   2^60        |
 *
 *
 */
class Storage {
public:
	/**
	 * \brief Storage Class constructor
	 *
	 * The constructor creates an empty object since it does not
	 * accept any data to translate.
	 *
	 */
	Storage();

	/**
	 * \brief Storage Class constructor, accepts number of bytes as storage amount
	 *
	 * The constructor accept the amount of storage and creates an object with same amount provided by user.
	 *
	 * Supported only uint64_t value
	 *
	 *
	 * \param storageBytes The amount of storage
	 *
	 */
	Storage(uint64_t storageBytes);

	/**
	 * \brief Storage Class constructor, accepts human readable string as storage amount
	 *
	 * The constructor accepts the amount of storage in string format e.g 5GB and translates
	 * it to the amount of bytes.
	 *
	 * Supported unit string:
	 * byte, B \n
	 * kilobyte, KB \n
	 * megabyte, MB \n
	 * gigabyte, GB \n
	 * terabyte, TB \n
	 * petabyte, PB \n
	 * exabyte, EB \n
	 *
	 * \param storage The amount in human readable format
	 */
	Storage(std::string storage);

	/**
	 * \return true if this storage is less than rhs
	 *
	 * \param rhs the storage to compare to this storage
	 */
	bool operator <(const Storage& rhs) const;

	/**
	 * \return true if this storage is less than or equal to rhs
	 *
	 * \param rhs the storage to compare to this storage
	 */
	bool operator <=(const Storage& rhs) const;

	/**
	 * \return true if this storage is greater than rhs
	 *
	 * \param rhs the storage to compare to this storage
	 */
	bool operator >(const Storage& rhs) const;

	/**
	 * \return true if this storage is greater than or equal to rhs
	 *
	 * \param rhs the storage to compare to this storage
	 */
	bool operator >=(const Storage& rhs) const;

	/**
	 * \return true if this storage is equal to rhs
	 *
	 * \param rhs the storage to compare to this storage
	 */
	bool operator ==(const Storage& rhs) const;

	/**
	 * \return true if this storage is not equal to rhs
	 *
	 * \param rhs the storage to compare to this storage
	 */
	bool operator !=(const Storage& rhs) const;

	/**
	 * \brief performs addition of rhs storage to this storage
	 *
	 * \return Storage Sum of rhs and this storage
	 * \param rhs the storage to add to this storage
	 */
	Storage operator +(const Storage& rhs) const;

	/**
	 * \brief performs subtraction of rhs storage to this storage
	 *
	 * \return Storage Sum of rhs and this storage
	 * \param rhs the storage to add to this storage
	 *
	 * \note rhs must be <= this storage if greater this object is returned.
	 */
	Storage operator -(const Storage& rhs) const;

	/**
	 * \brief Calculates data read write time
	 *
	 * It calculates the amount of time that is required to write the storage i.e data size
	 * onto a disk that has a particular read/write rate.
	 *
	 * \param dataSize Storage object with defined value
	 * \param rwRate an integer in MB/s i.e. if the read/write rate is 50 MB/s,
	 * the user must enter the value 50 as rwRate parameter
	 *
	 * \return double the time required to complete the read/write job
	 *
	 * \note the read/write time calculation does not consider paging.
	 */
	double CalculateDRWxTime(Storage& dataSize, uint32_t rwRate) const;

	/**
	 * \brief Getter for the storage value
	 *
	 * \return uint64_t value
	 */
	uint64_t GetStorage() const;

private:

	/**
	 * \brief Parse a string representing a Storage into an uint64_t
	 *
	 * Allowed unit representations include all combinations of
	 *
	 * * An SI prefix: K, M, G, T, P, E
	 * * Decimal or string value such as 5GB
	 * * byte or B
	 *
	 * \param [in] s The string representation, including unit
	 * \param [in,out] v The location to put the value, in bytes.
	 * \return true if parsing was successful.
	 */
	static bool DoParse(const std::string s, uint64_t *v);

	friend std::ostream & operator <<(std::ostream &os, const Storage &storage);

	friend std::istream & operator >>(std::istream &is, Storage &storage);

	uint64_t m_bytes; //!< storage [bytes]

};
// class Storage

/**
 * \brief Stream insertion operator.
 *
 * \param os the stream
 * \param storage the Storage
 * \returns a reference to the stream
 */
std::ostream & operator <<(std::ostream &os, const Storage &storage);
/**
 * \brief Stream extraction operator.
 *
 * \param is the stream
 * \param storage the Storage
 * \returns a reference to the stream
 */
std::istream & operator >>(std::istream &is, Storage &storage);

ATTRIBUTE_HELPER_HEADER(Storage);

} // namespace ns3
#endif	/* STORAGE_H */

