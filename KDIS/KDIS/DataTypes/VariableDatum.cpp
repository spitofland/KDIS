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

#include "./VariableDatum.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace KDIS;
using namespace DATA_TYPE;
using namespace ENUMS;
using namespace UTILS;

//////////////////////////////////////////////////////////////////////////
// Public:
//////////////////////////////////////////////////////////////////////////

VariableDatum::VariableDatum() :
    m_ui32DatumLength( 0 ),
    m_ui32DatumID( 0 )
{
}

//////////////////////////////////////////////////////////////////////////

VariableDatum::VariableDatum( DatumID ID, const KString & Value )
{
    m_ui32DatumID = ID;

    SetDatumValue( Value );
}

//////////////////////////////////////////////////////////////////////////

VariableDatum::VariableDatum( DatumID ID, const KOCTET* data, KUINT32 sizeInBits )
{
    m_ui32DatumID = ID;

    SetDatumValue( data, sizeInBits );
}

//////////////////////////////////////////////////////////////////////////

VariableDatum::VariableDatum( KDataStream & stream ) throw( KException )
{
    Decode( stream );
}

//////////////////////////////////////////////////////////////////////////

VariableDatum::~VariableDatum()
{
}

//////////////////////////////////////////////////////////////////////////

void VariableDatum::SetDatumID( DatumID ID )
{
    m_ui32DatumID = ID;
}

//////////////////////////////////////////////////////////////////////////

DatumID VariableDatum::GetDatumID() const
{
    return ( DatumID )m_ui32DatumID;
}

//////////////////////////////////////////////////////////////////////////

KUINT32 VariableDatum::GetDatumLength() const
{
    return m_ui32DatumLength;
}

//////////////////////////////////////////////////////////////////////////

KUINT32 VariableDatum::GetDatumLengthInOctets() const
{
    return static_cast<KUINT32>( ceil( m_ui32DatumLength / 8.0 ) );
}


//////////////////////////////////////////////////////////////////////////

KUINT32 VariableDatum::GetPDULength() const
{
    return VARIABLE_DATUM_SIZE + m_v8DatumValue.size();
}

//////////////////////////////////////////////////////////////////////////

void VariableDatum::SetDatumValue( const KString & s )
{
    SetDatumValue( s.c_str(), ( s.length() + 1 ) * 8 ); // +1 to allow for terminating NULL ...
}

//////////////////////////////////////////////////////////////////////////

void VariableDatum::SetDatumValue( const KOCTET * data, KUINT32 sizeInBits )
{
    KUINT32 sizeInOctets = (KUINT32)ceil( sizeInBits / 8.0 );
    // Make sure it's padded to an 8-byte boundary. 
	KUINT32 arraySize = (KUINT32)ceil(sizeInBits / 64.0) * 8;

    m_v8DatumValue.resize( arraySize );
    
    for( KUINT32 i = 0; i < sizeInOctets; ++i )
    {
        m_v8DatumValue[i] = data[i];
    }

    // Don't forget to clear the padding 
    for( size_t i = sizeInOctets; i < m_v8DatumValue.size(); ++i )
    {
        m_v8DatumValue[i] = 0;
    }

    m_ui32DatumLength = sizeInBits;
}

//////////////////////////////////////////////////////////////////////////

KUINT32 VariableDatum::GetDatumValueCopyIntoBuffer( KOCTET * Buffer, KUINT16 BufferSize ) const throw( KException )
{
    KUINT32 sizeInOctets = GetDatumLengthInOctets( );

    if( BufferSize < sizeInOctets ) throw KException( __FUNCTION__, BUFFER_TOO_SMALL );

    memcpy( Buffer, &m_v8DatumValue[0], sizeInOctets );

    return sizeInOctets;
}

//////////////////////////////////////////////////////////////////////////

KString VariableDatum::GetDatumValueAsKString() const
{
    KString str;

    KUINT32 ui32LengthInOctets = GetDatumLengthInOctets( );
    if( ui32LengthInOctets > 0 )
    {
        str.assign( m_v8DatumValue.begin(), m_v8DatumValue.end( ) );
    }

    return str;
}

//////////////////////////////////////////////////////////////////////////

vector<KUINT64> VariableDatum::GetDatumValueAsKUINT64() const
{
    KBOOL bSwapBytes = !IsMachineBigEndian();

    KUINT32 ui32LengthInOctets = GetDatumLengthInOctets( );

    vector<KUINT64> m_Return;
    m_Return.resize( ceil( ui32LengthInOctets / 8.0 ) );

    for( size_t i = 0, j = 0; i < ui32LengthInOctets; i += 8, ++j )
    {
        m_Return[j] = NetToKUINT64( &m_v8DatumValue[i], bSwapBytes ).m_Value;
    }

    return m_Return;
}

//////////////////////////////////////////////////////////////////////////

vector<KFLOAT64> VariableDatum::GetDatumValueAsKFLOAT64() const
{
    KBOOL bSwapBytes = !IsMachineBigEndian();

    KUINT32 ui32LengthInOctets = GetDatumLengthInOctets( );

    vector<KFLOAT64> m_Return;
    m_Return.resize( ceil( ui32LengthInOctets / 8.0 ) );

    for( size_t i = 0, j = 0; i < ui32LengthInOctets; i += 8, ++j )
    {
        m_Return[j] = NetToKUINT64( &m_v8DatumValue[i], bSwapBytes ).m_Value;
    }
    return m_Return;
}

//////////////////////////////////////////////////////////////////////////

void VariableDatum::ClearDatumValue()
{
    m_v8DatumValue.clear();
    m_ui32DatumLength = 0;
}

//////////////////////////////////////////////////////////////////////////

KString VariableDatum::GetAsString() const
{
    KStringStream ss;

    // For now we return the datum value as a string.
    ss << "Variable Datum:"
       << "\n\tID:          " << GetEnumAsStringDatumID( m_ui32DatumID )
       << "\n\tLength:      " << m_ui32DatumLength
       << "\n\tValue Hex: "
       << "\n"              << HexDump( &m_v8DatumValue[0], m_v8DatumValue.size( ) )
       << "\n\tValue Ascii: "
       << "\n"              << AsciiDump( &m_v8DatumValue[0], m_v8DatumValue.size( ) )
       << "\n";

    return ss.str();
}

//////////////////////////////////////////////////////////////////////////

void VariableDatum::Decode( KDataStream & stream ) throw( KException )
{
    if( stream.GetBufferSize() < VARIABLE_DATUM_SIZE )throw KException( __FUNCTION__, NOT_ENOUGH_DATA_IN_BUFFER );

    m_v8DatumValue.clear();

    stream >> m_ui32DatumID
           >> m_ui32DatumLength;

    if( m_ui32DatumLength > 0 )
    {
        // Datum length is returned in bits, so we need to convert to octets
        KUINT32 sizeInOctets = (KUINT32)ceil( m_ui32DatumLength / 8.0 );
        KUINT32 arraySize = (KUINT32)ceil(m_ui32DatumLength / 64.0) * 8;

        m_v8DatumValue.resize( arraySize );

        for( size_t i = 0; i < m_v8DatumValue.size( ); ++i )
        {
            stream >> m_v8DatumValue[i];
        }
    }
}

//////////////////////////////////////////////////////////////////////////

KDataStream VariableDatum::Encode() const
{
    KDataStream stream;

    VariableDatum::Encode( stream );

    return stream;
}

//////////////////////////////////////////////////////////////////////////

void VariableDatum::Encode( KDataStream & stream ) const
{
    stream << m_ui32DatumID
           << m_ui32DatumLength;

    if( m_ui32DatumLength > 0 )
    {
        for( size_t i = 0; i < m_v8DatumValue.size(); ++i )
        {
            stream << m_v8DatumValue[i];
        }
    }
}

//////////////////////////////////////////////////////////////////////////

KBOOL VariableDatum::operator == ( const VariableDatum & Value ) const
{
    if( m_ui32DatumID     != Value.m_ui32DatumID )     return false;
    if( m_ui32DatumLength != Value.m_ui32DatumLength ) return false;

    return m_v8DatumValue == Value.m_v8DatumValue;
}

//////////////////////////////////////////////////////////////////////////

KBOOL VariableDatum::operator != ( const VariableDatum & Value ) const
{
    return !( *this == Value );
}

//////////////////////////////////////////////////////////////////////////

