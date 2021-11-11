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
 * Authors: Ubaid ur Rahman (ubaid@ifast.pk, u.ur.rahman@gmail.com)
 *
 *
 */

#ifndef NUTSHELL_DATA_COLLECTOR_H
#define NUTSHELL_DATA_COLLECTOR_H

#include <vector>
#include <string>
#include "ns3/core-module.h"

namespace ns3 {

/**
 * \brief The class is a bird eye view for data collection and exporting
 *
 * This class maintains a reference to data collection classes that extend this
 * class and provide an easy interface to configure those classes and their output.
 *
 */
class NutshellDataCollector {

public:

	void Test(std::string t);

	/**
	 * \brief constructor for creating this class object
	 *
	 * The list created with this constructor is empty, use configuration methods
	 * to add collectors.
	 */
	NutshellDataCollector();
	/**
	 * \brief Adds a collector to the data collector list.
	 *
	 * The method accepts a child class object reference and adds the
	 * reference to the collector list. This method enable user to add their
	 * own collectors.
	 *
	 * \param newCollector Reference of child class object.
	 */
	void AddDataCollector(NutshellDataCollector& newCollector);

	/**
	 * \brief Attach sinks
	 *
	 * Loops through the data collector list and call their function AttachTraceSink().
	 */
	void AttachSinks();
	/**
	 * \brief Exports data.
	 *
	 * Loops through the data collector list, and call their function ExportData().
	 */
	void Export();

	virtual ~NutshellDataCollector();

	/**
	 * \brief Set the time interval after which the data mapping occur
	 *
	 * \param t the time interval in Seconds, a Uint32_t value
	 */
	void SetTimeInterval(uint16_t t);
	/**
	 * \brief Set the simulation prefix for file names
	 * \param s The prefix name
	 */
	void SetSimulationPrefix(std::string s);


	// ---------------- data collector configuration --------
protected:
	/**
	 * \brief Creates a file name with current timestamp
	 */
	std::string MakeFileName(std::string name, std::string extension);

	std::string MakeFileName(uint32_t identifier, std::string extension);

	/**
	 * \brief Concatenate strings together from the list
	 * \param s The vector list of strings to concatenate
	 * \return The concatenated string.
	 */
	std::string StringConcat(std::vector< std::string > s);

	/**
	 * \brief Concatenate two strings
	 * \param s1 The first string
	 * \param s2 The second string
	 * \return The concatenated string.
	 */
	std::string StringConcat(std::string s1, std::string s2);

	uint16_t m_timeInterval; //!< Time interval
	std::string m_simulationPrefix; //!< Simulation Prefix
private:

	/**
	 * \brief Attach a trace sink to trace source
	 *
	 * This method enable the class to attach different trace sinks to
	 * trace source provided by different NS-3 classes.
	 */
	virtual void AttachTraceSink();

	/**
	 * \brief Exports collected data to CSV format.
	 *
	 * This method exports data to a CSV format, defined by each
	 * child class.
	 */
	virtual void ExportData();

	std::vector< NutshellDataCollector* > m_collectors; //!< Data collector classes pointer

};

} /* namespace ns3 */

#endif /* SCRATCH_NUTSHELL_NUTSHELL_DATA_COLLECTOR_H */
