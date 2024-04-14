/*
 * Copyright (c) 2024 Amir Czwink (amir130@hotmail.de)
 *
 * This file is part of exedump.
 *
 * exedump is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * exedump is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with exedump.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PortableExecutable.hpp"

//Private methods
void PortableExecutable::Read(SeekableInputStream& inputStream)
{
	DataReader dataReader(false, inputStream);

	dataReader.Skip(60); //skip most of DOS header
	uint32 offsetToPEHeader = dataReader.ReadUInt32();

	inputStream.SeekTo(offsetToPEHeader);

	uint32 readSignature = dataReader.ReadUInt32();
	ASSERT_EQUALS(FOURCC(u8"PE\0\0"), readSignature); //TODO: implement me

	this->ReadPEHeader(dataReader);
	this->ReadOptionalHeader(dataReader);

	FixedArray<SectionHeader> sectionHeaders(this->header.NumberOfSections);
	for(uint16 i = 0; i < this->header.NumberOfSections; i++)
	{
		this->ReadSectionHeader(sectionHeaders[i], dataReader);
		this->sectionMetadata[sectionHeaders[i].Name] = sectionHeaders[i];
	}

	for(uint16 i = 0; i < this->header.NumberOfSections; i++)
	{
		const auto& sectionHeader = sectionHeaders[i];
		inputStream.SeekTo(sectionHeader.PointerToRawData);

		this->sectionContents.Insert(sectionHeader.Name, Move(FixedSizeBuffer(sectionHeader.sizeOfRawData)));
		auto& buffer = this->sectionContents.Get(sectionHeader.Name);
		dataReader.ReadBytes(buffer.Data(), buffer.Size());
	}

	uint32 lastDataOffset = 0;
	for(const auto& sectionHeader : sectionHeaders)
	{
		uint32 endOffset = sectionHeader.PointerToRawData + sectionHeader.sizeOfRawData;
		if(endOffset > lastDataOffset)
			lastDataOffset = endOffset;
	}

	uint32 trailingBytes = static_cast<uint32>(inputStream.QuerySize() - lastDataOffset);
	if(trailingBytes)
	{
		this->trailingData.Resize(trailingBytes);
		inputStream.SeekTo(lastDataOffset);
		dataReader.ReadBytes(this->trailingData.Data(), trailingBytes);
	}
}

void PortableExecutable::ReadPEHeader(DataReader& dataReader)
{
	dataReader.Skip(2);

	this->header.NumberOfSections = dataReader.ReadUInt16();

	dataReader.Skip(12);

	this->header.SizeOfOptionalHeader = dataReader.ReadUInt16();

	dataReader.Skip(2); //Characteristics
}

void PortableExecutable::ReadOptionalHeader(DataReader& dataReader)
{
	dataReader.Skip(96);
	dataReader.Skip(16*8);
}

void PortableExecutable::ReadSectionHeader(SectionHeader& sectionHeader, DataReader &dataReader)
{
	TextReader textReader(dataReader.InputStream(), TextCodecType::UTF8);

	sectionHeader.Name = textReader.ReadZeroTerminatedStringBySize(8);
	dataReader.Skip(8);
	sectionHeader.sizeOfRawData = dataReader.ReadUInt32();
	sectionHeader.PointerToRawData = dataReader.ReadUInt32();
	dataReader.Skip(16);
}