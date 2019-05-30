/*********************************************************************
Copyright 2013 Karl Jones
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

For Further Information Please Contact me at
Karljj1@yahoo.com
http://p.sf.net/kdis/UserGuide
*********************************************************************/

#include "./Comment_PDU.h"

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

Comment_PDU::Comment_PDU() :
    m_ui32NumFixedDatum( 0 ),
    m_ui32NumVariableDatum( 0 )
{
    m_ui8PDUType = Message_PDU_Type;
    m_ui16PDULength = COMMENT_PDU_SIZE;
}

//////////////////////////////////////////////////////////////////////////

Comment_PDU::Comment_PDU( const Header & H ) :
    Simulation_Management_Header( H ),
    m_ui32NumFixedDatum( 0 ),
    m_ui32NumVariableDatum( 0 )
{
}

//////////////////////////////////////////////////////////////////////////

Comment_PDU::Comment_PDU( KDataStream & stream ) throw( KException )
{
    Decode( stream, false );
}

//////////////////////////////////////////////////////////////////////////

Comment_PDU::Comment_PDU( const Header & H, KDataStream & stream ) throw( KException ) :
    Simulation_Management_Header( H )
{
    Decode( stream, true );
}

//////////////////////////////////////////////////////////////////////////

Comment_PDU::Comment_PDU( const EntityIdentifier & OriginatingEntityID, const EntityIdentifier & ReceivingEntityID ) :
    Simulation_Management_Header( OriginatingEntityID, ReceivingEntityID ),
    m_ui32NumFixedDatum( 0 ),
    m_ui32NumVariableDatum( 0 )
{
    m_ui8PDUType = Message_PDU_Type;
    m_ui16PDULength = COMMENT_PDU_SIZE;
}

//////////////////////////////////////////////////////////////////////////

Comment_PDU::~Comment_PDU()
{
}

//////////////////////////////////////////////////////////////////////////

KUINT32 Comment_PDU::GetNumberFixedDatum() const
{
    return m_ui32NumFixedDatum;
}

//////////////////////////////////////////////////////////////////////////

KUINT32 Comment_PDU::GetNumberVariableDatum() const
{
    return m_ui32NumVariableDatum;
}

//////////////////////////////////////////////////////////////////////////

void Comment_PDU::AddFixedDatum( FixDtmPtr FD )
{
    m_vFixedDatum.push_back( FD );

    ++m_ui32NumFixedDatum;

    m_ui16PDULength += FixedDatum::FIXED_DATUM_SIZE;
}

//////////////////////////////////////////////////////////////////////////

void Comment_PDU::SetFixedDatum( const fixed_datum_ctor & FD )
{
    // Subtract old length
    m_ui16PDULength -= m_ui32NumFixedDatum * FixedDatum::FIXED_DATUM_SIZE;

    m_vFixedDatum = FD;
    m_ui32NumFixedDatum = m_vFixedDatum.size();

    // Calculate new length
    m_ui16PDULength += m_ui32NumFixedDatum * FixedDatum::FIXED_DATUM_SIZE;
}

//////////////////////////////////////////////////////////////////////////

const Comment_PDU::fixed_datum_ctor & Comment_PDU::GetFixedDatum() const
{
    return m_vFixedDatum;
}

//////////////////////////////////////////////////////////////////////////

void Comment_PDU::AddVariableDatum( VarDtmPtr VD )
{
    m_vVariableDatum.push_back( VD );

    ++m_ui32NumVariableDatum;

    m_ui16PDULength += VD->GetPDULength();
}

//////////////////////////////////////////////////////////////////////////

void Comment_PDU::SetVariableDatum( const var_datum_ctor & VD )
{
    m_vVariableDatum = VD;
    m_ui32NumVariableDatum = m_vVariableDatum.size();

    // Reset length
    m_ui16PDULength = COMMENT_PDU_SIZE + ( m_ui32NumFixedDatum * FixedDatum::FIXED_DATUM_SIZE );

    // Calculate the new length
    var_datum_ctor::const_iterator citr = m_vVariableDatum.begin();
    var_datum_ctor::const_iterator citrEnd = m_vVariableDatum.end();
    for( ; citr != citrEnd; ++citr )
    {
        m_ui16PDULength += ( *citr )->GetPDULength();
    }
}

//////////////////////////////////////////////////////////////////////////

const Comment_PDU::var_datum_ctor & Comment_PDU::GetVariableDatum() const
{
    return m_vVariableDatum;
}

//////////////////////////////////////////////////////////////////////////

bool KDIS::PDU::Comment_PDU::IsEmpty( )
{
    return !( m_ui32NumFixedDatum + m_ui32NumVariableDatum );
}

//////////////////////////////////////////////////////////////////////////

void KDIS::PDU::Comment_PDU::Clear( )
{
    m_vFixedDatum.clear( );
    m_ui32NumFixedDatum = 0;

    m_vVariableDatum.clear( );
    m_ui32NumVariableDatum = 0;

    m_ui16PDULength = COMMENT_PDU_SIZE;
}

//////////////////////////////////////////////////////////////////////////

KString Comment_PDU::GetAsString() const
{
    KStringStream ss;

    ss << Header::GetAsString()
       << "-Comment PDU-\n"
       << Simulation_Management_Header::GetAsString()
       << "\nNumber Fixed Datum:         " << m_ui32NumFixedDatum
       << "\nNumber Variable Datum:      " << m_ui32NumVariableDatum
       << "\n";

    ss << "Fixed Datum\n";
    fixed_datum_ctor::const_iterator citrFixed = m_vFixedDatum.begin();
    fixed_datum_ctor::const_iterator citrFixedEnd = m_vFixedDatum.end();
    for( ; citrFixed != citrFixedEnd; ++citrFixed )
    {
        ss << IndentString( ( *citrFixed )->GetAsString() );
    }

    ss << "Variable Datum\n";
    var_datum_ctor::const_iterator citrVar = m_vVariableDatum.begin();
    var_datum_ctor::const_iterator citrVarEnd = m_vVariableDatum.end();
    for( ; citrVar != citrVarEnd; ++citrVar )
    {
        ss << IndentString( ( *citrVar )->GetAsString() );
    }

    return ss.str();
}

//////////////////////////////////////////////////////////////////////////

void Comment_PDU::Decode( KDataStream & stream, bool ignoreHeader /*= true*/ ) throw( KException )
{
    if( ( stream.GetBufferSize() + ( ignoreHeader ? Header::HEADER6_PDU_SIZE : 0 ) ) < COMMENT_PDU_SIZE )throw KException( __FUNCTION__, NOT_ENOUGH_DATA_IN_BUFFER );

    Simulation_Management_Header::Decode( stream, ignoreHeader );

    stream >> m_ui32NumFixedDatum
           >> m_ui32NumVariableDatum;

    // FixedDatum
    for( KUINT16 i = 0; i < m_ui32NumFixedDatum; ++i )
    {
        // Save the current write position so we can peek.
		KSIZE_T pos = stream.GetCurrentReadPosition();
        KUINT32 datumID;

        // Extract the datum id then reset the stream.
        stream >> datumID;
        stream.SetCurrentReadPosition( pos );

        // Use the factory decoder.
        FixedDatum * p = FixedDatum::FactoryDecode( datumID, stream );

        // Did we find a custom decoder? if not then use the default.
        if( p )
        {
            m_vFixedDatum.push_back( FixDtmPtr( p ) );
        }
        else
        {
            // Default
            m_vFixedDatum.push_back( KDIS_MAKE_REF( FixedDatum, stream ) );
        }
    }

    // VariableDatum
    for( KUINT16 i = 0; i < m_ui32NumVariableDatum; ++i )
    {
        // Save the current write position so we can peek.
		KSIZE_T pos = stream.GetCurrentReadPosition();
        KUINT32 datumID;

        // Extract the datum id then reset the stream.
        stream >> datumID;
        stream.SetCurrentReadPosition( pos );

        // Use the factory decoder.
        VariableDatum * p = VariableDatum::FactoryDecode( datumID, stream );

        // Did we find a custom decoder? if not then use the default.
        if( p )
        {
            m_vVariableDatum.push_back( VarDtmPtr( p ) );
        }
        else
        {
            // Default
            m_vVariableDatum.push_back( KDIS_MAKE_REF( VariableDatum, stream ) );
        }
    }
}

//////////////////////////////////////////////////////////////////////////

KDataStream Comment_PDU::Encode() const
{
    KDataStream stream;

    Comment_PDU::Encode( stream );

    return stream;
}

//////////////////////////////////////////////////////////////////////////

void Comment_PDU::Encode( KDataStream & stream ) const
{
    Simulation_Management_Header::Encode( stream );
    stream << m_ui32NumFixedDatum
           << m_ui32NumVariableDatum;

    fixed_datum_ctor::const_iterator citrFixed = m_vFixedDatum.begin();
    fixed_datum_ctor::const_iterator citrFixedEnd = m_vFixedDatum.end();
    for( ; citrFixed != citrFixedEnd; ++citrFixed )
    {
        ( *citrFixed )->Encode( stream );
    }

    var_datum_ctor::const_iterator citrVar = m_vVariableDatum.begin();
    var_datum_ctor::const_iterator citrVarEnd = m_vVariableDatum.end();
    for( ; citrVar != citrVarEnd; ++citrVar )
    {
        ( *citrVar )->Encode( stream );
    }
}

//////////////////////////////////////////////////////////////////////////

KBOOL Comment_PDU::operator == ( const Comment_PDU & Value ) const
{
    if( Simulation_Management_Header::operator  !=( Value ) )                     return false;
    if( m_ui32NumFixedDatum                     != Value.m_ui32NumFixedDatum )    return false;
    if( m_ui32NumVariableDatum                  != Value.m_ui32NumVariableDatum ) return false;
    if( m_vFixedDatum                           != Value.m_vFixedDatum )          return false;
    if( m_vVariableDatum                        != Value.m_vVariableDatum )       return false;
    return true;
}

//////////////////////////////////////////////////////////////////////////

KBOOL Comment_PDU::operator != ( const Comment_PDU & Value ) const
{
    return !( *this == Value );
}

//////////////////////////////////////////////////////////////////////////

