
#ifndef _FLEX_H
#define _FLEX_H

#define MAX_FLEX_GAMES 50

struct flex_data
{
	U8 level;
	U8 games;
	U8 history[MAX_FLEX_GAMES];
};


struct flex_config
{
	U8 min_level;
	U8 max_level;
	U8 *adj_level;
	U8 min_percent;
	U8 max_percent;
	U8 *adj_percent;
	U8 frequency;
	struct flex_data *data;
	struct area_csum *csum;
};

__common2__ void flex_end_game (__fardata__ const struct flex_config *fconf);
__common2__ void flex_end_player (__fardata__ const struct flex_config *fconf, U8 value);
__common2__ void flex_reset (__fardata__ const struct flex_config *fconf);

#endif /* _FLEX_H */

