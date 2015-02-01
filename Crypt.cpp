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
#include "Tiger.h"

std::string tiger_ofb(const std::string &in, const std::string &key, std::string iv)
{
	const Hash *hash = 0;
	Algorithm_Tiger tiger;
	const std::size_t CRYPT_BLOCK_SIZE = tiger.id()->len();

	std::string result(in);
	for (std::size_t i = 0; i < in.size(); ++i)
	{
		const std::size_t hash_idx = i % CRYPT_BLOCK_SIZE;
		// generate key block
		if ((i % CRYPT_BLOCK_SIZE) == 0)
		{
			// (1st block) iv = hash'(plaintext); (nth block) iv = previous key block
			tiger.process(iv + "\n");
			// key - user input
			tiger.process(key + "\n");
			// prevent key block repeats (probably impossible anyway)
			tiger.process(std::to_string(i) + "\n");
			// make key stream unique with respect to message length
			tiger.process(std::to_string(in.size()) + "\n");
			if (hash)
				delete hash;
			hash = tiger.finish();
			iv = hash->str();
		}
		result[i] ^= (hash->data()[hash_idx]);
	}
	delete hash;
	return result;
}

// hash' = hash(hash(value))
// prevent direct access to hash(value) that gets used in first stage of block cipher
const std::string hash_prime(const std::string &value)
{
	Algorithm_Tiger tiger;
	const Hash *h0 = tiger.process(value)->finish();
	const Hash *hp = tiger.process(h0->str())->finish();
	delete h0;
	const std::string result(reinterpret_cast<const char*>(hp->data()), hp->id()->len());
	delete hp;
	return result;
}

// message is prefixed by INTRO_SIZE bytes of: hash'(plaintext) ^ hash'(key) = intro
static const std::size_t INTRO_SIZE = 3;

// iv = hash'(plaintext) [truncated to INTRO_SIZE]
// intro = hash'(key) ^ iv [truncated to INTRO_SIZE]
// intro gives some protection to the key stream against known plaintext attacks
// and decreases possiblity of message tampering

// cur:  3 byte => 2^24 possible iv's => 4 byte header in base64
// max: 24 byte = len(tiger_hash) => 2^192 possible iv's => 32 byte header in base64

std::string encode(const std::string &in, const std::string &key)
{
	if (in.size() == 0)
		return "";

	const std::string h_k = hash_prime(key);
	const std::string h_p = hash_prime(in);
	std::string intro(INTRO_SIZE, '\0');
	for (size_t i = 0; i < INTRO_SIZE; ++i)
		intro[i] = h_k.at(i) ^ h_p.at(i);
	std::string iv = h_p.substr(0, INTRO_SIZE);

	return intro + tiger_ofb(in, key, iv);
}

std::string decode(const std::string &in, const std::string &key, DecodeError *err)
{
	*err = DecodeError::NONE;
	if (in.size() == 0)
		return "";
	else if (in.size() < INTRO_SIZE)
	{
		*err = DecodeError::TOO_SHORT;
		return "";
	}

	const std::string h_k = hash_prime(key);
	std::string iv(in.substr(0, INTRO_SIZE), '\0');
	for (size_t i = 0; i < INTRO_SIZE; ++i)
		iv[i] ^= h_k.at(i);

	const std::string out = tiger_ofb(in.substr(INTRO_SIZE), key, iv);
	// notify about message tampering
	if (iv != hash_prime(out).substr(0, INTRO_SIZE))
	{
		*err = DecodeError::TAMPERED;
		return "";
	}
	return out;
}
