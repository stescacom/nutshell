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

#ifndef PROCESSINGPOWER_H
#define	PROCESSINGPOWER_H

#include <string>
#include <iostream>
#include <stdint.h>
#include "ns3/nstime.h"
#include "ns3/attribute.h"
#include "ns3/attribute-helper.h"

namespace ns3 {

/**
 * \brief Converts human readable processing power into a base unit
 *
 * This class converts processing power amount provided in human readable
 * format e.g. 5MIPS or 5GFLOPS to its equivalent base metric. The code use of this class:
 * \code
 *
 * ProcessingPower p("5mips");
 * ProcessingPower p2("5FLOPS");
 *
 * \endcode
 *
 * The class can also calculate the amount of time it takes to
 * execute a given number of instructions of some applications e.g.
 * \code
 * ProcessingPower x("5MIPS");
 * double time = x.CalculateExTime(20000, ProcessingPower::METRIC_INSTRUCTION_COUNT);
 *
 * ProcessingPower y("10FLOPS");
 * double time = y.CalculateExTime(30000, ProcessingPower::METRIC_FLOP);
 * \endcode
 *
 * This class also supports the regular comparison operators \c <, \c >,
 * \c <=, \c >=, \c ==, and \c !=
 *
 *
 *
 */
class ProcessingPower {
public:
	/**
	 * \brief ProcessingPower Class constructor
	 *
	 * The constructor creates an empty object since it does not
	 * accept any data to translate. The default metric is set to FLOPS
	 * (Floadting Point Operations Per Second).
	 *
	 */
	ProcessingPower();
	/**
	 * \brief ProcessingPower constructor
	 *
	 * The constructor creates an object with supplied amount of
	 * processing power and set the type of processing metric, currently
	 * the system supports MIPS (Million of Instructions Per Second) and FLOPS
	 * (Floadting Point Operations Per Second).
	 *
	 * \param power uint64_t value of processing power.
	 * \param mips boolean value is a flag which is set to true if the metric is MIPS
	 * \param flops boolean value is a flag which is set to true if metric is FLOPS
	 *
	 * \note You can not set both flags to true or false, in case the user set them equal it is default to FLOPS
	 *
	 */
	ProcessingPower(uint64_t power, bool mips, bool flops);
	/**
	 * \brief ProccessingPower constructor
	 *
	 * The constructor creates an object with supplied amount of
	 * processing power and set the type metric e.g.
	 * \code
	 * ProcessingPower p1(222, ProcessingPower::POWER_MIPS);
	 * ProcessingPower p2(333, ProcessingPwer::POWER_FLOPS);
	 * \endcode
	 *
	 * \param power uint64_t value of processing power.
	 * \param metric int value set via ProcessingPower constant for metric.
	 *
	 */
	ProcessingPower(uint64_t power, const int metric);

	/**
	 * \brief ProcessingPower constructor
	 *
	 * The constructor creates an object with supplied amount of power. This
	 * allows users to set the power value using natural and familiar statement.
	 * e.g 10MIPS or 100FLOPS
	 *
	 * Supported unit strings:
	 * MIPS, mips, Mips \n
	 * FLOPS, flops, Flops \n
	 * MFLOPS, mflops, Mflops \n
	 * GFLOPS, gflops, Gflops \n
	 * TFLOPS, tflops, Tflops
	 *
	 */
	ProcessingPower(std::string power);
	/**
	 * \return true if this processing power is less than rhs
	 *
	 * \param rhs the processing power to compare to this processing power
	 */
	bool operator <(const ProcessingPower& rhs) const;
	/**
	 * \return true if this processing power is less than or equal rhs
	 *
	 * \param rhs the processing power to compare to this processing power
	 */
	bool operator <=(const ProcessingPower& rhs) const;
	/**
	 * \return true if this processing power is greater than rhs
	 *
	 * \param rhs the processing power to compare to this processing power
	 */
	bool operator >(const ProcessingPower& rhs) const;
	/**
	 * \return true if this processing power is greater than or equal rhs
	 *
	 * \param rhs the processing power to compare to this processing power
	 */
	bool operator >=(const ProcessingPower& rhs) const;
	/**
	 * \return true if this processing power is equal to rhs
	 *
	 * \param rhs the processing power to compare to this processing power
	 */
	bool operator ==(const ProcessingPower& rhs) const;
	/**
	 * \return true if this processing power is not equal to rhs
	 *
	 * \param rhs the processing power to compare to this processing power
	 */
	bool operator !=(const ProcessingPower& rhs) const;

	/**
	 * \brief performs addition of rhs processing power to this processing power
	 *
	 * \return Storage Sum of rhs and this processing power
	 * \param rhs the processing power to add to this processing power
	 */
	ProcessingPower operator +(ProcessingPower& rhs);
	/**
	 * \brief performs subtraction of rhs processing power to this processing power
	 *
	 * \return Storage Sum of rhs and this processing power
	 * \param rhs the processing power to add to this processing power
	 *
	 * \note rhs must be <= this processing power if greater this object is returned.
	 */
	ProcessingPower operator -(ProcessingPower& rhs);

	/**
	 * \brief Calculates execution time for a given number of instructions/floating point operations.
	 *
	 * It calculates the amount of time that is required to complete the execution
	 * of application, the application number of instruction count or operations is passed
	 * to this method along with the metric.
	 *
	 * \param instructionCount a uint64_t value representing the number of instructions or operations
	 * \param metric a ProcessingPower integer constant
	 * \return double value representing time
	 *
	 * \note use METRIC_INSTRUCTION_COUNT if processing power metric is MIPS (POWER_MIPS) and METRIC_FLOP if processing power is FLOPS (POWER_FLOPS)
	 */
	double CalculateExTime(const uint64_t instructionCount,
			const int metric) const;

	/**
	 * \brief Getter for the processing power value
	 *
	 * \return uint64_t value
	 */
	uint64_t GetProcessingPower() const;

	/**
	 * \brief To check if this processing power is of MIPS metric
	 *
	 * \return true if the processing power is MIPS.
	 */
	bool IsMips() const;
	/**
	 * \brief To check if this processing power is of FLOPS metric
	 *
	 * \return true if the processing power is FLOPS.
	 */
	bool IsFlops() const;

	static const int METRIC_INSTRUCTION_COUNT = 1; //!< Constant indicating the application has instruction count and must be handled by MIPS type processor
	static const int METRIC_FLOP = 2; //!< Constant indicating the application has floating point operations and must be handled by FLOPS type processor

	static const int POWER_MIPS = 3; //!< To set the processor type to MIPS
	static const int POWER_FLOPS = 4; //!< To set the processor type to FLOPS

private:

	/**
	 * \brief Parse a string representing a Processing Power into an uint64_t
	 *
	 * Allowed unit representations include all combinations of
	 *
	 * * An SI prefix: M, G, T for FLOPS and only MIPS
	 * * Decimal or string value such as 5mips, 15GFLOPS
	 * * MIPS, or mips, or Mips, and FLOPS, or flops, or Flops.
	 *
	 * \param [in] s The string representation, including unit
	 * \param [in,out] v The location to put the value, in bytes.
	 * \param mips A boolean true value if metric is MIPS, false otherwise
	 * \param flops A boolean true value if metric is FLOPS, false otherwise
	 * \return true if parsing was successful.
	 */
	static bool DoParse(const std::string s, uint64_t *v, bool *mips,
			bool *flops);


	friend std::istream &operator >>(std::istream &is, ProcessingPower &power);

	uint64_t m_processingPower; //!< processing power [mips or flops]

	bool m_mips; //!< if set to true, indicates the processor type as MIPS

	bool m_flops; //!< if set to true, indicates the processor type as FLOPS

};
// class ProcessingPower
/**
 * \brief Stream insertion operator.
 *
 * \param os the stream
 * \param power the processing power
 * \returns a reference to the stream
 */
std::ostream &operator <<(std::ostream &os, const ProcessingPower &power);
/**
 * \brief Stream extraction operator.
 *
 * \param is the stream
 * \param power the processing power
 * \returns a reference to the stream
 */
std::istream &operator >>(std::istream &is, ProcessingPower &power);

ATTRIBUTE_HELPER_HEADER(ProcessingPower);

} // namespace ns3

#endif	/* PROCESSINGPOWER_H */

