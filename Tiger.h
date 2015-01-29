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

#ifndef TIGER_H
#define TIGER_H

#include "Hash.h"

class Algorithm_Tiger : public PaddedHashAlgorithm
{
public:
	Algorithm_Tiger();
	virtual void check() override;
	virtual HashAlgorithm *clone() const override;
	virtual const Hash *finish() override;
	virtual const Hash *parse(const uint8_t *buffer) const override;
	virtual const Hash *parse(const std::string str) const override;
	virtual HashAlgorithm *processBuffer(const uint8_t *buffer, const size_t len) override;
	virtual void reset() override;

private:
	class Hash_Tiger : public Hash
	{
	public:
		virtual HashAlgorithm *algorithm() const override;
		virtual const Hash *clone() const override;
		virtual const uint8_t *data() const override;
		virtual const std::size_t len() const override { return 24; };
		virtual const std::string name() const { return "Tiger"; }
	private:
		Hash_Tiger() = default;
		Hash_Tiger(const uint64_t _internal[3]);
		friend class Algorithm_Tiger;
		uint64_t internal[3];
	} _hash;
	uint64_t nBytes;
	uint32_t bufferOffset;
	uint8_t buffer[64];

	virtual void padHash() override;
};

#endif
