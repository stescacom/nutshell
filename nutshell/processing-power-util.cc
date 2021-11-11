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

#include "processing-power-util.h"

#include "ns3/nstime.h"
#include "ns3/fatal-error.h"
#include "ns3/log.h"

#include <math.h>

namespace ns3
{

    NS_LOG_COMPONENT_DEFINE ( "ProcessingPower" );

    ATTRIBUTE_HELPER_CPP ( ProcessingPower );

    /* static*/
    bool
    ProcessingPower::DoParse ( const std::string s, uint64_t* v, bool *mips, bool *flops )
    {
        NS_LOG_FUNCTION (s << v);
        std::string::size_type n = s.find_first_not_of ("0123456789.");
        if(n != std::string::npos) {
            std::istringstream iss;
            iss.str(s.substr (0, n));
            double r;
            iss >> r;
            std::string trailer = s.substr (n, std::string::npos);
            
            if(trailer == "mips" || trailer == "Mips" || trailer == "MIPS")
            {
                *mips = true;
                *flops = false;
                *v = (uint64_t) r;
            }
            else if (trailer == "flops" || trailer == "Flops" || trailer == "FLOPS")
            {
                *mips = false;
                *flops = true;
                *v = (uint64_t) r;
            }
            else if(trailer == "mflops" || trailer == "Mflops" || trailer == "MFLOPS")
            {
                *mips = false;
                *flops = true;
                *v = (uint64_t) (r * pow(10,6));
            }
            else if(trailer == "gflops" || trailer == "Gflops" || trailer == "GFLOPS")
            {
                *mips = false;
                *flops = true;
                *v = (uint64_t) (r * pow(10,9));
            }
            else if (trailer == "tflops" || trailer == "Tflops" || trailer == "TFLOPS")
            {
                *mips = false;
                *flops = true;
                *v = (uint64_t) (r * pow(10,12));
            }
            else
            {
                return false;
            }
            return true;
        }
        std::istringstream iss;
        iss.str(s);
        iss >> *v;
        return true;
    }
    
    ProcessingPower::ProcessingPower(void)
    	: m_processingPower(0), m_mips (false), m_flops(true)
    {

    }

    ProcessingPower::ProcessingPower (uint64_t power, bool mips, bool flops)
    {
    	if(mips != flops)
    	{
    		m_processingPower = power;
    		m_mips = mips;
    		m_flops = flops;
    	}
    	else
    	{
    		m_processingPower = power;
			m_mips = false;
			m_flops = true;
    	}
    }

    ProcessingPower::ProcessingPower ( uint64_t power, const int metric )
    {
        m_processingPower = power;
        if(metric == ProcessingPower::POWER_MIPS)
        {
            m_mips = true;
            m_flops = false;
        }
        else if(metric == ProcessingPower::POWER_FLOPS)
        {
            m_mips = false;
            m_flops = true;
        }
        else
        {
        	NS_FATAL_ERROR("Invalid processing power");
        }
    }
    


    bool ProcessingPower::operator != ( const ProcessingPower& rhs) const
    {
        if((m_mips && rhs.m_mips) || (m_flops && rhs.m_flops))
        {
            return m_processingPower != rhs.m_processingPower;
        }
        else 
        {
            NS_FATAL_ERROR ("Could not compare, object mismatch" << this << rhs);
            return false;
        }
    }
    
    bool ProcessingPower::operator < (const ProcessingPower& rhs) const
    {
        if((m_mips && rhs.m_mips) || (m_flops && rhs.m_flops))
        {
            return m_processingPower < rhs.m_processingPower;
        }
        else 
        {
            NS_FATAL_ERROR ("Could not compare, object mismatch" << this << rhs);
            return false;
        }
    }
    
    bool ProcessingPower::operator <= ( const ProcessingPower& rhs) const
    {
        if((m_mips && rhs.m_mips) || (m_flops && rhs.m_flops))
        {
            return m_processingPower <= rhs.m_processingPower;
        }
        else 
        {
            NS_FATAL_ERROR ("Could not compare, object mismatch" << this << rhs);
            return false;
        }
    }
    bool ProcessingPower::operator == ( const ProcessingPower& rhs) const
    {
        if((m_mips && rhs.m_mips) || (m_flops && rhs.m_flops))
        {
            return m_processingPower == rhs.m_processingPower;
        }
        else 
        {
            NS_FATAL_ERROR ("Could not compare, object mismatch" << this << rhs);
            return false;
        }
    }

    bool ProcessingPower::operator > ( const ProcessingPower& rhs) const
    {
        if((m_mips && rhs.m_mips) || (m_flops && rhs.m_flops))
        {
            return m_processingPower > rhs.m_processingPower;
        }
        else 
        {
            NS_FATAL_ERROR ("Could not compare, object mismatch" << this << rhs);
            return false;
        }
    }
    bool ProcessingPower::operator >= ( const ProcessingPower& rhs) const
    {
        if((m_mips && rhs.m_mips) || (m_flops && rhs.m_flops))
        {
            return m_processingPower >= rhs.m_processingPower;
        }
        else 
        {
            NS_FATAL_ERROR ("Could not compare, object mismatch" << this << rhs);
            return false;
        }
    }
    
    ProcessingPower 
    ProcessingPower::operator + (ProcessingPower& rhs)
    {
        bool m = m_mips && rhs.m_mips;
        bool f = m_flops && rhs.m_flops;
        uint64_t np = m_processingPower + rhs.m_processingPower;
        if(f || m)
        {
            return ProcessingPower (np, m, f);
        }
        else
        {
            NS_FATAL_ERROR ("Addition Processing power miss match " << this << "  " << rhs);
            return *this;
        }
    }
    
    ProcessingPower 
    ProcessingPower::operator - (ProcessingPower& rhs)
    {
        bool m = m_mips && rhs.m_mips;
        bool f = m_flops && rhs.m_flops;
        uint64_t np = m_processingPower - rhs.m_processingPower;
        if(f || m)
        {
        	if(rhs.m_processingPower <= m_processingPower)
        	{
        		return ProcessingPower (np, m, f);
        	}
        	else
        	{
        		return *this;
        	}

        }
        else
        {
            NS_FATAL_ERROR ("Subtraction Processing power miss match " << this << "  " << rhs);
            return *this;
        }
    }

    
    ProcessingPower::ProcessingPower ( std::string power )
    {
        NS_LOG_FUNCTION (this << power);
        bool ok = DoParse (power, &m_processingPower, &m_mips, &m_flops);
        if( !ok )
        {
            NS_FATAL_ERROR ("Could not parse processing power" << power);
        }
    }

    
    double
    ProcessingPower::CalculateExTime ( const uint64_t instructionCount, const int metric ) const
    {
        double ct = instructionCount/m_processingPower;
        if ( (metric == ProcessingPower::METRIC_FLOP ) && m_flops )
        {
            return ct;
        }
        else if ( (metric == ProcessingPower::METRIC_INSTRUCTION_COUNT ) && m_mips )
        {
            return ct;
        }
        else
        {
            NS_FATAL_ERROR ("Metric do not match " << this << "  " << metric);
            return 0;
        }
    }
    uint64_t
    ProcessingPower::GetProcessingPower ( ) const
    {
        return m_processingPower;
    }
    
    bool ProcessingPower::IsMips() const
    {
        return m_mips;
    }
    
    bool ProcessingPower::IsFlops () const
    {
        return m_flops;
    }

    /* For printing of processing power */
    
    std::istream &operator >> (std::istream &is, ProcessingPower &power)
    {
        std::string value;
        is >> value;
        uint64_t v;
        bool m, f;
        bool ok = ProcessingPower::DoParse (value, &v, &m, &f);
        if( !ok )
        {
            is.setstate (std::ios_base::failbit);
        }
        power = ProcessingPower (v, m, f);
        return is;
    }
    
    std::ostream &operator << (std::ostream &os, const ProcessingPower &power)
    {
        std::string metric;
        if(power.IsFlops ())
        {
            metric = "FLOPS";
        }
        else if ( power.IsMips () )
        {
            metric = "IPS";
        }
        os << power.GetProcessingPower () << metric;
        return os;
    }
    
    

} // namespace ns3
