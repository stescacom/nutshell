/* 
 * File:   application-size-util.h
 * Author: ubaid <u.ur.rahman@gmail.com>
 *
 * Created on July 16, 2015, 10:50 PM
 */

#ifndef APPLICATIONSIZE_H
#define	APPLICATIONSIZE_H

#include <string>
#include <iostream>
#include <stdint.h>

#include "ns3/nstime.h"
#include "ns3/attribute.h"
#include "ns3/attribute-helper.h"

namespace ns3 {
    
    // accepts flop, M,G and T-flop and instructions in Million (10^6) Instructions
/**
 * \brief Converts human readable application size into a base unit
 *
 * This class converts application size amount provided in human readable
 * format e.g. 5MI or 5GFLOP to its equivalent base metric. The code use of this class:
 *
 * This class also supports the regular comparison operators \c <, \c >,
 * \c <=, \c >=, \c ==, and \c !=
 *
 */
class ApplicationSize {
public:
	/**
	 * \brief Class constructor
	 *
	 */
    ApplicationSize();
    /**
     * \brief Class constructor
     * \param instOpCount the number of instructions or operation of the application
     * \param instCount a boolean value true is set if the instOpCount is the number of instructions, false otherwise
     * \param flop a boolean value true is set if the instOpCount is the number of floating point operations, false otherwise
     */
    ApplicationSize (uint64_t instOpCount, bool instCount, bool flop);
    /**
     * \brief Class constructor
     * \param instOpCount the number of instructions or operation of the application
     * \param metric string value "flop" for FLOP and "ic" for Instruction Count
     */
    ApplicationSize(uint64_t instOpCount, std::string metric);
    /**
     * \brief Class constructor
     * \param appSize The application size with metric e.g. 500MI or 500TFLOP
     */
    ApplicationSize(std::string appSize);
    /**
	 * \return true if this application size is less than rhs
	 *
	 * \param rhs the the application size to compare to this application size
	 */
    bool operator < (ApplicationSize& rhs) const;
    /**
	 * \return true if this application size is greater than rhs
	 *
	 * \param rhs the the application size to compare to this application size
	 */
    bool operator > (ApplicationSize& rhs) const;
    /**
	 * \return true if this application size is less than or equal to rhs
	 *
	 * \param rhs the the application size to compare to this application size
	 */
    bool operator <= (ApplicationSize& rhs) const;
    /**
	 * \return true if this application size is equal to rhs
	 *
	 * \param rhs the the application size to compare to this application size
	 */
    bool operator == (ApplicationSize& rhs) const;
    /**
	 * \return true if this application size is greater than or equal to rhs
	 *
	 * \param rhs the the application size to compare to this application size
	 */
    bool operator >= (ApplicationSize& rhs) const;
    /**
	 * \return true if this application size is not equal to rhs
	 *
	 * \param rhs the the application size to compare to this application size
	 */
    bool operator != (ApplicationSize& rhs) const;
    /**
     * \brief Checks if the application size is in FLOP
     * \return true if the metric is FLOP, false otherwise
     */
    bool IsFlop() const;
    /**
     * \brief Checks if the application size is defined with instruction count
     * \return true if the metric is instruction, false otherwise
     */
    bool IsInstruction() const;
    /**
     * \brief Gets the size of application
     * \return an integer value
     */
    uint64_t GetApplicationSize() const;
    
private:
	/**
	 * \brief Parse a string representing an application size into a uint64_t
	 *
	 * Allowed unit representations include all combinations of
	 *
	 * * An SI prefix: M, G, T for FLOP and only MI
	 * * Decimal or string value such as 5mi, 15GFLOP
	 * * MIPS, or mips, or Mips, and FLOPS, or flops, or Flops.
	 *
	 * \param [in] s The string representation, including unit
	 * \param [in,out] v The location to put the value, in bytes.
	 * \param mips A boolean true value if metric is instruction count, false otherwise
	 * \param flops A boolean true value if metric is FLOP, false otherwise
	 * \return true if parsing was successful.
	 */
    static bool DoParse(const std::string s, uint64_t *v, bool *inst, bool *flop);
    
    friend std::istream &operator >> (std::istream &is, ApplicationSize &appSize);
    
    bool m_flop; //!< flag to identify if the application unit is FLOP
    bool m_instructions; //!< flag to identify if the application unit is in MI (million instructions)
    
    uint64_t m_instOpCount; //!< size of application
}; // class ApplicationSize
/**
 * \brief Stream insertion operator.
 *
 * \param os the stream
 * \param appSize the application size
 * \returns a reference to the stream
 */
std::ostream &operator << (std::ostream &os, const ApplicationSize &appSize);
/**
 * \brief Stream extraction operator.
 *
 * \param is the stream
 * \param appSize the application size
 * \returns a reference to the stream
 */
std::istream &operator >> (std::istream &is, ApplicationSize &appSize);

ATTRIBUTE_HELPER_HEADER(ApplicationSize);

} // namespace ns3
#endif	/* APPLICATIONSIZE_H */

