
#include <freewpc.h>
#include <simulation.h>

#define HASH_SIZE 101

struct conf_item
{
	const char *name;
	unsigned int hash;
	int *valp;
	struct conf_item *chain;
};


struct conf_item *conf_table[HASH_SIZE] = { NULL, };

static unsigned int conf_hash (const char *name)
{
	unsigned int hash = 0;
	while (*name)
	{
		hash = hash * 17 + *name;
		name++;
	}
	return hash;
}

static struct conf_item *conf_find (const char *name)
{
	unsigned int hash = conf_hash (name);
	struct conf_item *cf = conf_table[hash % HASH_SIZE];
	while (cf != NULL)
	{
		if (cf->hash == hash)
			return cf;
		cf = cf->chain;
	}
	return NULL;
}

void conf_add (const char *name, int *valp)
{
	struct conf_item *cf = malloc (sizeof (struct conf_item));
	cf->name = name;
	cf->hash = conf_hash (name);
	cf->valp = valp;
	cf->chain = conf_table[cf->hash % HASH_SIZE];
	conf_table[cf->hash % HASH_SIZE] = cf;
}

int conf_read (const char *name)
{
	struct conf_item *cf = conf_find (name);
	if (cf)
		return *(cf->valp);
	return 0;
}

void conf_write (const char *name, int val)
{
	struct conf_item *cf = conf_find (name);
	if (cf)
		*(cf->valp) = val;
	else
		simlog (SLC_DEBUG, "No such conf item '%s'\n", name);
}


