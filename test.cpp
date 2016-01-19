/*
 * Copyright (C) 2015 by Fred Stober
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

#include "Crypt.h"
#include "HashTools.h"
#include <cassert>
#include <iostream>
#include <sstream>
#include <iomanip>

static const std::string hexify(const std::string &data)
{
	std::stringstream result;
	for (std::size_t i = 0; i < data.size(); ++i)
		result << std::hex << std::setw(2) << std::setfill('0')
			<< static_cast<int>(static_cast<uint8_t>(data[i]));
	return result.str();
}

int main()
{
	std::string k = "Key";
	std::string p = "Hello World!";
	std::string e = encode(p, k);
	std::cout << hexify(e) << std::endl;
	DecodeError err = DecodeError::NONE;
	std::string d = decode(e, k, &err);
	std::cout << "------";
	for (std::size_t i = 0; i < d.size(); ++i)
		std::cout << d[i] << " ";
	std::cout << std::endl;
	assert(d == p);
}
