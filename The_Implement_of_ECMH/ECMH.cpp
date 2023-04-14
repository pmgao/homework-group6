#include<stdio.h>
#include<NTL/ZZ.h>
#include<utility>
#include<string>
#include<sstream>
#include<vector>
#include<openssl/sha.h> 

using namespace NTL;
using std::pair;
using std::vector;
using std::string;
using std::to_string;
using std::stringstream;
using std::cout;
using std::endl;
typedef pair<ZZ, ZZ> node;

ZZ A = to_ZZ(0);
ZZ B = to_ZZ(7);
ZZ P = to_ZZ("115792089237316195423570985008687907853269984665640564039457584007908834671663");
ZZ N = to_ZZ("115792089237316195423570985008687907852837564279074904382605163141518161494337");

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

node elliptic_inv(node p) {
	return node(p.first, P - p.second);
}

node elliptic_sub(node p, node q) {
	return elliptic_add(p, elliptic_inv(q));
}

ZZ Legendre(ZZ y, ZZ p) {
	return PowerMod(y, (p - 1) / 2, p);
}

ZZ msg_to_x(ZZ m) {
	string x_;
	SHA256((const unsigned char*)conv<string>(m).data(), conv<string>(m).length(), (unsigned char*)x_.data());
	ZZ x;
	while (1) {
		ZZ x = conv<ZZ>(x_);
		if (Legendre(x, P) != 0) {
			break;
		}
		SHA256((const unsigned char*)conv<string>(m).data(), conv<string>(m).length(), (unsigned char*)x_.data());
	}
	return x;
}

ZZ get_y(ZZ x) {
	ZZ right = (power(x, 3) + 7) % P;
	ZZ a;
	while (1) {
		a = RandomBnd(P);
		if (Legendre(a, P) == P - 1) {
			break;
		}
	}
	ZZ base = a + SqrRoot(power(a, 2) - right);
	ZZ expo = (P + 1) / 2;
	return PowerMod(base, expo, P);
}

node msg_to_dot(string m) {
	ZZ M = conv<ZZ>(m);
	ZZ x = msg_to_x(M);
	ZZ y = get_y(x);
	return node(x, y);

}

node ADD(node ecmh, string msg) {
	return elliptic_add(ecmh, msg_to_dot(msg));
}

node elliptic_double(node p) {
	static ZZ r[2];
	ZZ slope = (3 * power(p.first, 2) + A) * InvMod(2 * p.second % P, P) % P;
	r[0] = (power(slope, 2) - p.first * 2) % P;
	r[1] = (slope * (p.first - r[0]) - p.second) % P;
	return node(r[0], r[1]);
}

node single(string msg) {
	return ADD(node(to_ZZ(0), to_ZZ(0)), msg);
}

node remove(node ecmh, string msg) {
	return elliptic_sub(ecmh, msg_to_dot(msg));
}

node combine(vector<string> msg_set) {
	node ans = single(msg_set[0]);
	size_t len = msg_set.size() - 1;
	for (size_t i = 0; i < len; i++) {
		ans = ADD(ans, msg_set[i + 1]);
	}
	return ans;
}

int main() {
	string m1 = "202100460055";
	string m2 = "1234567890";
	cout << single(m1).first << " " << single(m1).second << endl;
	return 0;
}
