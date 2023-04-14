#include<stdio.h>
#include<NTL/ZZ.h>
#include<utility>
#include<string>
#include<sstream>
#include<openssl/sha.h>
#include<openssl/evp.h>
using namespace NTL;
using std::pair;
using std::string;
using std::to_string;
using std::stringstream;
typedef pair<ZZ, ZZ> node;
typedef pair<ZZ, node> keys;

ZZ A = to_ZZ(0);
ZZ B = to_ZZ("7");
ZZ P = to_ZZ("115792089237316195423570985008687907853269984665640564039457584007908834671663");
ZZ N = to_ZZ("115792089237316195423570985008687907852837564279074904382605163141518161494337");
ZZ G_X = to_ZZ("55066263022277343669578718895168534326250603453777594175500187360389116729240");
ZZ G_Y = to_ZZ("32670510020758816978083085130507043184471273380659243275938904335757337482424");
node G = node(G_X, G_Y);

ZZ Legendre(ZZ y, ZZ p) {
	return PowerMod(y, (p - 1) / 2, p);
}

ZZ tonelli_Shanks(ZZ y, ZZ p) {
	if (p % 4 == 3) {
		return PowerMod(y, (p + 1) / 4, p);
	}
	ZZ q = p - 1;
	long s = 0;
	ZZ c;
	while (q % 2 == 0) {
		q = q / 2;
		s += 1;
	}
	for (ZZ z = to_ZZ("2"); z < p; z++) {
		if (Legendre(z, p) == p - 1) {
			c = PowerMod(z, q, p);
			break;
		}
	}
	ZZ r = PowerMod(y, (q + 1) / 2, p);
	ZZ t = PowerMod(y, q, p);
	long m = s;
	if (t % p == 1) {
		return r;
	}
	else {
		long i = 0;
		ZZ temp;
		ZZ b;
		while (t % p != 1) {
			temp = PowerMod(t, power(to_ZZ("2"), i + 1), p);
			i += 1;
			if (temp % p == 1) {
				b = PowerMod(c, power(to_ZZ("2"), m - i - 1), p);
				r = r * b % p;
				c = b * b % p;
				t = t * c % p;
				m = i;
				i = 0;
			}
		}
		return r;
	}
}

node elliptic_add(node p, node q) {
	if ((p.first | p.second | q.first | q.second) != 0) {
		return node(to_ZZ(0), to_ZZ(0));
	}
	else if ((p.first | p.second) != 0) {
		return q;
	}
	else if ((q.first | q.second) != 0) {
		return p;
	}
	else {
		if (p.first > q.first) {
			node temp = p;
			p = q;
			q = temp;
		}
		static ZZ r[2];
		ZZ slope = (q.second - p.second) * InvMod(q.first - p.first, P) % P;
		r[0] = (power(slope, 2) - p.first - q.first) % P;
		r[1] = (slope * (p.first - r[0]) - p.second) % P;
		return node(r[0], r[1]);
	}
}

node elliptic_double(node p) {
	ZZ r[2];
	ZZ slope = (3 * power(p.first, 2) + A) * InvMod(2 * p.second, P) % P;
	r[0] = (power(slope, 2) - 2 * p.first) % P;
	r[1] = (slope * (p.first - r[0]) - p.second) % P;
	return node(r[0], r[1]);
}

node elliptic_multiply(ZZ s, node p) {
	node r = node(to_ZZ(0), to_ZZ(0));
	long length = NumBytes(s);
	uint8_t* s_binary = new uint8_t[length];
	BytesFromZZ(s_binary, s, length);
	for (size_t i = 0; i < length; i++) {
		if (s_binary[i] == 1) {
			r = elliptic_add(r, p);
		}
		p = elliptic_double(p);
	}
	return r;
}

string precompute(string id, ZZ a, ZZ b, ZZ g_x, ZZ g_y, ZZ x_a, ZZ y_a) {
	stringstream buffer;

	buffer << a;
	string a_temp = buffer.str();

	buffer << b;
	string b_temp = buffer.str();

	buffer << g_x;
	string gx_temp = buffer.str();

	buffer << g_y;
	string gy_temp = buffer.str();

	buffer << x_a;
	string xa_temp = buffer.str();

	buffer << y_a;
	string ya_temp = buffer.str();

	string ENTL = to_string(id.length() << 3);

	string joint = ENTL + id + a_temp + b_temp + gx_temp + gy_temp + xa_temp + ya_temp;
	char digest[256];
	SHA256((const unsigned char*)joint.data(), joint.length(), (unsigned char*)digest);
	return string(digest);
}

keys generate_key() {
	ZZ a = power(to_ZZ(2), 255);
	ZZ private_key = a + to_ZZ(rand()) % (power(to_ZZ(2), 256) - 1 + a);
	node public_key = elliptic_multiply(private_key, G);
	return keys(private_key, public_key);
}

node sign(ZZ private_key, string message, string Z_A) {
	string _M = Z_A + message;
	string _M_b;
	EVP_MD_CTX* ctx;
	const EVP_MD* md;
	size_t len;

	ctx = EVP_MD_CTX_new();
	md = EVP_sm3();
	EVP_DigestInit_ex(ctx, md, NULL);
	EVP_DigestUpdate(ctx, _M.data(), _M.length());
	EVP_DigestFinal_ex(ctx, (unsigned char*)_M_b.data(), &len);
	EVP_MD_CTX_free(ctx);
	ZZ e = conv<ZZ>(_M_b);

	string e_;
	ctx = EVP_MD_CTX_new();
	md = EVP_sm3();
	EVP_DigestInit_ex(ctx, md, NULL);
	EVP_DigestUpdate(ctx, message.data(), message.length());
	EVP_DigestFinal_ex(ctx, (unsigned char*)e_.data(), &len);
	EVP_MD_CTX_free(ctx);

	string k_;
	SHA256((const unsigned char*)(conv<string>(private_key) + e_).data(), (conv<string>(private_key) + e_).length(), (unsigned char*)k_.data());
	ZZ k = conv<ZZ>(k_);
	if (k >= P) {
		return node(to_ZZ(0), to_ZZ(0));
	}

	node random_point = elliptic_multiply(k, G);
	ZZ r = (e + random_point.first) % N;
	ZZ s = (InvMod((1 + private_key) % N, N) * (k - r * private_key)) % N;
	return node(r, s);
}

bool verify(node public_key, string id, string message, node signature) {
	ZZ r = signature.first;
	ZZ s = signature.second;
	string Z = precompute(id, A, B, G_X, G_Y, public_key.first, public_key.second);
	string _M = Z + message;

	EVP_MD_CTX* ctx;
	const EVP_MD* md;
	size_t len;
	string e_;
	ctx = EVP_MD_CTX_new();
	md = EVP_sm3();
	EVP_DigestInit_ex(ctx, md, NULL);
	EVP_DigestUpdate(ctx, _M.data(), _M.length());
	EVP_DigestFinal_ex(ctx, (unsigned char*)e_.data(), &len);
	EVP_MD_CTX_free(ctx);

	ZZ e = conv<ZZ>(e_);
	ZZ t = (r + s) % N;

	node point = elliptic_multiply(s, G);
	node point1 = elliptic_multiply(t, public_key);
	point = elliptic_add(point, point1);

	ZZ x1 = point.first;
	ZZ R = (e + x1) % N;
	return R == r;
}

int main() {
	keys key = generate_key();
	string message = "helloworld\0";
	string ID = "202100460055";
	string Z_A = precompute(ID, A, B, G_X, G_Y, key.second.first, key.second.second);

	node signature = sign(key.first, message, Z_A);
	while (signature.first == 0 && signature.second == 0) {
		keys key = generate_key();
		string Z_A = precompute(ID, A, B, G_X, G_Y, key.second.first, key.second.second);
		node signature = sign(key.first, message, Z_A);
	}

	if (verify(key.second, ID, message, signature) == 1) {
		printf("verified!");
	}
	return 0;
}
