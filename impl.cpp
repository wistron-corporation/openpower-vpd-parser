#include <exception>
#include <iostream>
#include <iterator>
#include "impl.hpp"

namespace openpower
{
namespace vpd
{
namespace parser
{

namespace
{

using RecordId = uint8_t;
using RecordOffset = uint16_t;
using RecordSize = uint16_t;
using RecordType = uint16_t;
using RecordLength = uint16_t;
using KwSize = uint8_t;
using ECCOffset = uint16_t;
using ECCLength = uint16_t;

}

namespace offsets
{

enum Offsets
{
    VHDR = 17,
    VHDR_TOC_ENTRY = 29,
    VTOC_PTR = 35,
};

}

namespace lengths
{

enum Lengths
{
    RECORD_NAME = 4,
    KW_NAME = 2,
    RECORD_MIN = 44,
};

}

void Impl::checkHeader() const
{
    if (vpd.empty() || (lengths::RECORD_MIN > vpd.size()))
    {
        throw std::runtime_error("Malformed VPD");
    }
    else
    {
        auto iterator = vpd.cbegin();
        std::advance(iterator, offsets::VHDR);
        auto stop = std::next(iterator, lengths::RECORD_NAME);
        std::string record(iterator, stop);
        if ("VHDR" != record)
        {
            throw std::runtime_error("VHDR record not found");
        }
    }
}

internal::OffsetList Impl::readTOC() const
{
    internal::OffsetList offsets {};

    // The offset to VTOC could be 1 or 2 bytes long
    RecordOffset vtocOffset = vpd.at(offsets::VTOC_PTR);
    RecordOffset highByte = vpd.at(offsets::VTOC_PTR + 1);
    vtocOffset |= (highByte << 8);

    // Got the offset to VTOC, skip past record header and keyword header
    // to get to the record name.
    auto iterator = vpd.cbegin();
    std::advance(iterator,
                 vtocOffset +
                 sizeof(RecordId) +
                 sizeof(RecordSize) +
                 // Skip past the RT keyword, which contains
                 // the record name.
                 lengths::KW_NAME +
                 sizeof(KwSize));

    auto stop = std::next(iterator, lengths::RECORD_NAME);
    std::string record(iterator, stop);
    if ("VTOC" != record)
    {
        throw std::runtime_error("VTOC record not found");
    }

    // VTOC record name is good, now read through the TOC, stored in the PT
    // PT keyword; vpdBuffer is now pointing at the first character of the
    // name 'VTOC', jump to PT data.
    // Skip past record name and KW name, 'PT'
    std::advance(iterator, lengths::RECORD_NAME + lengths::KW_NAME);
    // Note size of PT
    std::size_t ptLen = *iterator;
    // Skip past PT size
    std::advance(iterator, sizeof(KwSize));

    // vpdBuffer is now pointing to PT data
    return readPT(iterator, ptLen);
}

internal::OffsetList Impl::readPT(Binary::const_iterator iterator,
                                  std::size_t ptLength) const
{
    internal::OffsetList offsets{};

    auto end = iterator;
    std::advance(end, ptLength);

    // Look at each entry in the PT keyword. In the entry,
    // we care only about the record offset information.
    while (iterator < end)
    {
        // Skip record name and record type
        std::advance(iterator, lengths::RECORD_NAME + sizeof(RecordType));

        // Get record offset
        RecordOffset offset = *iterator;
        RecordOffset highByte = *(iterator + 1);
        offset |= (highByte << 8);
        offsets.push_back(offset);

        // Jump record size, record length, ECC offset and ECC length
        std::advance(iterator,
                     sizeof(RecordSize) +
                     sizeof(RecordLength) +
                     sizeof(ECCOffset) +
                     sizeof(ECCLength));
    }

    return offsets;
}

} // namespace parser
} // namespace vpd
} // namespace openpower