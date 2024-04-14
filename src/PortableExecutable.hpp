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
#include <StdXX.hpp>
using namespace StdXX;

class PortableExecutable
{
	struct PEHeader
	{
		uint16 NumberOfSections;
		uint16 SizeOfOptionalHeader;
	};

	struct SectionHeader
	{
		String Name;
		uint32 sizeOfRawData;
		uint32 PointerToRawData;
	};

public:
	//Constructor
	inline PortableExecutable(SeekableInputStream& inputStream)
	{
		this->Read(inputStream);
	}

	//Properties
	inline const BinaryTreeMap<String, SectionHeader>& SectionMetadata() const
	{
		return this->sectionMetadata;
	}

	inline const DynamicByteBuffer& TrailingData() const
	{
		return this->trailingData;
	}

	//Inline
	inline const FixedSizeBuffer& GetResourcesSection() const
	{
		return this->sectionContents.Get(u8".rsrc");
	}

	inline const FixedSizeBuffer& GetTextSection() const
	{
		return this->sectionContents.Get(u8".text");
	}

	inline const SectionHeader& GetTextSectionMetaData() const
	{
		return this->sectionMetadata.Get(u8".text");
	}

private:
	//State
	PEHeader header;
	BinaryTreeMap<String, SectionHeader> sectionMetadata;
	BinaryTreeMap<String, FixedSizeBuffer> sectionContents;
	DynamicByteBuffer trailingData;

	//Methods
	void Read(SeekableInputStream& inputStream);
	void ReadPEHeader(DataReader& dataReader);
	void ReadOptionalHeader(DataReader& dataReader);
	void ReadSectionHeader(SectionHeader& sectionHeader, DataReader &dataReader);
};