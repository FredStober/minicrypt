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

#ifndef HASH_H
#define HASH_H

#include <cstdint>
#include <string>
#include <vector>

class HashDescription
{
public:
	HashDescription() = default;
	virtual ~HashDescription();
	virtual std::size_t len() const = 0;
	virtual const std::string name() const = 0;
	virtual class HashAlgorithm *algorithm() const = 0;

private:
	HashDescription(const HashDescription &) = delete;
	HashDescription &operator=(const HashDescription &) = delete;
	HashDescription(HashDescription &&) = delete;
	HashDescription &operator=(HashDescription &&) = delete;
};

class Hash
{
public:
	virtual ~Hash() = default;
	virtual class HashAlgorithm *algorithm() const = 0;
	virtual const Hash *clone() const = 0;
	int cmp(const Hash *hash) const;
	virtual const uint8_t *data() const = 0;
	virtual const HashDescription *id() const = 0;
	const virtual std::string str() const;

	static int cmpHash(const Hash *a, const Hash *b);
};

std::ostream &operator<<(std::ostream &os, const Hash &h);


struct HashCmp
{
	inline bool operator() (const Hash *a, const Hash *b) const { return a->cmp(b) > 0; }
};


// Factory to create Hash objects of the appropriate type
class HashAlgorithm
{
public:
	virtual ~HashAlgorithm() = default;
	virtual void check() = 0;
	virtual HashAlgorithm *clone() const = 0;
	virtual const Hash *finish() = 0;
	virtual const HashDescription *id() const = 0;
	virtual const Hash *parse(const uint8_t *buffer) const = 0;
	virtual const Hash *parse(const std::string str) const = 0;
	virtual HashAlgorithm *processBuffer(const uint8_t *buffer, const size_t len) = 0;
	virtual void reset() = 0;

	// Stream needs to support "read(buffer, size)" and "size_t gcount()" functions
	template<typename Stream>
	HashAlgorithm *processStream(Stream &is)
	{
		const int maxCount = 1024 * 1024 * 2;
		uint8_t *buffer = new uint8_t[maxCount];
		while (is)
		{
			is.read(buffer, maxCount);
			if (processBuffer(buffer, is.gcount()) == nullptr)
				return nullptr;
		}
		delete [] buffer;
		return this;
	}
	template<typename InputIterator>
	HashAlgorithm *process(InputIterator start, InputIterator end)
	{
		return processBuffer(reinterpret_cast<uint8_t*>(&(*start)), (end - start) * sizeof(*start));
	}
	HashAlgorithm *process(const std::string &in);

protected:
	void checkHash(const std::string &input, std::string ref);
	virtual const Hash *finish_clone(const Hash *src);
};

class PaddedHashAlgorithm : public HashAlgorithm
{
protected:
	virtual void padHash() = 0;
	virtual const Hash *finish_clone(const Hash *src) override;
};

#endif
