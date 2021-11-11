/* 
 * File:   application-size-util.cc
 * Author: ubaid <u.ur.rahman@gmail.com>
 * 
 * Created on July 16, 2015, 10:50 PM
 */

#include "application-size-util.h"

#include "ns3/nstime.h"
#include "ns3/fatal-error.h"
#include "ns3/log.h"

#include <math.h>

namespace ns3 {

    NS_LOG_COMPONENT_DEFINE("ApplicationSize");
    
    ATTRIBUTE_HELPER_CPP ( ApplicationSize );
    
    /*static*/
    bool
    ApplicationSize::DoParse (const std::string s, uint64_t* v, bool* inst, bool* flop)
    {
        NS_LOG_FUNCTION (s << v << inst << flop);
        std::string::size_type n = s.find_first_not_of ("0123456789.");
        if(n != std::string::npos) {
            std::istringstream iss;
            iss.str(s.substr (0, n));
            uint64_t r;
            iss >> r;
            std::string trailer = s.substr (n, std::string::npos);
            if ( trailer == "flop" )
            {
                *flop = true;
                *inst = false;
                *v = (uint64_t) r;
            }
            else if (trailer == "mflop" || trailer == "Mflop" || trailer == "MFLOP" || trailer == "MFlop")
            {
                *flop = true;
                *inst = false;
                *v = (uint64_t) (r * pow(10, 6));
            }
            else if (trailer == "gflop" || trailer == "Gflop" || trailer == "GFLOP" || trailer == "GFlop")
            {
                *flop = true;
                *inst = false;
                uint64_t p = pow(10, 9);
                *v = (uint64_t) (r * p);

            }
            else if (trailer == "tflop" || trailer == "Tflop" || trailer == "TFLOP" || trailer == "TFlop")
            {
                *flop = true;
                *inst = false;
                *v = (uint64_t) (r * pow(10, 12));
            }
            else if (trailer == "mi" || trailer == "MI")
            {
                *flop = false;
                *inst = true;
                *v = (uint64_t) (r * pow(10, 6));
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

    ApplicationSize::ApplicationSize()
    {
    	NS_LOG_FUNCTION(this);
    	m_instOpCount = 0;
		m_instructions = false;
		m_flop = true;
    }

    ApplicationSize::ApplicationSize ( uint64_t instOpCount, std::string metric )
        : m_instOpCount(instOpCount)
    {
        if(metric == "flop") 
        {
            m_flop = true;
            m_instructions = false;
        }
        else if (metric == "ic")
        {
            m_flop = false;
            m_instructions = true;
        }
    }
    ApplicationSize::ApplicationSize ( uint64_t instOpCount, bool instCount, bool flop )
    {
        NS_LOG_FUNCTION (this);
        m_instOpCount = instOpCount;
        m_instructions = instCount;
        m_flop = flop;
    }

    bool
    ApplicationSize::operator < (ApplicationSize& rhs) const
    {
        
        if((m_flop && rhs.m_flop) || 
           (m_instructions && rhs.m_instructions))
        {
            return (m_instOpCount < rhs.m_instOpCount);
        }
        else
        {
            NS_FATAL_ERROR ("Objects are not of same metric" << this << rhs);
            return false;
        }
    }
    
    bool
    ApplicationSize::operator > (ApplicationSize& rhs) const
    {
        if((m_flop && rhs.m_flop) || 
           (m_instructions && rhs.m_instructions))
        {
            return (m_instOpCount > rhs.m_instOpCount);
        }
        else
        {
            NS_FATAL_ERROR ("Objects are not of same metric" << this << rhs);
            return false;
        }
    }
    
    bool
    ApplicationSize::operator <= (ApplicationSize& rhs) const
    {
        if((m_flop && rhs.m_flop) || 
           (m_instructions && rhs.m_instructions))
        {
            return (m_instOpCount <= rhs.m_instOpCount);
        }
        else
        {
            NS_FATAL_ERROR ("Objects are not of same metric" << this << rhs);
            return false;
        }
    }
    
    bool
    ApplicationSize::operator == (ApplicationSize& rhs) const
    {
        if((m_flop && rhs.m_flop) || 
           (m_instructions && rhs.m_instructions))
        {
            return (m_instOpCount == rhs.m_instOpCount);
        }
        else
        {
            NS_FATAL_ERROR ("Objects are not of same metric" << this << rhs);
            return false;
        }
    }
    
    bool
    ApplicationSize::operator >= (ApplicationSize& rhs) const
    {
        if((m_flop && rhs.m_flop) || 
           (m_instructions && rhs.m_instructions))
        {
            return (m_instOpCount >= rhs.m_instOpCount);
        }
        else
        {
            NS_FATAL_ERROR ("Objects are not of same metric" << this << rhs);
            return false;
        }
    }
    
    bool
    ApplicationSize::operator != (ApplicationSize& rhs) const
    {
        if((m_flop && rhs.m_flop) || 
           (m_instructions && rhs.m_instructions))
        {
            return (m_instOpCount != rhs.m_instOpCount);
        }
        else
        {
            NS_FATAL_ERROR ("Objects are not of same metric" << this << rhs);
            return false;
        }
    }
    
    
    ApplicationSize::ApplicationSize ( std::string appSize )
    {
        NS_LOG_FUNCTION (this << appSize);
        bool ok = DoParse (appSize, &m_instOpCount, &m_instructions, &m_flop);
        if(!ok) 
        {
            NS_FATAL_ERROR ("Could not parse application size" << appSize);
        }

    }
    
    uint64_t
    ApplicationSize::GetApplicationSize () const
    {

        return m_instOpCount;
    }
    
    bool
    ApplicationSize::IsFlop () const
    {
        return m_flop;
    }
    
    bool
    ApplicationSize::IsInstruction () const
    {
        return m_instructions;
    }
    

    /* For printing */
    
    std::istream &operator >> (std::istream &is, ApplicationSize &appSize)
    {
        std::string value;
        is >> value;
        uint64_t v;
        bool ic, f;
        bool ok = ApplicationSize::DoParse (value, &v, &ic, &f);
        if( !ok )
        {
            is.setstate (std::ios_base::failbit);
        }
        appSize = ApplicationSize(v, ic, f);
        return is;
    }
    
    std::ostream &operator << (std::ostream &os, const ApplicationSize &appSize)
    {
        std::string outMetric;
        if(appSize.IsFlop ())
        {
            outMetric = "flop";
        }
        else
        {
            outMetric = "instructions";
        }
        os << appSize.GetApplicationSize () << outMetric;
        return os;
        
    }


}
