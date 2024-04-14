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
//Namespaces
using namespace StdXX::CommandLine;

int32 Main(const String& programName, const FixedArray<String>& args)
{
	Parser commandLineParser(programName);
	commandLineParser.AddHelpOption();

	PathArgument inputPathArg(u8"input-path", u8"path to the input file");
	commandLineParser.AddPositionalArgument(inputPathArg);

	if(!commandLineParser.Parse(args))
	{
		stdErr << commandLineParser.GetErrorText() << endl;
		return EXIT_FAILURE;
	}

	if(commandLineParser.IsHelpActivated())
	{
		commandLineParser.PrintHelp();
		return EXIT_SUCCESS;
	}

	const MatchResult& matchResult = commandLineParser.ParseResult();
	FileSystem::Path inputPath = inputPathArg.Value(matchResult);
	FileInputStream fileInputStream(inputPath);

	PortableExecutable pe(fileInputStream);

	for(const auto& section : pe.SectionMetadata())
	{
		stdOut << u8"Section " << section.key << u8" with offset: " << section.value.PointerToRawData << u8" and size: " << String::FormatBinaryPrefixed(section.value.sizeOfRawData) << endl;
	}
	if(pe.TrailingData().Size() > 0)
		stdOut << u8"Trailing data exists with size: " << String::FormatBinaryPrefixed(pe.TrailingData().Size()) << endl;

	return EXIT_SUCCESS;
}