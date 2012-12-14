/*********************************************************************
KDIS is free software; you can redistribute it and/or modify it under the
terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option)
any later version.

KDIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this library; if not, write to the Free Software Foundation, Inc.,
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.

For Further Information Please Contact me at
Karljj1@yahoo.com
http://p.sf.net/kdis/UserGuide
*********************************************************************/

#include "./Resupply_Received_PDU.h"

//////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace KDIS;
using namespace PDU;
using namespace DATA_TYPE;
using namespace ENUMS;
using namespace UTILS;

//////////////////////////////////////////////////////////////////////////
// public:
//////////////////////////////////////////////////////////////////////////

Resupply_Received_PDU::Resupply_Received_PDU( void ) :
    m_ui16Padding1( 0 ),
    m_ui8Padding2( 0 ),
    m_ui8NumSupplyTypes( 0 )
{
    m_ui8PDUType = Resupply_Received_PDU_Type;
    m_ui16PDULength = RESUPPLY_RECEIVED_PDU_SIZE;
}

//////////////////////////////////////////////////////////////////////////

Resupply_Received_PDU::Resupply_Received_PDU( KDataStream & stream ) throw( KException )
{
    Decode( stream );
}

//////////////////////////////////////////////////////////////////////////

Resupply_Received_PDU::Resupply_Received_PDU( const EntityIdentifier & ReceivingEntity, const EntityIdentifier & SupplyingEntity )  :
    Logistics_Header( ReceivingEntity, SupplyingEntity ),
    m_ui16Padding1( 0 ),
    m_ui8Padding2( 0 ),
    m_ui8NumSupplyTypes( 0 )
{
    m_ui8PDUType = Resupply_Received_PDU_Type;
    m_ui16PDULength = RESUPPLY_RECEIVED_PDU_SIZE;
}

//////////////////////////////////////////////////////////////////////////

Resupply_Received_PDU::~Resupply_Received_PDU( void )
{
    m_vSupplies.clear();
}

//////////////////////////////////////////////////////////////////////////

void Resupply_Received_PDU::SetNumberOfSupplyTypes( KUINT8  UI )
{
    m_ui8NumSupplyTypes = UI;
    m_ui16PDULength = ( RESUPPLY_RECEIVED_PDU_SIZE + ( m_ui8NumSupplyTypes * Supplies::SUPPLIES_SIZE ) );
}

//////////////////////////////////////////////////////////////////////////

KUINT8 Resupply_Received_PDU::GetNumberOfSupplyTypes() const
{
    return m_ui8NumSupplyTypes;
}

//////////////////////////////////////////////////////////////////////////

void Resupply_Received_PDU::AddSupply( const Supplies & S )
{
    m_vSupplies.push_back( S );

    // Update the number of supplies
    m_ui8NumSupplyTypes = m_vSupplies.size();

    // Update the PDU size
    m_ui16PDULength = ( RESUPPLY_RECEIVED_PDU_SIZE + ( m_ui8NumSupplyTypes * Supplies::SUPPLIES_SIZE ) );
}

//////////////////////////////////////////////////////////////////////////

void Resupply_Received_PDU::SetSupplies( const vector<Supplies> & S )
{
    m_vSupplies = S;

    // Update the number of supplies
    m_ui8NumSupplyTypes = m_vSupplies.size();

    // Update the PDU size
    m_ui16PDULength = ( RESUPPLY_RECEIVED_PDU_SIZE + ( m_ui8NumSupplyTypes * Supplies::SUPPLIES_SIZE ) );
}

//////////////////////////////////////////////////////////////////////////

vector<Supplies> Resupply_Received_PDU::GetSupplies() const
{
    return m_vSupplies;
}

//////////////////////////////////////////////////////////////////////////

KString Resupply_Received_PDU::GetAsString() const
{
    KStringStream ss;

    ss << Header::GetAsString()
       << "-Resupply Received PDU-\n"
       << IndentString( Logistics_Header::GetAsString(), 1 )
       << "\tNumber Of Supply Types: " << ( KUINT16 )m_ui8NumSupplyTypes;

    // Now add supplies
    vector<Supplies>::const_iterator citr = m_vSupplies.begin();
    vector<Supplies>::const_iterator citrEnd = m_vSupplies.end();

    // Add supplies to the stream
    for( ; citr != citrEnd; ++citr )
    {
        ss << IndentString( citr->GetAsString(), 1 )
           << "\tQuantity :	" << citr->GetQuantity();
    }

    return ss.str();
}

//////////////////////////////////////////////////////////////////////////

void Resupply_Received_PDU::Decode( KDataStream & stream ) throw( KException )
{
    if( stream.GetBufferSize() < RESUPPLY_RECEIVED_PDU_SIZE )throw KException( __FUNCTION__, NOT_ENOUGH_DATA_IN_BUFFER );

    Logistics_Header::Decode( stream );

    stream >> m_ui8NumSupplyTypes;

    // Now recheck the size of the packet as we now know the number of supply types.
    if( stream.GetBufferSize() < RESUPPLY_RECEIVED_PDU_SIZE + ( m_ui8NumSupplyTypes * Supplies::SUPPLIES_SIZE ) )throw KException( __FUNCTION__, RESUPPLY_RECEIVED_PDU_SIZE );

    stream >> m_ui16Padding1
           >> m_ui8Padding2;

    for( KUINT16 i = 0; i < m_ui8NumSupplyTypes; ++i )
    {
        Supplies sup;
        stream >> KDIS_STREAM sup;
        m_vSupplies.push_back( sup );
    }
}

//////////////////////////////////////////////////////////////////////////

KDataStream Resupply_Received_PDU::Encode() const
{
    KDataStream stream;

    Resupply_Received_PDU::Encode( stream );

    return stream;
}

//////////////////////////////////////////////////////////////////////////

void Resupply_Received_PDU::Encode( KDataStream & stream ) const
{
    Logistics_Header::Encode( stream );
    stream << m_ui8NumSupplyTypes
           << m_ui16Padding1
           << m_ui8Padding2;

    vector<Supplies>::const_iterator citr = m_vSupplies.begin();
    vector<Supplies>::const_iterator citrEnd = m_vSupplies.end();

    // Add supplies to the stream
    for( ; citr != citrEnd; ++citr )
    {
        citr->Encode( stream );
    }
}

//////////////////////////////////////////////////////////////////////////

KBOOL Resupply_Received_PDU::operator == ( const Resupply_Received_PDU & Value ) const
{
    if( Logistics_Header::operator  !=( Value ) )                  return false;
    if( m_ui8NumSupplyTypes         != Value.m_ui8NumSupplyTypes ) return false;
    if( m_vSupplies                 != Value.m_vSupplies )         return false;
    return true;
}

//////////////////////////////////////////////////////////////////////////

KBOOL Resupply_Received_PDU::operator != ( const Resupply_Received_PDU & Value ) const
{
    return !( *this == Value );
}

//////////////////////////////////////////////////////////////////////////