/*
 * \brief  Internet protocol version 4.
 * \author Stefan Kalkowski
 * \date   2010-08-19
 */

/*
 * Copyright (C) 2010-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#ifndef _IPV4_H_
#define _IPV4_H_

/* Genode */
#include <base/exception.h>
#include <util/string.h>
#include <util/token.h>

#include <util/endian.h>
#include <net/netaddress.h>

namespace Genode { class Output; }

namespace Net
{
	enum { IPV4_ADDR_LEN = 4 };

	class Ipv4_address;

	class Ipv4_address_prefix;

	class Ipv4_packet;
}


struct Net::Ipv4_address : Network_address<IPV4_ADDR_LEN, '.', false>
{
	Ipv4_address(Genode::uint8_t value = 0) : Network_address(value) { }

	Ipv4_address(void *src) : Network_address(src) { }

	bool valid() const { return *this != Ipv4_address(); }

	Genode::uint32_t to_uint32_big_endian() const;

	static Ipv4_address from_uint32_big_endian(Genode::uint32_t ip_raw);

	Genode::uint32_t to_uint32_little_endian() const;

	static Ipv4_address from_uint32_little_endian(Genode::uint32_t ip_raw);

	bool is_in_range(Ipv4_address const &first,
	                 Ipv4_address const &last) const;
}
__attribute__((packed));


/**
 * Data layout of this class conforms to an IPv4 packet (RFC 791)
 *
 * IPv4-header-format:
 *
 *  ----------------------------------------------------------------
 * |   0-3   |  4-7  | 8-11 | 12-15 | 16-18 | 19-23 | 24-27 | 28-31 |
 *  ----------------------------------------------------------------
 * | version |  IHL  | service-type |         total-length          |
 *  ----------------------------------------------------------------
 * |         identifikation         | flags |     fragment-offset   |
 *  ----------------------------------------------------------------
 * |        ttl      |   protocol   |       header-checksum         |
 *  ----------------------------------------------------------------
 * |                       source-ip-address                        |
 *  ----------------------------------------------------------------
 * |                     destination-ip-address                     |
 *  ----------------------------------------------------------------
 * |                            options ...                         |
 *  ----------------------------------------------------------------
 */
class Net::Ipv4_packet
{
	public:

		enum Size {
			ADDR_LEN = IPV4_ADDR_LEN, /* Ip address length in bytes */
		};

		static const Ipv4_address CURRENT;    /* current network   */
		static const Ipv4_address BROADCAST;  /* broadcast address */

		static Ipv4_address ip_from_string(const char *ip);

		static Genode::uint16_t calculate_checksum(Ipv4_packet const &packet);

	private:

		/************************
		 ** IPv4 header fields **
		 ************************/

		unsigned         _header_length   : 4;
		unsigned         _version         : 4;
		unsigned         _diff_service    : 6;
		unsigned         _ecn             : 2;
		Genode::uint16_t _total_length;
		Genode::uint16_t _identification;
		unsigned         _flags           : 3;
		unsigned         _fragment_offset : 13;
		Genode::uint8_t  _time_to_live;
		Genode::uint8_t  _protocol;
		Genode::uint16_t _checksum;
		Genode::uint8_t  _src[ADDR_LEN];
		Genode::uint8_t  _dst[ADDR_LEN];
		unsigned         _data[0];

		/**
		 * Bitmasks for differentiated services field.
		 */
		enum Differentiated_services {
			PRECEDENCE  = 0x7,
			DELAY       = 0x8,
			THROUGHPUT  = 0x10,
			RELIABILITY = 0x20,
			COST        = 0x40
		};

	public:

		enum class Protocol : Genode::uint8_t
		{
			TCP = 6,
			UDP = 17,
		};

		enum Precedence {
			NETWORK_CONTROL      = 7,
			INTERNETWORK_CONTROL = 6,
			CRITIC_ECP           = 5,
			FLASH_OVERRIDE       = 4,
			FLASH                = 3,
			IMMEDIATE            = 2,
			PRIORITY             = 1,
			ROUTINE              = 0
		};

		enum Flags {
			NO_FRAGMENT    = 0x2,
			MORE_FRAGMENTS = 0x4
		};


		/**
		 * Exception used to indicate protocol violation.
		 */
		class No_ip_packet : Genode::Exception {};


		/*****************
		 ** Constructor **
		 *****************/

		Ipv4_packet(Genode::size_t size) {
			/* ip header needs to fit in */
			if (size < sizeof(Ipv4_packet))
				throw No_ip_packet();
		}


		/***************
		 ** Accessors **
		 ***************/

		Genode::size_t                  header_length()          const { return _header_length; }
		Genode::uint8_t                 version()                const { return _version; }
		Genode::uint8_t                 diff_service()           const { return _diff_service; }
		Genode::uint8_t                 ecn()                    const { return _ecn; }
		Genode::size_t                  total_length()           const { return host_to_big_endian(_total_length); }
		Genode::uint16_t                identification()         const { return host_to_big_endian(_identification); }
		Genode::uint8_t                 flags()                  const { return _flags; }
		Genode::size_t                  fragment_offset()        const { return _fragment_offset; }
		Genode::uint8_t                 time_to_live()           const { return _time_to_live; }
		Protocol                        protocol()               const { return (Protocol)_protocol; }
		Genode::uint16_t                checksum()               const { return host_to_big_endian(_checksum); }
		Ipv4_address                    src()                    const { return Ipv4_address((void *)&_src); }
		Ipv4_address                    dst()                    const { return Ipv4_address((void *)&_dst); }
		template <typename T> T const * data()                   const { return (T const *)(_data); }
		template <typename T> T *       data()                         { return (T *)(_data); }

		void header_length(Genode::size_t v)     { _header_length = v; }
		void version(Genode::uint8_t v)          { _version = v; }
		void diff_service(Genode::uint8_t v)     { _diff_service = v; ; }
		void ecn(Genode::uint8_t v)              { _ecn = v; ; }
		void total_length(Genode::size_t v)      { _total_length = host_to_big_endian((Genode::uint16_t)v); }
		void identification(Genode::uint16_t v)  { _identification = host_to_big_endian(v); }
		void flags(Genode::uint8_t v)            { _flags = v; ; }
		void fragment_offset(Genode::size_t v)   { _fragment_offset = v; ; }
		void time_to_live(Genode::uint8_t v)     { _time_to_live = v; }
		void protocol(Protocol v)                { _protocol = (Genode::uint8_t)v; }
		void checksum(Genode::uint16_t checksum) { _checksum = host_to_big_endian(checksum); }
		void src(Ipv4_address v)                 { v.copy(&_src); }
		void dst(Ipv4_address v)                 { v.copy(&_dst); }


		/***************
		 ** Operators **
		 ***************/

		/**
		 * Placement new.
		 */
		void * operator new(__SIZE_TYPE__ size, void* addr) { return addr; }


		/*********
		 ** log **
		 *********/

		void print(Genode::Output &output) const;

} __attribute__((packed));


struct Net::Ipv4_address_prefix
{
	Ipv4_address    address;
	Genode::uint8_t prefix = 32;

	bool valid() const { return address.valid() || !prefix; }

	void print(Genode::Output &output) const;

	bool prefix_matches(Ipv4_address const &ip) const;

	Ipv4_address subnet_mask() const
	{
		Ipv4_address result;
		if (prefix >= 8) {

			result.addr[0] = 0xff;

			if (prefix >= 16) {

				result.addr[1] = 0xff;

				if (prefix >= 24) {

					result.addr[2] = 0xff;
					result.addr[3] = 0xff << (32 - prefix);
				} else {
					result.addr[2] = 0xff << (24 - prefix);
				}
			} else {
				result.addr[1] = 0xff << (16 - prefix);
			}
		} else {
			result.addr[0] = 0xff << (8 - prefix);
		}
		return result;
	}

	Ipv4_address broadcast_address() const
	{
		Ipv4_address result = address;
		Ipv4_address const mask = subnet_mask();
		for (unsigned i = 0; i < 4; i++) {
			result.addr[i] |= ~mask.addr[i];
		}
		return result;
	}
};


namespace Genode {

	inline size_t ascii_to(char const *s, Net::Ipv4_address &result);

	inline size_t ascii_to(char const *s, Net::Ipv4_address_prefix &result);
}


Genode::size_t Genode::ascii_to(char const *s, Net::Ipv4_address &result)
{
	Net::Ipv4_address buf;
	size_t            number_idx = 0;
	size_t            read_len   = 0;
	while (1) {

		/* read the current number, fail if there's no number */
		size_t number_len = ascii_to_unsigned(s, buf.addr[number_idx], 10);
		if (!number_len) {
			return 0; }

		/* update read length and number index */
		read_len += number_len;
		number_idx++;

		/* if we have all numbers, fill result and return read length */
		if (number_idx == sizeof(buf.addr) / sizeof(buf.addr[0])) {
			result = buf;
			return read_len;
		}
		/* as it was not the last number, check for the following dot */
		s += number_len;
		if (*s != '.') {
			return 0; }
		read_len++;
		s++;
	}
}


Genode::size_t Genode::ascii_to(char const *s, Net::Ipv4_address_prefix &result)
{
	/* read the leading IPv4 address, fail if there's no address */
	Net::Ipv4_address_prefix buf;
	size_t read_len = ascii_to(s, buf.address);
	if (!read_len) {
		return 0; }

	/* check for the following slash */
	s += read_len;
	if (*s != '/') {
		return 0; }
	read_len++;
	s++;

	/* read the prefix, fail if there's no prefix */
	size_t prefix_len = ascii_to_unsigned(s, buf.prefix, 10);
	if (!prefix_len) {
		return 0; }

	/* fill result and return read length */
	result = buf;
	return read_len + prefix_len;
}

#endif /* _IPV4_H_ */
