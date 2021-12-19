#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PI 3.141592653589793

typedef struct vec3
{
	int v[3];
} vec3;


typedef struct mat3
{
	int v[9];
} mat3;

struct scanner
{
	int id;
	vec3 *beacons;
	size_t count;

	const struct scanner *ref;
	mat3 rot;
	vec3 tran;
	vec3 pos;
};

struct vdict
{
	struct helem
	{
		vec3 v;
		int count;
	} *table;
	size_t count;
	size_t size;
};

static mat3 rotations[24];

static inline vec3 vv_sub(vec3 a, vec3 b)
{
	vec3 r;
	for (int i = 0; i < 3; i++)
	{
		r.v[i] = a.v[i] - b.v[i];
	}
	return r;
}

static inline vec3 mv_mul(mat3 m, vec3 v)
{
	vec3 r = {0};
	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			r.v[y] += m.v[y * 3 + x] * v.v[x];
		}
	}
	return r;
}

static inline mat3 mm_mul(mat3 a, mat3 b)
{
	mat3 r = {0};
	for (int y = 0; y < 3; y++)
	{
		for (int x = 0; x < 3; x++)
		{
			for (int k = 0; k < 3; k++)
			{
				r.v[y * 3 + x] += a.v[y * 3 + k] * b.v[k * 3 + x];
			}
		}
	}
	return r;
}

static inline size_t hashfn(vec3 key)
{
	return key.v[0] + key.v[1] + key.v[2];
}

static void rehash(struct vdict *d)
{
	size_t size = d->size ? d->size * 2 : 64;
	struct helem *table = calloc(size, sizeof(*table));
	if (!table) { abort(); }
	for (size_t i = 0; i < size; i++)
	{
		table[i].v.v[0] = INT_MAX;
	}
	for (size_t i = 0; i < d->size; i++)
	{
		if (d->table[i].v.v[0] == INT_MAX)
		{
			continue;
		}

		size_t idx = hashfn(d->table[i].v) % size;
		while (table[idx].v.v[0] != INT_MAX)
		{
			if (++idx >= size)
			{
				idx = 0;
			}
		}
		table[idx] = d->table[i];
	}
	free(d->table);
	d->table = table;
	d->size = size;
}

static void vdict_release(struct vdict *d)
{
	free(d->table);
}

static void vdict_clear(struct vdict *d)
{
	for (size_t i = 0; i < d->size; i++)
	{
		d->table[i].v.v[0] = INT_MAX;
	}
	d->count = 0;
}

static int vdict_inc(struct vdict *d, vec3 key)
{
	if (d->count * 100 >= d->size * 75)
	{
		rehash(d);
	}
	size_t idx = hashfn(key) % d->size;
	while (d->table[idx].v.v[0] != INT_MAX)
	{
		if (!memcmp(&d->table[idx].v, &key, sizeof(key)))
		{
			break;
		}
		if (++idx >= d->size)
		{
			idx = 0;
		}
	}
	if (d->table[idx].v.v[0] == INT_MAX)
	{
		d->table[idx].v = key;
		d->table[idx].count = 0;
		d->count++;
	}
	return ++d->table[idx].count;
}

static void build_rotations(void)
{
	size_t count = 0;
	int icos[4];
	int isin[4];
	for (int i = 0; i < 4; i++)
	{
		icos[i] = roundl(cos(PI * 0.5 * i));
		isin[i] = roundl(sin(PI * 0.5 * i));
	}
	for (int rx = 0; rx < 4; rx++)
	{
		mat3 mx = {0};
		mx.v[0] = 1;
		mx.v[4] = mx.v[8] = icos[rx];
		mx.v[5] = -isin[rx];
		mx.v[7] = isin[rx];
		for (int ry = 0; ry < 4; ry++)
		{
			mat3 my = {0};
			my.v[4] = 1;
			my.v[0] = my.v[8] = icos[ry];
			my.v[2] = isin[ry];
			my.v[6] = -isin[ry];
			for (int rz = 0; rz < 4; rz++)
			{
				mat3 mz = {0};
				mz.v[8] = 1;
				mz.v[0] = mz.v[4] = icos[rz];
				mz.v[1] = -isin[rz];
				mz.v[3] = isin[rz];

				mat3 r = mm_mul(mm_mul(mx, my), mz);
				int found = 0;
				for (size_t i = 0; i < count; i++)
				{
					if (!memcmp(&rotations[i], &r, sizeof(r)))
					{
						found = 1;
						break;
					}
				}
				if (!found)
				{
					rotations[count++] = r;
				}
			}
		}
	}
	assert(count == 24);
}

static int set_reference(const struct scanner *ref, struct scanner *s)
{
	struct vdict delta = {0};
	for (size_t r = 0; r < 24; r++)
	{
		vdict_clear(&delta);
		for (size_t i = 0; i < s->count; i++)
		{
			vec3 br = mv_mul(rotations[r], s->beacons[i]);
			for (size_t j = 0; j < ref->count; j++)
			{
				vec3 d = vv_sub(br, ref->beacons[j]);
				if (vdict_inc(&delta, d) >= 12)
				{
					s->ref = ref;
					s->rot = rotations[r];
					s->tran = d;
					vdict_release(&delta);
					return 1;
				}
			}
		}
	}
	vdict_release(&delta);
	return 0;
}

static void update_map(struct vdict *map, const struct scanner *s)
{
	for (size_t i = 0; i < s->count; i++)
	{
		vec3 v = s->beacons[i];
		for (const struct scanner *sc = s; sc->ref; sc = sc->ref)
		{
			v = vv_sub(mv_mul(sc->rot, v), sc->tran);
		}
		vdict_inc(map, v);
	}
}

static void find_position(struct scanner *s)
{
	vec3 v = {0};
	for (const struct scanner *sc = s; sc->ref; sc = sc->ref)
	{
		v = vv_sub(mv_mul(sc->rot, v), sc->tran);
	}
	s->pos = v;
}

static int manhattan(const struct scanner *a, const struct scanner *b)
{
	int d = 0;
	for (size_t i = 0; i < 3; i++)
	{
		int t = a->pos.v[i] - b->pos.v[i];
		d += t > 0 ? t : -t;
	}
	return d;
}

static void scanner_release(struct scanner *s)
{
	free(s->beacons);
}

static int scanner_load(FILE *in, struct scanner *s)
{
	memset(s, 0, sizeof(*s));
	if (fscanf(in, " --- scanner %d ---", &s->id) != 1)
	{
		return -1;
	}

	size_t size = 0;
	int x, y, z;
	while (fscanf(in, " %d,%d,%d", &x, &y, &z) == 3)
	{
		if (s->count == size)
		{
			size_t nsize = size ? size * 2 : 8;
			vec3 *nb = realloc(s->beacons, nsize * sizeof(*nb));
			if (!nb)
			{
				break;
			}
			size = nsize;
			s->beacons = nb;
		}
		s->beacons[s->count].v[0] = x;
		s->beacons[s->count].v[1] = y;
		s->beacons[s->count].v[2] = z;
		s->count++;
	}
	/* fscanf eats the minus */
	ungetc('-', in);
	return 0;
}

static struct scanner *load_all(FILE *in, size_t *cnt)
{
	struct scanner *arr = NULL;
	size_t count = 0;
	size_t size = 0;
	struct scanner tmp;
	while (scanner_load(in, &tmp) >= 0)
	{
		if (count == size)
		{
			size_t nsize = size ? size * 2 : 8;
			struct scanner *narr = realloc(arr, nsize * sizeof(*narr));
			if (!narr)
			{
				break;
			}
			size = nsize;
			arr = narr;
		}
		arr[count++] = tmp;
	}
	*cnt = count;
	return arr;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		return 1;
	}

	FILE *in = fopen(argv[1], "rb");
	if (!in)
	{
		fprintf(stderr, "Cannot open %s\n", argv[1]);
		return -1;
	}
	size_t count;
	struct scanner *arr = load_all(in, &count);
	fclose(in);

	build_rotations();
	size_t solved = 1;
	for (size_t i = 0; i < solved; i++)
	{
		for (size_t j = solved; j < count; j++)
		{
			if (set_reference(&arr[i], &arr[j]))
			{
				struct scanner tmp = arr[solved];
				arr[solved] = arr[j];
				arr[j] = tmp;
				solved++;
			}
		}
	}

	struct vdict map = {0};
	for (size_t i = 0; i < count; i++)
	{
		update_map(&map, &arr[i]);
	}
	printf("Part1: %zu\n", map.count);
	vdict_release(&map);

	for (size_t i = 0; i < count; i++)
	{
		find_position(&arr[i]);
	}
	int maxd = 0;
	for (size_t i = 0; i < count; i++)
	{
		for (size_t j = 0; j < i; j++)
		{
			int d = manhattan(&arr[i], &arr[j]);
			if (maxd < d)
			{
				maxd = d;
			}
		}
	}
	printf("Part2: %d\n", maxd);

	for (size_t i = 0; i < count; i++)
	{
		scanner_release(&arr[i]);
	}
	free(arr);
	return 0;
}
