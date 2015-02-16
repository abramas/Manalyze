/*
    This file is part of Spike Guard.

    Spike Guard is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Spike Guard is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Spike Guard.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _SECTION_H_
#define _SECTION_H_

#include <stdio.h>
#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>
#include <boost/system/api_config.hpp>
#include <vector>

#include "pe_structs.h"
#include "color.h"
#include "utils.h"

#if defined BOOST_WINDOWS_API
	#ifdef SGPE_EXPORT
		#define DECLSPEC    __declspec(dllexport)
	#else
		#define DECLSPEC    __declspec(dllimport)
	#endif
#else
	#define DECLSPEC
#endif

namespace sg {

typedef boost::shared_ptr<std::string> pString;
typedef boost::shared_ptr<const std::vector<boost::uint8_t> > shared_bytes;

class Section
{

public:
	/**
	 *	@brief	Create a Section object from a raw image_section_header structure.
	 *
	 *	@param	const image_section_header& header The structure on which the section will be based.
	 *	@param	const std::string& path The path to the PE (in order to be able to access the section bytes)
	 *	@param	const std::vector<pString>& coff_string_table An optional COFF string table, in case section
	 *			names are located in it.
	 */
	Section(const image_section_header& header, 
		    const std::string& path, 
			const std::vector<pString>& coff_string_table = std::vector<pString>());

    virtual ~Section() {}

	/**
	 *	@brief	Returns the raw bytes of the section.
	 *
	 *	Note that calling this function for PEs which have giant sections may end up 
	 *	eating a lot of memory.
	 *
	 *	@return	A shared vector containing the raw bytes of the section. If an error occurs, the vector
	 *			will be empty.
	 */
	DECLSPEC shared_bytes get_raw_data() const;

	DECLSPEC pString		 get_name()						const { return pString(new std::string(_name)); }
	DECLSPEC boost::uint32_t get_virtual_size()				const { return _virtual_size; }
	DECLSPEC boost::uint32_t get_virtual_address()			const { return _virtual_address; }
	DECLSPEC boost::uint32_t get_size_of_raw_data()			const { return _size_of_raw_data; }
	DECLSPEC boost::uint32_t get_pointer_to_raw_data()		const { return _pointer_to_raw_data; }
	DECLSPEC boost::uint32_t get_pointer_to_relocations()	const { return _pointer_to_relocations; }
	DECLSPEC boost::uint32_t get_pointer_to_line_numbers()	const { return _pointer_to_line_numbers; }
	DECLSPEC boost::uint32_t get_number_of_relocations()	const { return _number_of_relocations; }
	DECLSPEC boost::uint32_t get_number_of_line_numbers()	const { return _number_of_line_numbers; }
	DECLSPEC boost::uint32_t get_characteristics()			const { return _characteristics; }
	DECLSPEC double			 get_entropy()					const { return utils::shannon_entropy(*get_raw_data()); }

private:

	// Fields that match the PE structure
	std::string		_name;
	boost::uint32_t _virtual_size;
	boost::uint32_t _virtual_address;
	boost::uint32_t _size_of_raw_data;
	boost::uint32_t _pointer_to_raw_data;
	boost::uint32_t _pointer_to_relocations;
	boost::uint32_t _pointer_to_line_numbers;
	boost::uint16_t _number_of_relocations;
	boost::uint16_t _number_of_line_numbers;
	boost::uint32_t _characteristics;

	// Path to the executable file and location inside it:
	std::string		_path;
	boost::uint32_t	_offset;
};

typedef boost::shared_ptr<Section> pSection;

/**
 *	@brief	Checks whether the address belongs to a section.
 *
 *	@param	unsigned int rva The address (RVA) to check.
 *	@param	sg::pSection section The section in which the address may belong.
 *	@param	bool check_raw_size Use the SizeOfRawData instead of VirtualSize to determine the bounds of the section.
 *
 *	@return	Whether the RVA is between the bounds of the section.
 */
bool DECLSPEC is_address_in_section(boost::uint64_t rva, sg::pSection section, bool check_raw_size = false);

/**
 *	@brief	Finds the section containing a given RVA.
 *
 *	@param	unsigned int rva The address whose section we want to identify.
 *	@param	const std::vector<sg::pSection>& section_list A list of all the sections of the PE.
 *
 *	@return	A pointer to the section containing the input address. NULL if no sections match.
 */
sg::pSection DECLSPEC find_section(unsigned int rva, const std::vector<sg::pSection>& section_list);

} // !namespace sg

#endif // !_SECTION_H_