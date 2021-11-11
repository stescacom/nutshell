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
 * computational-node-container.h
 *
 *  Created on: Sep 15, 2015
 *      Author: ubaid
 */

#ifndef COMPUTATIONAL_NODE_CONTAINER_H
#define COMPUTATIONAL_NODE_CONTAINER_H

#include <stdint.h>
#include <vector>
#include "ns3/node-container.h"
#include "computational-node.h"
#include "processing-power-util.h"
#include "storage-util.h"

namespace ns3 {
/**
 * \brief Creates and Keeps track of a set of computational node pointers.
 *
 *	The class is a Helper, that makes it easy to create computational
 *	Nodes and keeps track of their pointer.
 */
class ComputationalNodeContainer {
public:
	/// ComputationalNode container iterator
	typedef std::vector<Ptr<ComputationalNode> >::const_iterator Iterator;



	/**
	* \brief Create an empty ComputationalNodeContainer.
	*/
	ComputationalNodeContainer();
	/**
	* Create a ComputationalNodeContainer with exactly one node which has been previously
	* instantiated.  The single ComputationalNode is specified by a smart pointer.
	*
	* \param node The Ptr<ComputationalNode> to add to the container.
	*/
	ComputationalNodeContainer (Ptr<ComputationalNode> node);

	/**
	* Create a ComputationalNodeContainer with exactly one node which has been previously
	* instantiated and assigned a name using the Object Name Service.  This
	* ComputationalNode is then specified by its assigned name.
	*
	* \param nodeName The name of the ComputationalNode Object to add to the container.
	*/
	ComputationalNodeContainer (std::string nodeName);

	/**
	* Create a node container which is a concatenation of two input
	* ComputationalNodeContainers.
	*
	* \param a The first ComputationalNodeContainer
	* \param b The second ComputationalNodeContainer
	*
	* \note A frequently seen idiom that uses these constructors involves the
	* implicit conversion by constructor of Ptr<ComputationalNode>.  When used, two
	* Ptr<ComputationalNode> will be passed to this constructor instead of ComputationalNodeContainer&.
	* C++ will notice the implicit conversion path that goes through the
	* ComputationalNodeContainer (Ptr<ComputationalNode> node) constructor above.  Using this conversion
	* one may provide optionally provide arguments of Ptr<ComputationalNode> to these
	* constructors.
	*/
	ComputationalNodeContainer (const ComputationalNodeContainer &a, const ComputationalNodeContainer &b);

	/**
	* Create a node container which is a concatenation of three input
	* ComputationalNodeContainers.
	*
	* \param a The first ComputationalNodeContainer
	* \param b The second ComputationalNodeContainer
	* \param c The third ComputationalNodeContainer
	*
	* \note A frequently seen idiom that uses these constructors involves the
	* implicit conversion by constructor of Ptr<ComputationalNode>.  When used, two
	* Ptr<ComputationalNode> will be passed to this constructor instead of ComputationalNodeContainer&.
	* C++ will notice the implicit conversion path that goes through the
	* ComputationalNodeContainer (Ptr<ComputationalNode> node) constructor above.  Using this conversion
	* one may provide optionally provide arguments of Ptr<ComputationalNode> to these
	* constructors.
	*/
	ComputationalNodeContainer (const ComputationalNodeContainer &a, const ComputationalNodeContainer &b, const ComputationalNodeContainer &c);

	/**
	* Create a node container which is a concatenation of four input
	* ComputationalNodeContainers.
	*
	* \param a The first ComputationalNodeContainer
	* \param b The second ComputationalNodeContainer
	* \param c The third ComputationalNodeContainer
	* \param d The fourth ComputationalNodeContainer
	*
	* \note A frequently seen idiom that uses these constructors involves the
	* implicit conversion by constructor of Ptr<ComputationalNode>.  When used, two
	* Ptr<ComputationalNode> will be passed to this constructor instead of ComputationalNodeContainer&.
	* C++ will notice the implicit conversion path that goes through the
	* ComputationalNodeContainer (Ptr<ComputationalNode> node) constructor above.  Using this conversion
	* one may provide optionally provide arguments of Ptr<ComputationalNode> to these
	* constructors.
	*/
	ComputationalNodeContainer (const ComputationalNodeContainer &a, const ComputationalNodeContainer &b, const ComputationalNodeContainer &c, const ComputationalNodeContainer &d);

	/**
	* Create a node container which is a concatenation of five input
	* ComputationalNodeContainers.
	*
	* \param a The first ComputationalNodeContainer
	* \param b The second ComputationalNodeContainer
	* \param c The third ComputationalNodeContainer
	* \param d The fourth ComputationalNodeContainer
	* \param e The fifth ComputationalNodeContainer
	*
	* \note A frequently seen idiom that uses these constructors involves the
	* implicit conversion by constructor of Ptr<ComputationalNode>.  When used, two
	* Ptr<ComputationalNode> will be passed to this constructor instead of ComputationalNodeContainer&.
	* C++ will notice the implicit conversion path that goes through the
	* ComputationalNodeContainer (Ptr<ComputationalNode> node) constructor above.  Using this conversion
	* one may provide optionally provide arguments of Ptr<ComputationalNode> to these
	* constructors.
	*/
	ComputationalNodeContainer (const ComputationalNodeContainer &a, const ComputationalNodeContainer &b, const ComputationalNodeContainer &c, const ComputationalNodeContainer &d,
				 const ComputationalNodeContainer &e);

	/**
	* \brief Get an iterator which refers to the first ComputationalNode in the
	* container.
	*
	* Computational Nodes can be retrieved from the container in two ways.  First,
	* directly by an index into the container, and second, using an iterator.
	* This method is used in the iterator method and is typically used in a
	* for-loop to run through the Computational Nodes
	*
	* \code
	*   ComputationalNodeContainer::Iterator i;
	*   for (i = container.Begin (); i != container.End (); ++i)
	*     {
	*       (*i)->method ();  // some ComputationalNode method
	*     }
	* \endcode
	*
	* \returns an iterator which refers to the first ComputationalNode in the container.
	*/
	Iterator Begin (void) const;

	/**
	* \brief Get an iterator which indicates past-the-last ComputationalNode in the
	* container.
	*
	* Computational Nodes can be retrieved from the container in two ways.  First,
	* directly by an index into the container, and second, using an iterator.
	* This method is used in the iterator method and is typically used in a
	* for-loop to run through the Computational Nodes
	*
	* \code
	*   ComputationalNodeContainer::Iterator i;
	*   for (i = container.Begin (); i != container.End (); ++i)
	*     {
	*       (*i)->method ();  // some ComputationalNode method
	*     }
	* \endcode
	*
	* \returns an iterator which indicates an ending condition for a loop.
	*/
	Iterator End (void) const;

	/**
	* \brief Get the number of Ptr<ComputationalNode> stored in this container.
	*
	* Computational Nodes can be retrieved from the container in two ways.  First,
	* directly by an index into the container, and second, using an iterator.
	* This method is used in the direct method and is typically used to
	* define an ending condition in a for-loop that runs through the stored
	* Nodes
	*
	* \code
	*   uint32_t nNodes = container.GetN ();
	*   for (uint32_t i = 0 i < nNodes; ++i)
	*     {
	*       Ptr<ComputationalNode> p = container.Get (i)
	*       i->method ();  // some ComputationalNode method
	*     }
	* \endcode
	*
	* \returns the number of Ptr<ComputationalNode> stored in this container.
	*/
	uint32_t GetN (void) const;

	/**
	* \brief Get the Ptr<ComputationalNode> stored in this container at a given
	* index.
	*
	* Nodes can be retrieved from the container in two ways.  First,
	* directly by an index into the container, and second, using an iterator.
	* This method is used in the direct method and is used to retrieve the
	* indexed Ptr<Appliation>.
	*
	* \code
	*   uint32_t nNodes = container.GetN ();
	*   for (uint32_t i = 0 i < nNodes; ++i)
	*     {
	*       Ptr<ComputationalNode> p = container.Get (i)
	*       i->method ();  // some ComputationalNode method
	*     }
	* \endcode
	*
	* \param i the index of the requested node pointer.
	* \returns the requested node pointer.
	*/
	Ptr<ComputationalNode> Get (uint32_t i) const;

	/**
	 * \brief Creates a node container, adds the Computational Nodes that lies
	 * inside the starting and end point and return the NodeContainer
	 *
	 * ComputationalNode containers are used by most helpers in ns-3, so to prevent
	 * changes in core of ns-3 ComputationalNodeContainer creates a node container
	 * from the list of Ptr<ComputationalNode>, the index of which lies inside the
	 * starting and end point. Then returns the NodeContainer Object.
	 *
	 *
	 * \param start the starting point index of computational node
	 * \param end the end point index of computational node
	 * \returns the created NodeContainer.
	 */
	NodeContainer GetNodeContainerOfRange(uint32_t start, uint32_t end);

	/**
	 * \brief Sets the configuration range strings of resources to assign to when creating
	 * the computational nodes.
	 *
	 * The configuration range is defined, which enable the helper to create
	 * computational nodes with resources between the range defined, and according
	 * to a distribution. The computational nodes will have same values if both
	 * minimum and maximum value of range is equal.
	 *
	 * \param minPower the minimum power a computational node can have
	 * \param maxPower the maximum power a computational node can have
	 * \param minPrimary the minimum primary storage a computational node can have
	 * \param maxPrimary the maximum primary storage a computational node can have
	 * \param minSecondary the minimum secondary storage a computational node can have
	 * \param maxSecondary the maximum secondary storage a computational node can have
	 */
	void SetConfigRangeStrings(const std::string minPower,
								const std::string maxPower,
								const std::string minPrimary,
								const std::string maxPrimary,
								const std::string minSecondary,
								const std::string maxSecondary);

	/**
	* \brief Create n nodes and append pointers to them to the end of this
	* ComputationalNodeContainer.
	*
	* Nodes are at the heart of any ns-3 simulation.  One of the first tasks that
	* any simulation needs to do is to create a number of nodes.  This method
	* automates that task.
	*
	* \param n The number of Nodes to create
	*/
	void Create (uint32_t n);

	/**
	* \brief Create n nodes with specified systemId for distributed simulations
	* and append pointers to them to the end of this ComputationalNodeContainer.
	*
	* Nodes are at the heart of any ns-3 simulation.  One of the first tasks that
	* any simulation needs to do is to create a number of nodes.  This method
	* automates that task, and adds the ability to specify systemId for
	* distributed simulations.
	*
	* \param n The number of Nodes to create
	* \param systemId The system id or rank associated with this node
	*/
	void Create (uint32_t n, uint32_t systemId);

	/**
	* \brief Append the contents of another ComputationalNodeContainer to the end of
	* this container.
	*
	* \param other The ComputationalNodeContainer to append.
	*/
	void Add (ComputationalNodeContainer other);

	/**
	* \brief Append a single Ptr<ComputationalNode> to this container.
	*
	* \param node The Ptr<ComputationalNode> to append.
	*/
	void Add (Ptr<ComputationalNode> node);

	/**
	* \brief Append to this container the single Ptr<ComputationalNode> referred to
	* via its object name service registered name.
	*
	* \param nodeName The name of the ComputationalNode Object to add to the container.
	*/
	void Add (std::string nodeName);

	virtual ~ComputationalNodeContainer();

private:
	/**
	 * \brief Get a storage value between a range
	 * \param min the minimum storage
	 * \param max the maximum storage
	 * \return the resulting storage between the min and max
	 */
	Storage GetStorageValue(Storage min, Storage max);
	/**
	 * \brief Get a processing power value between a range
	 * \param min the minimum processing power
	 * \param max the maximum processing power
	 * \return the resulting processing power between the min and max
	 */
	ProcessingPower GetProcessingValue (ProcessingPower min, ProcessingPower max);


	std::vector<Ptr<ComputationalNode> > m_nodes; //!< Nodes smart pointers
	/**
	 * \brief Structure to hold minimum and maximum resources strings
	 */
	struct Config
	{
		std::string 	processingMin, processingMax;
		std::string  	primaryMin, primaryMax;
		std::string  	secondaryMin, secondaryMax;
	};

	Config m_config; //!< Configuration strings

};

} /* namespace ns3 */

#endif /* SCRATCH_NUTSHELL_COMPUTATIONAL_NODE_CONTAINER_H_ */
