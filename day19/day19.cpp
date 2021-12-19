#include <algorithm>
#include <sstream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <fmt/format.h>

using namespace std;

struct Vec3
{
	int v[3];

	Vec3() : v{0, 0, 0} {}
	Vec3(int x, int y, int z): v{x, y, z} {}
};

static bool operator==(const Vec3 &a, const Vec3 &b)
{
	return a.v[0] == b.v[0]
		&& a.v[1] == b.v[1]
		&& a.v[2] == b.v[2];
}

namespace std
{
	template<>
	struct hash<Vec3>
	{
		size_t operator()(const Vec3& k) const
		{
			return k.v[0] + k.v[1] + k.v[2];
		}
	};
}

struct Mat3
{
	int v[9];

	Mat3() : v{1, 0, 0, 0, 1, 0, 0, 0, 1} {}
};

static bool operator==(const Mat3& a, const Mat3& b)
{
	for (int i = 0; i < 9; i++)
	{
		if (a.v[i] != b.v[i])
		{
			return false;
		}
	}
	return true;
}

static inline Mat3 mul(const Mat3& a, const Mat3 b)
{
	Mat3 res;
	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			res.v[y * 3 + x] = 0;
			for (int i = 0; i < 3; i++)
			{
				res.v[y * 3 + x] += a.v[y * 3 + i] * b.v[i * 3 + x];
			}
		}
	}
	return res;
}

static inline Vec3 mul(const Mat3& a, const Vec3 v)
{
	Vec3 res;
	for (int y = 0; y < 3; y++)
	{
		res.v[y] = 0;
		for (int x = 0; x < 3; x++)
		{
			res.v[y] += a.v[y * 3 + x] * v.v[x];
		}
	}
	return res;
}

static inline Vec3 sub(const Vec3 a, const Vec3 b)
{
	Vec3 res;
	for (int x = 0; x < 3; x++)
	{
		res.v[x] = a.v[x] - b.v[x];
	}
	return res;
}

#define PI 3.141592653589793
static vector<Mat3> rotations;

struct Scanner
{
	int id;
	vector<Vec3> beacons;
	const struct Scanner *ref;
	Mat3 rot;
	Vec3 tran;
	Vec3 pos;

	Scanner() : id(-1), ref(nullptr) {}
};

istream& operator>>(istream& in, Scanner& s)
{
	string str;
	s.id = -1;
	s.beacons.clear();
	while (getline(in, str))
	{
		if (str.empty())
		{
			if (!s.beacons.empty())
			{
				break;
			}
		}
		else if (str.rfind("---", 0) == 0)
		{
			istringstream iss(str);
			string tmp;
			iss >> tmp;
			iss >> tmp;
			iss >> tmp;
			s.id = stoi(tmp);
		}
		else
		{
			istringstream iss(str);
			Vec3 tmp;
			iss >> tmp.v[0];
			iss.get();
			iss >> tmp.v[1];
			iss.get();
			iss >> tmp.v[2];
			s.beacons.push_back(tmp);
		}
	}
	if (!s.beacons.empty())
	{
		in.clear();
	}
	return in;
}

static void compute_rotations()
{
	int ctable[4];
	int stable[4];
	for (int i = 0; i < 4; i++)
	{
		ctable[i] = roundl(cos(PI * 0.5 * i));
		stable[i] = roundl(sin(PI * 0.5 * i));
	}
	for (int alfa = 0; alfa < 4; alfa++)
	{
		Mat3 malfa;
		malfa.v[4] = malfa.v[8] = ctable[alfa];
		malfa.v[5] = -stable[alfa];
		malfa.v[7] = stable[alfa];

		for (int beta = 0; beta < 4; beta++)
		{
			Mat3 mbeta;
			mbeta.v[0] = mbeta.v[8] = ctable[beta];
			mbeta.v[2] = stable[beta];
			mbeta.v[6] = -stable[beta];

			Mat3 r1 = mul(malfa, mbeta);
			for (int gamma = 0; gamma < 4; gamma++)
			{
				Mat3 mgamma;
				mgamma.v[0] = mgamma.v[4] = ctable[gamma];
				mgamma.v[1] = -stable[gamma];
				mgamma.v[3] = stable[gamma];

				Mat3 r2 = mul(r1, mgamma);
				if (find(rotations.begin(), rotations.end(), r2) == rotations.end())
				{
					rotations.push_back(r2);
				}
			}
		}
	}
}

static bool set_reference(const Scanner& ref, Scanner& s)
{
	unordered_map<Vec3, int> delta;
	for (size_t i = 0; i < rotations.size(); i++)
	{
		delta.clear();
		for (const auto& b: s.beacons)
		{
			auto br = mul(rotations.at(i), b);
			for (auto& p: ref.beacons)
			{
				Vec3 d = sub(br, p);
				if (++delta[d] >= 12)
				{
					s.ref = &ref;
					s.rot = rotations.at(i);
					s.tran = d;
					return true;
				}
			}
		}
	}
	return false;
}

static void update_map(unordered_map<Vec3, bool>& m, const Scanner& s)
{
	for (auto v: s.beacons)
	{
		for (const Scanner *sc = &s; sc->ref; sc = sc->ref)
		{
			v = sub(mul(sc->rot, v), sc->tran);
		}
		m[v] = true;
	}
}

static void find_position(Scanner &s)
{
	Vec3 res;
	for (const Scanner *sc = &s; sc->ref; sc = sc->ref)
	{
		res = sub(mul(sc->rot, res), sc->tran);
	}
	s.pos = res;
}

static int manhattan(const Vec3& a, const Vec3& b)
{
	int d = 0;
	for (size_t i = 0; i < 3; i++)
	{
		int t = a.v[i] - b.v[i];
		d += t >= 0 ? t : -t;
	}
	return d;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fmt::print(stderr, "Usage: {} <filename>\n", argv[0]);
		return 1;
	}

	ifstream in(argv[1]);
	if (!in)
	{
		fmt::print(stderr, "Cannot open {}\n", argv[1]);
		return 1;
	}

	compute_rotations();
	vector<Scanner> scanners;
	Scanner tmp;
	while (in >> tmp)
	{
		scanners.emplace_back(move(tmp));
	}
	in.close();

	size_t solved = 1;
	for (size_t i = 0; i < solved; i++)
	{
		for (size_t j = solved; j < scanners.size(); j++)
		{
			if (set_reference(scanners.at(i), scanners.at(j)))
			{
				swap(scanners[solved], scanners[j]);
				solved++;
			}
		}
	}

	unordered_map<Vec3, bool> map;
	for (auto& s: scanners)
	{
		update_map(map, s);
	}
	fmt::print("Part1: {}\n", map.size());

	for (auto &s: scanners)
	{
		find_position(s);
	}
	int dmax = 0;
	for (auto &a : scanners)
	{
		for (auto& b: scanners)
		{
			if (a.id != b.id)
			{
				int d = manhattan(a.pos, b.pos);
				if (dmax < d)
				{
					dmax = d;
				}
			}
		}
	}
	fmt::print("Part2: {}\n", dmax);
	return 0;
}
