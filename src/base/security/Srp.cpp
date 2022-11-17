/*
 * Srp.cpp
 *
 * SRP-6a implementation
 *
 *
 * Copyright (C) 2018 Amit Kumar (amitkriit@gmail.com)
 * This program is part of the Wanhive IoT Platform.
 * Check the COPYING file for the license.
 *
 */

#include "Srp.h"
#include <cstring>
#include <new>

namespace {

struct SRPConstants {
	const char *Nhex;
	const char *ghex;
};

/*
 * Populated from Appendix A of RFC 5054
 */
const SRPConstants Nghex[7] = { {
/* 1024 */
"EEAF0AB9ADB38DD69C33F80AFA8FC5E86072618775FF3C0B9EA2314C"
		"9C256576D674DF7496EA81D3383B4813D692C6E0E0D5D8E250B98BE4"
		"8E495C1D6089DAD15DC7D7B46154D6B6CE8EF4AD69B15D4982559B29"
		"7BCF1885C529F566660E57EC68EDBC3C05726CC02FD4CBF4976EAA9A"
		"FD5138FE8376435B9FC61D2FC0EB06E3", "2" }, {
/* 1536 */
"9DEF3CAFB939277AB1F12A8617A47BBBDBA51DF499AC4C80BEEEA961"
		"4B19CC4D5F4F5F556E27CBDE51C6A94BE4607A291558903BA0D0F843"
		"80B655BB9A22E8DCDF028A7CEC67F0D08134B1C8B97989149B609E0B"
		"E3BAB63D47548381DBC5B1FC764E3F4B53DD9DA1158BFD3E2B9C8CF5"
		"6EDF019539349627DB2FD53D24B7C48665772E437D6C7F8CE442734A"
		"F7CCB7AE837C264AE3A9BEB87F8A2FE9B8B5292E5A021FFF5E91479E"
		"8CE7A28C2442C6F315180F93499A234DCF76E3FED135F9BB", "2" }, {
/* 2048 */
"AC6BDB41324A9A9BF166DE5E1389582FAF72B6651987EE07FC319294"
		"3DB56050A37329CBB4A099ED8193E0757767A13DD52312AB4B03310D"
		"CD7F48A9DA04FD50E8083969EDB767B0CF6095179A163AB3661A05FB"
		"D5FAAAE82918A9962F0B93B855F97993EC975EEAA80D740ADBF4FF74"
		"7359D041D5C33EA71D281E446B14773BCA97B43A23FB801676BD207A"
		"436C6481F1D2B9078717461A5B9D32E688F87748544523B524B0D57D"
		"5EA77A2775D2ECFA032CFBDBF52FB3786160279004E57AE6AF874E73"
		"03CE53299CCC041C7BC308D82A5698F3A8D0C38271AE35F8E9DBFBB6"
		"94B5C803D89F7AE435DE236D525F54759B65E372FCD68EF20FA7111F"
		"9E4AFF73", "2" }, {
/* 3072 */
"FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E08"
		"8A67CC74020BBEA63B139B22514A08798E3404DDEF9519B3CD3A431B"
		"302B0A6DF25F14374FE1356D6D51C245E485B576625E7EC6F44C42E9"
		"A637ED6B0BFF5CB6F406B7EDEE386BFB5A899FA5AE9F24117C4B1FE6"
		"49286651ECE45B3DC2007CB8A163BF0598DA48361C55D39A69163FA8"
		"FD24CF5F83655D23DCA3AD961C62F356208552BB9ED529077096966D"
		"670C354E4ABC9804F1746C08CA18217C32905E462E36CE3BE39E772C"
		"180E86039B2783A2EC07A28FB5C55DF06F4C52C9DE2BCBF695581718"
		"3995497CEA956AE515D2261898FA051015728E5A8AAAC42DAD33170D"
		"04507A33A85521ABDF1CBA64ECFB850458DBEF0A8AEA71575D060C7D"
		"B3970F85A6E1E4C7ABF5AE8CDB0933D71E8C94E04A25619DCEE3D226"
		"1AD2EE6BF12FFA06D98A0864D87602733EC86A64521F2B18177B200C"
		"BBE117577A615D6C770988C0BAD946E208E24FA074E5AB3143DB5BFC"
		"E0FD108E4B82D120A93AD2CAFFFFFFFFFFFFFFFF", "5" }, {
/* 4096 */
"FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E08"
		"8A67CC74020BBEA63B139B22514A08798E3404DDEF9519B3CD3A431B"
		"302B0A6DF25F14374FE1356D6D51C245E485B576625E7EC6F44C42E9"
		"A637ED6B0BFF5CB6F406B7EDEE386BFB5A899FA5AE9F24117C4B1FE6"
		"49286651ECE45B3DC2007CB8A163BF0598DA48361C55D39A69163FA8"
		"FD24CF5F83655D23DCA3AD961C62F356208552BB9ED529077096966D"
		"670C354E4ABC9804F1746C08CA18217C32905E462E36CE3BE39E772C"
		"180E86039B2783A2EC07A28FB5C55DF06F4C52C9DE2BCBF695581718"
		"3995497CEA956AE515D2261898FA051015728E5A8AAAC42DAD33170D"
		"04507A33A85521ABDF1CBA64ECFB850458DBEF0A8AEA71575D060C7D"
		"B3970F85A6E1E4C7ABF5AE8CDB0933D71E8C94E04A25619DCEE3D226"
		"1AD2EE6BF12FFA06D98A0864D87602733EC86A64521F2B18177B200C"
		"BBE117577A615D6C770988C0BAD946E208E24FA074E5AB3143DB5BFC"
		"E0FD108E4B82D120A92108011A723C12A787E6D788719A10BDBA5B26"
		"99C327186AF4E23C1A946834B6150BDA2583E9CA2AD44CE8DBBBC2DB"
		"04DE8EF92E8EFC141FBECAA6287C59474E6BC05D99B2964FA090C3A2"
		"233BA186515BE7ED1F612970CEE2D7AFB81BDD762170481CD0069127"
		"D5B05AA993B4EA988D8FDDC186FFB7DC90A6C08F4DF435C934063199"
		"FFFFFFFFFFFFFFFF", "5" }, {
/* 6144 */
"FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E08"
		"8A67CC74020BBEA63B139B22514A08798E3404DDEF9519B3CD3A431B"
		"302B0A6DF25F14374FE1356D6D51C245E485B576625E7EC6F44C42E9"
		"A637ED6B0BFF5CB6F406B7EDEE386BFB5A899FA5AE9F24117C4B1FE6"
		"49286651ECE45B3DC2007CB8A163BF0598DA48361C55D39A69163FA8"
		"FD24CF5F83655D23DCA3AD961C62F356208552BB9ED529077096966D"
		"670C354E4ABC9804F1746C08CA18217C32905E462E36CE3BE39E772C"
		"180E86039B2783A2EC07A28FB5C55DF06F4C52C9DE2BCBF695581718"
		"3995497CEA956AE515D2261898FA051015728E5A8AAAC42DAD33170D"
		"04507A33A85521ABDF1CBA64ECFB850458DBEF0A8AEA71575D060C7D"
		"B3970F85A6E1E4C7ABF5AE8CDB0933D71E8C94E04A25619DCEE3D226"
		"1AD2EE6BF12FFA06D98A0864D87602733EC86A64521F2B18177B200C"
		"BBE117577A615D6C770988C0BAD946E208E24FA074E5AB3143DB5BFC"
		"E0FD108E4B82D120A92108011A723C12A787E6D788719A10BDBA5B26"
		"99C327186AF4E23C1A946834B6150BDA2583E9CA2AD44CE8DBBBC2DB"
		"04DE8EF92E8EFC141FBECAA6287C59474E6BC05D99B2964FA090C3A2"
		"233BA186515BE7ED1F612970CEE2D7AFB81BDD762170481CD0069127"
		"D5B05AA993B4EA988D8FDDC186FFB7DC90A6C08F4DF435C934028492"
		"36C3FAB4D27C7026C1D4DCB2602646DEC9751E763DBA37BDF8FF9406"
		"AD9E530EE5DB382F413001AEB06A53ED9027D831179727B0865A8918"
		"DA3EDBEBCF9B14ED44CE6CBACED4BB1BDB7F1447E6CC254B33205151"
		"2BD7AF426FB8F401378CD2BF5983CA01C64B92ECF032EA15D1721D03"
		"F482D7CE6E74FEF6D55E702F46980C82B5A84031900B1C9E59E7C97F"
		"BEC7E8F323A97A7E36CC88BE0F1D45B7FF585AC54BD407B22B4154AA"
		"CC8F6D7EBF48E1D814CC5ED20F8037E0A79715EEF29BE32806A1D58B"
		"B7C5DA76F550AA3D8A1FBFF0EB19CCB1A313D55CDA56C9EC2EF29632"
		"387FE8D76E3C0468043E8F663F4860EE12BF2D5B0B7474D6E694F91E"
		"6DCC4024FFFFFFFFFFFFFFFF", "5" }, {
/* 8192 */
"FFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E08"
		"8A67CC74020BBEA63B139B22514A08798E3404DDEF9519B3CD3A431B"
		"302B0A6DF25F14374FE1356D6D51C245E485B576625E7EC6F44C42E9"
		"A637ED6B0BFF5CB6F406B7EDEE386BFB5A899FA5AE9F24117C4B1FE6"
		"49286651ECE45B3DC2007CB8A163BF0598DA48361C55D39A69163FA8"
		"FD24CF5F83655D23DCA3AD961C62F356208552BB9ED529077096966D"
		"670C354E4ABC9804F1746C08CA18217C32905E462E36CE3BE39E772C"
		"180E86039B2783A2EC07A28FB5C55DF06F4C52C9DE2BCBF695581718"
		"3995497CEA956AE515D2261898FA051015728E5A8AAAC42DAD33170D"
		"04507A33A85521ABDF1CBA64ECFB850458DBEF0A8AEA71575D060C7D"
		"B3970F85A6E1E4C7ABF5AE8CDB0933D71E8C94E04A25619DCEE3D226"
		"1AD2EE6BF12FFA06D98A0864D87602733EC86A64521F2B18177B200C"
		"BBE117577A615D6C770988C0BAD946E208E24FA074E5AB3143DB5BFC"
		"E0FD108E4B82D120A92108011A723C12A787E6D788719A10BDBA5B26"
		"99C327186AF4E23C1A946834B6150BDA2583E9CA2AD44CE8DBBBC2DB"
		"04DE8EF92E8EFC141FBECAA6287C59474E6BC05D99B2964FA090C3A2"
		"233BA186515BE7ED1F612970CEE2D7AFB81BDD762170481CD0069127"
		"D5B05AA993B4EA988D8FDDC186FFB7DC90A6C08F4DF435C934028492"
		"36C3FAB4D27C7026C1D4DCB2602646DEC9751E763DBA37BDF8FF9406"
		"AD9E530EE5DB382F413001AEB06A53ED9027D831179727B0865A8918"
		"DA3EDBEBCF9B14ED44CE6CBACED4BB1BDB7F1447E6CC254B33205151"
		"2BD7AF426FB8F401378CD2BF5983CA01C64B92ECF032EA15D1721D03"
		"F482D7CE6E74FEF6D55E702F46980C82B5A84031900B1C9E59E7C97F"
		"BEC7E8F323A97A7E36CC88BE0F1D45B7FF585AC54BD407B22B4154AA"
		"CC8F6D7EBF48E1D814CC5ED20F8037E0A79715EEF29BE32806A1D58B"
		"B7C5DA76F550AA3D8A1FBFF0EB19CCB1A313D55CDA56C9EC2EF29632"
		"387FE8D76E3C0468043E8F663F4860EE12BF2D5B0B7474D6E694F91E"
		"6DBE115974A3926F12FEE5E438777CB6A932DF8CD8BEC4D073B931BA"
		"3BC832B68D9DD300741FA7BF8AFC47ED2576F6936BA424663AAB639C"
		"5AE4F5683423B4742BF1C978238F16CBE39D652DE3FDB8BEFC848AD9"
		"22222E04A4037C0713EB57A81A23F0C73473FC646CEA306B4BCBC886"
		"2F8385DDFA9D4B7FA2C087E879683303ED5BDD3A062B3CF5B3A278A6"
		"6D2A13F83F44F82DDF310EE074AB6A364597E899A0255DC164F31CC5"
		"0846851DF9AB48195DED7EA1B1D510BD7EE74D73FAF36BC31ECFA268"
		"359046F4EB879F924009438B481C6CD7889A002ED5EE382BC9190DA6"
		"FC026E479558E4475677E9AA9E3050E2765694DFC81F56E880B96E71"
		"60C980DD98EDD3DFFFFFFFFFFFFFFFFF", "13" } };

/*
 * For left padding to N (maximum 8192 bits)
 */
const unsigned char zeros[1024] { };

}  // namespace

namespace wanhive {

Srp::Srp(SrpGroup type, DigestType dType) noexcept :
		type(type), ctx(nullptr), H(dType), status(0) {
	memset(key.K, 0, MDSIZE);
	memset(proof.M, 0, MDSIZE);
	memset(proof.AMK, 0, MDSIZE);
	memset(fake.salt, 0, MDSIZE);
}

Srp::~Srp() {
	memset(key.K, 0, MDSIZE);
	memset(proof.M, 0, MDSIZE);
	memset(proof.AMK, 0, MDSIZE);
	memset(fake.salt, 0, MDSIZE);
	freeContext();
}

bool Srp::initialize() noexcept {
	return newContext() && loadPrime() && loadGenerator()
			&& loadMultiplierParameter();
}

bool Srp::loadSalt(unsigned int bytes) noexcept {
	if (!bytes) {
		bytes = SALTLENGTH;
	}
	return user.s.random(bytes * 8);
}

bool Srp::loadSalt(const char *salt) noexcept {
	if (salt) {
		return user.s.put(salt);
	} else {
		return false;
	}
}

bool Srp::loadSalt(const unsigned char *salt, unsigned int length) noexcept {
	if (salt && length) {
		return user.s.put(salt, length);
	} else {
		return false;
	}
}

bool Srp::loadPrivateKey(const char *I, const unsigned char *p,
		unsigned int pLength, unsigned int rounds) noexcept {
	if (!p || !pLength || !user.s.get()) {
		return false;
	}

	//x = H(s | H ( I | ":" | p) )
	unsigned char x[MDSIZE];
	auto success = H.init()
			&& (I ? H.update(I, strlen(I)) && H.update(":", 1) : true)
			&& H.update(p, pLength) && H.final(x); //H ( I | ":" | p)

	rounds = !rounds ? 1 : rounds;
	while (success && (rounds--)) {
		success = success && H.init()
				&& H.update(user.s.getBinary(), user.s.size())
				&& H.update(x, H.length()) && H.final(x); //H(s | H ( I | ":" | p) )
	}

	return success && user.x.put(x, H.length());
}

bool Srp::loadPasswordVerifier() noexcept {
	if (!group.N.get() || !group.g.get() || !user.x.get() || !ctx) {
		return false;
	}

	auto n = BN_new();
	if (!n) {
		return false;
	}
	//v = g^x
	if (!BN_mod_exp(n, group.g.get(), user.x.get(), group.N.get(), ctx)) {
		BN_clear_free(n);
		return false;
	}

	if (user.v.put(n)) {
		return true;
	} else {
		BN_clear_free(n);
		return false;
	}
}

bool Srp::loadPasswordVerifier(const char *verifier) noexcept {
	if (verifier) {
		return user.v.put(verifier);
	} else {
		return false;
	}
}

bool Srp::loadPasswordVerifier(const unsigned char *verifier,
		unsigned int length) noexcept {
	if (verifier && length) {
		return user.v.put(verifier, length);
	} else {
		return false;
	}
}

bool Srp::loadUserSecret(unsigned int bytes) noexcept {
	if (!bytes) {
		bytes = SECRETLENGTH;
	}
	return secret.a.random(bytes * 8);
}

bool Srp::loadUserSecret(const char *nonce) noexcept {
	if (nonce) {
		return secret.a.put(nonce);
	} else {
		return false;
	}
}

bool Srp::loadUserSecret(const unsigned char *nonce,
		unsigned int length) noexcept {
	if (nonce && length) {
		return secret.a.put(nonce, length);
	} else {
		return false;
	}
}

bool Srp::loadHostSecret(unsigned int bytes) noexcept {
	if (!bytes) {
		bytes = SECRETLENGTH;
	}
	return secret.b.random(bytes * 8);
}

bool Srp::loadHostSecret(const char *nonce) noexcept {
	if (nonce) {
		return secret.b.put(nonce);
	} else {
		return false;
	}
}

bool Srp::loadHostSecret(const unsigned char *nonce,
		unsigned int length) noexcept {
	if (nonce && length) {
		return secret.b.put(nonce, length);
	} else {
		return false;
	}
}

bool Srp::loadUserNonce() noexcept {
	if (!group.N.get() || !group.g.get() || !secret.a.get() || !ctx) {
		return false;
	}

	auto n = BN_new();
	if (!n) {
		return false;
	}

	//A = g^a
	if (!BN_mod_exp(n, group.g.get(), secret.a.get(), group.N.get(), ctx)) {
		BN_clear_free(n);
		return false;
	}

	if (shared.A.put(n)) {
		return true;
	} else {
		BN_clear_free(n);
		return false;
	}
}

bool Srp::loadUserNonce(const char *nonce) noexcept {
	if (nonce) {
		return shared.A.put(nonce) && checkRange(shared.A.get())
				&& checkNotZero(shared.A.get());
	} else {
		return false;
	}
}

bool Srp::loadUserNonce(const unsigned char *nonce,
		unsigned int length) noexcept {
	if (nonce && length) {
		return shared.A.put(nonce, length) && checkRange(shared.A.get())
				&& checkNotZero(shared.A.get());
	} else {
		return false;
	}
}

bool Srp::loadHostNonce() noexcept {
	if (!group.k.get() || !user.v.get() || !secret.b.get() || !ctx) {
		return false;
	}

	//B = kv + g^b
	auto n = BN_new();
	if (!n) {
		return false;
	}

	BN_CTX_start(ctx);
	auto m = BN_CTX_get(ctx); //kv
	auto p = BN_CTX_get(ctx); //g^b

	if (!m || !p) {
		BN_clear_free(n);
		BN_CTX_end(ctx);
		return false;
	}

	auto ret = BN_mul(m, group.k.get(), user.v.get(), ctx) //kv
	&& BN_mod_exp(p, group.g.get(), secret.b.get(), group.N.get(), ctx) //g^b mod N
			&& BN_mod_add(n, m, p, group.N.get(), ctx); //(kv + g^b) mod N
	BN_CTX_end(ctx);

	if (ret && shared.B.put(n)) {
		return true;
	} else {
		BN_clear_free(n);
		return false;
	}
}

bool Srp::loadHostNonce(const char *nonce) noexcept {
	if (nonce) {
		return shared.B.put(nonce) && checkRange(shared.B.get())
				&& checkNotZero(shared.B.get());
	} else {
		return false;
	}
}

bool Srp::loadHostNonce(const unsigned char *nonce,
		unsigned int length) noexcept {
	if (nonce && length) {
		return shared.B.put(nonce, length) && checkRange(shared.B.get())
				&& checkNotZero(shared.B.get());
	} else {
		return false;
	}

}

bool Srp::loadRandomScramblingParameter() noexcept {
	//A and B should be less than N
	if (!checkRange(shared.A.get()) || !checkRange(shared.B.get())) {
		return false;
	}

	//u = H(PAD(A), PAD(B))
	unsigned char md[MDSIZE]; //H(PAD(A), PAD(B))

	auto padLenA = group.N.size() - shared.A.size();
	auto padLenB = group.N.size() - shared.B.size();

	return H.init() && H.update(zeros, padLenA)
			&& H.update(shared.A.getBinary(), shared.A.size())
			&& H.update(zeros, padLenB)
			&& H.update(shared.B.getBinary(), shared.B.size()) && H.final(md)
			&& shared.u.put(md, H.length()) && !BN_is_zero(shared.u.get());
}

bool Srp::loadSessionKey(bool isHost) noexcept {
	if (!group.k.get() || !user.v.get() || !shared.u.get() || !ctx) {
		return false;
	} else if (!isHost && !(user.x.get() && secret.a.get())) {
		return false;
	}

	auto ret = false;
	auto num = BN_new();
	if (!num) {
		return false;
	}

	if (isHost) {
		BN_CTX_start(ctx);
		auto m = BN_CTX_get(ctx); //v^u
		auto n = BN_CTX_get(ctx); //(Av^u)
		if (!m || !n) {
			BN_clear_free(num);
			BN_CTX_end(ctx);
			return false;
		}

		//S = (Av^u) ^ b
		ret = BN_mod_exp(m, user.v.get(), shared.u.get(), group.N.get(), ctx) //v^u mod N
		&& BN_mul(n, shared.A.get(), m, ctx) //Av^u
				&& BN_mod_exp(num, n, secret.b.get(), group.N.get(), ctx) //(Av^u) ^ b mod N
				&& key.S.put(num);

		BN_CTX_end(ctx);
	} else {
		BN_CTX_start(ctx);
		auto m = BN_CTX_get(ctx); //ux, (B - kg^x)
		auto n = BN_CTX_get(ctx); //(a + ux)
		auto p = BN_CTX_get(ctx); //kg^x

		if (!m || !n || !p) {
			BN_clear_free(num);
			BN_CTX_end(ctx);
			return false;
		}

		//S = (B - kg^x) ^ (a + ux)
		ret = BN_mul(m, shared.u.get(), user.x.get(), ctx) //ux
		&& BN_add(n, secret.a.get(), m) //a + ux
				&& BN_mul(p, group.k.get(), user.v.get(), ctx) //kv
				&& BN_sub(m, shared.B.get(), p) //(B - kv)
				&& BN_mod_exp(num, m, n, group.N.get(), ctx) //(B - kg^x) ^ (a + ux) mod N
				&& key.S.put(num);

		BN_CTX_end(ctx);
	}

	if (!ret) {
		BN_clear_free(num);
		return false;
	}

	unsigned int padLenS = group.N.size() - key.S.size();
	return H.init() && H.update(zeros, padLenS)
			&& H.update(key.S.getBinary(), key.S.size()) && H.final(key.K);
}

bool Srp::generateUserProof(const char *I) noexcept {
	if (!group.N.size() || !group.g.size() || !user.s.size() || !shared.A.size()
			|| !shared.B.size()) {
		return false;
	} else if ((user.s.size() > group.N.size())
			|| (shared.A.size() > group.N.size())
			|| (shared.B.size() > group.N.size())) {
		return false;
	}

	unsigned char H_I[MDSIZE];
	unsigned char H_N[MDSIZE];
	unsigned char H_g[MDSIZE];

	auto padLeng = group.N.size() - group.g.size();
	auto padLens = group.N.size() - user.s.size();
	auto padLenA = group.N.size() - shared.A.size();
	auto padLenB = group.N.size() - shared.B.size();
	auto padLenH = group.N.size() - H.length();

	auto ret =
			(I ? (bool) H.create((const unsigned char*) I, strlen(I), H_I) : true)
					&& H.create(group.N.getBinary(), group.N.size(), H_N)
					&& H.init() && H.update(zeros, padLeng)
					&& H.update(group.g.getBinary(), group.g.size())
					&& H.final(H_g);

	if (!ret) {
		return false;
	}

	//H(N) xor H(g)
	for (unsigned int i = 0; i < H.length(); ++i) {
		H_N[i] ^= H_g[i];
	}

	//M = H(H(N) xor H(g), H(I), s, A, B, K)
	return H.init() && H.update(zeros, padLenH) && H.update(H_N, H.length())
			&& (I ? (H.update(zeros, padLenH) && H.update(H_I, H.length())) : true)
			&& H.update(zeros, padLens)
			&& H.update(user.s.getBinary(), user.s.size())
			&& H.update(zeros, padLenA)
			&& H.update(shared.A.getBinary(), shared.A.size())
			&& H.update(zeros, padLenB)
			&& H.update(shared.B.getBinary(), shared.B.size())
			&& H.update(zeros, padLenH) && H.update(key.K, H.length())
			&& H.final(proof.M);
}

bool Srp::generateHostProof() noexcept {
	if (!shared.A.size() || (shared.A.size() > group.N.size())) {
		return false;
	}

	//H(A, M, K)
	auto padLenA = group.N.size() - shared.A.size();
	auto padLenH = group.N.size() - H.length();
	return H.init() && H.update(zeros, padLenA)
			&& H.update(shared.A.getBinary(), shared.A.size())
			&& H.update(zeros, padLenH) && H.update(proof.M, H.length())
			&& H.update(zeros, padLenH) && H.update(key.K, H.length())
			&& H.final(proof.AMK);
}

bool Srp::generateUserEvidence() noexcept {
	if (!group.N.size() || !shared.A.size() || !shared.B.size()) {
		return false;
	} else if ((shared.A.size() > group.N.size())
			|| (shared.B.size() > group.N.size())
			|| (key.S.size() > group.N.size())) {
		return false;
	}

	//H(PAD(A), PAD(B), PAD(S))
	auto padLenA = group.N.size() - shared.A.size();
	auto padLenB = group.N.size() - shared.B.size();
	auto padLenS = group.N.size() - key.S.size();

	return H.init() && H.update(zeros, padLenA)
			&& H.update(shared.A.getBinary(), shared.A.size())
			&& H.update(zeros, padLenB)
			&& H.update(shared.B.getBinary(), shared.B.size())
			&& H.update(zeros, padLenS)
			&& H.update(key.S.getBinary(), key.S.size()) && H.final(proof.M);
}

bool Srp::generateHostEvidence() noexcept {
	if (!group.N.size() || !shared.A.size() || !key.S.size()) {
		return false;
	} else if ((shared.A.size() > group.N.size())
			|| (key.S.size() > group.N.size())) {
		return false;
	}

	//H(PAD(A), PAD(M), PAD(S))
	auto padLenA = group.N.size() - shared.A.size();
	auto padLenH = group.N.size() - H.length();
	auto padLenS = group.N.size() - key.S.size();

	return H.init() && H.update(zeros, padLenA)
			&& H.update(shared.A.getBinary(), shared.A.size())
			&& H.update(zeros, padLenH) && H.update(proof.M, H.length())
			&& H.update(zeros, padLenS)
			&& H.update(key.S.getBinary(), key.S.size()) && H.final(proof.AMK);
}

bool Srp::verifyUserProof(const unsigned char *proof,
		unsigned int length) noexcept {
	if (proof && length == H.length() && key.S.get()) {
		return !memcmp(this->proof.M, proof, H.length());
	} else {
		return false;
	}
}

bool Srp::verifyHostProof(const unsigned char *proof,
		unsigned int length) noexcept {
	if (proof && length == H.length() && key.S.get()) {
		return !memcmp(this->proof.AMK, proof, H.length());
	} else {
		return false;
	}
}

bool Srp::generateFakeNonce() noexcept {
	if (!group.N.get()) {
		return false;
	} else {
		return fake.nonce.pseudoRandom(group.N.get());
	}
}

bool Srp::generateFakeSalt(const char *I, const unsigned char *s,
		unsigned int sLen) noexcept {
	return H.init() && H.update(I, (I ? strlen(I) : 0)) && H.update(s, sLen)
			&& H.final(fake.salt);
}

void Srp::getSalt(const unsigned char *&binary,
		unsigned int &bytes) const noexcept {
	binary = user.s.getBinary();
	bytes = user.s.size();
}

void Srp::getPasswordVerifier(const unsigned char *&binary,
		unsigned int &bytes) const noexcept {
	binary = user.v.getBinary();
	bytes = user.v.size();
}

void Srp::getUserNonce(const unsigned char *&binary,
		unsigned int &bytes) const noexcept {
	binary = shared.A.getBinary();
	bytes = shared.A.size();
}

void Srp::getHostNonce(const unsigned char *&binary,
		unsigned int &bytes) const noexcept {
	binary = shared.B.getBinary();
	bytes = shared.B.size();
}

void Srp::getSessionKey(const unsigned char *&binary,
		unsigned int &bytes) const noexcept {
	binary = key.S.getBinary();
	bytes = key.S.size();
}

const unsigned char* Srp::getKey() const noexcept {
	return this->key.K;
}

void Srp::getUserProof(const unsigned char *&binary,
		unsigned int &bytes) const noexcept {
	binary = proof.M;
	bytes = H.length();
}

void Srp::getHostProof(const unsigned char *&binary,
		unsigned int &bytes) const noexcept {
	binary = proof.AMK;
	bytes = H.length();
}

void Srp::getFakeNonce(const unsigned char *&binary,
		unsigned int &bytes) const noexcept {
	binary = fake.nonce.getBinary();
	bytes = fake.nonce.size();
}

void Srp::getFakeSalt(const unsigned char *&binary,
		unsigned int &bytes) const noexcept {
	binary = fake.salt;
	bytes = H.length();
}

unsigned int Srp::keySize() const noexcept {
	return H.length();
}

unsigned int Srp::groupSize() const noexcept {
	return group.N.size();
}

int Srp::getStatus() const noexcept {
	return status;
}

void Srp::setStatus(int status) noexcept {
	this->status = status;
}

void Srp::print(const char *msg) const noexcept {
	if (msg) {
		printf("%s\n", msg);
	}
	printf("GROUP SIZE: %u bits\n", (groupSize() * 8));
	printf("KEY SIZE: %u bits\n", (keySize() * 8));
	printf("\n");

	printf("N= ");
	group.N.print();
	printf("g= ");
	group.g.print();
	printf("k= ");
	group.k.print();
	printf("\n");

	printf("s= ");
	user.s.print();
	printf("x= ");
	user.x.print();
	printf("v= ");
	user.v.print();
	printf("\n");

	printf("a= ");
	secret.a.print();
	printf("b= ");
	secret.b.print();
	printf("\n");

	printf("A= ");
	shared.A.print();
	printf("B= ");
	shared.B.print();
	printf("u= ");
	shared.u.print();
	printf("\n");

	printf("S= ");
	key.S.print();
	printf("\n");
}

void Srp::test() noexcept {
	Srp *host = nullptr;
	Srp *user = nullptr;
	if (!(host = new (std::nothrow) Srp())
			|| !(user = new (std::nothrow) Srp())) {
		printf("Insufficient memory");
		delete host;
		delete user;
		return;
	}

	host->initialize();
	user->initialize();

	//-----------------------------------------------------------------
	//Load credentials at host
	//x = H(s, p)               (s is chosen randomly)
	//v = g^x                   (computes password verifier)
	host->loadSalt("BEB25379D1A8581EB5A727673A2441EE");
	host->loadPrivateKey("alice", (const unsigned char*) "password123",
			strlen("password123"));
	host->loadPasswordVerifier();
	//-----------------------------------------------------------------
	//User -> Host:  I, A = g^a
	user->loadUserSecret(
			"60975527035CF2AD1989806F0407210BC81EDC04E2762A56AFD529DDDA2D4393");
	user->loadUserNonce();

	if (!host->loadUserNonce(user->shared.A.getBinary(),
			user->shared.A.size())) {
		printf("Host: A = 0 (mod N) violation\n");
	}
	//-----------------------------------------------------------------
	//Host -> User:  s, B = kv + g^b
	host->loadHostSecret(
			"E487CB59D31AC550471E81F00F6928E01DDA08E974A004F49E61F5D105284D20");
	host->loadHostNonce();

	user->loadSalt(host->user.s.getBinary(), host->user.s.size());
	if (!user->loadHostNonce(host->shared.B.getBinary(),
			host->shared.B.size())) {
		printf("User: B = 0 (mod N) violation\n");
	}
	//-----------------------------------------------------------------
	//Both:  u = H(A, B)
	if (!host->loadRandomScramblingParameter()) {
		printf("Scrambling Parameter not loaded at host\n");
	}
	if (!user->loadRandomScramblingParameter()) {
		printf("Scrambling Parameter not loaded at user\n");
	}
	printf("Computed u = ");
	host->shared.u.print();
	printf("Expected u = CE38B9593487DA98554ED47D70A7AE5F462EF019\n");
	//-----------------------------------------------------------------
	//User:  x = H(s, p)                 (user enters password)
	//User:  S = (B - kg^x) ^ (a + ux)   (computes session key)
	//User:  K = H(S)
	user->loadPrivateKey("alice", (const unsigned char*) "password123",
			strlen("password123"));
	user->loadPasswordVerifier();
	user->loadSessionKey(false);

	if (memcmp(host->user.v.getBinary(), user->user.v.getBinary(),
			host->user.v.size())) {
		printf("Verifiers did not match\n");
	}
	//-----------------------------------------------------------------
	//Host:  S = (Av^u) ^ b              (computes session key)
	//      Host:  K = H(S)
	host->loadSessionKey(true);
	if (memcmp(host->key.K, user->key.K, host->keySize())) {
		printf("Keys did not match\n");
	}
	printf("Computed S = ");
	host->key.S.print();
	printf("Expected S = "
			"B0DC82BABCF30674AE450C0287745E7990A3381F63B387AAF271A10D"
			"233861E359B48220F7C4693C9AE12B0A6F67809F0876E2D013800D6C"
			"41BB59B6D5979B5C00A172B4A2A5903A0BDCAF8A709585EB2AFAFA8F"
			"3499B200210DCC1F10EB33943CD67FC88A2F39A4BE5BEC4EC0A3212D"
			"C346D7E474B29EDE8A469FFECA686E5A\n");
	//-----------------------------------------------------------------
	//User -> Host:  M = H(H(N) xor H(g), H(I), s, A, B, K)
	user->generateUserProof("alice");
	memcpy(host->proof.M, user->proof.M, host->keySize());
	//-----------------------------------------------------------------
	//Host -> User:  H(A, M, K)
	host->generateHostProof();
	user->generateHostProof();
	if (memcmp(host->proof.AMK, user->proof.AMK, host->keySize())) {
		printf("Challenge did not match\n");
	} else {
		printf("Session established\n");
	}
	//-----------------------------------------------------------------
	delete host;
	delete user;
}

void Srp::test(const char *I, const unsigned char *p, unsigned int pwdLen,
		unsigned int iterations, SrpGroup type, DigestType dType) noexcept {
	Srp *host = nullptr;
	Srp *user = nullptr;
	if (!(host = new (std::nothrow) Srp(type, dType))
			|| !(user = new (std::nothrow) Srp(type, dType))) {
		printf("Insufficient memory");
		delete host;
		delete user;
		return;
	}

	host->initialize();
	user->initialize();

	for (unsigned int i = 0; i < iterations; ++i) {
		//-----------------------------------------------------------------
		//Load credentials at host
		//x = H(s, p)               (s is chosen randomly)
		//v = g^x                   (computes password verifier)
		host->loadSalt();
		host->loadPrivateKey(I, p, pwdLen);
		host->loadPasswordVerifier();
		//-----------------------------------------------------------------
		//User -> Host:  I, A = g^a
		user->loadUserSecret();
		user->loadUserNonce();

		if (!host->loadUserNonce(user->shared.A.getBinary(),
				user->shared.A.size())) {
			printf("Host: A = 0 (mod N) violation\n");
		}
		//-----------------------------------------------------------------
		//Host -> User:  s, B = kv + g^b
		host->loadHostSecret();
		host->loadHostNonce();

		user->loadSalt(host->user.s.getBinary(), host->user.s.size());
		if (!user->loadHostNonce(host->shared.B.getBinary(),
				host->shared.B.size())) {
			printf("User: B = 0 (mod N) violation\n");
		}
		//-----------------------------------------------------------------
		//Both:  u = H(A, B)
		if (!host->loadRandomScramblingParameter()) {
			printf("Scrambling Parameter not loaded at host\n");
		}
		if (!user->loadRandomScramblingParameter()) {
			printf("Scrambling Parameter not loaded at user\n");
		}
		//-----------------------------------------------------------------
		//User:  x = H(s, p)                 (user enters password)
		//User:  S = (B - kg^x) ^ (a + ux)   (computes session key)
		//User:  K = H(S)
		user->loadPrivateKey(I, p, pwdLen);
		user->loadPasswordVerifier();
		user->loadSessionKey(false);
		if (memcmp(host->user.v.getBinary(), user->user.v.getBinary(),
				host->user.v.size())) {
			printf("Verifier did not match\n");
		}
		//-----------------------------------------------------------------
		//Host:  S = (Av^u) ^ b              (computes session key)
		//      Host:  K = H(S)
		host->loadSessionKey(true);
		if (memcmp(host->key.K, user->key.K, host->keySize())) {
			printf("Keys did not match\n");
		}
		//-----------------------------------------------------------------
		//User -> Host:  M = H(H(N) xor H(g), H(I), s, A, B, K)
		user->generateUserProof(I);
		memcpy(host->proof.M, user->proof.M, host->keySize());
		//-----------------------------------------------------------------
		//Host -> User:  H(A, M, K)
		host->generateHostProof();
		user->generateHostProof();
		if (memcmp(host->proof.AMK, user->proof.AMK, host->keySize())) {
			printf("Challenge did not match\n");
		}
	}
	delete host;
	delete user;
}

bool Srp::loadPrime() noexcept {
	return (group.N.get() || group.N.put(Nghex[type].Nhex));
}

bool Srp::loadGenerator() noexcept {
	return (group.g.get() || group.g.put(Nghex[type].ghex));
}

bool Srp::loadMultiplierParameter() noexcept {
	if (!group.N.size() || !group.g.size()) {
		return false;
	} else if (group.k.get()) {
		return true;
	}

	unsigned char md[MDSIZE];
	//Multiplier parameter k = H(N, PAD(g))
	auto padLen = group.N.size() - group.g.size();
	return H.init() && H.update(group.N.getBinary(), group.N.size())
			&& H.update(zeros, padLen)
			&& H.update(group.g.getBinary(), group.g.size()) && H.final(md)
			&& group.k.put(md, H.length());
}

bool Srp::checkNotZero(BIGNUM *n) noexcept {
	if (!n || !group.N.get() || !ctx) {
		return false;
	}

	BN_CTX_start(ctx);
	auto m = BN_CTX_get(ctx);

	if (!m) {
		BN_CTX_end(ctx);
		return false;
	}

	auto ret = BN_nnmod(m, n, group.N.get(), ctx) && !BN_is_zero(m);
	BN_CTX_end(ctx);

	return ret;
}

bool Srp::checkRange(BIGNUM *n) noexcept {
	return n && BN_ucmp(n, group.N.get()) < 0;
}

bool Srp::newContext() noexcept {
	return ctx || (ctx = BN_CTX_new());
}

void Srp::freeContext() noexcept {
	BN_CTX_free(ctx);
}

Srp::BigNumber::BigNumber() noexcept {

}

Srp::BigNumber::~BigNumber() {
	clear();
}

bool Srp::BigNumber::put(BIGNUM *n) noexcept {
	if (this->n != n) {
		clear();
	}

	unsigned int size = BN_num_bytes(n);
	if (size <= 1024 && BN_bn2bin(n, binary)) {
		this->n = n;
		this->bytes = size;
		return true;
	} else {
		clear();
		return false;
	}
}

bool Srp::BigNumber::put(const unsigned char *binary,
		unsigned int length) noexcept {
	auto n = BN_bin2bn(binary, length, this->n);
	if (!n) {
		clear();
		return false;
	} else {
		return put(n);
	}
}

bool Srp::BigNumber::put(const char *hex) noexcept {
	if (!BN_hex2bn(&n, hex)) {
		clear();
		return false;
	} else {
		return put(n);
	}
}

bool Srp::BigNumber::random(int bits, int top, int bottom) noexcept {
	if (!n && !(n = BN_new())) {
		return false;
	} else if (!BN_rand(n, bits, top, bottom)) {
		clear();
		return false;
	} else {
		return put(n);
	}
}

bool Srp::BigNumber::pseudoRandom(const BIGNUM *range) noexcept {
	if (!n && !(n = BN_new())) {
		return false;
	} else if (!BN_rand_range(n, range)) {
		clear();
		return false;
	} else {
		return put(n);
	}
}

BIGNUM* Srp::BigNumber::get() const noexcept {
	return n;
}

const unsigned char* Srp::BigNumber::getBinary() const noexcept {
	return binary;
}

unsigned int Srp::BigNumber::size() const noexcept {
	return bytes;
}

void Srp::BigNumber::print() const noexcept {
	if (n && BN_print_fp(stdout, n)) {
		printf("\nSIZE: %u\n", bytes);
	} else {
		printf("UNDEFINED\n");
	}
}

void Srp::BigNumber::clear() noexcept {
	BN_clear_free(n);
	n = nullptr;
	bytes = 0;
	memset(binary, 0, sizeof(binary));
}

} /* namespace wanhive */
