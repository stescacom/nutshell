/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 *
 * addressing-scheme.h
 *
 *  Created on: Oct 19, 2015
 *      Author: ubaid
 */

#ifndef ADDRESSING_SCHEME
#define ADDRESSING_SCHEME

#include <vector>
#include <sstream>

namespace ns3 {
/**
 * \brief The class is used to create an addressing scheme for Data center architecture
 *
 * The class convert a class full address to number of class less subnets, the subnet
 * formed are according to the number of networks requested by the user.
 */
class AddressingScheme {
public:
	/**
	 * \brief Class constructor
	 */
	AddressingScheme();
	/**
	 * \brief Network address setter
	 * \param networkAddress the subnet network address
	 * \param subnet the network mask of subnet
	 */
	void SetNetworkAddress(std::string networkAddress, std::string subnet);
	/**
	 * \brief Setter for number of hosts and number of networks
	 * \param hosts the number of hosts
	 * \param netNum the number of networks
	 */
	void SetComponentNumber(uint32_t hosts, uint32_t netNum);
	/**
	 * \brief Setter for defining jump after a network
	 * \param jumpValues vector listing a number, to be used as a jump number after last network
	 */
	void SetJumpValues(std::vector<uint16_t> &jumpValues);
	/**
	 * \brief Initiate the network address creation
	 *
	 */
	virtual void CreateNetworkAddresses();
	/**
	 * \brief Get a network address by index
	 * \param id the index
	 * \return network address string
	 */
	std::string GetNetwork(uint32_t id);
	/**
	 * \brief Get the network subnet mask
	 * \param id the index
	 * \return subnet mask string
	 */
	std::string GetSubnet(uint32_t id);
	/**
	 * \brief Get the complete list of networks created
	 * \return a 2D vector of string.
	 */
	std::vector< std::vector< std::string > > GetNetworkList() const;
	virtual ~AddressingScheme();

protected:
	/**
	 * \brief Splits the string and converts the string value of network address and subnet mask to an integer value
	 * \param networkAddress the network address of subnet
	 * \param subnet the subnet mask of the subnet
	 */
	void ConvertAndSplit(std::string networkAddress, std::string subnet);
	/**
	 * \brief Resizes the network vector to the number of networks required
	 * \param numNet the number of networks
	 */
	void ResizeNetworksVector(uint32_t numNet);
	/**
	 * \brief Converts a string to integer value
	 * \param s the string to convert
	 * \return an integer value
	 */
	uint16_t ConvertString(std::string s);
	/**
	 * \brief Converts and concatenate supplied variables to string
	 * \param fo the first octet of network address
	 * \param so the second octet of network address
	 * \param to the third octet of network address
	 * \param foo the fourth octet of network address
	 * \return the concatenated network address dot notation
	 */
	std::string ConvertToString(uint16_t fo, uint16_t so, uint16_t to, uint16_t foo);
	/**
	 * \brief Processes the IP address octet array
	 *
	 * The function checks for current working octet, if reaches 256, then making current
	 * octet to 0 and increments the previous octet by 1
	 *
	 * \param array the array of IP octets
	 * \param workingOct the working octet
	 */
	void ProcessAddress(uint16_t *array, uint16_t workingOct);

	uint16_t m_oct[4]; //!< holds the IP octets as a list
	uint16_t m_snetP[4]; //!< holds the subnet mask octets

	uint32_t m_numNet; //!< number of networks required
	uint32_t m_hosts; //!< number of hosts required in a single subnet


	std::vector< std::vector< std::string > > m_networks; //!< list of network addresses
	std::vector<uint16_t> m_jumpValues; //!< list of jumps after a network

//private:
//	virtual void CreateNetworkAddresses();

};

} /* namespace ns3 */

#endif /* ADDRESSING_SCHEME */
