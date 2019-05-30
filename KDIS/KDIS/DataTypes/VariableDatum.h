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

/********************************************************************
    class:      VariableDatum
    created:    18/09/2008
    author:     Karl Jones

    purpose:    Stores variable data types and their values. Such as strings.
                Get as string does not know what the data type is so for now assumes
                each byte is a ASCII char.
                
                Note: See FactoryDecoder for a guide to adding support for using your own VariableDatums. 

    size:       64 bits / 8 octets - Min size
*********************************************************************/

#pragma once

#include "./DataTypeBase.h"
#include "./FactoryDecoder.h"
#include "./../Extras/KRef_Ptr.h"

namespace KDIS {
namespace DATA_TYPE {

/************************************************************************/
// Define the type of pointer we are using for VariableDatum Records,
// do we want a weak reference or a ref counter?
// By default we use a ref pointer, however if you want to use a standard
// pointer or one of your own then simply change it below.
/************************************************************************/
class VariableDatum;
#ifdef KDIS_CPP11
typedef std::shared_ptr<VariableDatum> VarDtmPtr; // Ref counter
#else
typedef KDIS::UTILS::KRef_Ptr<VariableDatum> VarDtmPtr; // Ref counter
#endif
//typedef VariableDatum* VarDtmPtr; // Weak ref

class KDIS_EXPORT VariableDatum : public DataTypeBase, public FactoryDecoderUser<VariableDatum>
{
protected:

    KUINT32 m_ui32DatumID;

    KUINT32 m_ui32DatumLength;

    // Holds 64 bits, not all bits may belong to the value as padding is also added.
    std::vector<KOCTET> m_v8DatumValue;

public:

    static const KUINT16 VARIABLE_DATUM_SIZE = 8; // Min Size

    VariableDatum();

    VariableDatum( KDIS::DATA_TYPE::ENUMS::DatumID ID, const KString & Value );

    VariableDatum( KDIS::DATA_TYPE::ENUMS::DatumID ID, const KOCTET * data, KUINT32 sizeInBits );

    VariableDatum( KDataStream & stream ) throw( KException );

    virtual ~VariableDatum();

    //************************************
    // FullName:    KDIS::DATA_TYPE::VariableDatum::SetDatumID
    //              KDIS::DATA_TYPE::VariableDatum::GetDatumID
    // Description: Set the datum id, indicates what the datum value
    //              is for and what format it should be in.
    // Parameter:   DatumID ID
    //************************************
    virtual void SetDatumID( KDIS::DATA_TYPE::ENUMS::DatumID ID );
    virtual KDIS::DATA_TYPE::ENUMS::DatumID GetDatumID() const;

    //************************************
    // FullName:    KDIS::DATA_TYPE::VariableDatum::GetDatumLength
    // Description: Returns length of datum VALUE in bits.
    //              Note: Does not include the datum id or length field.
    //************************************
    virtual KUINT32 GetDatumLength() const;

    //************************************
    // FullName:    KDIS::DATA_TYPE::VariableDatum::GetDatumLengthInOctets
    // Description: Returns length of datum VALUE in octets.
    //              Note: Does not include the datum id or length field.
    //************************************
    virtual KUINT32 GetDatumLengthInOctets() const;

    //************************************
    // FullName:    KDIS::DATA_TYPE::VariableDatum::GetDatumLength
    // Description: Returns length of Datum in octets that it will
    //              occupy when put into a PDU.
    //************************************
    virtual KUINT32 GetPDULength() const;

    //************************************
    // FullName:    KDIS::DATA_TYPE::VariableDatum::GetDatumValueCopyIntoBuffer
    //              KDIS::DATA_TYPE::VariableDatum::GetDatumValueAsKString
    //              KDIS::DATA_TYPE::VariableDatum::GetDatumValueAsKUINT64
    //              KDIS::DATA_TYPE::VariableDatum::GetDatumValueAsKFLOAT64
    //              KDIS::DATA_TYPE::VariableDatum::ClearDatumValue
    // Description: Copy datum value into a buffer or
    //              return as a privative data type.
    //              primitives are returned as vectors,
    //              If the datum length is not a multiple of
    //              8 then the last octets are ignored.
    // Parameter:   KOCTET * Buffer
    // Parameter:   KUINT16 BufferSize
    //************************************
    virtual KUINT32 GetDatumValueCopyIntoBuffer( KOCTET * Buffer, KUINT16 BufferSize ) const throw( KException );
    virtual KString GetDatumValueAsKString() const;
    virtual const KOCTET* GetDatumBuffer( ) const { return &m_v8DatumValue[0]; }
    virtual std::vector<KUINT64> GetDatumValueAsKUINT64() const;
    virtual std::vector<KFLOAT64> GetDatumValueAsKFLOAT64() const;
    virtual void SetDatumValue( const KString & s );
    virtual void ClearDatumValue();

    //************************************
    // FullName:    KDIS::DATA_TYPE::VariableDatum<Type>::SetDatumValue
    //              KDIS::DATA_TYPE::VariableDatum<Type>::GetDatumValue
    // Description: Returns datum value in required format.
    // Parameter:   Type val
    //************************************
    template<class Type>
    void SetDatumValue( Type val ) throw( KException );
    template<class Type>
    Type GetDatumValue() const throw( KException );

    //************************************
    // FullName:    KDIS::DATA_TYPE::VariableDatum::SetDatumValue
    // Description: Set value from a byte array ... note that length is in bits.
    //************************************
    virtual void SetDatumValue( const KOCTET * data, KUINT32 sizeInBits );

    //************************************
    // FullName:    KDIS::DATA_TYPE::VariableDatum::GetAsString
    // Description: Returns a string representation.
    //************************************
    virtual KString GetAsString() const;

    //************************************
    // FullName:    KDIS::DATA_TYPE::VariableDatum::Decode
    // Description: Convert From Network Data.
    // Parameter:   KDataStream & stream
    //************************************
    virtual void Decode( KDataStream & stream ) throw( KException );

    //************************************
    // FullName:    KDIS::DATA_TYPE::VariableDatum::Encode
    // Description: Convert To Network Data.
    // Parameter:   KDataStream & stream
    //************************************
    virtual KDataStream Encode() const;
    virtual void Encode( KDataStream & stream ) const;

    KBOOL operator == ( const VariableDatum & Value ) const;
    KBOOL operator != ( const VariableDatum & Value ) const;
};
//////////////////////////////////////////////////////////////////////////

template<class Type>
Type VariableDatum::GetDatumValue() const throw( KException )
{
    if( sizeof( Type ) < GetDatumLengthInOctets( )  ) throw KException( __FUNCTION__, DATA_TYPE_TOO_LARGE );

    NetToDataType<Type> NetValue( &m_v8DatumValue[0], false );

    // Do we need to convert the data back to machine endian?
    if( IsMachineBigEndian() == false )
    {
        // Need to convert
        NetValue.SwapBytes();
    }

    return NetValue.m_Value;
}

//////////////////////////////////////////////////////////////////////////

template<class Type>
void VariableDatum::SetDatumValue( Type val ) throw( KException )
{
    // Datum length is returned in bits, so we need to convert to octets
    KUINT32 sizeInOctets = ceil( m_ui32DatumLength / 8.0 );
    KUINT32 arraySize = sizeInOctets + sizeInOctets % 8;

    m_v8DatumValue.resize( arraySize );

    // Now convert the data into big endian, we want to store the value like this
    // as when we encode/decode we have no way to know what the data type is or how many
    // octets long it is. We only convert to machine endian when the data is requested.
    NetToDataType<Type> NetValue( val, !IsMachineBigEndian( ) );

    // Copy into datum value.
    for( size_t i = 0; i < sizeof( Type ); ++i )
    {
        m_cDatumValue[i] = NetValue.m_Octs[i];
    }

    // Don't forget to clear the padding.
    for( size_t i = sizeof( Type ); i < arraySize; ++i )
    {
        m_cDatumValue[i] = 0;
    }
}

} // END namespace DATA_TYPES
} // END namespace KDIS
