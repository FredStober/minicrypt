/*
 * Copyright (C) 2012-2015 by Fred Stober
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Hash.h"
#include <cstring>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>

int Hash::cmp(const Hash *hash) const
{
	return std::memcmp(this->data(), hash->data(), this->len());
}

int Hash::cmpHash(const Hash *a, const Hash *b)
{
	return a->cmp(b);
}

const std::string Hash::str() const
{
	const uint8_t *digest = data();
	const std::size_t length = len();
	std::ostringstream out;
	for (unsigned int i = 0; i < length; ++i)
		out << std::hex << std::setfill('0') << std::setw(2) << (uint16_t)digest[i];
	return out.str();
}

std::ostream &operator<<(std::ostream &os, const Hash &h)
{
	return os << h.name() << ':' << h.str();
}

void checkHashCMP(const std::string &hashName, const size_t seg, const std::string &out, const std::string &ref)
{
	if (out != ref)
	{
		std::cerr << hashName << " (" << seg << ") fails check " << out << " != " << ref << std::endl;
		exit(1);
	}
	else
		std::cerr << hashName << " OK (" << seg << ") checks " << out << std::endl;
}

void checkHashCMP2(const std::string &hashName, const Hash *h1, const Hash *h2)
{
	if (Hash::cmpHash(h1, h2) == 0)
		return;
	std::cerr << hashName << " failed comparison" << std::endl;
	exit(1);
}

HashAlgorithm::HashAlgorithm(const std::string &name, const std::size_t length) : _name(name), _length(length)
{
}

void HashAlgorithm::checkHash(const std::string &input, std::string ref)
{
	const uint8_t *data = (const uint8_t*)(input.c_str());
	std::transform(ref.begin(), ref.end(), ref.begin(), ::tolower);

	reset();
	processBuffer(data, input.size());
	const Hash *tmp = finish();
	checkHashCMP(_name, input.size(), tmp->str(), ref);
	const Hash *tmp_clone = tmp->clone();
	checkHashCMP2(_name, tmp, tmp_clone);
	delete tmp;
	checkHashCMP(_name, input.size(), tmp_clone->str(), ref);
	const Hash *tmp_parse_data = parse(tmp_clone->data());
	checkHashCMP2(_name, tmp_clone, tmp_parse_data);
	delete tmp_clone;
	checkHashCMP(_name, input.size(), tmp_parse_data->str(), ref);
	const Hash *tmp_parse_str = parse(tmp_parse_data->str());
	checkHashCMP2(_name, tmp_parse_str, tmp_parse_data);
	delete tmp_parse_data;
	checkHashCMP(_name, input.size(), tmp_parse_str->str(), ref);
	delete tmp_parse_str;

	return;
	for (size_t segment = 1; segment < 1024; ++segment)
	{
		reset();
		size_t offset = 0;
		do
		{
			processBuffer(data + offset, std::min(segment, input.size() - offset));
			offset += segment;
		} while (offset < input.size());
		const Hash *tmp = finish();
		checkHashCMP(_name, segment, tmp->str(), ref);
		delete tmp;
	}
	reset();
}

const Hash *HashAlgorithm::finish(const Hash *src)
{
	const Hash *result = src->clone();
	reset();
	return result;
}

const std::size_t HashAlgorithm::len() const
{
	return _length;
}

const std::string HashAlgorithm::name() const
{
	return _name;
}

HashAlgorithm *HashAlgorithm::process(const std::string &in)
{
	return processBuffer((uint8_t*)in.c_str(), in.size() * sizeof(in[0]));
}

PaddedHashAlgorithm::PaddedHashAlgorithm(const std::string &name, const std::size_t length)
	: HashAlgorithm(name, length)
{
}

const Hash *PaddedHashAlgorithm::finish(const Hash *src)
{
	padHash();
	return HashAlgorithm::finish(src);
}
