
typedef struct duty_pulse
{
	uint8_t on_time;
	uint8_t off_time;
} duty_pulse_t;


typedef uint8_t duty_state_t;


typedef struct duty_entry
{
	const duty_pulse_t *pulse;
	const duty_ops_t *ops;
	uint8_t id;
	duty_state_t state;
	uint8_t cycle_limit;
} duty_entry_t;


typedef struct duty_ops
{
	void (*on) (uint8_t id);
	void (*off) (uint8_t id);
	void (*limit) (uint8_t id);
} duty_ops_t;


void duty_init (
	duty_entry_t *entry, 
	duty_ops_t *ops, 
	uint8_t id, 
	const duty_pulse_t *pulse )
{
	entry->pulse = pulse;
	entry->ops = ops;
	entry->id = id;
	entry->state = pulse->on_time;
	entry->cycle_limit = cycle_limit;
}

void duty_update (duty_entry_t *entry)
{
	if (entry->state < 0)
	{
		/* Output is currently off */
		entry->state++;
		if (entry->state == 0)
		{
			/* A full cycle has completed */
			if (entry->cycle_limit > 0)
			{
				entry->cycle_limit--;
				if (entry->cycle_limit == 0)
				{
					/* Keep off permanently */
					return;
				}
			}
			/* Output should be switched on */
			(*entry->ops->on) (entry->id);
			/* Reset state to limit on time */
			entry->state = pulse->on_time;
		}
	}
	else
	{
		/* Output is currently on */
		entry->state--;
		if (entry->state == 0)
		{
			/* Output should be switched off */
			(*entry->ops->off) (entry->id);
			/* Reset state to limit off time */
			entry->state = -pulse->off_time;
		}
	}
}


