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
#include "storage-util.h"

#include "ns3/nstime.h"
#include "ns3/fatal-error.h"
#include "ns3/log.h"

#include <math.h>

namespace ns3
{

    NS_LOG_COMPONENT_DEFINE ( "Storage" );

    ATTRIBUTE_HELPER_CPP ( Storage );

    /* static*/
    bool
    Storage::DoParse ( const std::string s, uint64_t* v )
    {
       NS_LOG_FUNCTION (s << v);
        std::string::size_type n = s.find_first_not_of ("0123456789.");
        if(n != std::string::npos)
        {
            std::istringstream iss;
            iss.str(s.substr (0,n));
            double r;
            iss >> r;
            std::string trailer = s.substr (n, std::string::npos);
            if(trailer == "B" || trailer == "bytes")
            {
                *v = (uint64_t) r;
            }
            else if(trailer == "KB" || trailer == "kilobytes")
            {
                *v = (uint64_t) (r * pow (2, 10));
            }
            else if(trailer == "MB" || trailer == "megabytes")
            {
                *v = (uint64_t) (r * pow (2, 20));
            }
            else if(trailer == "GB" || trailer == "gigabytes")
            {
                *v = (uint64_t) (r * pow (2, 30));
            }
            else if(trailer == "TB" || trailer == "terabytes")
            {
                *v = (uint64_t) (r * pow (2, 40));
            }
            else if(trailer == "PB" || trailer == "petabytes")
            {
                *v = (uint64_t) (r * pow (2, 50));
            }
            else if(trailer == "EB" || trailer == "exabytes")
            {
                *v = (uint64_t) (r * pow (2, 60));
            }
            else
            {
                return false;
            }
            return true;
        }
        std::istringstream iss;
        iss.str (s);
        iss >> *v;
        return true;
    }

    Storage::Storage ( )
        : m_bytes(0)
    {
        NS_LOG_FUNCTION (this);
    }

    Storage::Storage ( uint64_t storageBytes)
        : m_bytes(storageBytes)
    {
        NS_LOG_FUNCTION (this);
    }

    bool Storage::operator != ( const Storage& rhs) const
    {
        return (m_bytes != rhs.m_bytes);
    }
    
    bool Storage::operator < (const Storage& rhs) const
    {
        return (m_bytes < rhs.m_bytes);
    }
    
    bool Storage::operator <= ( const Storage& rhs) const
    {
        return (m_bytes <= rhs.m_bytes);
    }
    bool Storage::operator == ( const Storage& rhs) const
    {
        return (m_bytes == rhs.m_bytes);
    }

    bool Storage::operator > ( const Storage& rhs) const
    {
        return (m_bytes > rhs.m_bytes);
    }
    bool Storage::operator >= ( const Storage& rhs) const
    {
        return (m_bytes >= rhs.m_bytes);
    }
    
    Storage 
    Storage::operator + ( const Storage& rhs) const
    {
        return Storage(m_bytes + rhs.m_bytes);
    }

    Storage 
    Storage::operator - ( const Storage& rhs) const
    {
    	if ( rhs.m_bytes <= m_bytes)
    	{
    		return Storage(m_bytes - rhs.m_bytes);
    	}
    	return *this;
    }

    
    double
    Storage::CalculateDRWxTime ( Storage& dataSize, uint32_t rwRate = 50) const
    {
        NS_LOG_FUNCTION (this << dataSize << rwRate);
        uint64_t rate = rwRate * pow(2, 20); // converting MB/s to B/s
        return (dataSize.m_bytes / rate);
    }
    uint64_t
    Storage::GetStorage ( ) const
    {
        NS_LOG_FUNCTION (this);
        return m_bytes;
    }

    Storage::Storage (std::string storage)
    {
        NS_LOG_FUNCTION (this << storage);
        bool ok = DoParse (storage, &m_bytes);
        if(!ok)
        {
            NS_FATAL_ERROR ("Could not parse storage: "<< storage);
        }
    }
    
    /* For printing of processing storage */
    
    std::istream & operator >> (std::istream &is, Storage &storage)
    {
        std::string value;
        is >> value;
        uint64_t v;
        bool ok = Storage::DoParse(value, &v);
        if(!ok)
        {
            is.setstate (std::ios_base::failbit);
        }
        storage = Storage(v);
        return is;
    }
    std::ostream & operator << (std::ostream &os, const Storage &storage)
    {
        return os << storage.GetStorage () << "Bytes";
    }
    
    

} // namespace ns3


